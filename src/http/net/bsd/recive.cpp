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
#include "../net.h"
#include "http/stat.h"

namespace http
{

void unlockclient(int i)
{
    http::ulock[i]=false;
}

namespace bsd
{
void reciver()
{
    while(1)
    {
        SOCKET mfd;
        fd_set set;
        struct timeval tv;
        int slr;

        tv.tv_sec = 0;
		tv.tv_usec = 0;

        for(int i=0;i<http::maxConnections;i++)
        {
            if(http::connected[i]!=-1&&!http::ulock[i])
            {
                if(mfd<http::connected[i])
                {
                    mfd=http::connected[i];
                }
            }
        }

        FD_ZERO(&set);

        for(int i=0;i<http::maxConnections;i++)
        {
            if(http::connected[i]!=-1&&!http::ulock[i])
            {
                FD_SET(http::connected[i], &set);
            }
        }

        slr = select(mfd+1, &set, NULL, NULL, &tv);

        for(int i=0;i<http::maxConnections&&(slr>0);i++)
        {
            if(http::connected[i]!=-1&&
                !http::ulock[i]&&
                FD_ISSET(http::connected[i], &set))
            {
                http::request* trq=new http::request();

                trq->request = new char[(HTTP_MAX_USER_HEADER_SIZE+1)];
                int ra = recv(http::connected[i], (char *) trq->request, HTTP_MAX_USER_HEADER_SIZE, 0);

                if(ra>0)
                {
                    ((char*)trq->request)[ra]='\0';
                    http::statdata::onrecv(ra);
                    nh::server::log("recive.cpp","recv");
                    trq->taken=-1;
                    trq->uid=i;
                    http::ulock[i]=true;
                    SDL_mutexP(http::mtx_exec);
                    http::toexec.push(trq);
                    SDL_mutexV(http::mtx_exec);
                }
                else
                {
                    delete[] trq->request;
                    delete trq;
                    nh::server::log("recive.cpp","disconnected?");
                    http::bsd::disconnect(i);
                }

            }
        }
        SDL_Delay(1);
    }
}
}
}
