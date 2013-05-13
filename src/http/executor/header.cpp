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
#include "protocol.h"
#include "../executor.h"
#include "../data.h"
#include "../stat.h"
#include "../net/net.h"

#ifdef NHDBG
#include "utils/memory.h"
#endif

namespace http
{

	namespace rproc
	{

		void header(http::request *process, nativehttp::rdata &rd, http::rproc::lrqd &ld)
		{
			nativehttp::data::superstring hss(process->request);
			hss.to("\r\n");
			hss.str = hss.to("\r\n\r\n");
			hss.pos = 0;

			ld.rng_start=0;
			ld.rng_end=-1;

            hss.add_token(nativehttp::data::token("_%$<Unimplemented>$%_", -1));
			hss.add_token(nativehttp::data::token("\r\n\r\n", 0));
			hss.add_token(nativehttp::data::token("Host: ", 1));
			hss.add_token(nativehttp::data::token("User-Agent: ", 2));
			hss.add_token(nativehttp::data::token("Referer: ", 3));
			hss.add_token(nativehttp::data::token("Cookie: ", 4));
			hss.add_token(nativehttp::data::token("Content-Length: ", 5));
			hss.add_token(nativehttp::data::token("Range: ", 6));

			while (hss.pos < hss.str.size())
			{
				nativehttp::data::token pt = hss.tok();
				if (pt.id == 0)break;
				switch (pt.id)
				{
                    case -1:
						ld.d501 = 1;
						break;
					case 1:
						rd.host = hss.to("\r\n");
						break;
					case 2:
						rd.userAgent = hss.to("\r\n");
						break;
					case 3:
						rd.referer = hss.to("\r\n");
						break;
					case 4:
						rd.cookie = new nativehttp::data::cookiedata(hss.to("\r\n"));
						break;
					case 5:
						ld.clen = strtol(hss.to("\r\n").c_str(), NULL, 10);
						break;
                    case 6:
                        nativehttp::data::superstring btr(hss.to("\r\n"));
                        btr(btr.from("bytes="));
                        btr.pos=0;
                        if(btr.str[0]!='-')
                        {
                            ld.rng_start = nativehttp::data::superstring::int64_from_str(btr.to("-"));
                        }
                        else
                        {
                            btr.pos++;
                            ld.rng_start = -nativehttp::data::superstring::int64_from_str(btr.to(","));///handling only 1 range. @TODO 206 here
                            btr.pos = btr.str.size()-1;
                        }
                        if(btr.str[btr.str.size()-1]!='-'&&btr.pos<btr.str.size()-1)
                        {
                            ld.rng_end = nativehttp::data::superstring::int64_from_str(btr.to(","));///handling only 1 range. @TODO 206 here
                        }
                        break;
				}
			}

			hss.str.clear();
		}

		void line0(http::request *process, nativehttp::rdata &rd, http::rproc::lrqd &ld)
		{
			nativehttp::data::superstring rss(nativehttp::data::superstring(process->request).to("\r\n"));
			rss.add_token(nativehttp::data::token(" ", 0));
			rss.add_token(nativehttp::data::token("GET", 1));
			rss.add_token(nativehttp::data::token("HEAD", 3));
			rss.add_token(nativehttp::data::token("POST", 2));
			rss.add_token(nativehttp::data::token("DELETE", 3));
			rss.add_token(nativehttp::data::token("TRACE", 3));
			rss.add_token(nativehttp::data::token("CONNECT", 3));
			rss.add_token(nativehttp::data::token("OPTIONS", 3));

			process->method = rss.tok().id;

			rss.clear_tokens();
			rss.pos++;
			nativehttp::data::superstring rawuri(rss.to(" "));
			process->http11 = rss.check("HTTP/1.1");

			rd.uri = rawuri.to("?");
			string gu = rawuri.to("#");
			if (!gu.empty())
			{
				rd.get = new nativehttp::data::postgetdata(gu);
			}
		}

    }
}
