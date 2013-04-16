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
#include <iostream>

namespace data
{
namespace session
{

sbmain::sbmain()
{
    keys=NULL;
    fileds=0;
}

void sbmain::alloc_keys()
{
    size_t newsize=fileds+3;//alloc 3 more keys
    nativehttp::data::cfgfil** newkeys=new nativehttp::data::cfgfil*[newsize];
    size_t i;
    for(i=0;i<fileds;i++)
    {
        newkeys[i]=keys[i];
    }
    for(;i<newsize;i++)
    {
        newkeys[i]=NULL;
    }
    nativehttp::data::cfgfil** oks=keys;
    keys=newkeys;
    if(oks)delete[] oks;
    fileds=newsize;
}

size_t sbmain::getfreekeyid()
{
    for(size_t i=0;i<fileds;i++)
    {
        if(!keys[i])
        {
            return i;
        }
    }
    this->alloc_keys();
    return this->getfreekeyid();
}

size_t sbmain::createkey(string kn)
{
    size_t kid=getfreekeyid();
    keys[kid]=new nativehttp::data::cfgfil;
    keys[kid]->name=kn;
    keys[kid]->cont="";
    return kid;
}

size_t sbmain::getkeyid(string kn)
{
    for(size_t i=0;i<fileds;i++)
    {
        if(keys[i])
        {
            if(keys[i]->name==kn)
            {
                return i;
            }
        }
    }
    return this->createkey(kn);
}

string& sbmain::getkey(string kn)
{
    size_t ki=this->getkeyid(kn);
    return keys[ki]->cont;
}

void sbmain::setkey(string kn, string kv)
{
    this->getkey(kn)=kv;
}

}
}
