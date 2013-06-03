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
#include "data.h"
#include "net/net.h"
#include "filesender.h"

namespace http
{

#ifdef NHDBG

	size_t init_memory = 0;
	bool extmemstats = false;

#endif

	int maxConnections = 0;
	unsigned int maxPost = 0;

	bool *ulock = NULL;
	uint32_t *client_ips = NULL;
	SOCKET *connected = NULL;
	SOCKET server = INVALID_SOCKET;
	uint8_t *client_protocol = NULL;
	uint32_t *client_prot_data = NULL;

	SSL **sslsck = NULL;
	bool onssl = false;

	int Nexec;
	int Nsend;
	Sexecutor *execUnits = NULL;
	size_t exec_heap_size;
	int asyncsnd = 0;

	int usesessions = 0;
	string sess_ssid_cnam;
	string sess_sslc_cnam;
	size_t max_sesions = 0;
	time_t sess_life = 0;

	uint32_t mExecQ;

	nativehttp::data::queue<request*>toexec;
	nativehttp::data::queue<outdata>tosend;

	nativehttp::data::queue<fsrq>fsend;
	fsrq_run* shq = NULL;
	size_t sqln = 0;
	size_t fsnd_fb_size;

	SDL_mutex *mtx_exec2 = NULL;
	SDL_mutex *mtx_exec = NULL;
	SDL_mutex *mtx_snd = NULL;
	SDL_mutex *mtx_fsnd = NULL;

	pthread_t *theard_nc = NULL;
	pthread_t **theard_sd = NULL;
	pthread_t *theard_mg = NULL;
	pthread_t *theard_fs = NULL;

	string nhpc_include_dir;
	string nhpc_nativehttph_dest;

	namespace headers
	{
		string standard = "Allow: GET, POST\r\nServer: NativeHTTP/0.1.1\r\n";
		string alive = "Connection: keep-alive\r\nKeep-Alive: timeout=";
		string alivetimeout;
	}

	namespace error
	{
		nativehttp::data::pagedata e400;
		nativehttp::data::pagedata e403;
		nativehttp::data::pagedata e404;
		nativehttp::data::pagedata e500;
		nativehttp::data::pagedata e501;
		nativehttp::data::pagedata e505;
	}

	namespace manager
	{
		int rate = 0;
		int postto = -1;
		int execto = -1;
		bool apr;
		string fileloc;
	}

	void request::free()
	{
		delete request;
	}

}



