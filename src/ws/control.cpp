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
#include "http/net/net.h"

namespace ws
{
    void control(int uid, uint8_t opcode, uint64_t frame_size, unsigned char* data)
    {
        if(opcode == 0x9)
        {
            unsigned char* tmd = new unsigned char[2 + frame_size];
            tmd[0] = 0b10001010; //FIN + PONG
            tmd[1] = (uint8_t)frame_size;
            memcpy(tmd + 2, data, frame_size);
            http::send(uid,2 + frame_size, (const char*)tmd, true);
        }
        if(opcode == 0x8)
        {
            nativehttp::server::log("control.cpp@ws","Disconnect");
            disconnect(uid);
        }
    }
}
