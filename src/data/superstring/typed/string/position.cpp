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

bool nativehttp::data::superstring::contain(string stb)
{
    bool *t=new bool[stb.size()];
    for(unsigned int i=0;i<stb.size();i++)t[i]=false;

    for(;pos<str.size();pos++)
    {
        for(unsigned int i=0;i<stb.size();i++)
        {
            if(str[pos]==stb[i])
                t[i]=true;
        }
    }
    for(unsigned int i=0;i<stb.size();i++)
    {
        if(!t[i])
        {
            delete[] t;
            SSLOCK;
            return false;
        }
    }
    delete[] t;
    SSLOCK;
    return true;
}

bool nativehttp::data::superstring::contain_not(string stb)
{
    for(;pos<str.size();pos++)
    {
        for(unsigned int i=0;i<stb.size();i++)
        {
            if(str[pos]==stb[i])
                return false;
        }
    }
    SSLOCK;
    return true;
}

bool nativehttp::data::superstring::contain_only(string stb)
{
    for(;pos<str.size();pos++)
    {
        for(unsigned int i=0;i<stb.size();i++)
        {
            if(str[pos]!=stb[i])
                return false;
        }
    }
    SSLOCK;
    return true;
}


string nativehttp::data::superstring::skip(string stb)
{
    string rt;
    for(;pos<str.size();pos++)
    {
        bool r=false;
        for(unsigned int i=0;i<stb.size();i++)
        {
            if(str[pos]==stb[i])r=true;
        }
        if(r)rt+=str[pos];
            else break;
    }
    SSLOCK;
    return rt;
}

string nativehttp::data::superstring::tochar(string fend)
{
    string rt;
    bool ex=false;
    for(;pos<str.size();pos++)
    {
        for(unsigned int i=0;i<fend.size();i++)
        {
            if(str[pos]==fend[i])
            {
                ex=true;
                break;
            }
        }
        if(ex)break;
            else rt+=str[pos];
    }
    SSLOCK;
    return rt;
}

string nativehttp::data::superstring::to(string fend)
{
	rae = false;
	unsigned int c = 0;
	string rt;
	string prt;
	while (pos < str.size())
	{
		if (str[pos] == fend[c])
		{
			prt += str[pos];
			c++;
		}
		else
		{
			if (!prt.empty())
			{
				rt += prt;
				prt.clear();
				c = 0;
			}
			rt += str[pos];
		}
		pos++;
		if (c >= fend.size())
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
	while (pos < str.size())
	{
		if (str[pos] == start[c])
		{
			c++;
		}
		else
		{
			if (c > 0)
			{
				c = 0;
			}
		}
		pos++;
		if (c >= start.size())
		{
			break;
		}
	}
	while (pos < str.size())
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
	for (int i = fend.size() - 1; i >= 0; i--)
	{
		nfe += fend[i];
	}
	unsigned int c = 0;
	string rt;
	string prt;
	while (pos > 0)
	{
		if (str[pos] == nfe[c])
		{
			prt += str[pos];
			c++;
		}
		else
		{
			if (!prt.empty())
			{
				rt += prt;
				prt.clear();
				c = 0;
			}
			rt += str[pos];
		}
		pos--;
		if (c >= nfe.size())
		{
			string ert;
			for (int i = rt.size() - 1; i >= 0; i--)
			{
				ert += rt[i];
			}
			SSLOCK;
			return ert;
		}
	}
	string ert;
	for (int i = rt.size() - 1; i >= 0; i--)
	{
		ert += rt[i];
	}
	SSLOCK;
	return ert;
}

unsigned int nativehttp::data::superstring::find(string str)
{
    unsigned int c = 0;
	while (pos < str.size())
	{
		if (str[pos] == str[c])
		{
			c++;
		}
		else
            c = 0;
		pos++;
		if (c >= str.size())
		{
			break;
		}
	}
	c = pos;
    SSLOCK;
    return c<str.size()?c:0;
}
