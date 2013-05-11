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


string nativehttp::data::superstring::str_from_int(int in)
{
	string tmp, ret;
	if(in < 0)
	{
		ret = "-";
		in = -in;
	}
	do
	{
		tmp += in % 10 + 48;
		in -= in % 10;
	}
	while(in /= 10);
	for(int i = tmp.size() - 1; i >= 0; i--)
		ret += tmp[i];

	return ret;
}

nativehttp::data::superstring nativehttp::data::superstring::sst_from_int(int in)
{
	string tmp;
	nativehttp::data::superstring ret;
	if(in < 0)
	{
		ret.str = "-";
		in = -in;
	}
	do
	{
		tmp += in % 10 + 48;
		in -= in % 10;
	}
	while(in /= 10);
	for(int i = tmp.size() - 1; i >= 0; i--)
		ret.str += tmp[i];

	return ret;
}
