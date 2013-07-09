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

namespace nativehttp
{
    namespace websock
    {
        int onConnect(int appid, cb_onConnect onConnect)
        {
            if(appid<0)return NH_ERROR;
            if(!ws::enabled)return NH_ERROR;
            if((unsigned int)appid>=ws::units->size())return NH_ERROR;

            (*ws::units)[appid].on_connect = onConnect;
            return NH_SUCCES;
        }

        int onDisconnect(int appid, cb_onDisconnect onDisconnect)
        {
            if(appid<0)return NH_ERROR;
            if(!ws::enabled)return NH_ERROR;
            if((unsigned int)appid>=ws::units->size())return NH_ERROR;

            (*ws::units)[appid].on_disconnect = onDisconnect;
            return NH_SUCCES;
        }

        int onText(int appid, cb_onTxtMsg onTMsg)
        {
            if(appid<0)return NH_ERROR;
            if(!ws::enabled)return NH_ERROR;
            if((unsigned int)appid>=ws::units->size())return NH_ERROR;

            (*ws::units)[appid].on_txt_msg = onTMsg;
            return NH_SUCCES;
        }

        int onBinary(int appid, cb_onBinMsg onBMsg)
        {
            if(appid<0)return NH_ERROR;
            if(!ws::enabled)return NH_ERROR;
            if((unsigned int)appid>=ws::units->size())return NH_ERROR;

            (*ws::units)[appid].on_bin_msg = onBMsg;
            return NH_SUCCES;
        }

    }
}
