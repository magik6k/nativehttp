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


string nativehttp::data::superstring::to(string fend)
{
	rae = false;
	unsigned int c = 0;
	string rt;
	string prt;
	while(pos < str.size())
	{
		if(str[pos] == fend[c])
		{
			prt += str[pos];
			c++;
		}
		else
		{
			if(!prt.empty())
			{
				rt += prt;
				prt.clear();
				c = 0;
			}
			rt += str[pos];
		}
		pos++;
		if(c >= fend.size())
		{
			SSLOCK;
			return rt;
		}
	}
	rae = true;
	SSLOCK;
	return rt;
}

string nativehttp::data::superstring::from(string start)
{
	unsigned int c = 0;
	string rt;
	while(pos < str.size())
	{
		if(str[pos] == start[c])
		{
			c++;
		}
		else
		{
			if(c > 0)
			{
				c = 0;
			}
		}
		pos++;
		if(c >= start.size())
		{
			break;
		}
	}
	while(pos < str.size())
	{
		rt += str[pos];
		pos++;
	}
	SSLOCK;
	return rt;
}

string nativehttp::data::superstring::back_to(string fend)
{
	string nfe;
	for(int i = fend.size() - 1; i >= 0; i--)
	{
		nfe += fend[i];
	}
	unsigned int c = 0;
	string rt;
	string prt;
	while(pos > 0)
	{
		if(str[pos] == nfe[c])
		{
			prt += str[pos];
			c++;
		}
		else
		{
			if(!prt.empty())
			{
				rt += prt;
				prt.clear();
				c = 0;
			}
			rt += str[pos];
		}
		pos--;
		if(c >= nfe.size())
		{
			string ert;
			for(int i = rt.size() - 1; i >= 0; i--)
			{
				ert += rt[i];
			}
			SSLOCK;
			return ert;
		}
	}
	string ert;
	for(int i = rt.size() - 1; i >= 0; i--)
	{
		ert += rt[i];
	}
	SSLOCK;
	return ert;
}
