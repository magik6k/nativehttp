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

nativehttp::data::postgetdata::postgetdata(string dstr)
{
	nativehttp::data::superstring cke(dstr);
	while(cke.pos < cke.str.size())
	{
		cfil ctm;
		ctm.name = cke.to("=");
		ctm.value = cke.to("&");
		data.push_back(ctm);
	}
}

string nativehttp::data::postgetdata::get(string name)
{
	for(unsigned int i = 0; i < data.size(); i++)
	{
		if(data[i].name == name)
		{
			return data[i].value;
		}
	}
	return "";
}

extern "C" string nativehttp::utils::decode_poststring(string str)
{
	string o;
	nativehttp::data::superstring s(str);
	s.change("+", " ");
	s.pos = 0;
	while(s.pos < s.str.size())
	{
		o += s.to("%");
		s.pos += 2;
		if(s.pos <= s.str.size())
		{
			char t[3] = {0, 0, 0};
			t[0] = s.str[s.pos-2];
			t[1] = s.str[s.pos-1];
			unsigned int op = 0;
			sscanf(t, "%x", &op);
			o += (char)op;
		}
	}
	return o;
}
