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

void page_mapper::reload_so(int pageId, time_t fatt, string dir, const char *f)
{
	//pthread_mutex_lock(http::mtx_exec); - any idea how to repair this?
	pthread_mutex_lock(http::mtx_exec2);

	dlclose(((nativepage*)(*base)[pageId].data)->handle);
	for (unsigned int i = 0; i < uris.size(); i++)
	{
		if (uris[i].siteId == pageId)
		{
			delete[] uris[i].url;
			uris.erase(uris.begin() + i);
		}
	}
	((nativepage*)(*base)[pageId].data)->handle = dlopen(f, RTLD_NOW | RTLD_LOCAL);
	if (!((nativepage*)(*base)[pageId].data)->handle)
	{
		nativehttp::server::log("ERROR@reloaders/so.cpp", string("can't open shared file: ") + f);
	}
	else
	{
		((nativepage*)(*base)[pageId].data)->onload = (nativehttp::data::Tonload) dlsym(((nativepage*)(*base)[pageId].data)->handle, "onload");
		((nativepage*)(*base)[pageId].data)->page = (nativehttp::data::Tpage) dlsym(((nativepage*)(*base)[pageId].data)->handle, "page");
		if (!((nativepage*)(*base)[pageId].data)->onload || !((nativepage*)(*base)[pageId].data)->page)
		{
			nativehttp::server::log("ERROR@reloaders/so.cpp", string("loading native symbols failed: ") + f);
			dlclose(((nativepage*)(*base)[pageId].data)->handle);
		}
		else
		{
			acp = pageId;
			int initstate = (*((nativepage*)(*base)[pageId].data)->onload)();
			if (initstate < 0)
			{
				if (initstate != -NATIVEHTTP_API_VERSION)
				{
					nativehttp::server::log("SO.loader@pagemap", "API version invalid: " + (string(f)));
					dlclose(((nativepage*)(*base)[pageId].data)->handle);
				}
			}
			else if (initstate == 1)
			{
#ifdef NHDBG
				nativehttp::server::log("SO.loader@pagemap", "page reloaded, but no API info were provided: " + (string(f)));
#endif
				initstate = -NATIVEHTTP_API_VERSION;
			}
			else
			{
				nativehttp::server::log("SO.loader@pagemap", "Page loading error(" + nativehttp::data::superstring::str_from_int(initstate - 2) + "): " + (string(f)));
				dlclose(((nativepage*)(*base)[pageId].data)->handle);
			}

			if (initstate == -NATIVEHTTP_API_VERSION)
			{
				nativehttp::data::superstring pgac((*base)[pageId].file);
				string furi = '/' + pgac.from(dir);
				char *tfu = new char[furi.size() + 1];
				memcpy(tfu, furi.c_str(), furi.size());
				tfu[furi.size()] = '\0';
				urlDef tmu = {tfu, pageId};

				uris.push_back(tmu);

				(*base)[pageId].type = page_native;
				(*base)[pageId].timestamp = fatt;
#ifdef NHDBG
                    if(http::log_detailed)nativehttp::server::log("DETAIL@SORL","RELOADED: "+furi);
#endif
			}
		}

	}

	//pthread_mutex_unlock(http::mtx_exec);
	pthread_mutex_unlock(http::mtx_exec2);
}
