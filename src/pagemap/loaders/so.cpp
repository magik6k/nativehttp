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

#include "../pagemap.h"
#include "http/data.h"
#include <iostream>
#include <string.h>

void page_mapper::load_so(page &tmp, const char *f, string dir, const char *nhp)
{
	tmp.type = page_native;
	nativepage *ntm = new nativepage;
	ntm->handle = dlopen(f, RTLD_NOW | RTLD_LOCAL);
	if(!ntm->handle)
	{
		cout << "can't open shared file: " << f << ": " << dlerror() << endl;
		delete ntm;
	}
	else
	{
		ntm->onload = (nativehttp::data::Tonload) dlsym(ntm->handle, "onload");
		ntm->page = (nativehttp::data::Tpage) dlsym(ntm->handle, "page");
		if(!ntm->onload || !ntm->page)
		{
			cout << "error loading native symbols: " << f << endl;
			dlclose(ntm->handle);
			delete ntm;
		}
		else
		{
			acp = base->size();
			int initstate = (*ntm->onload)();
			if(initstate != 1)
			{
				cout << "invalid init state(" << initstate << "): " << f << endl;
				dlclose(ntm->handle);
				delete ntm;
			}
			else
			{
				tmp.data = ntm;
				base->push_back(tmp);

				if(!nhp)
				{
					nativehttp::data::superstring pgac(f);
					string furi = '/' + pgac.from(dir);
					char *tfu = new char[furi.size()+1];
					memcpy(tfu, furi.c_str(), furi.size());
					tfu[furi.size()] = '\0';

					urimp tmu = {tfu, int(base->size()) - 1};
					uris.push_back(tmu);
				}
				else
				{
					nativehttp::data::superstring pgac(nhp);
					string furi = '/' + pgac.from(dir);
					char *tfu = new char[furi.size()+1];
					memcpy(tfu, furi.c_str(), furi.size());
					tfu[furi.size()] = '\0';

					urimp tmu = {tfu, int(base->size()) - 1};
					uris.push_back(tmu);
				}
			}
		}
	}
}


