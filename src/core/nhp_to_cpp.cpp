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
#include "protocol.h"

enum nhps
{
	nhps_html,
	nhps_top,
	nhps_init,
	nhps_page
};

string parse_nhp(string nhp)
{
	string top = "#include <nativehttp/nativehttp.h>\n";
	string init = "extern \"C\"\n{\nint onload()\n{\n";
	string page = "initated();\n}\nnativehttp::data::pagedata page(nativehttp::rdata* request)\n{\nstring result;\n";
	string end = "return nativehttp::data::pagedata(result);\n}\n}\n";
	string thtm = "";
	nativehttp::data::superstring data(nhp);
	nhps scope = nhps_html;
	bool iq = false;
	while(data.pos < data.str.size())
	{
		bool nap = false;
		//bool csc=false;
		if(scope != nhps_html)
		{
			if(data.check("\\"))
			{
				//data.pos+=2;
				//csc=true;
			}
			if(data.check("\"") && !nap)
			{
				if(iq)iq = false;
				else iq = true;
			}
			if(!iq)
			{
				//comment skipping
				if(data.check("//"))
				{
					data.to("\n");
					nap = true;
				}
				if(data.check("/*"))
				{
					data.to("*/");
					nap = true;
				}
				//comment skipping end
				//scope change detection
				if(data.check("%>"))     //init scope end
				{
					if(scope == nhps_init)
					{
						scope = nhps_html;
						nap = true;
					}
					else
					{
						cerr << "ERROR @ " << data.pos << ", use of '%>' is wrong\n";
						return "";
					}
					data.pos += 2;
					nap = true;
				}
				if(data.check("?>"))     //top scope end
				{
					if(scope == nhps_top)
					{
						scope = nhps_html;
					}
					else
					{
						cerr << "ERROR @ " << data.pos << ", use of '?>' is wrong\n";
						return "";
					}
					data.pos += 2;
					nap = true;
				}
				if(data.check("$>"))     //page scope end
				{
					if(scope == nhps_page)
					{
						scope = nhps_html;
					}
					else
					{
						cerr << "ERROR @ " << data.pos << ", use of '$>' is wrong\n";
						return "";
					}
					data.pos += 2;
					nap = true;
				}
			}
		}
		else
		{
			if(data.check("<%"))     //init scope begin
			{
				scope = nhps_init;
				data.pos += 2;
				nap = true;
			}
			if(data.check("<?"))     //top scope begin
			{
				scope = nhps_top;
				data.pos += 2;
				nap = true;
			}
			if(data.check("<$"))     //page scope begin
			{
				scope = nhps_page;
				data.pos += 2;
				nap = true;
			}
			if(nap && !thtm.empty())
			{
				nativehttp::data::superstring htenc(thtm);
				thtm.clear();
				htenc.change("\\", "\\\\");
				htenc.pos = 0;
				htenc.change("\n", "\\n");
				htenc.pos = 0;
				htenc.change("\r", "");
				htenc.pos = 0;
				htenc.change("\"", "\\\"");
				page += "result+=\"" + htenc.str + "\";\n";
			}
		}
		if(!nap)
		{
			//char tc=data.str[data.pos];
			switch(scope)
			{
			case nhps_html:
				thtm += data.str[data.pos];
				break;
			case nhps_init:
				init += data.str[data.pos];
				break;
			case nhps_page:
				page += data.str[data.pos];
				break;
			case nhps_top:
				top += data.str[data.pos];

			}
			data.pos++;
		}
	}
	if(!thtm.empty())
	{
		nativehttp::data::superstring htenc(thtm);
		thtm.clear();
		htenc.change("\\", "\\\\");
		htenc.pos = 0;
		htenc.change("\n", "\\n");
		htenc.pos = 0;
		htenc.change("\r", "");
		htenc.pos = 0;
		htenc.change("\"", "\\\"");
		page += "result+=\"" + htenc.str + "\";\n";
	}

	return top + init + page + end;

}
