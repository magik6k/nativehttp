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

namespace http
{
	namespace statdata
	{
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

						websocket::hrl_msgs_recv[i + 1] = websocket::hrl_msgs_recv[i];
						websocket::hrl_msgs_sent[i + 1] = websocket::hrl_msgs_sent[i];

						session::hrl_max_existing[i + 1] = session::hrl_max_existing[i];

					}

					activity::hrl_hits[0] = 0;
					activity::hrl_connections[0] = 0;

					transfer::hrl_ul[0] = 0;
					transfer::hrl_dl[0] = 0;

					websocket::hrl_msgs_recv[0] = 0;
					websocket::hrl_msgs_sent[0] = 0;

					session::hrl_max_existing[0] = 0;

				}
				if (time(0) - lastSave >= save_rate)
				{
					lastSave += save_rate;
					save();
				}
			}
		}
	}
}
