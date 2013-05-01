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
#include "session.h"
#include "http/data.h"

namespace nativehttp
{
	namespace data
	{
		void session::__init(cookiedata *cd)
		{
			ssid = 0;
			valid = false;
			if(!cd)return;
			string stsi = cd->get("NH_ssid");
			string stsv = cd->get("NH_sslc");
			if(stsi.empty() || stsv.empty())
			{
				unsigned int sslc = rand() % (2 << 30);
				ssid = sdata::session::storage.mksess(sslc);

				if(!sdata::session::storage.cksess(ssid, sslc))
				{
					valid = false;
					return;
				}
				cd->set(http::sess_ssid_cnam, nativehttp::data::superstring::from_int(ssid), "path=/");
				cd->set(http::sess_sslc_cnam, nativehttp::data::superstring::from_int(ssid), "path=/");
				valid = true;
				return;
			}
			else
			{
				size_t nsi = nativehttp::data::superstring::from_string(stsi);
				size_t nsv = nativehttp::data::superstring::from_string(stsv);

				if(sdata::session::storage.cksess(nsi, nsv))
				{
					ssid = nsi;
					valid = true;
					return;
				}
				else
				{
					unsigned int sslc = rand() % (2 << 30);
					ssid = sdata::session::storage.mksess(sslc);

					if(!sdata::session::storage.cksess(ssid, sslc))
					{
						valid = false;
						return;
					}

					cd->set(http::sess_ssid_cnam, nativehttp::data::superstring::from_int(ssid), "path=/");
					cd->set(http::sess_sslc_cnam, nativehttp::data::superstring::from_int(sslc), "path=/");
					valid = true;
					return;
				}
			}
		}

		string session::get(string name)
		{
			if(valid)
			{
				return sdata::session::storage.gtsess(ssid).getkey(name);
			}
			return "";
		}

		void session::set(string name, string value)
		{
			if(valid)
			{
				sdata::session::storage.gtsess(ssid).setkey(name, value);
			}
		}
	}
}
