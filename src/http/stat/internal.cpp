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
#include "../stat.h"
#include "nativehttp.h"

namespace http
{
	namespace statdata
	{

		void onrecv(unsigned long dlen)
		{
			if(toggle && transfer)
			{
				dlbytes += dlen;
				hrl_dl[0] += dlen;
			}
		}
		void onsend(unsigned long ulen)
		{
			if(toggle && transfer)
			{
				ulbytes += ulen;
				hrl_ul[0] += ulen;
			}
		}
		void onconnect()
		{
			if(toggle)
			{
				connections++;
				hrl_connections[0]++;
			}
		}

		void onpost()
		{
			if(toggle && method)
				post++;
		}
		void onget()
		{
			if(toggle && method)
				get++;
		}
		void onhit()
		{
			if(toggle && hitlog)
			{
				hits++;
				hrl_hits[0]++;
			}
		}
	}
}
