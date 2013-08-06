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
#include "protocol.h"
#include "../executor.h"
#include "../data.h"
#include "../stat.h"
#include "../net/net.h"

#ifdef NHDBG
#include "utils/memory.h"
size_t lastexecmem = 0;
#endif

namespace http
{
    namespace rproc
    {

        uint8_t ex(nativehttp::data::pagedata &pd, nativehttp::rdata *rd, http::rproc::lrqd &ld)
        {
            page pid = pmap.by_uri(rd->uri.c_str());

            switch (pid.type)
            {
                case page_native:
                    {
#ifdef NHDBG
                        if(http::log_detailed)nativehttp::server::log("DETAIL@execCore","Requested: NativePage; user = "+nativehttp::data::superstring::str_from_int(ld.uid)+";");
#endif

                        rd->ctype = "text/html;charset=" + charset;
                        rd->response = "200 OK";

                        nativepage *npp = (nativepage*)pid.data;
#ifdef NHDBG
                        size_t bm = getacmem() + getrsmem();
                        unsigned int pgt = utils::get_time();
#endif
                        nativehttp::data::pagedata ts;
                        if(!http::mtx_exec2)
                        {
                            ts = npp->page(rd);   //<<<execution of page
                        }
                        else
                        {
                            pthread_mutex_lock(http::mtx_exec2);
                            ts = npp->page(rd);   //<<<execution of page
                            pthread_mutex_unlock(http::mtx_exec2);
                        }
#ifdef NHDBG
                        unsigned int et = utils::get_time() - pgt;
                        if (!http::extmemstats)
                        {
                            cout << "[DBG:executor.cpp@http]Page execution allcocated: "
                                 << (getacmem() + getrsmem() - bm) / 1024LL << "kb, time: " << et << "ms\n";
                        }
                        else
                        {
                            cout << "-------\n";
                            cout << "[Debug]Native Page execution stats:\n";
                            cout << "File: " << pid.file << endl;
                            cout << "Time of execution: " << et << "ms\n";

                            cout << "Total Memory: ";
                            utils::memory::printmemsize(getacmem() + getrsmem());
                            cout << endl;

                            cout << "Memory allocated by page execution: ";
                            utils::memory::printmemsize(getacmem() + getrsmem() - bm);
                            cout << endl;

                            cout << "Memory change since init: ";
                            utils::memory::printmemsize
                            (int64_t(getacmem() + getrsmem()) - int64_t(http::init_memory));
                            cout << endl;

                            cout << "Memory change since LPE: ";
                            utils::memory::printmemsize
                            (int64_t(getacmem() + getrsmem()) - int64_t(lastexecmem));
                            cout << endl;

                            cout << "-------\n";
                        }

                        lastexecmem = getacmem() + getrsmem();
#endif
                        string snd = "HTTP/1.1 " + rd->response + "\r\n" + http::headers::standard;
                        snd += http::headers::alive + http::headers::alivetimeout;
                        snd += "Content-Type: " + rd->ctype + "\r\nContent-Length: ";
                        snd += its(ts.size);
                        snd += "\r\n";
                        snd += rd->cookie->gethead();
                        snd += "\r\n";
                        string snd2 = "\r\n";

                        statdata::onhttpsend(snd.size());

                        pd.size = snd.size() + ts.size + snd2.size();
                        pd.data = new char[pd.size];
                        memcpy(pd.data, snd.c_str(), snd.size());
                        memcpy(pd.data + snd.size(), ts.data, ts.size);
                        memcpy(pd.data + snd.size() + ts.size, snd2.c_str(), snd2.size());

                        #ifdef NHDBG

                        if(http::log_httprep)
                        {
                            nativehttp::server::log("DBG:excore.cpp@http",
                                ("Response:\n"+(http::log_newline?((nativehttp::data::superstring(pd.data).lock(0).change("\r","\\r").change("\n","\\n\n")).str)
                                :(string(pd.data)))).c_str());
                        }

                        #endif

                        delete[]ts.data;
                        return 0;
                    }
                    break;
                case page_file:
                    {
#ifdef NHDBG
                        if(http::log_detailed)nativehttp::server::log("DETAIL@execCore","Requested: FILE; user = "+nativehttp::data::superstring::str_from_int(ld.uid)+";");
#endif
                        pd.data = NULL;
                        fsrq req;
                        req.file = (const char*)pid.data;
                        req.uid = ld.uid;
                        req.rngs = ld.rng_start;
                        req.rnge = ld.rng_end;

                        pthread_mutex_lock(http::mtx_fsnd);
                        http::fsend.push(req);
                        pthread_mutex_unlock(http::mtx_fsnd);

                        return 2;

                    }
                    break;
                default:
                    {
#ifdef NHDBG
                        if(http::log_detailed)nativehttp::server::log("DETAIL@execCore","Requested: 404; user = "+nativehttp::data::superstring::str_from_int(ld.uid)+";");
#endif
                        return 1;
                    }
            }
            return 1;

        }

    }//namespace rproc
}//http namespace
