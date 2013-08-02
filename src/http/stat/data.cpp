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
#include "../stat.h"
#include "nativehttp.h"
#include <iostream>

namespace http
{
	namespace statdata
	{
        namespace info
        {
            bool toggle;

            int64_t hourlylen;
            int64_t dailylen;
            int64_t weeklylen;

		}

        namespace transfer
        {
            uint64_t ulbytes;
            uint64_t dlbytes;

            uint64_t *hrl_ul;
            uint64_t *hrl_dl;

            uint64_t *dly_ul;
            uint64_t *dly_dl;

            uint64_t *wkl_ul;
            uint64_t *wkl_dl;
        }

        namespace activity
        {
            uint64_t hits;
            uint64_t connections;

            uint64_t *hrl_hits;
            uint64_t *hrl_connections;

            uint64_t *dly_hits;
            uint64_t *dly_connections;

            uint64_t *wkl_hits;
            uint64_t *wkl_connections;
        }


		namespace method
		{
            uint64_t get;
            uint64_t post;
		}

		namespace websocket
		{
            uint64_t frames_recvd;
            uint64_t msgs_recvd;

            uint64_t frames_sent;
            uint64_t msgs_sent;

            uint64_t upload;
            uint64_t download;

            uint64_t connections;

            uint64_t* dly_connections;

            uint64_t* dly_upload;
            uint64_t* dly_download;


            uint64_t* hrl_msgs_sent;
            uint64_t* hrl_msgs_recv;

            uint64_t* dly_msgs_sent;
            uint64_t* dly_msgs_recv;

            uint64_t* wkl_msgs_sent;
            uint64_t* wkl_msgs_recv;
		}

		namespace shttp
		{
            uint64_t recv_header_size;
            uint64_t sent_header_size;

            uint64_t postdata_size;

            uint64_t* dly_postdata;

            uint64_t* dly_recv_header_size;
            uint64_t* dly_sent_header_size;
		}

		namespace session
		{
            uint64_t sessions_created;

            uint64_t* dly_sessions_created;
            uint64_t* wkl_sessions_created;

            uint64_t* hrl_max_existing;
		}

        namespace internal
        {
            time_t lastHrlFlp;
            time_t lastDlyFlp;
            time_t lastWklFlp;
            time_t lastSave;

            time_t save_rate;

            uint16_t filever = 0x0004;
            bool managersafe = true;
            string stfn;
        }
	}

}
