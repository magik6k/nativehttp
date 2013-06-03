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
#include "../net.h"
#include "ws/ws.h"

namespace http
{
	namespace bsd
	{
		void disconnect(int scid)
		{

            if(ws::enabled)
            {
                if(http::client_protocol[scid] == CLPROT_WEBSOCKETS)
                    if(((*ws::units)[http::client_prot_data[scid]].on_disconnect))
                        (*(*ws::units)[http::client_prot_data[scid]].on_disconnect)(scid);
            }

			if (http::connected[scid] != -1)
			{
				close(http::connected[scid]);
				http::connected[scid] = -1;
			}
			http::client_ips[scid] = 0;
		}
	}
}
