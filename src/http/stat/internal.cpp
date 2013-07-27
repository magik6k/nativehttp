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

namespace http
{
	namespace statdata
	{

		void onrecv(uint32_t dlen)
		{
			if (info::toggle)
			{
				transfer::dlbytes += dlen;
				if(info::hourlylen)transfer::hrl_dl[0] += dlen;
				if(info::dailylen)transfer::dly_dl[0] += dlen;
				if(info::weeklylen)transfer::wkl_dl[0] += dlen;
			}
		}
		void onsend(uint32_t ulen)
		{
			if (info::toggle)
			{
				transfer::ulbytes += ulen;
				if(info::hourlylen)transfer::hrl_ul[0] += ulen;
				if(info::dailylen)transfer::dly_ul[0] += ulen;
				if(info::weeklylen)transfer::wkl_ul[0] += ulen;
			}
		}
		void onconnect()
		{
			if (info::toggle)
			{
				activity::connections++;
				if(info::hourlylen)activity::hrl_connections[0]++;
				if(info::dailylen)activity::dly_connections[0]++;
				if(info::weeklylen)activity::wkl_connections[0]++;
			}
		}

        void onhit(uint32_t headsize)
		{
			if (info::toggle)
			{
				activity::hits++;
				shttp::recv_header_size+=headsize;
				if(info::dailylen)shttp::dly_recv_header_size+=headsize;

				if(info::hourlylen)activity::hrl_hits[0]++;
				if(info::dailylen)activity::dly_hits[0]++;
				if(info::weeklylen)activity::dly_hits[0]++;
			}
		}

		void onpost(uint32_t datasize)
		{
			if (info::toggle)
			{
				method::post++;

                shttp::postdata_size+=datasize;
                if(info::dailylen)shttp::dly_postdata[0]+=datasize;
            }
		}

		void onget()
		{
			if (info::toggle)
			{
                method::get++;
			}
		}

        void onhttpsend(uint32_t headsize)
        {
            if(info::toggle)
            {
                shttp::sent_header_size+=headsize;
                if(info::dailylen)shttp::dly_sent_header_size[0]+=headsize;
            }
        }

        void on_ws_frame_recv(uint32_t size)
        {
            if(info::toggle)
            {
                websocket::frames_recvd++;
                websocket::download+=size;
                if(info::dailylen)websocket::dly_download[0]+=size;
            }
        }

        void on_ws_frame_send(uint32_t size)
        {
            if(info::toggle)
            {
                websocket::frames_sent++;
                websocket::upload+=size;
                if(info::dailylen)websocket::dly_upload[0]+=size;
            }
        }

        void on_ws_connect()
        {
            if(info::toggle)
            {
                websocket::connections++;
                if(info::dailylen)websocket::dly_connections[0]++;
            }
        }

        void on_ws_msg_send()
        {
            if(info::toggle)
            {
                websocket::msgs_sent++;
                if(info::hourlylen)websocket::hrl_msgs_sent[0]++;
                if(info::dailylen)websocket::dly_msgs_sent[0]++;
                if(info::weeklylen)websocket::wkl_msgs_sent[0]++;
            }
        }

        void on_ws_msg_recv()
        {
            if(info::toggle)
            {
                websocket::msgs_recvd++;
                if(info::hourlylen)websocket::hrl_msgs_recv[0]++;
                if(info::dailylen)websocket::dly_msgs_recv[0]++;
                if(info::weeklylen)websocket::wkl_msgs_recv[0]++;
            }
        }

        void on_new_session(uint64_t active)
        {
            if(info::toggle)
            {
                session::sessions_created++;
                if(info::dailylen)session::dly_sessions_created[0]++;
                if(info::weeklylen)session::wkl_sessions_created[0]++;
                if(info::hourlylen)
                    if(session::hrl_max_existing[0]<active)
                        session::hrl_max_existing[0] = active;
            }
        }

	}
}
