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
        uint32_t datalen;
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
            const int uid = proc.uid;

            if(!ws::frames[uid].busy)
            {
                if(!(0b01110000&proc.data[0]))
                {
                    ws::frames[uid].busy = true;

                    ws::frames[uid].opcode = 0b00001111&proc.data[0];

                    ws::frames[uid].mask = (0b10000000&proc.data[1])/128;
                    ws::frames[uid].fin = (0b10000000&proc.data[0])/128;

                    ws::frames[uid].frame_size = (0b01111111&proc.data[1]);
                    ws::frames[uid].mkey = 0;

                    ws::frames[uid].pos = 2;

                    ws::frames[uid].fdata = NULL;

                    if(ws::frames[uid].frame_size == 126)
                    {
                        if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                        {
                            ws::frames[uid].frame_size = *((uint16_t*)(&proc.data[ws::frames[uid].pos]));
                        }
                        else
                        {
                            ws::frames[uid].frame_size = 0;
                            ((uint8_t*)&ws::frames[uid].frame_size)[1] = (proc.data[ws::frames[uid].pos]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[0] = (proc.data[ws::frames[uid].pos+1]);
                        }
                        ws::frames[uid].pos += 2;
                    }
                    else if(ws::frames[uid].frame_size == 127)
                    {
                        if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                        {
                            ws::frames[uid].frame_size = *((uint64_t*)(&proc.data[ws::frames[uid].pos]));
                        }
                        else
                        {
                            ws::frames[uid].frame_size = 0;
                            ((uint8_t*)&ws::frames[uid].frame_size)[7] = (proc.data[ws::frames[uid].pos]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[6] = (proc.data[ws::frames[uid].pos+1]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[5] = (proc.data[ws::frames[uid].pos+2]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[4] = (proc.data[ws::frames[uid].pos+3]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[3] = (proc.data[ws::frames[uid].pos+4]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[2] = (proc.data[ws::frames[uid].pos+5]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[1] = (proc.data[ws::frames[uid].pos+6]);
                            ((uint8_t*)&ws::frames[uid].frame_size)[0] = (proc.data[ws::frames[uid].pos+7]);
                        }
                        ws::frames[uid].pos += 8;
                    }

                    if(ws::frames[uid].frame_size > ws::maxframesize)
                    {
                        cout << "Oversized frame!\n";
                        ws::disconnect(uid);
                        continue;
                    }

                    if(ws::frames[uid].opcode >= 0x8 && ws::frames[uid].frame_size > 125)
                    {
                        cout << "Control frames must be sized up to 125 bytes!\n";
                        ws::disconnect(uid);
                        continue;
                    }

                    ws::frames[uid].fdata = new unsigned char[ws::frames[uid].frame_size+1];
                    ws::frames[uid].recived = 0;

                    if(ws::frames[uid].mask)
                    {
                        if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                        {
                            ws::frames[uid].mkey = *((uint32_t*)(&proc.data[ws::frames[uid].pos]));
                        }
                        else
                        {
                            ws::frames[uid].mkey = 0;
                            ((uint8_t*)&ws::frames[uid].mkey)[3] = (proc.data[ws::frames[uid].pos]);
                            ((uint8_t*)&ws::frames[uid].mkey)[2] = (proc.data[ws::frames[uid].pos+1]);
                            ((uint8_t*)&ws::frames[uid].mkey)[1] = (proc.data[ws::frames[uid].pos+2]);
                            ((uint8_t*)&ws::frames[uid].mkey)[0] = (proc.data[ws::frames[uid].pos+3]);
                        }
                        ws::frames[uid].mkey = *((uint32_t*)(&proc.data[ws::frames[uid].pos]));
                        ws::frames[uid].pos += 4;
                    }

                    for(uint64_t i = 0;i<ws::frames[uid].frame_size&&ws::frames[uid].pos+i<proc.datalen;i++)
                    {
                        ws::frames[uid].fdata[i] = ws::frames[uid].mask?proc.data[ws::frames[uid].pos+i]^(((uint8_t*)&ws::frames[uid].mkey)[i%4]):proc.data[ws::frames[uid].pos+i];
                        ws::frames[uid].recived++;
                    }
                    ws::frames[uid].fdata[ws::frames[uid].recived] = '\0';

                    cout << "Recived frame packet(valid:"<<!(0b01110000&proc.data[0])<<"), op: "<<int(ws::frames[uid].opcode)<<",size: "<<ws::frames[uid].frame_size<<"("<<int(0b01111111&proc.data[1])<<"), end:"<<int(0b10000000&proc.data[0])/128<<", mask: "<<ws::frames[uid].mkey<<"\n";

                    if(ws::frames[uid].recived >= ws::frames[uid].frame_size)
                    {
                        ws::frames[uid].busy = false;
                        if(ws::frames[uid].opcode < 0x8)
                        {
                            ws::msg_push(uid, ws::frames[uid].opcode, ws::frames[uid].fin, ws::frames[uid].frame_size, ws::frames[uid].fdata);
                        }
                        else
                        {
                            ws::control(uid, ws::frames[uid].opcode, ws::frames[uid].frame_size, ws::frames[uid].fdata);
                        }

                        if(ws::frames[uid].fdata)cout << "DATA("<<ws::frames[uid].recived<<"):"<<ws::frames[uid].fdata<<"\n";
                        if(ws::frames[uid].fdata)
                        {
                            delete[] ws::frames[uid].fdata;
                            ws::frames[uid].fdata = NULL;
                        }
                    }

                }
                else
                {
                    cout << "invalid frame["<<int(0b00001111&proc.data[0])<<","<<proc.data[0]<<"]("<<int(0b01000000&proc.data[0])<<","<<int(0b00100000&proc.data[0])<<","<<int(0b00010000&proc.data[0])<<")\n";
                }
            }
            else
            {
                cout << "Recived Data packet, size: "<<proc.datalen<<endl;
                for(uint64_t i = 0;ws::frames[uid].recived<ws::frames[uid].frame_size&&i<proc.datalen;i++)
                {
                    ws::frames[uid].fdata[i] = ws::frames[uid].mask?proc.data[i]^(((uint8_t*)&ws::frames[uid].mkey)[ws::frames[uid].recived%4]):proc.data[i];
                    ws::frames[uid].recived++;
                }
                if(ws::frames[uid].recived >= ws::frames[uid].frame_size)
                {
                    ws::frames[uid].busy = false;
                    if(ws::frames[uid].opcode < 0x8)
                    {
                        ws::msg_push(uid, ws::frames[uid].opcode, ws::frames[uid].fin, ws::frames[uid].frame_size, ws::frames[uid].fdata);
                    }
                    else
                    {
                        ws::control(uid, ws::frames[uid].opcode, ws::frames[uid].frame_size, ws::frames[uid].fdata);
                    }
                    if(ws::frames[uid].fdata)cout << "DATA("<<ws::frames[uid].recived<<"):"<<ws::frames[uid].fdata<<"\n";
                    if(ws::frames[uid].fdata)
                    {
                        delete[] ws::frames[uid].fdata;
                        ws::frames[uid].fdata = NULL;
                    }
                }
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
