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
#include <stdio.h>

#define SSLOCK if(lck)pos=lpos

int nativehttp::data::superstring::check(string sch)
{

	unsigned int left = sch.size();
	unsigned int tpos = pos;
	while (left > 0 && tpos < str.size())
	{
		if (str[tpos] == sch[sch.size() - left])
		{
			left--;
		}
		else
		{
			return 0;
		}
		tpos++;
	}


	if (left <= 0)
	{
		return 1;
	}
	return 0;
}

string nativehttp::data::superstring::get(unsigned int n)
{
    string rt;
    for(unsigned int i = pos;i<str.size()&&i-pos<n;i++)rt+=str[i];
    return rt;
}

string nativehttp::data::superstring::get_begin(unsigned int n)
{
    string rt;
    for(unsigned int i = 0;i<str.size()&&i<n;i++)rt+=str[i];
    return rt;
}

string nativehttp::data::superstring::get_end(unsigned int n)
{
    string rt;
    for(unsigned int i = str.size()-n;i<str.size();i++)rt+=str[i];
    return rt;
}
