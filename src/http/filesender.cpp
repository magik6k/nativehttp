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

#include "filesender.h"
#include "nativehttp.h"
#include "data.h"
#include "protocol.h"
#include "net/net.h"
#include <stdio.h>
#include "utils/memory.h"

namespace http
{
    void *fsender(void *unused)
    {
        prctl(PR_SET_NAME, "nh-fsend", 0, 0, 0);
        int fast = 100000;
        while(1)
        {
            if(fast > 0)fast--;
            for(size_t i = 0; i < http::sqln; i++)
            {
                if(http::shq[i].uid != -1)
                {
                    fast = 100000;
#ifdef NHDBG
                    if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","");
                    if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","FSEND for user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                        "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                    switch(http::shq[i].state)
                    {
                        case FSS_Init:
                            {
#ifdef NHDBG
                                if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Openning file, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                    "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                                http::shq[i].fd = open(http::shq[i].file, O_RDONLY, 0);
                                if(http::shq[i].fd == -1)
                                {
#ifdef NHDBG
                                    if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","File not found/IO error, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                        "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                                    http::send(http::shq[i].uid, http::error::e404.size, http::error::e404.data, false);
                                    http::unlockclient(http::shq[i].uid);
                                    http::shq[i].fd = -1;
                                    break;
                                }

                                struct stat fst;
                                if(fstat(http::shq[i].fd, &fst)<0)
                                {
                                    nativehttp::server::err("filesender.cpp@http","fstat error");

                                    http::send(http::shq[i].uid, http::error::e500.size, http::error::e500.data, false);
                                    http::unlockclient(http::shq[i].uid);
                                    http::shq[i].fd = -1;
                                    break;
                                }
                                http::shq[i].fsize = fst.st_size;


                                http::shq[i].buf = utils::memory::alloc<char>(http::fsnd_fb_size);

                                if(http::shq[i].rngs < 0)
                                {
                                    http::shq[i].rngs = http::shq[i].fsize - http::shq[i].rngs;
                                }

                                if(http::shq[i].rnge == -1)
                                {
                                    http::shq[i].rnge = http::shq[i].fsize;
                                }

                                string snd("HTTP/1.1 200 OK\r\n");
                                snd += http::headers::standard;
                                snd += http::headers::alive + http::headers::alivetimeout;
                                snd += mime->get_ctype(http::shq[i].file);
                                snd += "\r\nContent-Length: ";
                                snd += nativehttp::data::superstring::str_from_int64(fst.st_size);
                                snd += "\r\n\r\n";

                                char* hdbuf = utils::memory::alloc<char>(snd.size());
                                memcpy(hdbuf, snd.c_str(), snd.size());
#ifdef NHDBG
                                if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Sending header, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                    "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");


                                if(http::log_httprep)
                                {
                                    nativehttp::server::log("DBG:filesender.cpp@http",
                                        ("Response(header only):\n"+(http::log_newline?((nativehttp::data::superstring(hdbuf).lock(0).change("\r","\\r").change("\n","\\n\n")).str)
                                        :(string(hdbuf)))).c_str());
                                }

#endif

                                http::send(http::shq[i].uid, snd.size(), hdbuf, true);
                                http::shq[i].state = FSS_Reading;

                                break;
                            }
                        case FSS_Reading:
                            {
#ifdef NHDBG
                                if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Requesting async file read("
                                    +nativehttp::data::superstring::str_from_size(http::shq[i].rnge-http::shq[i].rngs)+" left), user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                        "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                                memset(&http::shq[i].aio, 0, sizeof(aiocb));

                                http::shq[i].aio.aio_fildes = http::shq[i].fd;
                                http::shq[i].aio.aio_buf = http::shq[i].buf;
                                http::shq[i].state = FSS_Sending;

                                http::shq[i].aio.aio_offset = http::shq[i].rngs;
                                http::shq[i].aio.aio_nbytes = http::fsnd_fb_size;

                                if (aio_read(&http::shq[i].aio) == -1)
                                {
#ifdef NHDBG
                                    if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Request failed, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                        "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                                    http::send(http::shq[i].uid, http::error::e500.size, http::error::e500.data, false);
                                    http::unlockclient(http::shq[i].uid);

                                    close(http::shq[i].fd);
                                    delete[] (char*)http::shq[i].buf;

                                    http::shq[i].fd = -1;
                                    break;
                                }

                                break;
                            }
                        case FSS_Sending:
                            {
                                int e = aio_error(&http::shq[i].aio);
                                if(e == EINPROGRESS)
                                {
#ifdef NHDBG
                                    if(http::log_detailed)
                                    {

                                        nativehttp::server::log("DETAIL@FileSender","Read Progress, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                            "; requesIDt = "+nativehttp::data::superstring::str_from_int(i)+";");
                                        utils::sleep(100);
                                    }
#endif
                                    break;
                                }
                                else if(e == ECANCELED)
                                {
                                    nativehttp::server::err("filesender.cpp@http","File sending canceled");
                                    http::kickclient(http::shq[i].uid);

                                    close(http::shq[i].fd);
                                    delete[] (char*)http::shq[i].buf;

                                    http::shq[i].fd = -1;
                                    break;
                                }

                                int brd = aio_return(&http::shq[i].aio);

#ifdef NHDBG
                                if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Reading chunk(result: "
                                    +nativehttp::data::superstring::str_from_size(brd)+"), user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                        "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif

                                if(brd <= 0)
                                {
#ifdef NHDBG
                                    if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Chunk read failed, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                        "; requesIDt = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                                    http::send(http::shq[i].uid, http::error::e500.size, http::error::e500.data, false);
                                    http::unlockclient(http::shq[i].uid);

                                    close(http::shq[i].fd);
                                    delete[] (char*)http::shq[i].buf;

                                    http::shq[i].fd = -1;
                                    break;
                                }
                                char* tdt = utils::memory::alloc<char>(brd);
                                memcpy(tdt, http::shq[i].buf, brd);

                                http::send(http::shq[i].uid, brd, tdt, true);
                                http::shq[i].state = FSS_Reading;

                                http::shq[i].rngs += brd;

                                if(http::shq[i].rnge <= http::shq[i].rngs)
                                {
                                    http::shq[i].state = FSS_Done;
                                }
                                break;
                            }
                        case FSS_Done:
                            {
#ifdef NHDBG
                                if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","File sending done, user = "+nativehttp::data::superstring::str_from_int(http::shq[i].uid)+
                                    "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                                http::unlockclient(http::shq[i].uid);

                                close(http::shq[i].fd);
                                delete[] (char*)http::shq[i].buf;

                                http::shq[i].uid = -1;
                                break;

                            }
                        default:
                            {
                                nativehttp::server::err("err@FileSender","Invalid FS OP-code, something went wrong");
                                utils::sleep(250);
                                exit(1);
                            }

                    }

                }
            }
            if(pthread_mutex_trylock(http::mtx_fsnd))
            {
#ifdef NHDBG
                if(http::log_detailed)nativehttp::server::err("DETAIL@FileSender","Could not lock request queue");
                utils::sleep(250);
#endif
                utils::sleep(1);
                continue;
            }
            if(http::fsend.size() > 0) ///Recive new sending requests
            {
                fsrq tr = http::fsend.front();
#ifdef NHDBG
                        if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Recivied file request, user = "+nativehttp::data::superstring::str_from_int(tr.uid)+"; file = "+tr.file);
#endif
                for(size_t i = 0; i < http::sqln; i++)
                {
                    if(http::shq[i].uid == -1)
                    {
#ifdef NHDBG
                        if(http::log_detailed)nativehttp::server::log("DETAIL@FileSender","Allocated request in table, user = "+nativehttp::data::superstring::str_from_int(tr.uid)+"; file = "+tr.file+
                            "; requestID = "+nativehttp::data::superstring::str_from_int(i)+";");
#endif
                        http::shq[i].uid = tr.uid;
                        http::shq[i].rngs = tr.rngs;
                        http::shq[i].rnge = tr.rnge;
                        http::shq[i].file = tr.file;

                        http::shq[i].state = FSS_Init;

                        http::fsend.pop();

                        break;

                    }
                }

            }
            pthread_mutex_unlock(http::mtx_fsnd);

            if(fast <= 0)
            {
                utils::sleep(5);
            }
            else
            {
                timespec t;
                t.tv_sec = 0;
                t.tv_nsec = 3000;//3 microseconds
                nanosleep(&t, NULL);
            }


        }

        return NULL;
    }
}
