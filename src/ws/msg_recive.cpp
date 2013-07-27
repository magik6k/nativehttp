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
#include "ws.h"
#include "http/stat.h"

namespace ws
{
    void msg_push(int uid, uint8_t opcode, bool fin, uint64_t frame_size, unsigned char* data)
    {
        http::statdata::on_ws_frame_recv(frame_size);
        if(messages[uid].type == WS_MTYPE_NONE)
        {
            if(fin)
            {
                http::statdata::on_ws_msg_recv();
                if(opcode == 0x1)
                {
                    if((*ws::units)[ws::client_unit[uid]].on_txt_msg)
                        (*((*ws::units)[ws::client_unit[uid]].on_txt_msg))(uid, (char*)data);
                }
                else if(opcode == 0x2)
                {
                    if((*ws::units)[ws::client_unit[uid]].on_bin_msg)
                        (*((*ws::units)[ws::client_unit[uid]].on_bin_msg))(uid, data, frame_size);
                }
            }
            else
            {
                if(opcode == 0)
                {
                    ///Disconnect - this op here is wrong
                    ws::disconnect(uid);
                    return;
                }
                messages[uid].type = opcode;
                messages[uid].len = frame_size;
                messages[uid].data = new unsigned char[frame_size];
                memcpy(messages[uid].data,data,frame_size);
            }
        }
        else
        {
            if(opcode != 0)
            {
                    ///Disconnect - this op here is wrong
                    ws::disconnect(uid);
                    return;
            }
            if(!fin)
            {
                if(ws::maxmsgsize <  messages[uid].len + frame_size)
                {
                    ///Disconnect - too much data
                    ws::disconnect(uid);
                    return;
                }
                unsigned char* newbuf = new unsigned char[messages[uid].len + frame_size];

                memcpy(newbuf, messages[uid].data, messages[uid].len);
                memcpy(newbuf + messages[uid].len, data, frame_size);

                messages[uid].len += frame_size;
                delete[] messages[uid].data;
                messages[uid].data = newbuf;
            }
            else
            {
                if(ws::maxmsgsize <  messages[uid].len + frame_size)
                {
                    ///Disconnect - too much data
                    ws::disconnect(uid);
                    return;
                }
                unsigned char* newbuf = new unsigned char[messages[uid].len + frame_size];

                memcpy(newbuf, messages[uid].data, messages[uid].len);
                memcpy(newbuf + messages[uid].len, data, frame_size);

                messages[uid].len += frame_size;
                delete[] messages[uid].data;
                messages[uid].data = newbuf;

                http::statdata::on_ws_msg_recv();

                if(opcode == 0x1)
                {
                    if((*ws::units)[ws::client_unit[uid]].on_txt_msg)
                        (*((*ws::units)[ws::client_unit[uid]].on_txt_msg))(uid, (char*)messages[uid].data);
                }
                else if(opcode == 0x2)
                {
                    if((*ws::units)[ws::client_unit[uid]].on_bin_msg)
                        (*((*ws::units)[ws::client_unit[uid]].on_bin_msg))(uid, messages[uid].data, messages[uid].len);
                }

                delete[] messages[uid].data;
                messages[uid].data = NULL;
                messages[uid].type = WS_MTYPE_NONE;
            }


        }
    }
}
