/*
Copyright (c) 2013 Lukasz Magiera

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include "protocol.h"
#include "http/data.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dlfcn.h>
#include <sstream>
#define is_dotso(_pn,_sl) (_pn[_sl-3]=='.'&&_pn[_sl-2]=='s'&&_pn[_sl-1]=='o')
#define is_dotnhp(_pn,_sl) (_pn[_sl-4]=='.'&&_pn[_sl-3]=='n'&&_pn[_sl-2]=='h'&&_pn[_sl-1]=='p')


void page_mapper::refresh(string d)
{
    files.clear();
    deque<string>todo;
    todo.push_front(d);

    while(todo.size()>0)
    {
        DIR *dp;
        if((dp  = opendir(todo.back().c_str())) == NULL)
        {
            cout <<"error: "<< errno << ", for directory " << todo.back().c_str() << endl;
        }
        else
        {
            struct dirent *de;
            while ((de = readdir(dp)) != NULL)
            {
                struct stat fi;
                if (lstat( (todo.back()+string(de->d_name)).c_str(), &fi)<0)
                {
                    cout << "error(pm.statact) \n";
                }
                else
                {
                    if(S_ISDIR(fi.st_mode))
                    {
                        if(de->d_name[0]!='.')
                        {
                            todo.push_front((todo.back()+string(de->d_name)+'/').c_str());
                        }
                    }
                    else
                    {
                        if(string(de->d_name)[string(de->d_name).size()-1]!='~')
                        {
                            files.push_back((todo.back()+string(de->d_name)).c_str());
                        }

                    }
                }
                //cout << (string(de->d_name)) << endl;
            }
            closedir(dp);
        }
        todo.pop_back();
    }

    for(unsigned int i=0;i<files.size();i++)
    {
        bool loaded=false;
        bool toref=false;
        time_t fatt=0;
        int pgi=-1;

        for(unsigned int j=0;j<base.size();j++)
        {
            if(base[j].file==files[i])
            {
                pgi=j;
                loaded=true;
                struct stat tst;
                int rst = stat(files[i].c_str(), &tst);
                if(rst != 0)
                {
                    log("pagemap.cpp:refresh","stat error");
                    continue;
                }
                fatt=tst.st_mtime;
                if(base[j].timestamp!=tst.st_mtime)
                {
                    toref=true;
                }
            }
        }
        if(loaded&&toref)
        {
            if(is_dotso(files[i],files[i].size()))
            {
                SDL_mutexP(http::mtx_exec);
                SDL_mutexP(http::mtx_exec2);

                dlclose(((nativepage*)base[pgi].data)->handle);
                ((nativepage*)base[pgi].data)->handle = dlopen(files[i].c_str(), RTLD_NOW|RTLD_LOCAL);
                if(!((nativepage*)base[pgi].data)->handle)
                {
                    log("ERROR@pagemap.cpp","can't open shared file: "+files[i]);
                }
                else
                {
                    ((nativepage*)base[pgi].data)->onload = (Tonload) dlsym(((nativepage*)base[pgi].data)->handle,"onload");
                    ((nativepage*)base[pgi].data)->page = (Tpage) dlsym(((nativepage*)base[pgi].data)->handle,"page");
                    if(!((nativepage*)base[pgi].data)->onload||!((nativepage*)base[pgi].data)->page)
                    {
                        log("ERROR@pagemap.cpp","loading native symbols failed: "+files[i]);
                        dlclose(((nativepage*)base[pgi].data)->handle);
                    }
                    else
                    {
                        int initstate = (*((nativepage*)base[pgi].data)->onload)();
                        if(initstate!=1)
                        {
                            logid(initstate,"WARNING@pagemap.cpp","invalid init state: "+files[i]);
                            dlclose(((nativepage*)base[pgi].data)->handle);
                        }
                        else
                        {
                            base[pgi].timestamp=fatt;
                            log("RELOAD@pagemap.cpp","succes[1]: "+files[i]);
                        }
                    }

                }



                SDL_mutexV(http::mtx_exec);
                SDL_mutexV(http::mtx_exec2);

            }
        }
    }
}


void page_mapper::page_mapper_init(string d)
{
    deque<string>todo;
    todo.push_front(d);

    while(todo.size()>0)
    {
        DIR *dp;
        if((dp  = opendir(todo.back().c_str())) == NULL)
        {
            cout <<"error: "<< errno << ", for directory " << todo.back().c_str() << endl;
        }
        else
        {
            struct dirent *de;
            while ((de = readdir(dp)) != NULL)
            {
                struct stat fi;
                if (lstat( (todo.back()+string(de->d_name)).c_str(), &fi)<0)
                {
                    cout << "error(pm.statact) \n";
                }
                else
                {
                    if(S_ISDIR(fi.st_mode))
                    {
                        if(de->d_name[0]!='.')
                        {
                            todo.push_front((todo.back()+string(de->d_name)+'/').c_str());
                        }
                    }
                    else
                    {
                        if(string(de->d_name)[string(de->d_name).size()-1]!='~')
                            files.push_back((todo.back()+string(de->d_name)).c_str());
                    }
                }
                //cout << (string(de->d_name)) << endl;
            }
            closedir(dp);
        }
        todo.pop_back();
    }
    for(unsigned int i=0; i<files.size(); i++)
    {
        page tmp;
        struct stat tst;
        int rst = stat(files[i].c_str(), &tst);
        if(rst != 0)
        {
            log("pagemap.cpp:init","stat error");
            continue;
        }
        tmp.timestamp=tst.st_mtime;
        tmp.file=files[i];
        if(is_dotso(files[i],files[i].size()))
        {
            tmp.type=page_native;
            nativepage* ntm = new nativepage;
            ntm->handle = dlopen(files[i].c_str(), RTLD_NOW|RTLD_LOCAL);
            if(!ntm->handle)
            {
                cout << "can't open shared file: "<<files[i].c_str()<<": "<<dlerror()<<endl;
                delete ntm;
            }
            else
            {
                ntm->onload = (Tonload) dlsym(ntm->handle,"onload");
                ntm->page = (Tpage) dlsym(ntm->handle,"page");
                if(!ntm->onload||!ntm->page)
                {
                    cout << "error loading native symbols: "<<files[i].c_str()<<endl;
                    dlclose(ntm->handle);
                    delete ntm;
                }
                else
                {
                    int initstate = (*ntm->onload)();
                    if(initstate!=1)
                    {
                        cout << "invalid init state("<<initstate<<"): "<<files[i].c_str()<<endl;
                        dlclose(ntm->handle);
                        delete ntm;
                    }
                    else
                    {
                        tmp.data = ntm;
                        base.push_back(tmp);
                        superstring pgac(files[i]);
                        string furi=pgac.from(d);
                        urimp tmu = {'/'+furi,int(base.size())-1};
                        uris.push_back(tmu);
                    }
                }
            }
        }
        else if(is_dotnhp(files[i],files[i].size()))
        {

            struct stat st;
drch:
            if(stat("/tmp/nativehttp",&st)==0)
            {
                if(S_ISDIR(st.st_mode))
                {
                    FILE* tmf=fopen("/tmp/nativehttp/tmp.cpp","w");
                    if(tmf!=NULL)
                    {
                        FILE* nhpf=fopen(files[i].c_str(),"r");
                        if(nhpf!=NULL)
                        {
                            fseek(nhpf,0,SEEK_END);
                            int size = ftell(nhpf);
                            rewind(nhpf);
                            string com;
                            com.resize(size);
                            size=fread((char*)com.c_str(),1,size,nhpf);
                            fclose(nhpf);
                            string out=parse_nhp(com);

                            //cout<<"compiled: \n"<<out.c_str()<<endl;

                            fwrite(out.c_str(),1,out.size(),tmf);
                            fclose(tmf);
                            superstring pgnam(files[i]);
                            pgnam.change(".nhp","nhp");
                            pgnam.pos=pgnam.str.size()-1;
                            string pname=pgnam.back_to("/");
                            srand(rand());
                            ostringstream tos;
                            tos << rand();
                            pname+=tos.str();
                            string command=cfg->get_var("cppcmp")+" -shared -fPIC "+cfg->get_var("flags")+" -o /tmp/nativehttp/nhpage_"+pname+".so /tmp/nativehttp/tmp.cpp";
                            bool shw;
                            if(cfg->get_int("cmpout")==1)
                            {
                                shw=true;
                            }
                            else
                            {
                                shw=false;
                            }
                            if(shw)
                            {
                                cout << "Compile "<<files[i].c_str()<<":"<<endl;
                                cout << command.c_str()<<endl;
                            }
                            FILE *cmp=NULL;
                            if(!(cmp = popen(command.c_str(), "r")))
                            {
                                cerr<<"CAN'T OPEN "<<cfg->get_var("cppcmp").c_str()<<endl;
                            }
                            char b[256];
                            while(fgets(b, 256, cmp)!=NULL)
                            {
                                if(shw)
                                {
                                    cout<<b;
                                }
                            }
                            pclose(cmp);

                            page tmp;
                            nativepage* ntm = new nativepage;
                            if(shw)cout << "file "<<"/tmp/nativehttp/nhpage_"+pname+".so"<<endl;
                            ntm->handle = dlopen(("/tmp/nativehttp/nhpage_"+pname+".so").c_str(), RTLD_NOW|RTLD_LOCAL);
                            if(!ntm->handle)
                            {
                                cout << "can't open compiled nhp file: "<<("/tmp/nativehttp/nhpage_"+pname+".so").c_str()<<": "<<dlerror()<<endl;
                                delete ntm;
                                exit(1);
                            }
                            else
                            {
                                ntm->onload = (Tonload) dlsym(ntm->handle,"onload");
                                ntm->page = (Tpage) dlsym(ntm->handle,"page");
                                if(!ntm->onload||!ntm->page)
                                {
                                    cout << "error loading native symbols: "<<("/tmp/nativehttp/nhpage_"+pname+".so").c_str()<<endl;
                                    dlclose(ntm->handle);
                                    delete ntm;
                                }
                                else
                                {
                                    int initstate = (*ntm->onload)();
                                    if(initstate!=1)
                                    {
                                        cout << "invalid init state("<<initstate<<"): "<<files[i].c_str()<<endl;
                                        dlclose(ntm->handle);
                                        delete ntm;
                                    }
                                    else
                                    {
                                        tmp.type=page_native;
                                        tmp.data=ntm;
                                        base.push_back(tmp);
                                        superstring pgac(files[i]);
                                        string furi=pgac.from(d);
                                        urimp tmu= {'/'+furi,int(base.size())-1};
                                        uris.push_back(tmu);
                                    }
                                }
                            }
                        }
                        else
                        {
                            cout << "can't open "<<files[i].c_str()<<endl;
                            fclose(tmf);
                        }

                    }
                    else
                    {
                        cout << "can't create /tmp/nativehttp/tmp.cpp\n";
                    }
                }
                else
                {
                    cout<<"/tmp/nativehttp IS NOT A DIRECTORY\n";
                }
            }
            else
            {
                mkdir("/tmp/nativehttp", 0755);
                goto drch;
            }
        }
        else
        {
            tmp.type=page_file;
            tmp.data=(void*)files[i].c_str();
            base.push_back(tmp);
            superstring pgac(files[i]);
            string furi=pgac.from(d);
            urimp tmu = {'/'+furi,int(base.size())-1};
            uris.push_back(tmu);
        }
    }
}

void page_mapper::adduri(string u,bool top)
{
    urimp tu= {u,int(base.size())};
    if(top)
    {
        uris.push_front(tu);
    }
    else
    {
        uris.push_back(tu);
    }
}

page page_mapper::by_uri(string u)
{
    vector<int>dn(uris.size(),0);
    page t= {-1,NULL};
    for(unsigned int i=0; i<u.size(); i++)
    {
        for(unsigned int j=0; j<dn.size(); j++)
        {
            if(dn[j]!=-1)
            {
                if(uris[j].u[dn[j]]==u[i]||uris[j].u[dn[j]]=='*')
                {
                    if(uris[j].u.size()==unsigned(dn[j])+1&&u.size()==unsigned(dn[j])+1)
                    {
                        return base[uris[j].sid];
                    }
                    else if(uris[j].u.size()==unsigned(dn[j])+1&&uris[j].u[dn[j]]=='*')
                    {
                        t=base[uris[j].sid];
                    }
                    dn[j]++;
                }
                else
                {
                    dn[j]=-1;
                }
            }
        }
    }

    return t;
}


