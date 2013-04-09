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

#include "nativehttp.h"
#include "protocol.h"
#include "manager.h"
#include "data.h"
#include "executor.h"
#include "sender.h"
#include "stat.h"

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
        if(pthread_equal(*http::theard_mg,pthread_self())!=0)
        {
            cout << "manager crash\n";
        }
        if(pthread_equal(*http::theard_nc,pthread_self())!=0)
        {
            cout << "ANC module crash\n";
        }
        for(int i=0; i<http::Nsend; i++)
        {
            if(pthread_equal(*(http::theard_sd[i]),pthread_self())!=0)
            {
                nativehttp::server::log("WARNING","Sender theard crashed, rescuing");
                SDL_mutexV(http::mtx_snd);//may be needed
                pthread_t* kth=http::theard_sd[i];

                pthread_attr_t at;
                if(pthread_attr_init(&at)!=0) {
                    nativehttp::server::log("init.cpp@http","ERROR: Sender attr setup failed");
                    continue;
                }
                pthread_attr_setstacksize(&at,10*1024);//10kb for sender should enough
                http::theard_sd[i]=new pthread_t;
                int tmks=pthread_create(http::theard_sd[i], &at, http::sender::sender, NULL);
                if(tmks!=0)nativehttp::server::logid(i,"init.cpp","Sender failed to start");

                pthread_cancel(*kth);
                delete kth;

            }
        }

        for(int i=0; i<http::Nexec; i++)
        {
            if(pthread_equal(*(http::execUnits[i].etheard),pthread_self())!=0)
            {
                nativehttp::server::logid(i,"WARNING","Execution theard crashed, rescuing");
                http::execUnits[i].state=-1;
                http::execUnits[i].in=0;
                SDL_mutexV(http::mtx_exec2);
                SDL_mutexV(http::mtx_exec);

                pthread_t* kth=http::execUnits[i].etheard;

                pthread_attr_t at;
                if(pthread_attr_init(&at)!=0)
                {
                    nativehttp::server::log("init.cpp@http","ERROR: executor attr setup failed");
                }
                if(pthread_attr_setstacksize(&at,http::exec_heap_size)!=0)
                {
                    nativehttp::server::log("init.cpp@http","ERROR: Setting executor heap size failed");
                }


                pthread_t* tt=new pthread_t;
                int tms=pthread_create(tt,&at,http::executor,&(http::execUnits[i]));

                http::execUnits[i].etheard=tt;
                if(tms!=0)nativehttp::server::logid(i,"init.cpp","Executor failed to start");

                pthread_cancel(*kth);
                delete kth;//this will never happen
            }
        }

    }
}

void* manager(void* unused)
{
    if(http::manager::rate==-1)return 0;
    while(1)
    {
        http::manager::timeouts();
        http::manager::fsrefresh();
        http::statdata::manage();
        http::manager::wait();
    }
    return NULL;
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
                    //SDL_KillThread(http::execUnits[i].etheard);
                    http::execUnits[i].in=0;
                    http::execUnits[i].state=-1;
                    http::execUnits[i].fd1=NULL;
                    http::execUnits[i].fd2=NULL;
                    //SDL_CreateThread(http::executor,&(http::execUnits[i]));
                }
                break;
            case 2:
                if(time(0)-http::execUnits[i].in>http::manager::postto&&execto!=-1)
                {

                    nativehttp::server::logid(i,"manager.cpp","PAGE EXECUTION TIMEOUT");
                    if(http::execUnits[i].fd1)delete (nativehttp::data::postgetdata*)http::execUnits[i].fd1;
                    if(http::execUnits[i].fd2)delete (nativehttp::data::postgetdata*)http::execUnits[i].fd2;
                    //SDL_KillThread(http::execUnits[i].etheard);
                    http::execUnits[i].in=0;
                    http::execUnits[i].state=-1;
                    http::execUnits[i].fd1=NULL;
                    http::execUnits[i].fd2=NULL;
                    //SDL_CreateThread(http::executor,&(http::execUnits[i]));
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
