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
#ifndef STAT_H_INCLUDED
#define STAT_H_INCLUDED
#include <time.h>
#include <string>
using namespace std;

namespace http
{
	namespace statdata
	{
		extern bool toggle;
		extern bool transfer;
		extern bool hitlog;
		extern bool method;

		extern long long hourlylen;

		extern unsigned long hits;
		extern unsigned long connections;

		extern unsigned long ulbytes;
		extern unsigned long dlbytes;

		extern unsigned long get;
		extern unsigned long post;

		extern unsigned long *hrl_hits;
		extern unsigned long *hrl_connections;
		extern unsigned long *hrl_ul;
		extern unsigned long *hrl_dl;

		extern time_t lastHrlFlp;
		extern time_t lastSave;

		extern time_t save_rate;

		extern uint16_t filever;
		extern string stfn;
		extern bool managersafe;

		struct stunit
		{
			unsigned long hits;
			unsigned long connections;

			unsigned long ulbytes;
			unsigned long dlbytes;
		};

		void manage();
		void init();
		void save();

		void onrecv(unsigned long dlen);
		void onsend(unsigned long ulen);
		void onconnect();
		void onhit();

		void onpost();
		void onget();

	}
}

#endif // STAT_H_INCLUDED
