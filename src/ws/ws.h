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
#ifndef WS_H_INCLUDED
#define WS_H_INCLUDED

#include "data.h"
#include "nativehttp.h"
#include "http/executor.h"

namespace ws
{
    void init();
    void handshake(int uid, nativehttp::rdata &rd, http::rproc::lrqd &ld);
    int find_uri(const char* uri);

    void *rmsgproc(void *unused);
    void rcv_push(unsigned char* data, int dlen, int uid);
    void msg_push(int uid, uint8_t opcode, bool fin, uint64_t frame_size, unsigned char* data);
    void control(int uid, uint8_t opcode, uint64_t frame_size, unsigned char* data);

    void disconnect(int uid);
    void clean(int uid);

    void chunked_send(int uid, const void* data, uint64_t dsize);

}

#endif // WS_H_INCLUDED
