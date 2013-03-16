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
#include "in.h"
#include "data.h"
#include "newclient.h"

namespace http
{

void unlockclient(int i)
{
    http::ulock[i]=false;
    SDLNet_TCP_AddSocket(http::CSet,http::connected[i]);
}

int reciver()
{
    int cr,ra;
    while(1)
    {
        cr=SDLNet_CheckSockets(http::CSet,0);
        if(cr!=0&&cr!=-1)
        {
            for(int i=0;i<http::maxConnections&&cr>0;i++)
            {
                if(http::connected[i]!=NULL&&!http::ulock[i])
                {
                    if(((SDLNet_GenericSocket)(http::connected[i]))->ready!=0)
                    {
                        cr--;
                        http::request trq;
                        trq.request.resize(HTTP_MAX_USER_HEADER_SIZE);
                        ra=SDLNet_TCP_Recv(http::connected[i],(void*)trq.request.c_str(),HTTP_MAX_USER_HEADER_SIZE);
                        if(ra>0)
                        {
                            logid(i,"in.cpp","Recived");
                            SDLNet_TCP_DelSocket(http::CSet,http::connected[i]);
                            http::ulock[i]=true;
                            trq.sender=http::connected[i];
                            trq.taken=false;
                            trq.uid=i;
                            http::toexec.push(trq);
                        }
                        else
                        {
                            http::disconnect(i);
                            logid(i,"in.cpp","User disconected");
                        }
                    }
                }
            }
        }
        else
        {
            SDL_Delay(1);
        }

    }
    return 1;
}

}