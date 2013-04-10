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
#include "../stat.h"
#include "nativehttp.h"

namespace http
{
namespace statdata
{
bool toggle;
bool transfer;
bool hitlog;
bool method;

long long hourlylen;

unsigned long hits;
unsigned long connections;

unsigned long ulbytes;
unsigned long dlbytes;

unsigned long get;
unsigned long post;

unsigned long* hrl_hits;
unsigned long* hrl_connections;
unsigned long* hrl_ul;
unsigned long* hrl_dl;

time_t lastHrlFlp;

void manage()
{
    if(time(0)-lastHrlFlp>=3600)
    {
        lastHrlFlp+=3600;
        for(long long i=hourlylen-2;i>=0;i--)
        {
            hrl_hits[i+1]=hrl_hits[i];
            hrl_connections[i+1]=hrl_connections[i];
            hrl_ul[i+1]=hrl_ul[i];
            hrl_dl[i+1]=hrl_dl[i];
        }
        hrl_hits[0]=0;
        hrl_connections[0]=0;
        hrl_ul[0]=0;
        hrl_dl[0]=0;
    }
}

}

}
