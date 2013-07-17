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
			if (info::toggle && info::transfer)
			{
				transfer::dlbytes += dlen;
				transfer::hrl_dl[0] += dlen;
			}
		}
		void onsend(unsigned long ulen)
		{
			if (info::toggle && info::transfer)
			{
				transfer::ulbytes += ulen;
				transfer::hrl_ul[0] += ulen;
			}
		}
		void onconnect()
		{
			if (info::toggle)
			{
				activity::connections++;
				activity::hrl_connections[0]++;
			}
		}

		void onpost()
		{
			if (info::toggle && info::method)
				method::post++;
		}
		void onget()
		{
			if (info::toggle && info::method)
				method::get++;
		}
		void onhit()
		{
			if (info::toggle && info::hitlog)
			{
				activity::hits++;
				activity::hrl_hits[0]++;
			}
		}
	}
}
