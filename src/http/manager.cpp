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
#include "../protocol.h"
#include "manager.h"
#include "data.h"
#include "executor.h"
#include "sender.h"

namespace http
{
namespace manager
{

void sig(int sig)
{
    nativehttp::server::logid(sig,"manager.cpp","SIG");
    if(sig==11)
    {
        /*
        log("manager.cpp","CRITICAL ERROR (SIGSEGV)");
        log("manager.cpp","trying to rescue, restarting http system");
        log("restarting","killing theards");
        SDL_KillThread(http::theard_sd);
        SDL_KillThread(http::theard_mg);
        SDL_KillThread(http::theard_mg);
        log("restarting","killing execution units");
        for(int i=0;i<http::Nexec;i++)
        {
            SDL_KillThread(http::execUnits[i].etheard);
        }
        log("restarting","disconnectig users");


        log("restarting","cleaning");

        */
        if(SDL_GetThreadID(http::theard_mg)==SDL_ThreadID())
        {
            cout << "manager crash\n";
        }
        if(SDL_GetThreadID(http::theard_nc)==SDL_ThreadID())
        {
            cout << "ACT module crash\n";
        }
        for(int i=0; i<http::Nsend; i++)
        {
            if(SDL_GetThreadID(http::theard_sd[i])==SDL_ThreadID())
            {
                nativehttp::server::log("WARNING","Sender theard crashed, rescuing");
                SDL_mutexV(http::mtx_snd);//may be needed
                SDL_Thread* kth=http::theard_sd[i];
                http::theard_sd[i]=SDL_CreateThread(http::sender::sender,NULL);
                SDL_KillThread(kth);
            }
        }

        for(int i=0; i<http::Nexec; i++)
        {
            if(SDL_GetThreadID(http::execUnits[i].etheard)==SDL_ThreadID())
            {
                nativehttp::server::logid(i,"WARNING","Execution theard crashed, rescuing");
                http::execUnits[i].state=-1;
                http::execUnits[i].in=0;
                SDL_mutexV(http::mtx_exec2);
                SDL_mutexV(http::mtx_exec);
                SDL_Thread* kth=http::execUnits[i].etheard;
                http::execUnits[i].etheard=SDL_CreateThread(http::executor,&(http::execUnits[i]));
                SDL_KillThread(kth);

            }
        }

    }
}

int manager(void* unused)
{
    if(http::manager::rate==-1)return 0;
    while(1)
    {
        http::manager::timeouts();
        http::manager::fsrefresh();
        http::manager::wait();
    }
    return 1;
}

void fsrefresh()
{
    if(http::manager::apr)pmap.refresh(http::manager::fileloc);
}

void timeouts()
{
    for(int i=0; i<http::Nexec; i++)
    {
        if(http::execUnits[i].state!=-1)
        {
            switch(http::execUnits[i].in)
            {
            case 1:
            case 3:
                if(time(0)-http::execUnits[i].in>http::manager::postto&&postto!=-1)
                {
                    if(http::execUnits[i].fd1)delete (nativehttp::data::postgetdata*)http::execUnits[i].fd1;
                    if(http::execUnits[i].fd2)delete (nativehttp::data::postgetdata*)http::execUnits[i].fd2;
                    SDL_KillThread(http::execUnits[i].etheard);
                    http::execUnits[i].in=0;
                    http::execUnits[i].state=-1;
                    http::execUnits[i].fd1=NULL;
                    http::execUnits[i].fd2=NULL;
                    SDL_CreateThread(http::executor,&(http::execUnits[i]));
                }
                break;
            case 2:
                if(time(0)-http::execUnits[i].in>http::manager::postto&&execto!=-1)
                {

                    nativehttp::server::logid(i,"manager.cpp","PAGE EXECUTION TIMEOUT");
                    if(http::execUnits[i].fd1)delete (nativehttp::data::postgetdata*)http::execUnits[i].fd1;
                    if(http::execUnits[i].fd2)delete (nativehttp::data::postgetdata*)http::execUnits[i].fd2;
                    SDL_KillThread(http::execUnits[i].etheard);
                    http::execUnits[i].in=0;
                    http::execUnits[i].state=-1;
                    http::execUnits[i].fd1=NULL;
                    http::execUnits[i].fd2=NULL;
                    SDL_CreateThread(http::executor,&(http::execUnits[i]));
                }
                break;
            }
        }
    }
}
void wait()
{
    SDL_Delay(http::manager::rate*1000);
}

}
}
