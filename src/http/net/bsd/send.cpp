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
#include "../net.h"
#include "http/stat.h"

int nhSend(SOCKET sock, const void *data, int len)
{
	const char *dp = (const char*)data;
	int sent, left;

	left = len;
	sent = 0;
	errno = 0;
	do
	{
		len = send(sock, (const char *)dp, left, http::asyncsnd ? MSG_DONTWAIT : 0);
		if (len > 0)
		{
			sent += len;
			left -= len;
			dp += len;
		}
	}
	while ((left > 0) && ((len > 0) || (errno == EINTR)));

	return (sent);
}

namespace http
{
	namespace bsd
	{
		void *sender(void *unused)
		{
			prctl(PR_SET_NAME, ("nh-snd-bsd-" + nativehttp::data::superstring::str_from_int(*(char*)unused)).c_str(), 0, 0, 0);
			int ts = 0;
			while (1)
			{
				if (http::tosend.empty())
				{
					utils::sleep(1);
					continue;
				}

				 pthread_mutex_lock(http::mtx_snd);

				outdata proc = http::tosend.front(ts);
				if (ts == 1)
				{
					 pthread_mutex_unlock(http::mtx_snd);
					continue;
				}
				if(proc.pktid!=http::packets_sent[proc.uid])
				{
                    http::tosend.front2back();
                     pthread_mutex_unlock(http::mtx_snd);
					continue;
				}
				http::tosend.pop();
				 pthread_mutex_unlock(http::mtx_snd);

				#ifdef NHDBG

                if(http::log_sender&&proc.data)
                {
                    char* td = new char[proc.size+1];
                    memcpy(td,proc.data,proc.size);
                    td[proc.size] = '\0';

                    nativehttp::server::log("DBG:sender.cpp["+nativehttp::data::superstring::str_from_int(*(char*)unused)+"]@bsd",
                        ("Sending:\n"+(http::log_newline?((nativehttp::data::superstring(td).lock(0).change("\r","\\r").change("\n","\\n\n")).str)
                        :(string(td)))).c_str());

                    delete[] td;
                }

                #endif

				if (http::connected[proc.uid])
				{

					nhSend(http::connected[proc.uid], proc.data, proc.size);
					http::packets_sent[proc.uid]++;
					http::statdata::onsend(proc.size);
				}


				if (proc.fas)
				{
					delete[] proc.data;
				}
			}
			return NULL;
		}

		void sendNow(int uid, unsigned long datasize, char *data, bool free)
		{
			nhSend(http::connected[uid], data, datasize);
			if (free)
			{
				delete[] data;
			}
		}
	}
}
