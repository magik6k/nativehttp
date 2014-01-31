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


    for(;!conf.atend();)
    {
        conf.skip(" \t\n\r");

        if(conf.check("//")||conf.check("#")|conf.check(";"))
        {
            conf.to("\n");
            continue;
        }
        if(conf.check("/*"))
        {
            conf.to("*/");
            continue;
        }
        if(conf.check("["))
        {
            conf.to("]");
            continue;
        }

        string name = conf.tochar(" \t\n\r=");
        if(name.empty()&&!conf.atend())
        {
            int line = nativehttp::data::superstring(conf.lock().to(0)).count('\n');
            string nr = conf.unlock().tochar("\r\n");
            nativehttp::server::err("parser@cfg.cpp", "[E1] No-name variables not allowed(near '"+nr+"', line: "+nativehttp::data::superstring::str_from_int(line)+")");
            continue;
        }
        else if(name.empty())
        {
            break;
        }

        conf.skip(" \t\n\r");


        if(conf.str[conf.pos]=='\0')
        {
            nativehttp::server::err("parser@cfg.cpp", "[E3] Reached unexpected end of file after variable name");
            continue;
        }

        if(!conf.check("="))
        {
            int line = nativehttp::data::superstring(conf.lock().to(0)).count('\n');
            string nr = conf.unlock().tochar("\r\n");
            nativehttp::server::err("parser@cfg.cpp", "[E2] Expected '=' after variable name(near '"+nr+"', key: '"+name+
                "', line: "+nativehttp::data::superstring::str_from_int(line)+"), got '"+conf.str[conf.pos]+"'");
            continue;
        }
        conf.pos++;
        conf.skip(" \t\n\r");

        string content;
        if(conf.check("\""))
        {
            conf.to("\"");
            content = conf.to("\"");
            conf.tochar("\r\n");
        }
        else
        {
            content = conf.tochar("\r\n");
        }

        cfgfil tmp;
        tmp.name = name;
        tmp.cont = content;
        fileds.push_back(tmp);
    }

}


string nativehttp::data::Ccfg::get_var(string name) const
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

int nativehttp::data::Ccfg::get_int(string name) const
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

vector<nativehttp::data::cfgfil> nativehttp::data::Ccfg::get_all() const
{
    return this->fileds;
}

vector<nativehttp::data::cfgfil> nativehttp::data::Ccfg::get_prefix(string name) const
{
    vector<nativehttp::data::cfgfil> rt;
    size_t nlen = name.size();
    bool* cand = new bool[this->fileds.size()];
    for(size_t i = 0; i < nlen; ++i)
    {
        for(size_t j = 0; j < this->fileds.size(); ++j)
        {
            if(i == 0 && name.size() <= this->fileds[j].name.size()){cand[j] = true;}else{cand[j] = false; continue;}
            if(cand[j] && name[i] == this->fileds[j].name[i] && name.size() - 1 == i)
            {
                rt.push_back(this->fileds[j]);
                cand[j] = false;
            }
            if(cand[j] && name[i] != this->fileds[j].name[i])
            {
                cand[j] = false;
            }
        }
    }
    delete[] cand;
    return rt;
}
