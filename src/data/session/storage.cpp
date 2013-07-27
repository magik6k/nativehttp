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
#include "session.h"
#include "http/data.h"
#include "http/stat.h"

namespace data
{
	namespace session
	{
		sstg storage;

		sstg::sstg()
		{
			scount = 0;
			data = NULL;
		}

		bool sstg::cksess(size_t id, unsigned int scd)
		{
			if (scount <= id)return false;
			if (data[id].started == 0)return false;
			return data[id].svid == scd;
		}

		void sstg::allocsessions()
		{
			size_t nsize = scount + 3;
			session *tsp = new session[nsize];
			if (!tsp)return;
			size_t i;
			for (i = 0; i < scount; i++)
			{
				tsp[i].started = data[i].started;
				tsp[i].svid = data[i].svid;
				tsp[i].data = data[i].data;
			}
			for (; i < nsize; i++)
			{
				tsp[i].started = 0;
				tsp[i].svid = 0;
			}
			delete[] data;
			data = tsp;
			scount = nsize;
		}

		void sstg::prealloc(size_t amnt)
		{
			data = new session[amnt];
			byuid = new nativehttp::data::session*[http::maxConnections];

			if (!data)return;

			for (size_t i = 0; i < amnt; i++)
			{
				data[i].started = 0;
				data[i].svid = 0;
			}

			for(int i = 0; i < http::maxConnections; i++)
			{
                byuid[i] = NULL;
			}
		}

		size_t sstg::findfreesess(bool &vld)
		{
			vld = true;
			for (size_t i = 0; i < scount; i++)
			{
				if (data[i].started == 0)return i;
			}
			if (scount + 3 > http::max_sesions)
			{
				vld = false;
				return 0;
			}
			this->allocsessions();
			return this->findfreesess(vld);
		}

		size_t sstg::mksess(unsigned int scd)
		{
			bool v;
			size_t si = this->findfreesess(v);
			if (v)
			{
				data[si].svid = scd;
				data[si].started = time(0);
				active++;
				http::statdata::on_new_session(active);
			}
			else
			{
				data[si].svid = 0;
				data[si].started = 0;
			}

			return si;
		}

		sbmain &sstg::gtsess(size_t id)
		{
			data[id].started = time(0);
			return data[id].data;
		}

		nativehttp::data::session* sstg::getuid(int uid)
		{
            if(!byuid[uid])
                byuid[uid] = new nativehttp::data::session;

            return byuid[uid];
		}

	}
}
