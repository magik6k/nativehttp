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

namespace http
{
namespace bsd
{

int findfreesock()
{
    for(int i=0;i<http::maxConnections;i++)
    {
        if(http::connected[i]==-1)return i;
    }
    return -1;
}

void* listener(void* unused)
{
    while(1)
    {
        struct sockaddr_in sock_addr;
        socklen_t sock_alen;
        SOCKET tmp;

        sock_alen = sizeof(sock_addr);

        tmp=accept(http::server,(struct sockaddr*)&sock_addr,&sock_alen);

        if (tmp==-1)
        {
            SDL_Delay(1);
            continue;
        }

        int flags = fcntl(tmp, F_GETFL, 0);
        fcntl(tmp, F_SETFL, flags & ~O_NONBLOCK);

        int fs=http::bsd::findfreesock();
        if(fs==-1)
        {
            close(tmp);
            SDL_Delay(10);
            continue;
        }
        http::connected[fs]=tmp;

    }
    return NULL;
}
}
}
