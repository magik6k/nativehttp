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
			http::statdata::info::transfer = cfg->get_int("transfer_stats");
			http::statdata::info::hitlog = cfg->get_int("hits_stat");
			http::statdata::info::hourlylen = cfg->get_int("hourly_length");
			http::statdata::info::method = cfg->get_int("method_stats");

			http::statdata::lastHrlFlp = time(0) + 5;
			http::statdata::lastSave = time(0) + 30;

			http::statdata::save_rate = cfg->get_int("stat_save_rate") * 60;

			http::statdata::activity::hrl_hits = new uint64_t[http::statdata::info::hourlylen];
			http::statdata::activity::hrl_connections = new uint64_t[http::statdata::info::hourlylen];
			http::statdata::transfer::hrl_dl = new uint64_t[http::statdata::info::hourlylen];
			http::statdata::transfer::hrl_ul = new uint64_t[http::statdata::info::hourlylen];

			for (size_t i = 0; i < http::statdata::info::hourlylen; i++)
			{
				http::statdata::activity::hrl_hits[i] = 0u;
				http::statdata::activity::hrl_connections[i] = 0u;
				http::statdata::transfer::hrl_dl[i] = 0u;
				http::statdata::transfer::hrl_ul[i] = 0u;
			}

			http::statdata::activity::hits = 0u;
			http::statdata::activity::connections = 0u;
			http::statdata::transfer::dlbytes = 0u;
			http::statdata::transfer::ulbytes = 0u;
			http::statdata::method::get = 0u;
			http::statdata::method::post = 0u;

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
					nativehttp::server::log("init.cpp@stat", "Could not load stats due to old file format");
					fclose(stf);
					return;
				}

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
				}

				for (int64_t i = info::hourlylen - 2; i >= 0; i--)
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

				fclose(stf);
			}

#ifdef NHDBG
			cout << "[DBG:init.cpp@http]Stat mem: " << (getrsmem() - bm) / 1024.f << "kb\n";
#endif
		}

	}
}
