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

            extern int64_t hourlylen;
            extern int64_t dailylen;
            extern int64_t weeklylen;
        }

        namespace transfer
        {
            extern uint64_t ulbytes;
            extern uint64_t dlbytes;

            extern uint64_t *hrl_ul;
            extern uint64_t *hrl_dl;

            extern uint64_t *dly_ul;
            extern uint64_t *dly_dl;

            extern uint64_t *wkl_ul;
            extern uint64_t *wkl_dl;
        }

        namespace activity
        {
            extern uint64_t hits;
            extern uint64_t connections;

            extern uint64_t *hrl_hits;
            extern uint64_t *hrl_connections;

            extern uint64_t *dly_hits;
            extern uint64_t *dly_connections;

            extern uint64_t *wkl_hits;
            extern uint64_t *wkl_connections;
		}

		namespace method
		{
            extern uint64_t get;
            extern uint64_t post;
        }

        namespace websocket
		{
            extern uint64_t frames_recvd;
            extern uint64_t msgs_recvd;

            extern uint64_t frames_sent;
            extern uint64_t msgs_sent;

            extern uint64_t upload;
            extern uint64_t download;

            extern uint64_t connections;

            extern uint64_t* dly_connections;

            extern uint64_t* dly_upload;
            extern uint64_t* dly_download;


            extern uint64_t* hrl_msgs_sent;
            extern uint64_t* hrl_msgs_recv;

            extern uint64_t* dly_msgs_sent;
            extern uint64_t* dly_msgs_recv;

            extern uint64_t* wkl_msgs_sent;
            extern uint64_t* wkl_msgs_recv;
		}

		namespace shttp
		{
            extern uint64_t recv_header_size;
            extern uint64_t sent_header_size;

            extern uint64_t postdata_size;

            extern uint64_t* dly_postdata;

            extern uint64_t* dly_recv_header_size;
            extern uint64_t* dly_sent_header_size;
		}

		namespace session
		{
            extern uint64_t sessions_created;

            extern uint64_t* dly_sessions_created;
            extern uint64_t* wkl_sessions_created;

            extern uint64_t* hrl_max_existing;
		}

        namespace internal
        {
            extern time_t lastHrlFlp;
            extern time_t lastDlyFlp;
            extern time_t lastWklFlp;
            extern time_t lastSave;

            extern time_t save_rate;

            extern uint16_t filever;
            extern string stfn;
            extern bool managersafe;
        }

		namespace file
		{
            struct head
            {
                int64_t hourlylen;
                int64_t dailylen;
                int64_t weeklylen;

                int64_t timesaved;
            };

            struct totals
            {
                uint64_t upload;
                uint64_t download;

                uint64_t hits;
                uint64_t connections;

                uint64_t req_get;
                uint64_t req_post;

                uint64_t ws_frames_rec;
                uint64_t ws_frames_snt;

                uint64_t ws_msgs_rec;
                uint64_t ws_msgd_snd;

                uint64_t ws_upload;
                uint64_t ws_download;

                uint64_t ws_connections;

                uint64_t http_rec_head_size;
                uint64_t http_snd_head_size;

                uint64_t http_post_size;

                uint64_t sess_created;
            };

            struct hourly
            {
                uint64_t upload;
                uint64_t download;

                uint64_t hits;
                uint64_t connections;

                uint64_t ws_msgs_recv;
                uint64_t ws_msgs_sent;

                uint64_t max_sessions;
            };

            struct daily
            {
                uint64_t upload;
                uint64_t download;

                uint64_t hits;
                uint64_t connections;

                uint64_t ws_connections;
                uint64_t ws_upload;
                uint64_t ws_download;
                uint64_t ws_msgs_recv;
                uint64_t ws_msgs_sent;

                uint64_t postdata_size;
                uint64_t http_recv_header_size;
                uint64_t http_sent_header_size;

                uint64_t sessions_created;

            };

            struct weekly
            {
                uint64_t upload;
                uint64_t download;

                uint64_t hits;
                uint64_t connections;

                uint64_t ws_msgs_recv;
                uint64_t ws_msgs_sent;

                uint64_t sessions_created;
            };
		}

		void manage();
		void init();
		void save();
		void load();

		void onrecv(uint32_t dlen);
		void onsend(uint32_t ulen);

		void onconnect();
		void onhit(uint32_t headsize);
		void onhttpsend(uint32_t headsize);

		void onpost(uint32_t datasize);
		void onget();

		void on_ws_frame_recv(uint32_t size);
		void on_ws_frame_send(uint32_t size);
		void on_ws_connect();
		void on_ws_msg_send();
		void on_ws_msg_recv();

		void on_new_session(uint64_t active);

	}
}

#endif // STAT_H_INCLUDED
