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
#include "utils/memory.h"

#ifdef NHDBG
#include "utils/memory.h"
#endif

namespace http
{

	namespace rproc
	{

		void post(nativehttp::rdata &rd, http::request *process, http::rproc::lrqd &ld)
		{
			nativehttp::data::superstring ars(process->request);


			ars.add_token(nativehttp::data::token("_%$<Unimple\t;mented\r\n\n\r>$%_", -1));
			ars.add_token(nativehttp::data::token("\r\n\r\n", 1));
			ars.add_token(nativehttp::data::token("\n\n", 1));

			if(ars.tok().id == 1)
			{
                ars.str = ars.from(ars.pos);
			}

            //#error TODO
                ///ars.str = post data

			if (ars.size() < ld.clen)
			{
				unsigned int ltrv = ld.clen - ars.size();
				#ifdef NHDBG
				if(http::extmemstats)
                    nativehttp::server::log("post.cpp@http","Waiting for "+
                        nativehttp::data::superstring::str_from_size(ltrv) + " of POST Data");
				#endif
				char *tv = utils::memory::alloc<char>(ltrv + 1);
				unsigned int ar = 0;
				while (0 < ltrv)
				{
					int rv;
					if (http::onssl)
					{
						rv = SSL_read(http::sslsck[process->uid], tv + ar, ltrv);
					}
					else
					{
						rv = recv(process->sender, tv + ar, ltrv, 0);
					}

					if (rv <= 0)
					{
						delete[] tv;
						tv = NULL;
						kickclient(ld.uid);
						break;
					}
					ar += rv;
					ltrv -= rv;
					(tv + ar)[0] = 0;
				}

				if (tv)
				{
					ars.str += tv;
					delete[] tv;
				}
			}
			rd.post = new nativehttp::data::postgetdata(ars.str);
		}

	}
}
