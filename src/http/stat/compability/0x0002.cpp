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
#include "../../stat.h"
#include "compability.h"

namespace http
{
	namespace statdata
	{

        namespace compability
        {
            bool load_0x0002(FILE *stf)
            {
                int64_t thl = 0;
				fread(&thl, sizeof(int64_t), 1, stf);
				if (thl > info::hourlylen)thl = info::hourlylen;

				fread(&method::get, sizeof(uint64_t), 1, stf);
				fread(&method::post, sizeof(uint64_t), 1, stf);

				stunit sd = {0, 0, 0, 0};

				fread(&sd, sizeof(stunit), 1, stf);

				activity::hits = sd.hits;
				activity::connections = sd.connections;
				transfer::ulbytes = sd.ulbytes;
				transfer::dlbytes = sd.dlbytes;

				for (int64_t i = 0; i < thl; i++)
				{
					fread(&sd, sizeof(stunit), 1, stf);
					activity::hrl_hits[i] = sd.hits;
					activity::hrl_connections[i] = sd.connections;
					transfer::hrl_ul[i] = sd.ulbytes;
					transfer::hrl_dl[i] = sd.dlbytes;

					if(thl/24 < info::dailylen)
					{
                        activity::dly_hits[i] += sd.hits;
                        activity::dly_connections[i] += sd.connections;
                        transfer::dly_ul[i] += sd.ulbytes;
                        transfer::dly_dl[i] += sd.dlbytes;
					}
					if(thl/(24*7) < info::weeklylen)
					{
                        activity::wkl_hits[i] += sd.hits;
                        activity::wkl_connections[i] += sd.connections;
                        transfer::wkl_ul[i] += sd.ulbytes;
                        transfer::wkl_dl[i] += sd.dlbytes;
					}
				}

				for (int64_t i = info::hourlylen - 2; i >= 0; i--)
				{
					activity::hrl_hits[i + 1] = activity::hrl_hits[i];
					activity::hrl_connections[i + 1] = activity::hrl_connections[i];
					transfer::hrl_ul[i + 1] = transfer::hrl_ul[i];
					transfer::hrl_dl[i + 1] = transfer::hrl_dl[i];
				}

				for (int64_t i = info::dailylen - 2; i >= 0; i--)
				{
					activity::dly_hits[i + 1] = activity::dly_hits[i];
					activity::dly_connections[i + 1] = activity::dly_connections[i];
					transfer::dly_ul[i + 1] = transfer::dly_ul[i];
					transfer::dly_dl[i + 1] = transfer::dly_dl[i];
				}

				for (int64_t i = info::weeklylen - 2; i >= 0; i--)
				{
					activity::wkl_hits[i + 1] = activity::wkl_hits[i];
					activity::wkl_connections[i + 1] = activity::wkl_connections[i];
					transfer::wkl_ul[i + 1] = transfer::wkl_ul[i];
					transfer::wkl_dl[i + 1] = transfer::wkl_dl[i];
				}

				if(info::hourlylen)
				{
                    activity::hrl_hits[0] = 0;
                    activity::hrl_connections[0] = 0;
                    transfer::hrl_ul[0] = 0;
                    transfer::hrl_dl[0] = 0;
				}

				if(info::dailylen)
				{
                    activity::dly_hits[0] = 0;
                    activity::dly_connections[0] = 0;
                    transfer::dly_ul[0] = 0;
                    transfer::dly_dl[0] = 0;
				}

				if(info::weeklylen)
				{
                    activity::wkl_hits[0] = 0;
                    activity::wkl_connections[0] = 0;
                    transfer::wkl_ul[0] = 0;
                    transfer::wkl_dl[0] = 0;
                }

				fclose(stf);
                return true;
            }
        }

	}

}

