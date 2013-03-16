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

#include "../nativehttp.h"
#include "init.h"
#include "data.h"
#include "executor.h"
#include "newclient.h"

namespace http
{
    void sdlinit()
    {
        SDL_Init(SDL_INIT_TIMER);
        SDLNet_Init();
    }
    void datainit()
    {
        http::maxConnections=cfg->get_int("maxconnections");
        http::connected=new TCPsocket[http::maxConnections];
        http::ulock=new bool[http::maxConnections];
        for(int i=0;i<http::maxConnections;i++)
        {
            http::connected[i]=NULL;
            http::ulock[i]=false;
        }
        http::CSet=SDLNet_AllocSocketSet(http::maxConnections);
        http::Nexec=cfg->get_int("exec_theards");
        http::execUnits=new http::Sexecutor[http::Nexec];
        http::mExecQ=cfg->get_int("maxexecutionqueue");
        http::headers::alivetimeout=cfg->get_var("normal_keep")+"\r\n";
    }
    void executorinit()
    {
        for(int i=0;i<http::Nexec;i++)
        {
            http::execUnits[i].state=-1;
            http::execUnits[i].etheard=SDL_CreateThread(http::executor,&(http::execUnits[i]));
        }
    }
    void netstart()
    {
        IPaddress tmp;
        SDLNet_ResolveHost(&tmp,NULL,cfg->get_int("port"));
        http::server=SDLNet_TCP_Open(&tmp);
        if(!http::server)
        {
            printf("INIT: %s\n", SDLNet_GetError());
            exit(1);
        }
    }
    void startsystem()
    {
        SDL_CreateThread(http::newclient,NULL);
    }
}

