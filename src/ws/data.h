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
#ifndef WSDATA_H_INCLUDED
#define WSDATA_H_INCLUDED

#include "data/vector.h"
#include "nativehttp.h"

#define WS_MTYPE_NONE 0
#define WS_MTYPE_TEXT 1
#define WS_MTYPE_BIN 2
#define WS_MTYPE_CONT 3

namespace ws
{
    struct ws_ent
    {
        const char* uri;
        int ulen;
        const char* protocol;
        nativehttp::websock::cb_onConnect on_connect;
        nativehttp::websock::cb_onDisconnect on_disconnect;
        nativehttp::websock::cb_onTxtMsg on_txt_msg;
        nativehttp::websock::cb_onBinMsg on_bin_msg;
    };

    struct framebuf
    {
        bool busy;

        uint8_t opcode;
        bool mask;
        bool fin;

        uint64_t frame_size;
        uint32_t mkey;

        uint32_t pos;
        unsigned char* fdata;
        uint64_t recived;
    };

    struct msgbuf
    {
        uint8_t type;
        uint64_t len;
        unsigned char* data;
    };

    extern bool enabled;
    extern data::vector<ws_ent>* units;
    extern uint64_t maxmsgsize;
    extern uint64_t maxframesize;

    extern uint64_t max_sender_chunk;
    extern uint64_t max_sendable;

    extern int* client_unit;

    extern ws::framebuf* frames;
    extern ws::msgbuf* messages;


}

#endif // DATA_H_INCLUDED
