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
#include "data/queue.h"

#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <openssl/ssl.h>

#ifndef SOCKET
#define SOCKET int
#endif

using namespace std;

#define HTTP_MAX_USER_HEADER_SIZE 8194 //orginal 2^13+2

namespace http
{

	struct request
	{
		SOCKET sender;
		const char *request;
		int taken;
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
		char *data;
		bool fas;//free after sending
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

#endif

	extern int maxConnections;
	extern unsigned int maxPost;

	extern bool *ulock;
	extern SOCKET *connected;
	extern SOCKET server;
	extern uint32_t *client_ips;

	extern bool onssl;
	extern SSL **sslsck;

	extern SDLNet_SocketSet CSet;

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

	extern SDL_mutex *mtx_exec2;
	extern SDL_mutex *mtx_exec;
	extern SDL_mutex *mtx_snd;

	extern pthread_t *theard_nc;
	extern pthread_t **theard_sd;
	extern pthread_t *theard_mg;

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
