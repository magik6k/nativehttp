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
#include <iostream>
#include "protocol.h"
#include "http/init.h"
#include "http/in.h"
#include "http/manager.h"
#include "data/queue.h"
#include <signal.h>

page_mapper pmap;
mimec *mime=NULL;
nativehttp::data::Ccfg *cfg;
string charset;

using namespace std;

int main(int argc, char *argv[])
{
    cout << "NativeHTTP DEV\nBy magik6000\nfor more info visit nativehttp.org\n";
#ifdef NHDBG
    size_t bim=(getacmem());
    cout <<"[DBG:main.cpp@core]before-init mem: "<<bim<<"b\n";
#endif
    bool dmnz=false;

    for(int i=1; i<argc; i++)
    {
        switch(argv[i][1])
        {
        case 'd':
            dmnz=true;
        }
    }
    cout << "pre-init\n";
    signal(SIGSEGV,http::manager::sig);
    signal(SIGABRT,http::manager::sig);
    cfg = new nativehttp::data::Ccfg("/etc/nativehttp/config.cfg");
    charset = cfg->get_var("charset");
    default_mime = cfg->get_var("default_content_type");
    if(cfg->get_int("instantso"))
    {
        setbuf(stdout, NULL);
    }
    mime = new mimec;
    nativehttp::server::log("INIT","Mapping server directory, loading native pages");
    pmap.preinit();
    pmap.page_mapper_init(cfg->get_var("files_location"));
    if(dmnz)deamonize();
    nativehttp::server::log("INIT","Starting HTTP system");


    http::sdlinit();
    http::datainit();
    http::executorinit();
    http::netstart();
    http::initstat();
    http::startsystem();

    nativehttp::server::log("INIT","Ready");


    http::reciver();

    return 0;
}


string its(int n)
{
    string tmp, ret;
    if(n < 0)
    {
        ret="-";
        n=-n;
    }
    do
    {
        tmp+=n%10+48;
        n-=n%10;
    }
    while(n/=10);
    for(int i=tmp.size()-1; i>=0; i--)
        ret+=tmp[i];
    return ret;
}
