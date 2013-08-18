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
#include "utils/memory.h"
#include <stdio.h>
#include <cstring>

#define SSLOCK if(lck)pos=lpos

namespace um = utils::memory;

string nativehttp::data::superstring::from(unsigned int sp)
{
    string rt;
    pos = sp;
    while (pos < str.size())
	{
		rt += str[pos];
		pos++;
	}
	SSLOCK;
	return rt;
}

string nativehttp::data::superstring::to(unsigned int ep)
{
    string rt;
    if(ep>=pos)
    {
        while (pos < str.size() && pos < ep)
        {
            rt += str[pos];
            pos++;
        }
	}
	else
	{
        string trt;
        while(pos >= ep && pos != 4294967295)
        {
            trt += str[pos];
            pos--;
        }
        rt.reserve(trt.size());
        for(unsigned int i=0;i<trt.size();i++)
        {
            rt += trt[trt.size()-i-1];
        }
        trt.clear();
	}
	SSLOCK;
	return rt;
}

nativehttp::data::superstring& nativehttp::data::superstring::remove(unsigned int from, unsigned int to)
{

    char *out = um::alloc<char>(str.size()-(to-from));
    if(from > 0)memcpy(out,str.c_str(),from);
    if(to+1 < str.size())memcpy(out+from,str.c_str()+to,str.size()-(to+1));
    out[str.size()-(to-from+1)]='\0';
    str = out;
    delete[] out;
    if(pos > str.size())pos = str.size();
    return *this;
}
