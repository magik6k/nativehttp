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
#include <iostream>

namespace http
{
	namespace statdata
	{

        namespace info
        {
            bool toggle;
            bool transfer;
            bool hitlog;
            bool method;

            int64_t hourlylen;
		}

        namespace transfer
        {
            uint64_t ulbytes;
            uint64_t dlbytes;

            uint64_t *hrl_ul;
            uint64_t *hrl_dl;
        }

        namespace activity
        {
            uint64_t hits;
            uint64_t connections;

            uint64_t *hrl_hits;
            uint64_t *hrl_connections;
        }


		namespace method
		{
            uint64_t get;
            uint64_t post;
		}

		time_t lastHrlFlp;
		time_t lastSave;

		time_t save_rate;

		uint16_t filever = 0x0002;
		bool managersafe = true;
		string stfn;

		void manage()
		{
			if (info::toggle && managersafe)
			{
				if (time(0) - lastHrlFlp >= 3600)
				{
					lastHrlFlp += 3600;
					for (int64_t i = info::hourlylen - 2LL; i >= 0; i--)
					{
						activity::hrl_hits[i + 1] = activity::hrl_hits[i];
						activity::hrl_connections[i + 1] = activity::hrl_connections[i];
						transfer::hrl_ul[i + 1] = transfer::hrl_ul[i];
						transfer::hrl_dl[i + 1] = transfer::hrl_dl[i];
					}
					activity::hrl_hits[0] = 0;
					activity::hrl_connections[0] = 0;
					transfer::hrl_ul[0] = 0;
					transfer::hrl_dl[0] = 0;
				}
				if (time(0) - lastSave >= save_rate)
				{
					lastSave += save_rate;
					http::statdata::save();
				}
			}
		}

		void save()
		{
			if (!stfn.empty())
			{
				FILE *stf = fopen(stfn.c_str(), "w");
				if (!stf)
				{
					stfn.clear();
					return;
				}
				fwrite("NSF", 1, 3, stf);
				fwrite(&filever, 2, 1, stf);
				fwrite(&info::hourlylen, sizeof(int64_t), 1, stf);

				fwrite(&method::get, sizeof(uint64_t), 1, stf);
				fwrite(&method::post, sizeof(uint64_t), 1, stf);

				stunit sd = {0, 0, 0, 0};
				sd.hits = activity::hits;
				sd.connections = activity::connections;
				sd.ulbytes = transfer::ulbytes;
				sd.dlbytes = transfer::dlbytes;

				fwrite(&sd, sizeof(stunit), 1, stf);

				for (int64_t i = 0LL; i < info::hourlylen; i++)
				{
					sd.hits = activity::hrl_hits[i];
					sd.connections = activity::hrl_connections[i];
					sd.ulbytes = transfer::hrl_ul[i];
					sd.dlbytes = transfer::hrl_dl[i];
					fwrite(&sd, sizeof(stunit), 1, stf);
				}

				fclose(stf);
			}
		}

	}

}
