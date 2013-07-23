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
#include "compability/compability.h"
#include "nativehttp.h"
#include "protocol.h"

namespace http
{
	namespace statdata
	{

		void init()
		{
#ifdef NHDBG
			size_t bm = getrsmem();
#endif
			http::statdata::info::toggle = cfg->get_int("statson");
            if(!http::statdata::info::toggle)return;

			http::statdata::info::hourlylen = cfg->get_int("hourly_length");
			http::statdata::info::dailylen = cfg->get_int("daily_length");
            http::statdata::info::weeklylen = cfg->get_int("weekly_length");

			http::statdata::lastHrlFlp = time(0) + 5;
			http::statdata::lastDlyFlp = time(0) + 5;
			http::statdata::lastWklFlp = time(0) + 5;
			http::statdata::lastSave = time(0) + 30;

			http::statdata::save_rate = cfg->get_int("stat_save_rate") * 60;


			http::statdata::activity::hrl_hits = new uint64_t[http::statdata::info::hourlylen];
			http::statdata::activity::hrl_connections = new uint64_t[http::statdata::info::hourlylen];
			http::statdata::activity::dly_hits = new uint64_t[http::statdata::info::dailylen];
			http::statdata::activity::dly_connections = new uint64_t[http::statdata::info::dailylen];
			http::statdata::activity::wkl_hits = new uint64_t[http::statdata::info::weeklylen];
			http::statdata::activity::wkl_connections = new uint64_t[http::statdata::info::weeklylen];


			http::statdata::transfer::hrl_dl = new uint64_t[http::statdata::info::hourlylen];
			http::statdata::transfer::hrl_ul = new uint64_t[http::statdata::info::hourlylen];
            http::statdata::transfer::dly_dl = new uint64_t[http::statdata::info::dailylen];
            http::statdata::transfer::dly_ul = new uint64_t[http::statdata::info::dailylen];
            http::statdata::transfer::wkl_dl = new uint64_t[http::statdata::info::weeklylen];
            http::statdata::transfer::wkl_ul = new uint64_t[http::statdata::info::weeklylen];


            http::statdata::websocket::dly_connections = new uint64_t[http::statdata::info::dailylen];
            http::statdata::websocket::dly_upload = new uint64_t[http::statdata::info::dailylen];
            http::statdata::websocket::dly_download = new uint64_t[http::statdata::info::dailylen];
            http::statdata::websocket::dly_msgs_sent = new uint64_t[http::statdata::info::dailylen];
            http::statdata::websocket::dly_msgs_recv = new uint64_t[http::statdata::info::dailylen];
            http::statdata::websocket::hrl_msgs_sent = new uint64_t[http::statdata::info::hourlylen];
            http::statdata::websocket::hrl_msgs_recv = new uint64_t[http::statdata::info::hourlylen];
            http::statdata::websocket::wkl_msgs_sent = new uint64_t[http::statdata::info::weeklylen];
            http::statdata::websocket::wkl_msgs_recv = new uint64_t[http::statdata::info::weeklylen];


            http::statdata::shttp::dly_postdata = new uint64_t[http::statdata::info::dailylen];
            http::statdata::shttp::dly_recv_header_size = new uint64_t[http::statdata::info::dailylen];
            http::statdata::shttp::dly_sent_header_size = new uint64_t[http::statdata::info::dailylen];


            http::statdata::session::hrl_max_existing = new uint64_t[http::statdata::info::hourlylen];
            http::statdata::session::dly_sessions_created = new uint64_t[http::statdata::info::dailylen];
            http::statdata::session::wkl_sessions_created = new uint64_t[http::statdata::info::weeklylen];


			for (size_t i = 0; i < http::statdata::info::hourlylen; i++)
			{
				http::statdata::activity::hrl_hits[i] = 0u;
				http::statdata::activity::hrl_connections[i] = 0u;
				http::statdata::transfer::hrl_dl[i] = 0u;
				http::statdata::transfer::hrl_ul[i] = 0u;

				http::statdata::websocket::hrl_msgs_sent[i] = 0u;
				http::statdata::websocket::hrl_msgs_recv[i] = 0u;

				http::statdata::session::hrl_max_existing[i] = 0u;
			}

			for (size_t i = 0; i < http::statdata::info::dailylen; i++)
			{
                http::statdata::activity::dly_hits[i] = 0u;
                http::statdata::activity::dly_connections[i] = 0u;

                http::statdata::transfer::dly_dl[i] = 0u;
                http::statdata::transfer::dly_ul[i] = 0u;

                http::statdata::websocket::dly_connections[i] = 0u;
                http::statdata::websocket::dly_upload[i] = 0u;
                http::statdata::websocket::dly_download[i] = 0u;
                http::statdata::websocket::dly_msgs_sent[i] = 0u;
                http::statdata::websocket::dly_msgs_recv[i] = 0u;

                http::statdata::shttp::dly_postdata[i] = 0u;
                http::statdata::shttp::dly_recv_header_size[i] = 0u;
                http::statdata::shttp::dly_sent_header_size[i] = 0u;

                http::statdata::session::dly_sessions_created[i] = 0u;
			}

			for (size_t i = 0; i < http::statdata::info::weeklylen; i++)
			{
                http::statdata::activity::wkl_hits[i] = 0u;
                http::statdata::activity::wkl_connections[i] = 0u;

                http::statdata::transfer::wkl_dl[i] = 0u;
                http::statdata::transfer::wkl_ul[i] = 0u;

                http::statdata::websocket::wkl_msgs_sent[i] = 0u;
                http::statdata::websocket::wkl_msgs_recv[i] = 0u;

                http::statdata::session::wkl_sessions_created[i] = 0u;
			}

			http::statdata::activity::hits = 0u;
			http::statdata::activity::connections = 0u;

			http::statdata::transfer::dlbytes = 0u;
			http::statdata::transfer::ulbytes = 0u;

			http::statdata::method::get = 0u;
			http::statdata::method::post = 0u;

			http::statdata::websocket::frames_recvd = 0u;
			http::statdata::websocket::msgs_recvd = 0u;
			http::statdata::websocket::frames_sent = 0u;
			http::statdata::websocket::msgs_sent = 0u;
			http::statdata::websocket::upload = 0u;
			http::statdata::websocket::download = 0u;
			http::statdata::websocket::connections = 0u;


			if (!cfg->get_var("statfile").empty())
			{
				stfn = cfg->get_var("statfile");

				FILE *stf = fopen(cfg->get_var("statfile").c_str(), "r");
				if (!stf)return;

				char ftc[3];
				fread(ftc, 1, 3, stf);
				if ((ftc[0] != 'N') || (ftc[1] != 'S') || (ftc[2] != 'F'))
				{
					fclose(stf);
					return;
				}
				uint16_t tfv = 0x0000;
				fread(&tfv, 2, 1, stf);
				if (tfv != filever)
				{
					nativehttp::server::log("init.cpp@stat", "Trying to load stat file through compability module");
					if(!compability::tryload(stf))
                        nativehttp::server::log("init.cpp@stat", "Stat loading failed - new stat file will be generated!");
                        else nativehttp::server::log("init.cpp@stat", "Old stat file loaded succesfully");
					return;
				}

				file::head head;
				file::totals total;

				fread(&head,sizeof(head),1,stf);
				fread(&total,sizeof(total),1,stf);

				transfer::ulbytes = total.upload;
                transfer::dlbytes = total.download;

                activity::hits = total.hits;
                activity::hits = total.connections;

                method::get = total.req_get;
                method::post = total.req_post;

                websocket::frames_recvd = total.ws_frames_rec;
                websocket::frames_sent = total.ws_frames_snt;
                websocket::msgs_recvd = total.ws_msgs_rec;
                websocket::msgs_sent = total.ws_msgd_snd;
                websocket::upload = total.ws_upload;
                websocket::download = total.ws_download;
                websocket::connections = total.ws_connections;

                shttp::recv_header_size = total.http_rec_head_size;
                shttp::sent_header_size = total.http_snd_head_size;
                shttp::postdata_size = total.http_post_size;

                session::sessions_created = total.sess_created;


				for(int64_t i=0;i<head.hourlylen&&i<info::hourlylen;i++)
				{
                    file::hourly hour;
                    fread(&hour,sizeof(hour),1,stf);

                    transfer::hrl_ul[i] = hour.upload;
					transfer::hrl_dl[i] = hour.download;

					activity::hrl_hits[i] = hour.hits;
					activity::hrl_connections[i] = hour.connections;

					websocket::hrl_msgs_recv[i] = hour.ws_msgs_recv;
					websocket::hrl_msgs_sent[i] = hour.ws_msgs_sent;

					session::hrl_max_existing[i] = hour.max_sessions;
				}

				for(int64_t i=0;i<head.dailylen&&i<info::dailylen;i++)
				{
                    file::daily day;
                    fread(&day,sizeof(day),1,stf);

                    transfer::dly_ul[i] = day.upload;
					transfer::dly_dl[i] = day.download;

					activity::dly_hits[i] = day.hits;
					activity::dly_connections[i] = day.connections;

                    websocket::dly_connections[i] = day.ws_connections;
                    websocket::dly_upload[i] = day.ws_upload;
                    websocket::dly_download[i] = day.ws_download;
					websocket::dly_msgs_recv[i] = day.ws_msgs_recv;
					websocket::dly_msgs_sent[i] = day.ws_msgs_sent;

					shttp::dly_postdata[i] = day.postdata_size;
					shttp::dly_recv_header_size[i] = day.http_recv_header_size;
					shttp::dly_sent_header_size[i] = day.http_sent_header_size;

					session::dly_sessions_created[i] = day.sessions_created;
				}

				for(int64_t i=0;i<head.weeklylen&&i<info::weeklylen;i++)
				{
                    file::weekly week;
                    fread(&week,sizeof(week),1,stf);

                    transfer::wkl_ul[i] = week.upload;
					transfer::wkl_dl[i] = week.download;

					activity::wkl_hits[i] = week.hits;
					activity::wkl_connections[i] = week.connections ;

					websocket::wkl_msgs_recv[i] = week.ws_msgs_recv;
					websocket::wkl_msgs_sent[i] = week.ws_msgs_sent;

					session::wkl_sessions_created[i] = week.sessions_created;
				}

				fclose(stf);
			}

#ifdef NHDBG
			cout << "[DBG:init.cpp@http]Stat mem: " << (getrsmem() - bm) / 1024.f << "kb\n";
#endif
		}

	}
}
