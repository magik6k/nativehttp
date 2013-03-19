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
#include "newclient.h"
#include "data.h"


namespace http
{
void disconnect(int i)
{
    if(http::connected[i])
    {
        SDLNet_TCP_DelSocket(http::CSet,http::connected[i]);
        TCPsocket td=http::connected[i];
        http::connected[i]=NULL;//safer to put null here, just to be sure, that it's not used until deleting it
        SDLNet_TCP_Close(http::connected[i]);
    }
}
int findfreeslot()
{
    for(int i=0; i<http::maxConnections; i++)
    {
        if(!connected[i])
        {
            return i;
        }
    }
    return -1;
}

int newclient(void* d)
{
    TCPsocket tclient=NULL;
    int toslot=-1;
    while(1)
    {
        if((tclient=SDLNet_TCP_Accept(server)))
        {
            if((toslot=findfreeslot())!=-1)
            {
                http::connected[toslot]=tclient;
                tclient=NULL;
                SDLNet_TCP_AddSocket(http::CSet,http::connected[toslot]);
            }
            else
            {
                SDLNet_TCP_Close(tclient);//full server
            }
        }
        else
        {
            SDL_Delay(1);
        }
    }
    return 0;
}
}
