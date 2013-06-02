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
#include <openssl/sha.h>

namespace ws
{
    void handshake(int uid, nativehttp::rdata &rd, http::rproc::lrqd &ld)
    {
        cout << "ACC: "<<ld.ws_acc_key<<endl;
        cout << "PROT: "<<ld.ws_prot<<endl;
        cout << "URID: "<<find_uri(rd.uri.c_str())<<endl;
    }
}
