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
#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include "nativehttp.h"
#include "protocol.h"
#include "filesender.h"
#include "data/queue.h"
#include "utils/thread.h"

#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <openssl/ssl.h>

#ifndef SOCKET
#define SOCKET int
#endif

using namespace std;

#define HTTP_MAX_USER_HEADER_SIZE 8194 //orginal 2^13+2

struct tms
{
    time_t tv_sec; /* seconds */
    long tv_nsec; /* nanoseconds */
};

namespace http
{

	struct request
	{
		SOCKET sender;
		const char *request;
		int uid;

		int method;
		bool http11;
		void *post;

		void free();
	};

	struct outdata
	{
		int uid;
		unsigned long size;
		const char *data;
		bool fas;//free after sending
		uint64_t pktid;
		uint32_t lost_ptk;
	};

	struct Sexecutor
	{
		pthread_t *etheard;
		long int state;//time execution/post reciving started OR -1 when waiting or requies stability
		int in;//3 - kick, 2 - exec, 1 - post recv/kicki, 0-nothing
		int id;

		void *fd1;
		void *fd2;
	};

#ifdef NHDBG

	extern size_t init_memory;
	extern bool extmemstats;

	extern bool log_requests;
	extern bool log_httprep;
	extern bool log_sender;
	extern bool log_newline;

	extern bool log_detailed;

#endif

    extern bool log_requests_short;

    extern int backtrace_len;

	extern int maxConnections;
	extern unsigned int maxPost;

	extern bool *ulock;
	extern SOCKET *connected;
	extern SOCKET server;
	extern uint32_t *client_ips;
	extern uint8_t *client_protocol;
	extern uint32_t *client_prot_data;
	extern uint64_t *packets_sent;
    extern uint64_t *packets_to_send;

	extern bool onssl;
	extern SSL **sslsck;

	extern int Nexec;
	extern int Nsend;
	extern Sexecutor *execUnits;
	extern size_t exec_heap_size;
	extern int asyncsnd;

	extern int usesessions;
	extern string sess_ssid_cnam;
	extern string sess_sslc_cnam;
	extern size_t max_sesions;
	extern time_t sess_life;

	extern uint32_t mExecQ;

	extern nativehttp::data::queue<request*>toexec;
	extern nativehttp::data::queue<outdata>tosend;

	extern nativehttp::data::queue<fsrq>fsend;//file sender
	extern fsrq_run* shq;
	extern size_t sqln;
	extern size_t fsnd_fb_size;

	extern pthread_mutex_t *mtx_exec2;
	extern pthread_mutex_t *mtx_fsnd;
	extern pthread_mutex_t *mtx_wsrc;

	extern utils::condex *cdx_snd;
	extern utils::condex *cdx_exec;

	extern pthread_t *theard_nc;
	extern pthread_t **theard_sd;
	extern pthread_t *theard_mg;
	extern pthread_t *theard_fs;
	extern pthread_t *theard_ws;

	extern string nhpc_include_dir;
	extern string nhpc_nativehttph_dest;

	extern bool logger_colors;

	extern bool http_realip_use;
    extern string http_realip_str;

	extern tms s_cdx_wakeup;

	namespace headers
	{
		extern string standard;
		extern string alive;
		extern string alivetimeout;
	}

	namespace error
	{
		extern nativehttp::data::pagedata e400;
		extern nativehttp::data::pagedata e403;
		extern nativehttp::data::pagedata e404;
		extern nativehttp::data::pagedata e500;
		extern nativehttp::data::pagedata e501;
		extern nativehttp::data::pagedata e505;
	}

	namespace manager
	{
		extern int rate;
		extern int postto;
		extern int execto;
		extern bool apr;
		extern string fileloc;
	}

}

#endif // DATA_H_INCLUDED
