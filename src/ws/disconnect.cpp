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
    void clean(int uid)
    {
        cout <<uid<<" - cleaning up\n";
        ws::frames[uid].busy = false;
        if(ws::frames[uid].fdata)
        {
            delete[] ws::frames[uid].fdata;
            ws::frames[uid].fdata = NULL;
        }

        ws::frames[uid].recived = 0;

        ws::messages[uid].type = WS_MTYPE_NONE;
        if(ws::messages[uid].data)
        {
            delete[] ws::messages[uid].data;
            ws::messages[uid].data = NULL;
        }

        ws::stream_state = WS_STREAMSTATE_FREE;

        if((*ws::units)[ws::client_unit[uid]].on_disconnect)
            (*((*ws::units)[ws::client_unit[uid]].on_disconnect))(uid);
    }

    void disconnect(int uid)
    {
        cout << "Disconnecting with "<<uid<<"\n";
        ws::clean(uid);
        http::client_protocol[uid] = CLPROT_HTTP;
        http::kickclient(uid);
    }
}

namespace nativehttp
{
    namespace websock
    {
        int disconnect(nativehttp::data::clientid uid)
        {
            if(ws::enabled)
            {
                if(http::client_protocol[uid] == CLPROT_WEBSOCKETS)
                {
                    http::kickclient(uid);
                }
                else
                {
                    return NH_ERROR;
                }
            }
            else
            {
                return NH_ERROR;
            }
            return NH_SUCCES;
        }
    }
}
