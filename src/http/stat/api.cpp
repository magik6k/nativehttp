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

namespace st = http::statdata;

namespace nativehttp
{
    namespace server
    {
        namespace stat
        {
            bool stats()
            {
                return http::statdata::info::toggle;
            }

            uint64_t get(unit value)
            {
                return get(value, -1);
            }

            uint64_t get(unit value, int column)
            {
                switch(value)
                {
                    /** Information */
                    case len_hourly:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::info::hourlylen;
                        break;
                    }
                    case len_daily:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::info::dailylen;
                        break;
                    }
                    case len_weekly:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::info::weeklylen;
                        break;
                    }

                    /** Transfer */
                    case upload:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::transfer::ulbytes;
                        break;
                    }
                    case download:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::transfer::dlbytes;
                        break;
                    }
                    case upload_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::transfer::hrl_ul[column];
                        break;
                    }
                    case download_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::transfer::hrl_dl[column];
                        break;
                    }
                    case upload_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::transfer::dly_ul[column];
                        break;
                    }
                    case download_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::transfer::dly_dl[column];
                        break;
                    }
                    case upload_weekly:
                    {
                        if(column<0||column>=st::info::weeklylen)return NH_STAT_ERROR;
                        return st::transfer::wkl_ul[column];
                        break;
                    }
                    case download_weekly:
                    {
                        if(column<0||column>=st::info::weeklylen)return NH_STAT_ERROR;
                        return st::transfer::wkl_dl[column];
                        break;
                    }

                    /**  Activity */
                    case requests:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::activity::hits;
                        break;
                    }
                    case connections:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::activity::hits;
                        break;
                    }
                    case requests_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::activity::hrl_hits[column];
                        break;
                    }
                    case connections_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::activity::hrl_connections[column];
                        break;
                    }
                    case requests_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::activity::dly_hits[column];
                        break;
                    }
                    case connections_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::activity::dly_connections[column];
                        break;
                    }
                    case requests_weekly:
                    {
                        if(column<0||column>=st::info::weeklylen)return NH_STAT_ERROR;
                        return st::activity::wkl_hits[column];
                        break;
                    }
                    case connections_weekly:
                    {
                        if(column<0||column>=st::info::weeklylen)return NH_STAT_ERROR;
                        return st::activity::wkl_connections[column];
                        break;
                    }

                    /** Request type */
                    case requests_get:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::method::get;
                        break;
                    }
                    case requests_post:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::method::post;
                        break;
                    }

                    /** WebSocket */
                    case ws_upload:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::upload;
                        break;
                    }
                    case ws_download:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::download;
                        break;
                    }
                    case ws_handshakes:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::connections;
                        break;
                    }
                    case ws_frames_received:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::frames_recvd;
                        break;
                    }
                    case ws_frames_sent:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::frames_sent;
                        break;
                    }
                    case ws_messages_received:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::msgs_recvd;
                        break;
                    }
                    case ws_messages_sent:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::websocket::msgs_sent;
                        break;
                    }
                    case ws_messages_received_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::websocket::hrl_msgs_recv[column];
                        break;
                    }
                    case ws_messages_sent_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::websocket::hrl_msgs_sent[column];
                        break;
                    }
                    case ws_messages_received_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::websocket::dly_msgs_recv[column];
                        break;
                    }
                    case ws_messages_sent_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::websocket::dly_msgs_sent[column];
                        break;
                    }
                    case ws_messages_received_weekly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::websocket::wkl_msgs_recv[column];
                        break;
                    }
                    case ws_messages_sent_weekly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::websocket::wkl_msgs_sent[column];
                        break;
                    }

                    /** HTTP related stats */
                    case http_head_size_received:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::shttp::recv_header_size;
                        break;
                    }
                    case http_head_size_sent:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::shttp::sent_header_size;
                        break;
                    }
                    case http_postdata_size:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::shttp::postdata_size;
                        break;
                    }
                    case http_postdata_size_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::shttp::dly_postdata[column];
                        break;
                    }
                    case http_head_size_received_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::shttp::dly_recv_header_size[column];
                        break;
                    }
                    case http_head_size_sent_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::shttp::dly_sent_header_size[column];
                        break;
                    }

                    /** Sessions */
                    case sessions_created:
                    {
                        if(column>=0)return NH_STAT_ERROR;
                        return st::session::sessions_created;
                        break;
                    }
                    case sessions_created_daily:
                    {
                        if(column<0||column>=st::info::dailylen)return NH_STAT_ERROR;
                        return st::session::dly_sessions_created[column];
                        break;
                    }
                    case sessions_created_weekly:
                    {
                        if(column<0||column>=st::info::weeklylen)return NH_STAT_ERROR;
                        return st::session::wkl_sessions_created[column];
                        break;
                    }
                    case sessions_max_existing_hourly:
                    {
                        if(column<0||column>=st::info::hourlylen)return NH_STAT_ERROR;
                        return st::session::hrl_max_existing[column];
                        break;
                    }
                }
                return NH_STAT_ERROR;
            }
        }
	}
}
