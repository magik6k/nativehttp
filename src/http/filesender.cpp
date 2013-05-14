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
#include "filesender.h"
#include "data.h"

namespace http
{
    void *fsender(void *unused)
    {
        prctl(PR_SET_NAME, "nh-fsend", 0, 0, 0);
        nativehttp::server::log("filesender.cpp","fs UP!");
        while(1)
        {


            for(size_t i = 0; i<http::sqln; i++)
            {

            }

            SDL_mutexP(http::mtx_fsnd);
            if(http::fsend.size()>0)///Recive new sending requests
            {
                fsrq tr = http::fsend.front();

                for(size_t i = 0; i < http::sqln; i++)
                {
                    if(http::shq[i].uid==-1)
                    {
                        http::shq[i].uid = tr.uid;
                        http::shq[i].rngs = tr.rngs;
                        http::shq[i].rnge = tr.rnge;
                        http::shq[i].file = tr.file;

                        http::shq[i].state = FSS_Init;

                        http::fsend.pop();

                        nativehttp::server::log("filesender.cpp","Recived request");
                        break;

                    }
                }

            }
            SDL_mutexV(http::mtx_fsnd);
        }

        return NULL;
    }
}
