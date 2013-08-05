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
#include "nativehttp.h"

nativehttp::data::pagedata ajax(nativehttp::rdata* request)
{

    if (request->session->get("@__NtStS#") != "l")
    {
        return nd::pagedata("{\"status\":\"e1\"}");
    }

    if(request->get->get("ajax").size()<3)
    {
        return nd::pagedata("{\"status\":\"e2\"}");
    }

    int pre = request->get->get("ajax")[0]-('0'+1);

    if(pre<0||pre>2)
    {
         return nd::pagedata("{\"status\":\"e3\"}");
    }

    unsigned int len = nd::ss::int_from_str(request->get->get("ajax").c_str()+2);

    switch(pre)
    {
        case 0:if(len>ns::stat::get(ns::stat::unit::len_hourly))return nd::pagedata("{\"status\":\"e40\"}");break;
        case 1:if(len>ns::stat::get(ns::stat::unit::len_daily))return nd::pagedata("{\"status\":\"e41\"}");break;
        case 2:if(len>ns::stat::get(ns::stat::unit::len_weekly))return nd::pagedata("{\"status\":\"e42\"}");break;
    }

    string out = "{\"status\":\"ok\",";

    uint64_t mx=0;
    /**

    TYPES:
    0 - size: KiB
    1 - size: B
    2 - integer

    **/

    switch(pre)
    {
        case 0:
        {
            switch(request->get->get("ajax")[1])
            {
                case 'n':///hourly network
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//UL, DL, TOTAL
                    out += "\"type\": 0,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Download\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Upload\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::download_hourly,d)/1024)+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::upload_hourly,d)/1024)+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64((ns::stat::get(ns::stat::unit::download_hourly,d)+ns::stat::get(ns::stat::unit::upload_hourly,d))/1024)+"}";
                        out += "]";
                        if(d+1 < len)out += ",";
                        if(mx < ns::stat::get(ns::stat::unit::download_hourly,d)+ns::stat::get(ns::stat::unit::upload_hourly,d))
                            mx = ns::stat::get(ns::stat::unit::download_hourly,d)+ns::stat::get(ns::stat::unit::upload_hourly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx/1024);

                    break;
                }
                case 'a':///activity
                {
                    out += "\"head\":{";
                    out += "\"lines\":2,";//CON, REQ
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"TCP Connections\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"HTTP Requests\",\"color\":\"#0b0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::connections_hourly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::requests_hourly,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::connections_hourly,d))
                            mx = ns::stat::get(ns::stat::unit::connections_hourly,d);
                        if(mx < ns::stat::get(ns::stat::unit::requests_hourly,d))
                            mx = ns::stat::get(ns::stat::unit::requests_hourly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'o':///Max Online
                {
                    out += "\"head\":{";
                    out += "\"lines\":1,";//MX OL
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Max online users\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::sessions_max_existing_hourly,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::sessions_max_existing_hourly,d))
                            mx = ns::stat::get(ns::stat::unit::sessions_max_existing_hourly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'q':///WebSocket msgs
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//RC, ST
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Messages Sent\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Messages Received\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_sent_hourly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received_hourly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received_hourly,d)+ns::stat::get(ns::stat::unit::ws_messages_sent_hourly,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::ws_messages_sent_hourly,d)+ns::stat::get(ns::stat::unit::ws_messages_received_hourly,d))
                            mx = ns::stat::get(ns::stat::unit::ws_messages_sent_hourly,d)+ns::stat::get(ns::stat::unit::ws_messages_received_hourly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
            }
            break;
        }
        case 1://daily
        {
            switch(request->get->get("ajax")[1])
            {
                case 'n':///daily network
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//UL, DL, TOTAL
                    out += "\"type\": 0,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Download\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Upload\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::download_daily,d)/1024)+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::upload_daily,d)/1024)+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64((ns::stat::get(ns::stat::unit::download_daily,d)+ns::stat::get(ns::stat::unit::upload_daily,d))/1024)+"}";
                        out += "]";
                        if(d+1 < len)out += ",";
                        if(mx < ns::stat::get(ns::stat::unit::download_daily,d)+ns::stat::get(ns::stat::unit::upload_daily,d))
                            mx = ns::stat::get(ns::stat::unit::download_daily,d)+ns::stat::get(ns::stat::unit::upload_daily,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx/1024);

                    break;
                }
                case 'a':///activity
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//CON, REQ, SES C
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"TCP Connections\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"HTTP Requests\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Sessions created/Visits\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::connections_daily,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::requests_daily,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::sessions_created_daily,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::connections_daily,d))
                            mx = ns::stat::get(ns::stat::unit::connections_daily,d);
                        if(mx < ns::stat::get(ns::stat::unit::requests_daily,d))
                            mx = ns::stat::get(ns::stat::unit::requests_daily,d);
                            if(mx < ns::stat::get(ns::stat::unit::sessions_created_daily,d))
                            mx = ns::stat::get(ns::stat::unit::sessions_created_daily,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'p':///POST data
                {
                    out += "\"head\":{";
                    out += "\"lines\":1,";//RC, ST
                    out += "\"type\": 1,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"POST data size\",\"color\":\"#b00\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::http_postdata_size_daily,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::http_postdata_size_daily,d))
                            mx = ns::stat::get(ns::stat::unit::http_postdata_size_daily,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'q':///WebSocket msgs
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//RC, ST
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Messages Sent\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Messages Received\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_sent_daily,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received_daily,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received_daily,d)+ns::stat::get(ns::stat::unit::ws_messages_sent_daily,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::ws_messages_sent_daily,d)+ns::stat::get(ns::stat::unit::ws_messages_received_daily,d))
                            mx = ns::stat::get(ns::stat::unit::ws_messages_sent_daily,d)+ns::stat::get(ns::stat::unit::ws_messages_received_daily,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'w':///WebSocket transfer
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//RC, ST
                    out += "\"type\": 1,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"WebSocket Upload\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"WebSocket Download\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_upload_daily,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_download_daily,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_download_daily,d)+ns::stat::get(ns::stat::unit::ws_upload_daily,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::ws_upload_daily,d)+ns::stat::get(ns::stat::unit::ws_download_daily,d))
                            mx = ns::stat::get(ns::stat::unit::ws_upload_daily,d)+ns::stat::get(ns::stat::unit::ws_download_daily,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'e':///WebSocket connections
                {
                    out += "\"head\":{";
                    out += "\"lines\":1,";//RC, ST
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"WebSocket Connections\",\"color\":\"#b00\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_handshakes_daily,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::ws_handshakes_daily,d))
                            mx = ns::stat::get(ns::stat::unit::ws_handshakes_daily,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }

            }
            break;
        }
        case 2://weekly
        {
            switch(request->get->get("ajax")[1])
            {
                case 'n':///weekly network
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//UL, DL, TOTAL
                    out += "\"type\": 0,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Download\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Upload\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::download_weekly,d)/1024)+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::upload_weekly,d)/1024)+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64((ns::stat::get(ns::stat::unit::download_weekly,d)+ns::stat::get(ns::stat::unit::upload_weekly,d))/1024)+"}";
                        out += "]";
                        if(d+1 < len)out += ",";
                        if(mx < ns::stat::get(ns::stat::unit::download_weekly,d)+ns::stat::get(ns::stat::unit::upload_weekly,d))
                            mx = ns::stat::get(ns::stat::unit::download_weekly,d)+ns::stat::get(ns::stat::unit::upload_weekly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx/1024);

                    break;
                }
                case 'a':///activity
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//CON, REQ, SES C
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"TCP Connections\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"HTTP Requests\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Sessions created/Visits\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::connections_weekly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::requests_weekly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::sessions_created_weekly,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::connections_weekly,d))
                            mx = ns::stat::get(ns::stat::unit::connections_weekly,d);
                        if(mx < ns::stat::get(ns::stat::unit::requests_weekly,d))
                            mx = ns::stat::get(ns::stat::unit::requests_weekly,d);
                            if(mx < ns::stat::get(ns::stat::unit::sessions_created_weekly,d))
                            mx = ns::stat::get(ns::stat::unit::sessions_created_weekly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }
                case 'q':///WebSocket msgs
                {
                    out += "\"head\":{";
                    out += "\"lines\":3,";//RC, ST
                    out += "\"type\": 2,";
                    out += "\"len\":"+nd::ss::str_from_int(len);
                    out += ",\"ldesc\":[";
                    out += "{\"name\":\"Messages Sent\",\"color\":\"#b00\"},";
                    out += "{\"name\":\"Messages Received\",\"color\":\"#0b0\"},";
                    out += "{\"name\":\"Total\",\"color\":\"#bb0\"}";
                    out += "]},";

                    out += "\"data\":[";

                    for(unsigned int d = 0;d < len;d++)
                    {
                        out += "[";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_sent_weekly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received_weekly,d))+"},";
                        out += "{\"d\":"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received_weekly,d)+ns::stat::get(ns::stat::unit::ws_messages_sent_weekly,d))+"}";
                        out += "]";
                        if(d+1 < len)out += ",";

                        if(mx < ns::stat::get(ns::stat::unit::ws_messages_sent_weekly,d)+ns::stat::get(ns::stat::unit::ws_messages_received_weekly,d))
                            mx = ns::stat::get(ns::stat::unit::ws_messages_sent_weekly,d)+ns::stat::get(ns::stat::unit::ws_messages_received_weekly,d);
                    }

                    out += "],\"max\":"+nd::ss::str_from_int64(mx);
                    break;
                }

            }
            break;
        }
    }

    out+="}";

    return nd::pagedata(out);
}
