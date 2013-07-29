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
#include "filesender.h"
#include "data/session/session.h"
#include "net/net.h"
#include "utils/thread.h"
#include "utils/version.h"
#include "protocol.h"
#include "ws/ws.h"
#include "utils/memory.h"
#ifdef NHDBG
#include <iostream>
#endif

namespace http
{
	void datainit()
	{
#ifdef NHDBG
		size_t bm = getrsmem();

		http::extmemstats = cfg->get_int("extended_native_memory_stats");

		http::log_httprep = cfg->get_int("log_httprep");
		http::log_newline = cfg->get_int("log_newline_visible");
		http::log_requests = cfg->get_int("log_requests");
		http::log_sender = cfg->get_int("log_sender");

#endif

        if (!cfg->get_var("log").empty())
        {
            logfile.open(cfg->get_var("log"), std::ofstream::out | std::ofstream::app);
            nativehttp::server::log("INIT","Openning logger file");
        }
		http::maxConnections = cfg->get_int("maxconnections");
		http::maxPost = cfg->get_int("max_post");

		http::connected = utils::memory::alloc<SOCKET>(http::maxConnections);
		http::ulock = utils::memory::alloc<bool>(http::maxConnections);
		http::client_ips = utils::memory::alloc<uint32_t>(http::maxConnections);
		http::client_protocol = utils::memory::alloc<uint8_t>(http::maxConnections);
		http::client_prot_data = utils::memory::alloc<uint32_t>(http::maxConnections);
		http::packets_sent = utils::memory::alloc<uint64_t>(http::maxConnections);
		http::packets_to_send = utils::memory::alloc<uint64_t>(http::maxConnections);

		if (cfg->get_int("use_ssl"))http::sslsck = utils::memory::alloc<SSL*>(http::maxConnections);

		for (int i = 0; i < http::maxConnections; i++)
		{
			http::connected[i] = INVALID_SOCKET;
			http::ulock[i] = false;
			http::client_protocol[i] = CLPROT_HTTP;
			http::client_prot_data[i] = 0;
			http::packets_sent[i] = 0LL;
			http::packets_to_send[i] = 0LL;

			if (cfg->get_int("use_ssl"))http::sslsck[i] = NULL;
		}

		http::Nexec = cfg->get_int("exec_theards");
		http::Nsend = cfg->get_int("send_theards");

		http::execUnits = utils::memory::alloc<http::Sexecutor>(http::Nexec);
		http::theard_sd = utils::memory::alloc<pthread_t*>(http::Nsend);

		if(cfg->get_int("ws_enable"))http::theard_ws = utils::memory::alloc<pthread_t>(cfg->get_int("ws_fproc_threads"));

		http::headers::alivetimeout = cfg->get_var("normal_keep") + "\r\n";
		http::mExecQ = cfg->get_int("maxexecutionqueue");
		http::exec_heap_size = cfg->get_int("exec_heap");
		http::asyncsnd = cfg->get_int("async_send");
		http::onssl = cfg->get_int("use_ssl");

        http::sqln = 100;
        http::shq = utils::memory::alloc<fsrq_run>(http::sqln);
        for(size_t i=0;i<http::sqln;i++)http::shq[i].uid=-1;
        fsnd_fb_size = cfg->get_int("fsn_frame_buf_size");

        if(cfg->get_int("global_pexec_mutex"))
            http::mtx_exec2 = utils::create_mutex();

		http::mtx_exec = utils::create_mutex();
		http::mtx_snd = utils::create_mutex();
		http::mtx_fsnd = utils::create_mutex();
		http::mtx_wsrc = utils::create_mutex();

		http::manager::rate = cfg->get_int("managerrate");
		http::manager::postto = cfg->get_int("posttimeout");
		http::manager::execto = cfg->get_int("exectimeout");
		http::manager::apr = cfg->get_int("autopagerefresh");
		http::manager::fileloc = cfg->get_var("files_location");

		http::error::e400 = http::error::load_error(cfg->get_var("error400"), "400 Bad Request");
		http::error::e403 = http::error::load_error(cfg->get_var("error403"), "403 Forbidden");
		http::error::e404 = http::error::load_error(cfg->get_var("error404"), "404 Not Found");
		http::error::e500 = http::error::load_error(cfg->get_var("error500"), "500 Internal Server Error");
		http::error::e501 = http::error::load_error(cfg->get_var("error501"), "501 Not Implemented");
		http::error::e505 = http::error::load_error(cfg->get_var("error505"), "505 HTTP Version Not Supported");

		http::usesessions = cfg->get_int("use_sessions");
		sess_ssid_cnam = cfg->get_var("cookie_ssid_name");
		sess_sslc_cnam = cfg->get_var("cookie_sslc_name");
		max_sesions = cfg->get_int("max_sessions");
		sess_life = cfg->get_int("session_life");
		sdata::session::storage.prealloc(cfg->get_int("inital_sessions"));

		http::nhpc_include_dir = cfg->get_var("include_dir");
		http::nhpc_nativehttph_dest = cfg->get_var("nativehttph_dst");

		http::logger_colors = cfg->get_int("logger_colors");

		if(utils::header_version_ok())
        {
#ifdef NHDBG
            nativehttp::server::log("DBG:init.cpp@http","nativehttp.h Versions matches");
#endif
        }
        else
        {
            nativehttp::server::err("init.cpp@http","[CRITICAL]nativehttp.h Version mismatch");
            exit(2);
        }

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

			pthread_t *tt = new pthread_t;
			int tms = utils::create_thread(tt, http::executor, &(http::execUnits[i]),http::exec_heap_size);

			http::execUnits[i].etheard = tt;
			if (tms != 0)nativehttp::server::logid(i, "init.cpp", "Executor failed to start");
		}
#ifdef NHDBG
		utils::sleep(250);
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
		utils::sleep(250);
		cout << "[DBG:init.cpp@http]net init mem: " << ((getrsmem() - bm) / 1024.f) + ((getacmem() - abm) / 1024.f) << "kb\n";
#endif
	}
	void startsystem()
	{
		http::statdata::init();

#ifdef NHDBG
		size_t bm = getacmem();
#endif

		pthread_t *tt = new pthread_t;

		int tms;
		if (cfg->get_int("use_ssl"))
		{
            tms = utils::create_thread(tt, http::ssl::listener, NULL, 16 * 1024);
		}
		else
		{
			tms = utils::create_thread(tt, http::bsd::listener, NULL, 16 * 1024);
		}

		http::theard_nc = tt;
		if (tms != 0)nativehttp::server::log("init.cpp", "ANC failed to start");


		for (int i = 0; i < http::Nsend; i++)
		{
			pthread_t *tmt = new pthread_t;

			int tmks;

			if (cfg->get_int("use_ssl"))
			{
                tmks = utils::create_thread(tmt, http::ssl::sender, new int(i), 16 * 1024);
			}
			else
			{
				tmks = utils::create_thread(tmt, http::bsd::sender, new int(i), 16 * 1024);
			}

			http::theard_sd[i] = tmt;
			if (tmks != 0)nativehttp::server::logid(i, "init.cpp", "Sender failed to start");
		}

		tt = new pthread_t;
		tms = utils::create_thread(tt, http::fsender, NULL, 256 * 1024);
		http::theard_fs = tt;
		if (tms != 0)nativehttp::server::log("init.cpp", "File Sender failed to start");

		tt = new pthread_t;
		tms = utils::create_thread(tt, http::manager::manager, NULL, 256 * 1024);
		http::theard_mg = tt;
		if (tms != 0)nativehttp::server::log("init.cpp", "Manager failed to start");

		if(ws::enabled)
		{
            int tn = cfg->get_int("ws_fproc_threads");
            size_t heapsz = cfg->get_int("ws_fproc_heap");
            for(int i=0;i<tn;i++)
            {
                utils::create_thread(&(http::theard_ws[i]), ws::rmsgproc, NULL, heapsz);
            }
        }

#ifdef NHDBG
		utils::sleep(250);
		cout << "[DBG:init.cpp@http]System init mem: "
		     << (getacmem() - bm) / 1024LL << "kb\n";
		cout << "[DBG:init.cpp@http]Total init mem: "
		     << (getacmem() + getrsmem()) / 1024LL << "kb("
		     << (getacmem() + getrsmem()) / 1024LL / 1024LL << "mb)\n";
		http::init_memory = getacmem() + getrsmem();
#endif
	}


}

