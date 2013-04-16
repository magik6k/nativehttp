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
#include "session.h"

namespace data
{
namespace session
{
sstg storage;
time_t sess_life;

sstg::sstg()
{
    scount=0;
    data=NULL;
}

bool sstg::cksess(size_t id, unsigned int scd)
{
    if(scount<=id)return false;
    if(data[id].started==0)return false;
    return data[id].svid==scd;
}

void sstg::allocsessions()
{
    size_t nsize=scount+3;
    session* tsp=new session[nsize];
    if(!tsp)return;
    size_t i;
    for(i=0;i<scount;i++)
    {
        tsp[i].started=data[i].started;
        tsp[i].svid=data[i].svid;
        tsp[i].data=data[i].data;
    }
    for(;i<nsize;i++)
    {
        tsp[i].started=0;
        tsp[i].svid=0;
    }
    delete[] data;
    data=tsp;
    scount=nsize;
}

size_t sstg::findfreesess()
{
    for(size_t i=0;i<scount;i++)
    {
        if(data[i].started==0)return i;
    }
    this->allocsessions();
    return this->findfreesess();
}

size_t sstg::mksess(unsigned int scd)
{
    size_t si=this->findfreesess();
    data[si].svid=scd;
    data[si].started=time(0);
    return si;
}

sbmain& sstg::gtsess(size_t id)
{
    return data[id].data;
}



}
}
