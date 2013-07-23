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

            int64_t hourlylen;
            int64_t dailylen;
            int64_t weeklylen;

		}

        namespace transfer
        {
            uint64_t ulbytes;
            uint64_t dlbytes;

            uint64_t *hrl_ul;
            uint64_t *hrl_dl;

            uint64_t *dly_ul;
            uint64_t *dly_dl;

            uint64_t *wkl_ul;
            uint64_t *wkl_dl;
        }

        namespace activity
        {
            uint64_t hits;
            uint64_t connections;

            uint64_t *hrl_hits;
            uint64_t *hrl_connections;

            uint64_t *dly_hits;
            uint64_t *dly_connections;

            uint64_t *wkl_hits;
            uint64_t *wkl_connections;
        }


		namespace method
		{
            uint64_t get;
            uint64_t post;
		}

		namespace websocket
		{
            uint64_t frames_recvd;
            uint64_t msgs_recvd;

            uint64_t frames_sent;
            uint64_t msgs_sent;

            uint64_t upload;
            uint64_t download;

            uint64_t connections;

            uint64_t* dly_connections;

            uint64_t* dly_upload;
            uint64_t* dly_download;


            uint64_t* hrl_msgs_sent;
            uint64_t* hrl_msgs_recv;

            uint64_t* dly_msgs_sent;
            uint64_t* dly_msgs_recv;

            uint64_t* wkl_msgs_sent;
            uint64_t* wkl_msgs_recv;
		}

		namespace shttp
		{
            uint64_t recv_header_size;
            uint64_t sent_header_size;

            uint64_t postdata_size;

            uint64_t* dly_postdata;

            uint64_t* dly_recv_header_size;
            uint64_t* dly_sent_header_size;
		}

		namespace session
		{
            uint64_t sessions_created;

            uint64_t* dly_sessions_created;
            uint64_t* wkl_sessions_created;

            uint64_t* hrl_max_existing;
		}

		time_t lastHrlFlp;
		time_t lastDlyFlp;
		time_t lastWklFlp;
		time_t lastSave;

		time_t save_rate;

		uint16_t filever = 0x0003;
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
					save();
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

				{
                    file::head head;
                    head.hourlylen = info::hourlylen;
                    head.dailylen = info::dailylen;
                    head.weeklylen = info::weeklylen;
                    fwrite(&head, sizeof(head), 1, stf);
				}

                {
                    file::totals total;

                    total.upload = transfer::ulbytes;
                    total.download = transfer::dlbytes;

                    total.hits = activity::hits;
                    total.connections = activity::hits;

                    total.req_get = method::get;
                    total.req_post = method::post;

                    total.ws_frames_rec = websocket::frames_recvd;
                    total.ws_frames_snt = websocket::frames_sent;
                    total.ws_msgs_rec = websocket::msgs_recvd;
                    total.ws_msgd_snd = websocket::msgs_sent;
                    total.ws_upload = websocket::upload;
                    total.ws_download = websocket::download;
                    total.ws_connections = websocket::connections;

                    total.http_rec_head_size = shttp::recv_header_size;
                    total.http_snd_head_size = shttp::sent_header_size;
                    total.http_post_size = shttp::postdata_size;

                    total.sess_created = session::sessions_created;

                    fwrite(&total, sizeof(total), 1, stf);
                }


				for (int64_t i = 0LL; i < info::hourlylen; i++)
				{
					file::hourly hour;
					hour.upload = transfer::hrl_ul[i];
					hour.download = transfer::hrl_dl[i];

					hour.hits = activity::hrl_hits[i];
					hour.connections = activity::hrl_connections[i];

					hour.ws_msgs_recv = websocket::hrl_msgs_recv[i];
					hour.ws_msgs_sent = websocket::hrl_msgs_sent[i];

					hour.max_sessions = session::hrl_max_existing[i];

					fwrite(&hour, sizeof(hour), 1, stf);
				}

				for (int64_t i = 0LL; i < info::dailylen; i++)
				{
					file::daily day;

					day.upload = transfer::dly_ul[i];
					day.download = transfer::dly_dl[i];

					day.hits = activity::dly_hits[i];
					day.connections = activity::dly_connections[i];

                    day.ws_connections = websocket::dly_connections[i];
                    day.ws_upload = websocket::dly_upload[i];
                    day.ws_download = websocket::dly_download[i];
					day.ws_msgs_recv = websocket::dly_msgs_recv[i];
					day.ws_msgs_sent = websocket::dly_msgs_sent[i];

					day.postdata_size = shttp::dly_postdata[i];
					day.http_recv_header_size = shttp::dly_recv_header_size[i];
					day.http_sent_header_size = shttp::dly_sent_header_size[i];

					day.sessions_created = session::dly_sessions_created[i];

					fwrite(&day, sizeof(day), 1, stf);
				}

				for (int64_t i = 0LL; i < info::weeklylen; i++)
				{
					file::weekly week;
					week.upload = transfer::wkl_ul[i];
					week.download = transfer::wkl_dl[i];

					week.hits = activity::wkl_hits[i];
					week.connections = activity::wkl_connections[i];

					week.ws_msgs_recv = websocket::wkl_msgs_recv[i];
					week.ws_msgs_sent = websocket::wkl_msgs_sent[i];

					week.sessions_created = session::wkl_sessions_created[i];

					fwrite(&week, sizeof(week), 1, stf);
				}

				fclose(stf);
			}
		}

	}

}
