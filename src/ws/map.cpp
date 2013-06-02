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
#include "nativehttp.h"
#include <string.h>


namespace nativehttp
{
    namespace websock
    {
        int add(const char* uri, const char* protocol_name, cb_onConnect onConnect, cb_onTxtFrame onTframe, cb_onBinFrame onBFrame)
        {
            if(ws::enabled)
            {
                ws::ws_ent temp;
                temp.uri = strdup(uri);
                temp.protocol = strdup(protocol_name);

                temp.on_connect = onConnect;
                temp.on_txt_frame = onTframe;
                temp.on_bin_frame = onBFrame;

                ws::units->push_back(temp);
            }
            return ws::enabled?1:0;
        }
    }
}
