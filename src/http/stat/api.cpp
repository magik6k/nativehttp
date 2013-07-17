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
			return http::statdata::info::toggle;
		}
		extern "C" bool transfer_stats()
		{
			return http::statdata::info::transfer;
		}
		extern "C" bool hit_stats()
		{
			return http::statdata::info::hitlog;
		}
		extern "C" bool method_stats()
		{
			return http::statdata::info::method;
		}

		extern "C" unsigned long long hits()
		{
			return http::statdata::activity::hits;
		}
		extern "C" unsigned long long connections()
		{
			return http::statdata::activity::connections;
		}
		extern "C" unsigned long long uploaded()
		{
			return http::statdata::transfer::ulbytes;
		}
		extern "C" unsigned long long downloaded()
		{
			return http::statdata::transfer::dlbytes;
		}

		extern "C" unsigned long long get_requests()
		{
			return http::statdata::method::get;
		}
		extern "C" unsigned long long post_requests()
		{
			return http::statdata::method::post;
		}

		extern "C" unsigned long long hourly_length()
		{
			return http::statdata::info::hourlylen;
		}

		extern "C" unsigned long long hour_hits(unsigned long hid)
		{
			if (hid < http::statdata::info::hourlylen)
			{
				return http::statdata::activity::hrl_hits[hid];
			}
			return 0;
		}
		extern "C" unsigned long long hour_connections(unsigned long hid)
		{
			if (hid < http::statdata::info::hourlylen)
			{
				return http::statdata::activity::hrl_connections[hid];
			}
			return 0;
		}
		extern "C" unsigned long long hour_upload(unsigned long hid)
		{
			if (hid < http::statdata::info::hourlylen)
			{
				return http::statdata::transfer::hrl_ul[hid];
			}
			return 0;
		}
		extern "C" unsigned long long hour_download(unsigned long hid)
		{
			if (hid < http::statdata::info::hourlylen)
			{
				return http::statdata::transfer::hrl_dl[hid];
			}
			return 0;
		}
	}
}
