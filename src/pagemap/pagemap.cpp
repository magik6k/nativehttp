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

#include "pagemap.h"
#include <string.h>

void page_mapper::preinit()
{
	base = new data::vector<page>(cfg->get_int("pagebase_max"));
}

page page_mapper::bscpageset(char *f)
{
	page tmp;
	tmp.type = 0;
	struct stat tst;
	int rst = stat(f, &tst);
	if (rst != 0)
	{
		nativehttp::server::log("pagemap.cpp:init", "stat error");
		tmp.type = -1;
		return tmp;
	}
	tmp.timestamp = tst.st_mtime;
	tmp.file = new char[strlen(f) + 1];
	memcpy(tmp.file, f, strlen(f));
	tmp.file[strlen(f)] = '\0';
	return tmp;
}

page page_mapper::by_uri(const char *u)
{
	vector<int>dn(uris.size(), 0);
	page t = { -1, NULL};
	for (unsigned int i = 0; i < strlen(u); i++)
	{
		for (unsigned int j = 0; j < dn.size(); j++)
		{
			if (dn[j] != -1)
			{
				if (uris[j].u[dn[j]] == u[i] || uris[j].u[dn[j]] == '*')
				{
					if (strlen(uris[j].u) == unsigned(dn[j]) + 1 && strlen(uris[j].u) == unsigned(dn[j]) + 1 && strlen(u) == strlen(uris[j].u))
					{
						return (*base)[uris[j].sid];
					}
					else if (strlen(uris[j].u) == unsigned(dn[j]) + 1 && uris[j].u[dn[j]] == '*')
					{
						t = (*base)[uris[j].sid];
					}
					dn[j]++;
				}
				else
				{
					dn[j] = -1;
				}
			}
		}
	}
	return t;
}

void page_mapper::adduri(string u, bool top)
{
	char *cu = new char[u.size() + 1];
	memcpy(cu, u.c_str(), u.size());
	cu[u.size()] = '\0';

	urimp tu = {cu, acp};
	if (top)
	{
		uris.push_front(tu);
	}
	else
	{
		uris.push_back(tu);
	}
}

