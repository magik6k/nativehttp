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

#include "../pagemap.h"
#include <iostream>


void page_mapper::load_nhp(page& tmp, const char* f, string dir)
{
    FILE* nhpf=NULL;
    FILE* tmf=NULL;

    if(nhp_prepare_env(f,nhpf,tmf))
    {
        load_so(tmp,nhp_compile(f,nhpf,tmf).c_str(),dir,f);
    }
}

string page_mapper::nhp_compile(const char* f,FILE*& nhpf, FILE*& tmf)
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
    nativehttp::data::superstring pgnam(f);
    pgnam.change(".nhp","nhp");
    pgnam.pos=pgnam.str.size()-1;
    string pname=pgnam.back_to("/");
    srand(rand());
    pname+=its(rand());
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
        cout << "Compile "<<f<<":"<<endl;
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
    return ("/tmp/nativehttp/nhpage_"+pname+".so");

}

bool page_mapper::nhp_prepare_env(const char* f, FILE*& nhpf, FILE*& tmf)
{
    struct stat st;
drch:
    if(stat("/tmp/nativehttp",&st)==0)
    {
        if(S_ISDIR(st.st_mode))
        {
            tmf=fopen("/tmp/nativehttp/tmp.cpp","w");
            if(tmf!=NULL)
            {
                nhpf=fopen(f,"r");
                if(nhpf!=NULL)
                {
                    return true;
                }
                else
                {
                    cout << "can't open "<<f<<endl;
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
    return false;
}

