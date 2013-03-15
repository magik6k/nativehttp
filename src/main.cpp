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

page_mapper pmap;
mimec *mime=NULL;
Ccfg *cfg;
string charset;

using namespace std;

int main(int argc, char *argv[])
{
    cout << "nativehttp DEV\nBy magik6000\nfor more info visit nativehttp.org\n";
    bool dmnz=false;

    for(int i=1;i<argc;i++)
    {
        switch(argv[i][1])
        {
            case 'd':
                dmnz=true;
        }
    }
    cfg = new Ccfg("/etc/nativehttp/config.cfg");
    charset = cfg->get_var("charset");
    //postmax = cfg->get_int("max_post");
    default_mime = cfg->get_var("default_content_type");
    if(cfg->get_int("instantso"))
    {
        setbuf(stdout, NULL);
    }
    mime = new mimec;
    pmap.page_mapper_init(cfg->get_var("files_location"));
    /*
    int logl=cfg->get_int("log_lenght");
    if(logl>0)
    {
        gstats.hourly=new int[logl];
        gstats.loghours=logl;
        for(int i=0;i<gstats.loghours-1;i++)
        {
            gstats.hourly[i]=0;
        }
    }
    */
    cout << "starting http system\n";

    http::sdlinit();
    http::datainit();
    http::executorinit();
    http::netstart();
    http::startsystem();

    cout << "Ready!\n";
    if(dmnz)deamonize();

    http::reciver();

    return 0;
}
