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

nativehttp::data::token::token(string st, int i)
{
	s = st;
	id = i;
}

void nativehttp::data::superstring::clear_tokens()
{
	tokens.clear();
}

void nativehttp::data::superstring::add_token(token t)
{
	tokens.push_back(t);
}

size_t nativehttp::data::superstring::num_tokens()
{
    return tokens.size();
}

nativehttp::data::token nativehttp::data::superstring::tok()
{
	vector<unsigned int>d(tokens.size(), 0);

	while(pos < str.size())
	{
		for(unsigned int i = 0; i < tokens.size(); i++)
		{
			if(str[pos] == tokens[i].s[d[i]])
			{
				d[i]++;
			}
			else
			{
				d[i] = 0;
			}
			if(d[i] >= tokens[i].s.size())
			{
				pos++;
				SSLOCK;
				return tokens[i];
			}
		}
		pos++;
	}
	SSLOCK;
	return tokens[0];
}

nativehttp::data::token nativehttp::data::superstring::tok(string &opt)
{
	vector<unsigned int>d(tokens.size(), 0);

	while(pos < str.size())
	{
		opt += str[pos];
		for(unsigned int i = 0; i < tokens.size(); i++)
		{
			if(str[pos] == tokens[i].s[d[i]])
			{
				d[i]++;
			}
			else
			{
				d[i] = 0;
			}
			if(d[i] >= tokens[i].s.size())
			{
				pos++;
				SSLOCK;
				return tokens[i];
			}
		}
		pos++;
	}
	SSLOCK;
	return tokens[0];
}
