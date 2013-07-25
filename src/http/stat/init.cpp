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

			http::statdata::internal::lastHrlFlp = time(0) + 5;
			http::statdata::internal::lastDlyFlp = time(0) + 5;
			http::statdata::internal::lastWklFlp = time(0) + 5;
			http::statdata::internal::lastSave = time(0) + 30;

			http::statdata::internal::save_rate = cfg->get_int("stat_save_rate") * 60;


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


			load();


#ifdef NHDBG
			cout << "[DBG:init.cpp@http]Stat mem: " << (getrsmem() - bm) / 1024.f << "kb\n";
#endif
		}

	}
}
