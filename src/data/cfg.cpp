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

void nativehttp::data::Ccfg::parse_file(const char *f)
{

	FILE *fil = fopen(f, "rt");

	if (fil == NULL)
	{
		return;
	}

	cfgfil tmp;

	char c;
	bool comment = false;
	bool acmt = false;
	bool sside = false;


	do
	{

		c = fgetc(fil);

		if (c != EOF)
		{
			if (c == '/')
			{
				if (acmt)
				{
					acmt = false;
					comment = true;
				}
				else
				{
					acmt = true;
				}
			}
			else
			{
				if (!comment)acmt = false;
			}
			if (c == '\n' && comment)
			{
				comment = false;
			}
			if (!comment || !acmt)
			{
				if (!sside)
				{
					if (c == '=')
					{
						sside = true;
					}
					else if (c == '\n')
					{
						tmp.name = "";
					}
					else
					{
						tmp.name += c;
					}
				}
				else
				{
					if (c == '\n')
					{
						fileds.push_back(tmp);
						tmp.name = "";
						tmp.cont = "";
						sside = false;
					}
					else if (c != '\r')
					{
						tmp.cont += c;
					}
				}
			}
		}


	}
	while (c != EOF);

	fclose(fil);


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


