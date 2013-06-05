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
#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <openssl/ssl.h>

#include "http/data.h"

#ifndef SOCKET
#define SOCKET int
#endif
#define INVALID_SOCKET	-1

namespace http
{
	void unlockclient(int i);
	void kickclient(int i);
	void send(int uid, unsigned long datasize, const char *data, bool free);
	namespace bsd
	{
		void disconnect(int scid);
		void *sender(void *unused);
		void reciver();
		int findfreesock();
		void init();
		void *listener(void *unused);
		void sendNow(int uid, unsigned long datasize, char *data, bool free);
	}
	namespace ssl
	{
		extern SSL_CTX *ctx;

		void *sender(void *unused);
		void disconnect(int scid);
		void reciver();
		int findfreesock();
		void *listener(void *unused);
		void init();
	}
}

#endif // NET_H_INCLUDED
