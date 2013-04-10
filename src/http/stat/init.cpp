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
#include "protocol.h"

namespace http
{
namespace statdata
{

void init()
{
#ifdef NHDBG
    size_t bm=getrsmem();
#endif
    http::statdata::toggle=cfg->get_int("statson");
    http::statdata::transfer=cfg->get_int("transfer_stats");
    http::statdata::hitlog=cfg->get_int("hits_stat");
    http::statdata::hourlylen=cfg->get_int("hourly_length");
    http::statdata::method=cfg->get_int("method_stats");

    http::statdata::lastHrlFlp=time(0);

    http::statdata::hrl_hits = new unsigned long[http::statdata::hourlylen];
    http::statdata::hrl_connections = new unsigned long[http::statdata::hourlylen];
    http::statdata::hrl_dl = new unsigned long[http::statdata::hourlylen];
    http::statdata::hrl_ul = new unsigned long[http::statdata::hourlylen];

    for(size_t i=0; i<http::statdata::hourlylen; i++)
    {
        http::statdata::hrl_hits[i]=0;
        http::statdata::hrl_connections[i]=0;
        http::statdata::hrl_dl[i]=0;
        http::statdata::hrl_ul[i]=0;
    }

    http::statdata::hits=0;
    http::statdata::connections=0;
    http::statdata::dlbytes=0;
    http::statdata::ulbytes=0;
    http::statdata::get=0;
    http::statdata::post=0;
#ifdef NHDBG
    cout <<"[DBG:init.cpp@http]Stat mem: "<<(getrsmem()-bm)/1024.f<<"kb\n";
#endif
}

}
}
