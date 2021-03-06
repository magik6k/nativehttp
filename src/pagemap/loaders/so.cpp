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

#ifdef NHDBG
    if(http::log_detailed)nativehttp::server::log("DETAIL@SO.loader","Openning shared lib: "+string(f));
#endif

	ntm->handle = dlopen(f, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
	if (!ntm->handle)
	{
		nativehttp::server::err("SO.loader@pagemap", "can't open shared file: " + string(f) + ":\n" + string(dlerror()));
		delete ntm;
		abort = true;
	}
	else
	{

#ifdef NHDBG
        if(http::log_detailed)nativehttp::server::log("DETAIL@SO.loader","Loading symbols");
#endif

		ntm->onload = (nativehttp::data::Tonload) dlsym(ntm->handle, "onload");
		ntm->page = (nativehttp::data::Tpage) dlsym(ntm->handle, "page");
		if (!ntm->onload || !ntm->page)
		{
            nativehttp::server::err("SO.loader@pagemap", "error loading native symbols: " + string(f));
			cout << "error loading native symbols: " << f << endl;
			dlclose(ntm->handle);
			delete ntm;
			abort = true;
		}
		else
		{
			acp = base->size();

#ifdef NHDBG
        if(http::log_detailed)nativehttp::server::log("DETAIL@SO.loader","Initating");
#endif

			int initstate = (*ntm->onload)();

			if (initstate < 0)
			{
				if (initstate != -NATIVEHTTP_API_VERSION)
				{
					nativehttp::server::err("SO.loader@pagemap", "API version invalid: " + (nhp ? string(nhp) : string(f)));
					dlclose(ntm->handle);
					delete ntm;
					abort = true;
				}
			}
			else if (initstate == 1)
			{
                nativehttp::server::err("SO.loader@pagemap", "No API version info: " + (nhp ? string(nhp) : string(f)));
                dlclose(ntm->handle);
                delete ntm;
                abort = true;
			}
			else
			{
				nativehttp::server::err("SO.loader@pagemap", "Page loading error(" + nativehttp::data::superstring::str_from_int(initstate - 2) + "): " + (nhp ? string(nhp) : string(f)));
				dlclose(ntm->handle);
				delete ntm;
				abort = true;
			}

			if (initstate == -NATIVEHTTP_API_VERSION)
			{

#ifdef NHDBG
                if(http::log_detailed)nativehttp::server::log("DETAIL@SO.loader","Adding do page list");
#endif

				tmp.data = ntm;
				base->push_back(tmp);

				if (!nhp)
				{
					nativehttp::data::superstring pgac(f);
					string furi = '/' + pgac.from(dir);
					char *tfu = new char[furi.size() + 1];
					memcpy(tfu, furi.c_str(), furi.size());
					tfu[furi.size()] = '\0';

					urimp tmu = {tfu, int(base->size()) - 1};
					uris.push_back(tmu);
				}
				else
				{
					nativehttp::data::superstring pgac(nhp);
					string furi = '/' + pgac.from(dir);
					char *tfu = new char[furi.size() + 1];
					memcpy(tfu, furi.c_str(), furi.size());
					tfu[furi.size()] = '\0';

					urimp tmu = {tfu, int(base->size()) - 1};
					uris.push_back(tmu);
				}
			}
		}
	}
}


