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
#include "assets.h"
#include "dtg.h"

#define config "./etc/nativehttp/nstat.cfg"

string pass = "root";

extern "C"
{
	int onload()
	{
		nh::data::superstring htmss(html_top);
		htmss.lock();
		htmss.change("[[srvVer]]", nh::server::version());
		htmss.change("[[style]]", page_style);
		page_style.clear();
		html_top = htmss.str;
		nativehttp::data::Ccfg cfg(config);
		pass = cfg.get_var("pass");
		initated();
	}
	nativehttp::data::pagedata page(nativehttp::rdata* request)
	{
		nh::data::superstring page(html_top);

		if (!request->session)
		{
			page.change("[[content]]", "NativeStat needs Session module enabled!<br/>");
			return nativehttp::data::pagedata(page.str);
		}

		if (request->session->get("@__NtStS#") != "l")
		{
			if (!request->post)
			{
				page.change("[[content]]", logform);
				return nativehttp::data::pagedata(page.str);
			}
			else
			{
				if (request->post->get("scky") == pass)
				{
					request->session->set("@__NtStS#", "l");
				}
				else
				{
					page.change("[[content]]", logformbad);
					return nativehttp::data::pagedata(page.str);
				}
			}
		}




		page.change("[[content]]", "W-I-P");

		return nativehttp::data::pagedata(page.str);
	}
}

