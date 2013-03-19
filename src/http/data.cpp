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
#include "data.h"

namespace http
{

int maxConnections=0;
unsigned int maxPost=0;

TCPsocket* connected=NULL;
SDLNet_SocketSet CSet=NULL;
TCPsocket server=NULL;
bool* ulock=NULL;

int Nexec;
Sexecutor* execUnits=NULL;

uint32_t mExecQ;

data::queue<request*>toexec;
data::queue<outdata>tosend;

SDL_mutex* mtx_exec2=NULL;
SDL_mutex* mtx_exec=NULL;
SDL_mutex* mtx_snd=NULL;

SDL_Thread* theard_nc=NULL;
SDL_Thread* theard_sd=NULL;
SDL_Thread* theard_mg=NULL;

namespace headers
{
string standard="Accept-Ranges: none\r\nAllow: GET, POST\r\nServer: NativeHTTP/0.1.0\r\n";
string alive="Connection: keep-alive\r\nKeep-Alive: timeout=";
string alivetimeout;
}

namespace error
{
pagedata e400;
pagedata e403;
pagedata e404;
pagedata e500;
pagedata e501;
pagedata e505;
}

namespace manager
{
int rate=0;
int postto=-1;
int execto=-1;
}

void request::free()
{
    delete request;
}

}



