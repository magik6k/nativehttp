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
#include "http/data.h"
#include "protocol.h"

namespace http
{
namespace bsd
{
void init()
{
    struct sockaddr_in sock_addr;
    http::server=socket(AF_INET, SOCK_STREAM, 0);

    if(http::server==INVALID_SOCKET)
    {
		nativehttp::server::log("start.cpp@bsd","Couldn't create socket");
		return;
	}

	memset(&sock_addr, 0, sizeof(sock_addr));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(cfg->get_int("port"));

    {
        int y = 1;
        setsockopt(http::server, SOL_SOCKET, SO_REUSEADDR, (char*)&y, sizeof(y));
    }

    if (bind(http::server,(struct sockaddr*)&sock_addr,sizeof(sock_addr))==-1)
    {
        nativehttp::server::log("start.cpp@bsd","Couldn't bind to local port");
        return;
    }

    if (listen(http::server, cfg->get_int("max_waiting_connectons"))==-1)
    {
        nativehttp::server::log("start.cpp@bsd","Couldn't listen to local port");
        return;
    }

    fcntl(http::server, F_SETFL, O_NONBLOCK);
}
}
}
