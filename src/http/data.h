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
#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include "../nativehttp.h"
#include "../protocol.h"
#include <SDL/SDL_net.h>
#include <queue>
using namespace std;

#define HTTP_MAX_USER_HEADER_SIZE 8194 //orginal 2^13+2

namespace http
{

struct request
{
    TCPsocket sender;
    string request;
    bool taken;
    int uid;

    int method;
    bool http11;
    void* post;
};

struct outdata
{
    int uid;
    unsigned long size;
    char* data;
    bool fas;//free after sending
};

struct Sexecutor
{
    SDL_Thread* etheard;
    long int state;//time execution/post reciving started OR -1 when waiting or requies stability
    int in;//3 - kick, 2 - exec, 1 - post recv/kicki, 0-nothing

    void* fd1;
    void* fd2;
};

extern int maxConnections;
extern unsigned int maxPost;

extern TCPsocket* connected;
extern SDLNet_SocketSet CSet;
extern TCPsocket server;
extern bool* ulock;


extern int Nexec;
extern Sexecutor* execUnits;

extern uint32_t mExecQ;

extern queue<request>toexec;
extern queue<outdata>tosend;

namespace headers
{
extern string standard;
extern string alive;
extern string alivetimeout;
}

namespace error
{
extern pagedata e400;
extern pagedata e403;
extern pagedata e404;
extern pagedata e500;
extern pagedata e501;
extern pagedata e505;
}

namespace manager
{
extern int rate;
extern int postto;
extern int execto;
}

}

#endif // DATA_H_INCLUDED
