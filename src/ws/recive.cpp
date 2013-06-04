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
#include "data/queue.h"

#define WOP_CNT 0
#define WOP_TXT 1
#define WOP_BIN 2
#define WOP_CLS 8
#define WOP_PIN 9
#define WOP_PON 10


namespace ws
{

    struct rmsp
    {
        unsigned char* data;
        int datalen;
        int uid;
    };

    nativehttp::data::queue<rmsp> rqueue;

    void *rmsgproc(void *unused)
    {
        prctl(PR_SET_NAME, "nh-ws-rcproc", 0, 0, 0);

        rmsp proc;
        proc.data = NULL;

        while(1)
        {
            if(proc.data)
            {
                delete[] proc.data;
                proc.data = NULL;
            }

            SDL_mutexP(http::mtx_wsrc);
            if(!rqueue.empty())
            {
                proc = rqueue.front();
            }
            else
            {
                SDL_mutexV(http::mtx_wsrc);
                SDL_Delay(1);
                continue;
            }
            rqueue.pop();
            SDL_mutexV(http::mtx_wsrc);

            if(!(0b01110000&proc.data[0]))
            {
                uint8_t opcode = 0b00001111&proc.data[0];

                bool mask = (0b10000000&proc.data[1])/128;
                bool fin = (0b10000000&proc.data[0])/128;

                uint64_t frame_size = (0b01111111&proc.data[1]);
                uint32_t mkey = 0;

                size_t pos = 2;

                unsigned char* fdata = NULL;

                if(frame_size == 126)
                {
                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        frame_size = *((uint16_t*)(&proc.data[pos]));
                    }
                    else
                    {
                        frame_size = 0;
                        ((uint8_t*)&frame_size)[1] = (proc.data[pos]);
                        ((uint8_t*)&frame_size)[0] = (proc.data[pos+1]);
                    }
                    pos += 2;
                }
                else if(frame_size == 127)
                {
                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        frame_size = *((uint64_t*)(&proc.data[pos]));
                    }
                    else
                    {
                        frame_size = 0;
                        ((uint8_t*)&frame_size)[7] = (proc.data[pos]);
                        ((uint8_t*)&frame_size)[6] = (proc.data[pos+1]);
                        ((uint8_t*)&frame_size)[5] = (proc.data[pos+2]);
                        ((uint8_t*)&frame_size)[4] = (proc.data[pos+3]);
                        ((uint8_t*)&frame_size)[3] = (proc.data[pos+4]);
                        ((uint8_t*)&frame_size)[2] = (proc.data[pos+5]);
                        ((uint8_t*)&frame_size)[1] = (proc.data[pos+6]);
                        ((uint8_t*)&frame_size)[0] = (proc.data[pos+7]);
                    }
                    pos += 8;
                }

                fdata = new unsigned char[frame_size+1];


                if(mask)
                {
                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        mkey = *((uint32_t*)(&proc.data[pos]));
                    }
                    else
                    {
                        mkey = 0;
                        ((uint8_t*)&mkey)[3] = (proc.data[pos]);
                        ((uint8_t*)&mkey)[2] = (proc.data[pos+1]);
                        ((uint8_t*)&mkey)[1] = (proc.data[pos+2]);
                        ((uint8_t*)&mkey)[0] = (proc.data[pos+3]);
                    }
                    mkey = *((uint32_t*)(&proc.data[pos]));
                    pos += 4;
                }

                uint64_t rcvd = 0;

                for(uint64_t i;i<frame_size&&pos+i<proc.datalen;i++)
                {
                    fdata[i] = mask?proc.data[pos+i]^(((uint8_t*)&mkey)[i%4]):proc.data[pos+i];rcvd++;
                }
                fdata[rcvd] = '\0';

                cout << "Recived frame packet(valid:"<<!(0b01110000&proc.data[0])<<"), op: "<<int(opcode)<<",size: "<<frame_size<<"("<<int(0b01111111&proc.data[1])<<"), end:"<<int(0b10000000&proc.data[0])/128<<", mask: "<<mkey<<"\n";
                cout << "DATA("<<rcvd<<"):"<<fdata<<"\n";


                delete[] fdata;


            }
            else
            {
                cout << "invalid frame["<<int(0b00001111&proc.data[0])<<","<<proc.data[0]<<"]("<<int(0b01000000&proc.data[0])<<","<<int(0b00100000&proc.data[0])<<","<<int(0b00010000&proc.data[0])<<")\n";
            }



        }
        return NULL;
    }

    void rcv_push(unsigned char* data, int dlen, int uid)
    {
        rmsp tmp;
        tmp.data=data;
        tmp.datalen=dlen;
        tmp.uid=uid;
        SDL_mutexP(http::mtx_wsrc);
        rqueue.push(tmp);
        SDL_mutexV(http::mtx_wsrc);
    }

}
