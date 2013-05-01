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

namespace nativehttp
{
	namespace stat
	{
		extern "C" bool stats()
		{
			return http::statdata::toggle;
		}
		extern "C" bool transfer_stats()
		{
			return http::statdata::transfer;
		}
		extern "C" bool hit_stats()
		{
			return http::statdata::hitlog;
		}
		extern "C" bool method_stats()
		{
			return http::statdata::method;
		}

		extern "C" unsigned long hits()
		{
			return http::statdata::hits;
		}
		extern "C" unsigned long connections()
		{
			return http::statdata::connections;
		}
		extern "C" unsigned long uploaded()
		{
			return http::statdata::ulbytes;
		}
		extern "C" unsigned long downloaded()
		{
			return http::statdata::dlbytes;
		}

		extern "C" unsigned long get_requests()
		{
			return http::statdata::get;
		}
		extern "C" unsigned long post_requests()
		{
			return http::statdata::post;
		}

		extern "C" unsigned long hourly_length()
		{
			return http::statdata::hourlylen;
		}

		extern "C" unsigned long hour_hits(unsigned long hid)
		{
			if(hid < http::statdata::hourlylen)
			{
				return http::statdata::hrl_hits[hid];
			}
			return 0;
		}
		extern "C" unsigned long hour_connections(unsigned long hid)
		{
			if(hid < http::statdata::hourlylen)
			{
				return http::statdata::hrl_connections[hid];
			}
			return 0;
		}
		extern "C" unsigned long hour_upload(unsigned long hid)
		{
			if(hid < http::statdata::hourlylen)
			{
				return http::statdata::hrl_ul[hid];
			}
			return 0;
		}
		extern "C" unsigned long hour_download(unsigned long hid)
		{
			if(hid < http::statdata::hourlylen)
			{
				return http::statdata::hrl_dl[hid];
			}
			return 0;
		}
	}
}
