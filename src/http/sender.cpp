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
#include "sender.h"
#include "data.h"

namespace http
{
namespace sender
{
int sender(void* unused)
{
    int ts=0;
    while(1)
    {
        SDL_mutexP(http::mtx_snd);
        if(http::tosend.empty())
        {
            SDL_Delay(1);
            continue;
        }
        outdata proc=http::tosend.front(ts);
        if(ts==1)continue;
        http::tosend.pop();
        SDL_mutexV(http::mtx_snd);

        SDLNet_TCP_Send(http::connected[proc.uid],proc.data,proc.size);

        if(proc.fas)
        {
            delete[] proc.data;
        }
    }
    return 1;
}
void send(int uid, unsigned long datasize, char* data, bool free)
{
    outdata t= {uid,datasize,data,free};
    http::tosend.push(t);
}
void sendNow(int uid, unsigned long datasize, char* data, bool free)
{
    SDLNet_TCP_Send(http::connected[uid],data,datasize);
    if(free)
    {
        delete[] data;
    }
}
}
}


