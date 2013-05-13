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

#include "nativehttp.h"
#include "init.h"
#include "data.h"
#include "executor.h"
#include "error.h"
#include "manager.h"
#include "stat.h"
#include "data/session/session.h"
#include "net/net.h"
#ifdef NHDBG
#include "protocol.h"
#include <iostream>
#endif

namespace http
{
	void sdlinit()
	{
#ifdef NHDBG
		size_t bm = getacmem();
#endif
		SDL_Init(SDL_INIT_TIMER);
#ifdef NHDBG
		cout << "[DBG:init.cpp@http]SDL init mem: " << ((getacmem()) - bm) / 1024.f << "kb\n";
#endif
	}
	void datainit()
	{
#ifdef NHDBG
		size_t bm = getrsmem();

		http::extmemstats = cfg->get_int("extended_native_memory_stats");

#endif
		http::maxConnections = cfg->get_int("maxconnections");
		http::maxPost = cfg->get_int("max_post");

		http::connected = new SOCKET[http::maxConnections];
		http::ulock = new bool[http::maxConnections];
		http::client_ips = new uint32_t[http::maxConnections];
		http::sslsck = new SSL*[http::maxConnections];
		for (int i = 0; i < http::maxConnections; i++)
		{
			http::connected[i] = INVALID_SOCKET;
			http::ulock[i] = false;
			http::sslsck[i] = NULL;
		}

		http::Nexec = cfg->get_int("exec_theards");
		http::Nsend = cfg->get_int("send_theards");

		http::execUnits = new http::Sexecutor[http::Nexec];
		http::theard_sd = new pthread_t*[http::Nsend];

		http::headers::alivetimeout = cfg->get_var("normal_keep") + "\r\n";
		http::mExecQ = cfg->get_int("maxexecutionqueue");
		http::exec_heap_size = cfg->get_int("exec_heap");
		http::asyncsnd = cfg->get_int("async_send");
		http::onssl = cfg->get_int("use_ssl");

		http::mtx_exec2 = SDL_CreateMutex();
		http::mtx_exec = SDL_CreateMutex();
		http::mtx_snd = SDL_CreateMutex();

		http::manager::rate = cfg->get_int("managerrate");
		http::manager::postto = cfg->get_int("posttimeout");
		http::manager::execto = cfg->get_int("exectimeout");
		http::manager::apr = cfg->get_int("autopagerefresh");
		http::manager::fileloc = cfg->get_var("files_location");

		http::error::e400 = http::error::load_error(cfg->get_var("error400"), "400 Bad Request");
		http::error::e403 = http::error::load_error(cfg->get_var("error403"), "403 Forbidden");
		http::error::e404 = http::error::load_error(cfg->get_var("error404"), "404 Not Found");
		http::error::e501 = http::error::load_error(cfg->get_var("error501"), "501 Not Implemented");
		http::error::e505 = http::error::load_error(cfg->get_var("error505"), "505 HTTP Version Not Supported");

		http::usesessions = cfg->get_int("use_sessions");
		sess_ssid_cnam = cfg->get_var("cookie_ssid_name");
		sess_sslc_cnam = cfg->get_var("cookie_sslc_name");
		max_sesions = cfg->get_int("max_sessions");
		sess_life = cfg->get_int("session_life");
		sdata::session::storage.prealloc(cfg->get_int("inital_sessions"));

		if (!cfg->get_var("log").empty())logfile.open(cfg->get_var("log"), std::ofstream::out | std::ofstream::app);

#ifdef NHDBG
		cout << "[DBG:init.cpp@http]Server data mem: " << (getrsmem() - bm) / 1024.f << "kb\n";
#endif

	}
	void executorinit()
	{
#ifdef NHDBG
		size_t abm = getacmem();
#endif
		for (int i = 0; i < http::Nexec; i++)
		{
			http::execUnits[i].state = -1;
			http::execUnits[i].in = 0;
			http::execUnits[i].id = i;

			http::execUnits[i].etheard = new pthread_t;

			pthread_attr_t at;
			if (pthread_attr_init(&at) != 0)
			{
				nativehttp::server::log("init.cpp@http", "ERROR: executor attr setup failed");
			}
			if (pthread_attr_setstacksize(&at, http::exec_heap_size) != 0)
			{
				nativehttp::server::log("init.cpp@http", "ERROR: Setting executor heap size failed");
			}

			pthread_t *tt = new pthread_t;
			int tms = pthread_create(tt, &at, http::executor, &(http::execUnits[i]));

			http::execUnits[i].etheard = tt;
			if (tms != 0)nativehttp::server::logid(i, "init.cpp", "Executor failed to start");
		}
#ifdef NHDBG
		SDL_Delay(250);
		cout << "[DBG:init.cpp@http]executors mem: " << (getacmem() - abm) / 1024.f << "kb\n";
#endif
	}
	void netstart()
	{
#ifdef NHDBG
		size_t abm = getacmem();
		size_t bm = getrsmem();
#endif
		if (cfg->get_int("use_ssl"))
		{
			http::ssl::init();
		}
		http::bsd::init();
#ifdef NHDBG
		SDL_Delay(250);
		cout << "[DBG:init.cpp@http]net init mem: " << ((getrsmem() - bm) / 1024.f) + ((getacmem() - abm) / 1024.f) << "kb\n";
#endif
	}
	void startsystem()
	{
		http::statdata::init();

#ifdef NHDBG
		size_t bm = getacmem();
#endif

		pthread_attr_t at;
		if (pthread_attr_init(&at) != 0)
		{
			nativehttp::server::log("init.cpp@http", "ERROR: attr setup failed");
		}
		if (pthread_attr_setstacksize(&at, 16 * 1024) != 0)     //16kb is minimal
		{
			nativehttp::server::log("init.cpp@http", "ERROR: Setting heap size failed");
		}

		pthread_t *tt = new pthread_t;

		int tms;
		if (cfg->get_int("use_ssl"))
		{
			tms = pthread_create(tt, &at, http::ssl::listener, NULL);
		}
		else
		{
			tms = pthread_create(tt, &at, http::bsd::listener, NULL);
		}


		http::theard_nc = tt;
		if (tms != 0)nativehttp::server::log("init.cpp", "ANC failed to start");

		for (int i = 0; i < http::Nsend; i++)
		{
			pthread_t *tmt = new pthread_t;

			int tmks;

			if (cfg->get_int("use_ssl"))
			{
				tmks = pthread_create(tmt, &at, http::ssl::sender, new int(i));
			}
			else
			{
				tmks = pthread_create(tmt, &at, http::bsd::sender, new int(i));
			}



			http::theard_sd[i] = tmt;
			if (tmks != 0)nativehttp::server::logid(i, "init.cpp", "Sender failed to start");
		}

		if (pthread_attr_setstacksize(&at, 256 * 1024) != 0)
		{
			nativehttp::server::log("init.cpp@http", "ERROR: Setting manager heap size failed");
		}

		tt = new pthread_t;
		tms = pthread_create(tt, &at, http::manager::manager, NULL);

		http::theard_mg = tt;
		if (tms != 0)nativehttp::server::log("init.cpp", "Manager failed to start");

#ifdef NHDBG
		SDL_Delay(250);
		cout << "[DBG:init.cpp@http]System init mem: "
		     << (getacmem() - bm) / 1024LL << "kb\n";
		cout << "[DBG:init.cpp@http]Total init mem: "
		     << (getacmem() + getrsmem()) / 1024LL << "kb("
		     << (getacmem() + getrsmem()) / 1024LL / 1024LL << "mb)\n";
		http::init_memory = getacmem() + getrsmem();
#endif
	}


}

