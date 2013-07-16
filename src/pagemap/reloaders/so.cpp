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

void page_mapper::reload_so(int pgi, time_t fatt, string dir, const char *f)
{
	pthread_mutex_lock(http::mtx_exec);
	pthread_mutex_lock(http::mtx_exec2);

	dlclose(((nativepage*)(*base)[pgi].data)->handle);
	for (unsigned int i = 0; i < uris.size(); i++)
	{
		if (uris[i].sid == pgi)
		{
			delete[] uris[i].u;
			uris.erase(uris.begin() + i);
		}
	}
	((nativepage*)(*base)[pgi].data)->handle = dlopen(f, RTLD_NOW | RTLD_LOCAL);
	if (!((nativepage*)(*base)[pgi].data)->handle)
	{
		nativehttp::server::log("ERROR@reloaders/so.cpp", string("can't open shared file: ") + f);
	}
	else
	{
		((nativepage*)(*base)[pgi].data)->onload = (nativehttp::data::Tonload) dlsym(((nativepage*)(*base)[pgi].data)->handle, "onload");
		((nativepage*)(*base)[pgi].data)->page = (nativehttp::data::Tpage) dlsym(((nativepage*)(*base)[pgi].data)->handle, "page");
		if (!((nativepage*)(*base)[pgi].data)->onload || !((nativepage*)(*base)[pgi].data)->page)
		{
			nativehttp::server::log("ERROR@reloaders/so.cpp", string("loading native symbols failed: ") + f);
			dlclose(((nativepage*)(*base)[pgi].data)->handle);
		}
		else
		{
			acp = pgi;
			int initstate = (*((nativepage*)(*base)[pgi].data)->onload)();
			if (initstate < 0)
			{
				if (initstate != -NATIVEHTTP_API_VERSION)
				{
					nativehttp::server::log("SO.loader@pagemap", "API version invalid: " + (string(f)));
					dlclose(((nativepage*)(*base)[pgi].data)->handle);
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
				dlclose(((nativepage*)(*base)[pgi].data)->handle);
			}

			if (initstate == -NATIVEHTTP_API_VERSION)
			{
				nativehttp::data::superstring pgac((*base)[pgi].file);
				string furi = '/' + pgac.from(dir);
				char *tfu = new char[furi.size() + 1];
				memcpy(tfu, furi.c_str(), furi.size());
				tfu[furi.size()] = '\0';
				urimp tmu = {tfu, pgi};

				uris.push_back(tmu);

				(*base)[pgi].type = page_native;
				(*base)[pgi].timestamp = fatt;
			}
		}

	}

	pthread_mutex_unlock(http::mtx_exec);
	pthread_mutex_unlock(http::mtx_exec2);
}
