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
			if (info::toggle && internal::managersafe)
			{
check_flip:
				if (time(0) - internal::lastHrlFlp >= 3600)
				{
					internal::lastHrlFlp += 3600;
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

					goto check_flip;
				}

				if (time(0) - internal::lastDlyFlp >= 3600 * 24)
				{
					internal::lastDlyFlp += 3600 * 24;
					for (int64_t i = info::dailylen - 2LL; i >= 0; i--)
					{
						activity::dly_hits[i + 1] = activity::dly_hits[i];
						activity::dly_connections[i + 1] = activity::dly_connections[i];

						transfer::dly_ul[i + 1] = transfer::dly_ul[i];
						transfer::dly_dl[i + 1] = transfer::dly_dl[i];

						websocket::dly_msgs_recv[i + 1] = websocket::dly_msgs_recv[i];
						websocket::dly_msgs_sent[i + 1] = websocket::dly_msgs_sent[i];

						websocket::dly_connections[i + 1] = websocket::dly_connections[i];
						websocket::dly_upload[i + 1] = websocket::dly_upload[i];
						websocket::dly_download[i + 1] = websocket::dly_download[i];

						shttp::dly_postdata[i + 1] = shttp::dly_postdata[i];
                        shttp::dly_recv_header_size[i + 1] = shttp::dly_recv_header_size[i];
                        shttp::dly_sent_header_size[i + 1] = shttp::dly_sent_header_size[i];

						session::dly_sessions_created[i + 1] = session::dly_sessions_created[i];

					}

					activity::dly_hits[0] = 0;
					activity::dly_connections[0] = 0;

					transfer::dly_ul[0] = 0;
					transfer::dly_dl[0] = 0;

					websocket::dly_msgs_recv[0] = 0;
					websocket::dly_msgs_sent[0] = 0;

					websocket::dly_connections[0] = 0;
					websocket::dly_upload[0] = 0;
					websocket::dly_download[0] = 0;

					shttp::dly_postdata[0] = 0;
					shttp::dly_recv_header_size[0] = 0;
					shttp::dly_sent_header_size[0] = 0;

					session::dly_sessions_created[0] = 0;

					goto check_flip;
				}

				if (time(0) - internal::lastWklFlp >= 3600 * 24 * 7)
				{
					internal::lastWklFlp += 3600 * 24 * 7;
					for (int64_t i = info::weeklylen - 2LL; i >= 0; i--)
					{
						activity::wkl_hits[i + 1] = activity::wkl_hits[i];
						activity::wkl_connections[i + 1] = activity::wkl_connections[i];

						transfer::wkl_ul[i + 1] = transfer::wkl_ul[i];
						transfer::wkl_dl[i + 1] = transfer::wkl_dl[i];

						websocket::wkl_msgs_recv[i + 1] = websocket::wkl_msgs_recv[i];
						websocket::wkl_msgs_sent[i + 1] = websocket::wkl_msgs_sent[i];

						session::wkl_sessions_created[i + 1] = session::wkl_sessions_created[i];
					}

					activity::wkl_hits[0] = 0;
					activity::wkl_connections[0] = 0;

                    transfer::wkl_ul[0] = 0;
                    transfer::wkl_dl[0] = 0;

                    websocket::wkl_msgs_recv[0] = 0;
                    websocket::wkl_msgs_sent[0] = 0;

                    session::wkl_sessions_created[0] = 0;

                    goto check_flip;
				}

				if (time(0) - internal::lastSave >= internal::save_rate)
				{
					internal::lastSave += internal::save_rate;
					save();
				}
			}
		}
	}
}
