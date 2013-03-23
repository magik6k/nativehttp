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

page_mapper::page_mapper()
{
    base=new vector<page>;
}

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

        for(unsigned int j=0;j<base->size();j++)
        {
            if((*base)[j].file==files[i])
            {
                pgi=j;
                loaded=true;
                struct stat tst;
                int rst = stat(files[i].c_str(), &tst);
                if(rst != 0)
                {
                    nativehttp::server::log("pagemap.cpp:refresh","stat error");
                    continue;
                }
                fatt=tst.st_mtime;
                if((*base)[j].timestamp!=tst.st_mtime)
                {
                    toref=true;
                }
            }
        }
        if(loaded&&toref)
        {
            if(is_dotnhp(files[i],files[i].size()))
            {
                SDL_mutexP(http::mtx_exec);
                SDL_mutexP(http::mtx_exec2);

                struct stat st2;
dch:
                if(stat("/tmp/nativehttp",&st2)==0)
                {
                    if(S_ISDIR(st2.st_mode))
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

                                fwrite(out.c_str(),1,out.size(),tmf);
                                fclose(tmf);

                                nativehttp::data::superstring pgnam(files[i]);
                                pgnam.change(".nhp","nhp");
                                pgnam.pos=pgnam.str.size()-1;
                                string pname=pgnam.back_to("/");
                                srand(rand());
                                ostringstream tos;
                                tos << rand();
                                pname+=tos.str();

                                string command=cfg->get_var("cppcmp")+" -shared -fPIC "+cfg->get_var("flags")+" -o /tmp/nativehttp/nhpage_"+pname+".so /tmp/nativehttp/tmp.cpp";

                                bool shw=(cfg->get_int("cmpout")==1);

                                if(shw)
                                {
                                    nativehttp::server::log("Compile ",files[i]);
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

                                if(shw)nativehttp::server::log("REFRESH@pagemap.cpp","/tmp/nativehttp/nhpage_"+pname+".so");

                                ((nativepage*)(*base)[pgi].data)->handle = dlopen(("/tmp/nativehttp/nhpage_"+pname+".so").c_str(), RTLD_NOW|RTLD_LOCAL);
                                if(!((nativepage*)(*base)[pgi].data)->handle)
                                {
                                    nativehttp::server::log("ERROR@pagemap.cpp","can't open shared file: "+files[i]);
                                }
                                else
                                {



                                    ((nativepage*)(*base)[pgi].data)->onload = (nativehttp::data::Tonload) dlsym(((nativepage*)(*base)[pgi].data)->handle,"onload");
                                    ((nativepage*)(*base)[pgi].data)->page = (nativehttp::data::Tpage) dlsym(((nativepage*)(*base)[pgi].data)->handle,"page");
                                    if(!((nativepage*)(*base)[pgi].data)->onload||!((nativepage*)(*base)[pgi].data)->page)
                                    {
                                        nativehttp::server::log("ERROR@pagemap.cpp","loading native symbols failed: "+files[i]);
                                        dlclose(((nativepage*)(*base)[pgi].data)->handle);
                                    }
                                    else
                                    {

                                        for(unsigned int j=0;j<uris.size();j++)
                                        {
                                            if(uris[i].sid==pgi)
                                            {
                                                delete[] uris[i].u;
                                                uris.erase(uris.begin()+i);
                                            }
                                        }

                                        acp=pgi;
                                        int initstate = (*((nativepage*)(*base)[pgi].data)->onload)();
                                        if(initstate!=1)
                                        {
                                            nativehttp::server::logid(initstate,"WARNING@pagemap.cpp","invalid init state: "+files[i]);
                                            dlclose(((nativepage*)(*base)[pgi].data)->handle);
                                        }
                                        else
                                        {
                                            nativehttp::data::superstring pgac(files[i]);
                                            string furi='/'+pgac.from(d);
                                            char* tfu=new char[furi.size()+1];
                                            memcpy(tfu,furi.c_str(),furi.size());
                                            tfu[furi.size()]='\0';
                                            urimp tmu={tfu,pgi};
                                            uris.push_back(tmu);

                                            (*base)[pgi].type=page_native;
                                            (*base)[pgi].timestamp=fatt;
                                            nativehttp::server::log("RELOAD@pagemap.cpp","succes[2]: "+files[i]);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                nativehttp::server::log("ERROR@pagemap.cpp","can't open: "+files[i]);
                                fclose(tmf);
                            }
                        }
                        else
                        {
                            nativehttp::server::log("ERROR@pagemap.cpp","can't create /tmp/nativehttp/tmp.cpp");
                        }
                    }
                    else
                    {
                        nativehttp::server::log("ERROR@pagemap.cpp","/tmp/nativehttp IS NOT A DIRECTORY");
                    }
                }
                else
                {
                    mkdir("/tmp/nativehttp", 0755);
                    goto dch;
                }

                SDL_mutexV(http::mtx_exec);
                SDL_mutexV(http::mtx_exec2);
            }
            else if(is_dotso(files[i],files[i].size()))
            {
                SDL_mutexP(http::mtx_exec);
                SDL_mutexP(http::mtx_exec2);

                dlclose(((nativepage*)(*base)[pgi].data)->handle);
                ((nativepage*)(*base)[pgi].data)->handle = dlopen(files[i].c_str(), RTLD_NOW|RTLD_LOCAL);
                if(!((nativepage*)(*base)[pgi].data)->handle)
                {
                    nativehttp::server::log("ERROR@pagemap.cpp","can't open shared file: "+files[i]);
                }
                else
                {

                    for(unsigned int j=0;j<uris.size();j++)
                    {
                        if(uris[i].sid==pgi)
                        {
                            delete[] uris[i].u;
                            uris.erase(uris.begin()+i);
                        }
                    }

                    ((nativepage*)(*base)[pgi].data)->onload = (nativehttp::data::Tonload) dlsym(((nativepage*)(*base)[pgi].data)->handle,"onload");
                    ((nativepage*)(*base)[pgi].data)->page = (nativehttp::data::Tpage) dlsym(((nativepage*)(*base)[pgi].data)->handle,"page");
                    if(!((nativepage*)(*base)[pgi].data)->onload||!((nativepage*)(*base)[pgi].data)->page)
                    {
                        nativehttp::server::log("ERROR@pagemap.cpp","loading native symbols failed: "+files[i]);
                        dlclose(((nativepage*)(*base)[pgi].data)->handle);
                    }
                    else
                    {
                        acp=pgi;
                        int initstate = (*((nativepage*)(*base)[pgi].data)->onload)();
                        if(initstate!=1)
                        {
                            nativehttp::server::logid(initstate,"WARNING@pagemap.cpp","invalid init state: "+files[i]);
                            dlclose(((nativepage*)(*base)[pgi].data)->handle);
                        }
                        else
                        {
                            nativehttp::data::superstring pgac(files[i]);
                            string furi='/'+pgac.from(d);
                            char* tfu=new char[furi.size()+1];
                            memcpy(tfu,furi.c_str(),furi.size());
                            tfu[furi.size()]='\0';
                            urimp tmu={tfu,pgi};

                            uris.push_back(tmu);

                            (*base)[pgi].type=page_native;
                            (*base)[pgi].timestamp=fatt;
                            nativehttp::server::log("RELOAD@pagemap.cpp","succes[1]: "+files[i]);
                        }
                    }

                }



                SDL_mutexV(http::mtx_exec);
                SDL_mutexV(http::mtx_exec2);

            }
        }
        else if(!loaded)
        {
            if(is_dotso(files[i],files[i].size()))
            {

            }
            else if(is_dotnhp(files[i],files[i].size()))
            {

            }
            else
            {
                page tmp;
                struct stat tst;
                int rst = stat(files[i].c_str(), &tst);
                if(rst != 0)
                {
                    nativehttp::server::log("pagemap.cpp:init","stat error");
                    continue;
                }
                tmp.timestamp=tst.st_mtime;
                tmp.file=new char[files[i].size()+1];
                memcpy(tmp.file,files[i].c_str(),files[i].size());
                tmp.file[files[i].size()]='\0';

                tmp.type=page_file;
                tmp.data=new char[files.size()+1];
                memcpy(tmp.data,files[i].c_str(),files[i].size());
                ((char*)tmp.data)[files[i].size()]='\0';
                (*base).push_back(tmp);
                nativehttp::data::superstring pgac(files[i]);

                string furi='/'+pgac.from(d);
                char* tfu=new char[furi.size()+1];
                memcpy(tfu,furi.c_str(),furi.size());
                tfu[furi.size()]='\0';

                urimp tmu={tfu,int((*base).size())-1};
                uris.push_back(tmu);
                nativehttp::server::log("RUNLOAD@pagemap.cpp","succes");
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
            nativehttp::server::log("pagemap.cpp:init","stat error");
            continue;
        }
        tmp.timestamp=tst.st_mtime;
        tmp.file=new char[files[i].size()+1];
        memcpy(tmp.file,files[i].c_str(),files[i].size());
        tmp.file[files[i].size()]='\0';

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
                ntm->onload = (nativehttp::data::Tonload) dlsym(ntm->handle,"onload");
                ntm->page = (nativehttp::data::Tpage) dlsym(ntm->handle,"page");
                if(!ntm->onload||!ntm->page)
                {
                    cout << "error loading native symbols: "<<files[i].c_str()<<endl;
                    dlclose(ntm->handle);
                    delete ntm;
                }
                else
                {
                    acp=base->size();
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
                        base->push_back(tmp);
                        nativehttp::data::superstring pgac(files[i]);
                        string furi='/'+pgac.from(d);
                        char* tfu=new char[furi.size()+1];
                        memcpy(tfu,furi.c_str(),furi.size());
                        tfu[furi.size()]='\0';

                        urimp tmu={tfu,int(base->size())-1};
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
                            nativehttp::data::superstring pgnam(files[i]);
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


                            nativepage* ntm = new nativepage;
                            if(shw)cout << "file "<<"/tmp/nativehttp/nhpage_"+pname+".so"<<endl;
                            ntm->handle = dlopen(("/tmp/nativehttp/nhpage_"+pname+".so").c_str(), RTLD_NOW|RTLD_LOCAL);
                            if(!ntm->handle)
                            {
                                cout << "can't open compiled nhp file: "<<("/tmp/nativehttp/nhpage_"+pname+".so").c_str()<<": "<<dlerror()<<endl;
                                delete ntm;
                                continue;
                            }
                            else
                            {
                                ntm->onload = (nativehttp::data::Tonload) dlsym(ntm->handle,"onload");
                                ntm->page = (nativehttp::data::Tpage) dlsym(ntm->handle,"page");
                                if(!ntm->onload||!ntm->page)
                                {
                                    cout << "error loading native symbols: "<<("/tmp/nativehttp/nhpage_"+pname+".so").c_str()<<endl;
                                    dlclose(ntm->handle);
                                    delete ntm;
                                }
                                else
                                {
                                    acp=base->size();
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
                                        base->push_back(tmp);
                                        nativehttp::data::superstring pgac(files[i]);
                                        string furi='/'+pgac.from(d);
                                        char* tfu=new char[furi.size()+1];
                                        memcpy(tfu,furi.c_str(),furi.size());
                                        tfu[furi.size()]='\0';

                                        urimp tmu={tfu,int(base->size())-1};
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
            tmp.data=new char[files.size()+1];
            memcpy(tmp.data,files[i].c_str(),files[i].size());
            ((char*)tmp.data)[files[i].size()]='\0';
            base->push_back(tmp);
            nativehttp::data::superstring pgac(files[i]);

            string furi='/'+pgac.from(d);
            char* tfu=new char[furi.size()+1];
            memcpy(tfu,furi.c_str(),furi.size());
            tfu[furi.size()]='\0';

            urimp tmu={tfu,int(base->size())-1};
            uris.push_back(tmu);
        }
    }
}

void page_mapper::adduri(string u,bool top)
{
    char* cu=new char[u.size()+1];
    memcpy(cu,u.c_str(),u.size());
    cu[u.size()]='\0';

    urimp tu= {cu,acp};
    if(top)
    {
        uris.push_front(tu);
    }
    else
    {
        uris.push_back(tu);
    }
}

page page_mapper::by_uri(const char* u)
{
    vector<int>dn(uris.size(),0);
    page t= {-1,NULL};
    for(unsigned int i=0; i<strlen(u); i++)
    {
        for(unsigned int j=0; j<dn.size(); j++)
        {
            if(dn[j]!=-1)
            {
                if(uris[j].u[dn[j]]==u[i]||uris[j].u[dn[j]]=='*')
                {
                    if(strlen(uris[j].u)==unsigned(dn[j])+1&&strlen(uris[j].u)==unsigned(dn[j])+1&&strlen(u)==strlen(uris[j].u))
                    {
                        return (*base)[uris[j].sid];
                    }
                    else if(strlen(uris[j].u)==unsigned(dn[j])+1&&uris[j].u[dn[j]]=='*')
                    {
                        t=(*base)[uris[j].sid];
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


