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
#include "sender.h"
#include "error.h"
#include "manager.h"

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
    http::maxPost=cfg->get_int("max_post");

    http::connected=new TCPsocket[http::maxConnections];
    http::ulock=new bool[http::maxConnections];
    for(int i=0; i<http::maxConnections; i++)
    {
        http::connected[i]=NULL;
        http::ulock[i]=false;
    }
    http::CSet=SDLNet_AllocSocketSet(http::maxConnections);
    http::Nexec=cfg->get_int("exec_theards");
    http::Nsend=cfg->get_int("send_theards");
    http::execUnits=new http::Sexecutor[http::Nexec];
    http::mExecQ=cfg->get_int("maxexecutionqueue");
    http::headers::alivetimeout=cfg->get_var("normal_keep")+"\r\n";
    http::theard_sd=new SDL_Thread*[http::Nsend];

    http::mtx_exec2=SDL_CreateMutex();
    http::mtx_exec=SDL_CreateMutex();
    http::mtx_snd=SDL_CreateMutex();

    http::manager::rate=cfg->get_int("managerrate");
    http::manager::postto=cfg->get_int("posttimeout");
    http::manager::execto=cfg->get_int("exectimeout");
    http::manager::apr=cfg->get_int("autopagerefresh");
    http::manager::fileloc=cfg->get_var("files_location");

    http::error::e400=http::error::load_error(cfg->get_var("error400"),"400 Bad Request");
    http::error::e403=http::error::load_error(cfg->get_var("error403"),"403 Forbidden");
    http::error::e404=http::error::load_error(cfg->get_var("error404"),"404 Not Found");
    http::error::e501=http::error::load_error(cfg->get_var("error501"),"501 Not Implemented");
    http::error::e505=http::error::load_error(cfg->get_var("error505"),"505 HTTP Version Not Supported");

}
void executorinit()
{
    for(int i=0; i<http::Nexec; i++)
    {
        http::execUnits[i].state=-1;
        http::execUnits[i].in=0;
        http::execUnits[i].id=i;
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
    http::theard_nc=SDL_CreateThread(http::newclient,NULL);
    for(int i=0;i<http::Nsend;i++)http::theard_sd[i]=SDL_CreateThread(http::sender::sender,NULL);
    http::theard_mg=SDL_CreateThread(http::manager::manager,NULL);
}


}

