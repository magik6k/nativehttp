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
#include "stat.h"
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
    if(time(0)-lastHrlFlp>=5)
    {
        lastHrlFlp+=5;
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

void onrecv(unsigned long dlen)
{
    if(toggle&&transfer)
    {
        dlbytes+=dlen;
        hrl_dl[0]+=dlen;
    }
}
void onsend(unsigned long ulen)
{
    if(toggle&&transfer)
    {
        ulbytes+=ulen;
        hrl_ul[0]+=ulen;
    }
}
void onconnect()
{
    if(toggle)
    {
        connections++;
        hrl_connections[0]++;
    }
}

void onpost()
{
    if(toggle&&method)
        post++;
}
void onget()
{
    if(toggle&&method)
        get++;
}
void onhit()
{
    if(toggle&&hitlog)
    {
        hits++;
        hrl_hits[0]++;
    }
}
}
}

namespace nativehttp
{
namespace stat
{
extern "C" bool stats()
{
    return http::statdata::toggle;
}
extern "C" bool transfer_stats()
{
    return http::statdata::transfer;
}
extern "C" bool hit_stats()
{
    return http::statdata::hitlog;
}
extern "C" bool method_stats()
{
    return http::statdata::method;
}

extern "C" unsigned long hits()
{
    return http::statdata::hits;
}
extern "C" unsigned long connections()
{
    return http::statdata::connections;
}
extern "C" unsigned long uploaded()
{
    return http::statdata::ulbytes;
}
extern "C" unsigned long downloaded()
{
    return http::statdata::dlbytes;
}

extern "C" unsigned long get_requests()
{
    return http::statdata::get;
}
extern "C" unsigned long post_requests()
{
    return http::statdata::post;
}

extern "C" unsigned long hourly_length()
{
    return http::statdata::hourlylen;
}

extern "C" unsigned long hour_hits(unsigned long hid)
{
    if(hid<http::statdata::hourlylen)
    {
        return http::statdata::hrl_hits[hid];
    }
    return 0;
}
extern "C" unsigned long hour_connections(unsigned long hid)
{
    if(hid<http::statdata::hourlylen)
    {
        return http::statdata::hrl_connections[hid];
    }
    return 0;
}
extern "C" unsigned long hour_upload(unsigned long hid)
{
    if(hid<http::statdata::hourlylen)
    {
        return http::statdata::hrl_ul[hid];
    }
    return 0;
}
extern "C" unsigned long hour_download(unsigned long hid)
{
    if(hid<http::statdata::hourlylen)
    {
        return http::statdata::hrl_dl[hid];
    }
    return 0;
}
}
}
