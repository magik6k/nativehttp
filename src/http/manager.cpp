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
#include "manager.h"
#include "data.h"
#include "executor.h"

namespace http
{
namespace manager
{
int manager(void* unused)
{
    if(http::manager::rate==-1)return 0;
    while(1)
    {
        http::manager::timeouts();
        http::manager::wait();
    }
    return 1;
}
void timeouts()
{
    for(int i=0;i<http::Nexec;i++)
    {
        if(http::execUnits[i].state!=-1)
        {
            switch(http::execUnits[i].in)
            {
                case 1:
                case 3:
                    if(time(0)-http::execUnits[i].in>http::manager::postto&&postto!=-1)
                    {
                        if(http::execUnits[i].fd1)delete (postgetdata*)http::execUnits[i].fd1;
                        if(http::execUnits[i].fd2)delete (postgetdata*)http::execUnits[i].fd2;
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
                        logid(i,"manager.cpp","PAGE EXECUTION TIMEOUT");
                        if(http::execUnits[i].fd1)delete (postgetdata*)http::execUnits[i].fd1;
                        if(http::execUnits[i].fd2)delete (postgetdata*)http::execUnits[i].fd2;
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
