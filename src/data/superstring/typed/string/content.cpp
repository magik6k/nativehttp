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


nativehttp::data::superstring& nativehttp::data::superstring::change(string from, string to)
{
	string out;
	bool ls = lck;
	if (lck)
	{
		lck = false;
	}
	while (pos < str.size())
	{
		string ctg = this->to(from);
		if (!ctg.empty())
		{
			out += ctg;
			if (!rae)
			{
				out += to;
			}
		}
	}
	lck = ls;
	str = out;
	SSLOCK;
	return *this;
}

nativehttp::data::superstring& nativehttp::data::superstring::remove(string from, string to)
{
	string out=this->get_begin(pos);
	bool ls = lck;
	if (lck)
	{
		lck = false;
	}
	while (pos < str.size())
	{

		string ctg = this->to(from);
		if (!ctg.empty())
		{
			out += ctg;
        }
        if (!rae)
        {
            this->to(to);
        }

	}
	lck = ls;
	str = out;
	SSLOCK;
	return *this;
}

nativehttp::data::superstring& nativehttp::data::superstring::remove(string s)
{
	string out=this->get_begin(pos);
	bool ls = lck;
	if (lck)
	{
		lck = false;
	}
	while (pos < str.size())
	{
		string ctg = this->to(s);
		out += ctg;
	}
	lck = ls;
	str = out;
	SSLOCK;
	return *this;
}
