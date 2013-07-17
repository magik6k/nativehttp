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

        namespace info
        {
            extern bool toggle;
            extern bool transfer;
            extern bool hitlog;
            extern bool method;

            extern int64_t hourlylen;
        }

        namespace transfer
        {
            extern uint64_t ulbytes;
            extern uint64_t dlbytes;

            extern uint64_t *hrl_ul;
            extern uint64_t *hrl_dl;
        }

        namespace activity
        {
            extern uint64_t hits;
            extern uint64_t connections;

            extern uint64_t *hrl_hits;
            extern uint64_t *hrl_connections;
		}

		namespace method
		{
            extern uint64_t get;
            extern uint64_t post;
        }


		extern time_t lastHrlFlp;
		extern time_t lastSave;

		extern time_t save_rate;

		extern uint16_t filever;
		extern string stfn;
		extern bool managersafe;

		struct stunit
		{
			uint64_t hits;
			uint64_t connections;

			uint64_t ulbytes;
			uint64_t dlbytes;
		};

		void manage();
		void init();
		void save();

		void onrecv(uint32_t dlen);
		void onsend(uint32_t ulen);
		void onconnect();
		void onhit();

		void onpost();
		void onget();

	}
}

#endif // STAT_H_INCLUDED
