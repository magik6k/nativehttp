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
#include "net.h"
#include "http/stat.h"

namespace http
{
	void send(int uid, unsigned long datasize, const char *data, bool free)
	{
		outdata t = {uid, datasize, data, free};
		SDL_mutexP(http::mtx_snd);
		http::tosend.push(t);
		SDL_mutexV(http::mtx_snd);
	}
	void unlockclient(int i)
	{
		http::ulock[i] = false;
	}

	void kickclient(int i)
	{
		if (http::onssl && http::sslsck[i])
		{
			SSL_shutdown(http::sslsck[i]);
			SSL_free(http::sslsck[i]);
		}
		http::ulock[i] = false;
		close(http::connected[i]);
	}
}

