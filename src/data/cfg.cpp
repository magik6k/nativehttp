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
#include <string.h>
#include <iostream>

void nativehttp::data::Ccfg::parse_file(const char *f)
{
    nativehttp::data::superstring conf;
    conf.set_file(f);



    conf.lock();

    conf.remove("/*","*/");
    conf.remove("//","\n");
    conf.remove(";","\n");
    conf.remove("[","]");

    conf.unlock();

    for(;!conf.atend();)
    {
        conf.skip(" \t\n\r");
        string name = conf.tochar(" \t\n\r=");
        if(name.empty()&&conf.pos>conf.size())
        {
            string nr = conf.tochar("\r\n");
            nativehttp::server::log("parser@cfg.cpp", "[E1] No-name variables not allowed(near '"+nr+"')");
            continue;
        }
        conf.skip(" \t\n\r");

        if(conf.str[conf.pos]=='\0')
        {
            nativehttp::server::log("parser@cfg.cpp", "[E3] Reached unexpected end of file after variable name");
            continue;
        }

        if(!conf.check("="))
        {
            string nr = conf.tochar("\r\n");
            nativehttp::server::log("parser@cfg.cpp", "[E2] Expected '=' after variable name(near '"+nr+"', key: '"+name+"'), got '"+conf.str[conf.pos]+"'");
            continue;
        }
        conf.pos++;
        conf.skip(" \t\n\r");
        conf.lock();
        string content;
        if(nativehttp::data::superstring(conf.tochar("\r\n")).contain("\""))
        {
            conf.unlock();
            conf.to("\"");
            content = conf.to("\"");
            conf.tochar("\r\n");
        }
        else
        {
            conf.unlock();
            content = conf.tochar("\r\n");
        }

        cfgfil tmp;
        tmp.name = name;
        tmp.cont = content;
        fileds.push_back(tmp);
    }

}


string nativehttp::data::Ccfg::get_var(string name)
{

	unsigned int i = 0;

	while (i != fileds.size())
	{
		if (fileds[i].name == name)
		{
			return fileds[i].cont;
		}
		i++;
	}

	return "";

}

int nativehttp::data::Ccfg::get_int(string name)
{
	string s = this->get_var(name);
	int tmp = 0;
	unsigned int i = 0;
	bool m = false;
	if (s[0] == '-')
	{
		m = true;
		i++;
	}
	for (; i < s.size(); i++)
		tmp = 10 * tmp + s[i] - 48;
	return m ? -tmp : tmp;
}


