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
#include "nativehttp.h"
#include "protocol.h"
#include <string.h>
pagedata::pagedata(string s)
{
    data=new char[s.size()];
    size=s.size();
    memcpy(data,s.c_str(),size);
}
void pagedata::operator()(string s)
{
    if(data)delete[]data;
    data=new char[s.size()];
    size=s.size();
    memcpy(data,s.c_str(),size);
}
pagedata::pagedata()
{
    data=NULL;
    size=0;
}

extern "C" void attach_uri(string uri,bool top)
{
    pmap.adduri(uri,top);
}

extern "C" string version()
{
    return "NativeHTTP Alpha 1";
}

extern "C" sstat *get_stats()
{
    return 0;
}

extern "C" void log(string lname, string value)
{
    if(!deamonized)cout << "["<<lname.c_str()<<"]"<<value.c_str()<<endl;
}

extern "C" void logid(int id, string lname, string value)
{
    if(!deamonized)cout << "["<<lname.c_str()<<"]("<<id<<")"<<value.c_str()<<endl;
}
