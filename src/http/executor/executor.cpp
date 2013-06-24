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
#include "ws/ws.h"
#include "data/session/session.h"

#ifdef NHDBG
#include "utils/memory.h"
#endif

namespace http
{

    void *executor(void *eid)
    {
        http::Sexecutor *exc = (http::Sexecutor*)eid;

        exc->state = -1;

        nativehttp::rdata rd;
        http::rproc::lrqd ld;

        prctl(PR_SET_NAME, ("nh-exec-" + nativehttp::data::superstring::str_from_int(exc->id)).c_str(), 0, 0, 0);

        while (true)
        {

            http::request *process = http::rproc::get_proc(exc);
            http::statdata::onhit();

            #ifdef NHDBG

            if(http::log_requests)
            {
                nativehttp::server::log("DBG:executor.cpp@http",
                    ("Request:\n"+(http::log_newline?((nativehttp::data::superstring(process->request).lock(0).change("\r","\\r").change("\n","\\n\n")).str)
                    :(string(process->request)))).c_str());
            }

            #endif

            rd.get = NULL;
            rd.post = NULL;
            rd.cookie = NULL;
            rd.session = NULL;

            ld.clen = 0;
            ld.d501 = 0;

            ld.upg = false;
            ld.uwebsck = false;
            ld.ws_acc_key = NULL;
            ld.ws_prot = NULL;

            http::rproc::line0(process, rd, ld);

            switch (process->method)
            {
                case 1:
                    http::statdata::onget();
                    break;
                case 2:
                    http::statdata::onpost();
                    break;
            }

            if (process->method == 0)///unkown method
            {

                http::send(process->uid, http::error::e400.size, http::error::e400.data, false);
                http::unlockclient(process->uid);
                delete[] process->request;
                continue;
            }

            if (process->method == 3)///unimplemented method
            {
                http::send(process->uid, http::error::e501.size, http::error::e501.data, false);
                http::unlockclient(process->uid);
                delete[] process->request;
                continue;
            }
            if (!process->http11)///HTTP/1.1 only
            {
                http::send(process->uid, http::error::e505.size, http::error::e505.data, false);
                http::unlockclient(process->uid);
                delete[] process->request;
                delete process;
                continue;
            }

            http::rproc::header(process, rd, ld);

            if(ld.d501)
            {
                if (rd.cookie)
                {
                    delete rd.cookie;
                }
                if (rd.get)
                {
                    delete rd.get;
                }
                if (rd.post)
                {
                    delete rd.post;
                }

                http::send(process->uid, http::error::e501.size, http::error::e501.data, false);
                http::unlockclient(process->uid);
                delete process;
                process = NULL;
                continue;
            }

            if (!rd.cookie)
            {
                rd.cookie = new nativehttp::data::cookiedata("");
            }

            if (rd.cookie && http::usesessions)///setup sessions
            {
                rd.session = data::session::storage.getuid(process->uid);
                rd.session->__init(rd.cookie, process->uid);
            }

            if(ld.upg)
            {
                if(ld.uwebsck)
                {

                    if(ws::enabled)ws::handshake(process->uid, rd, ld);
                    else
                    {

                        http::bsd::sendNow(process->uid, http::error::e403.size, http::error::e403.data, false);
                        exc->state = -1;
                        exc->in = 0;
                        http::kickclient(process->uid);
                        delete[] process->request;
                        delete process;
                        continue;

                    }

                    if (rd.cookie)
                    {
                        delete rd.cookie;
                    }
                    if (rd.get)
                    {
                        delete rd.get;
                    }
                    if (rd.post)
                    {
                        delete rd.post;
                    }
                    delete[] process->request;
                    delete process;
                    continue;
                }
            }

            ///POST checking begin
            if (ld.clen > 0 && process->method == 2)
            {
                if (ld.clen > http::maxPost)
                {
                    if (rd.cookie)
                    {
                        delete rd.cookie;
                    }
                    if (rd.get)
                    {
                        delete rd.get;
                    }
                    if (rd.post)
                    {
                        delete rd.post;
                    }
                    exc->fd1 = NULL;
                    exc->fd2 = NULL;
                    exc->state = time(0);
                    exc->in = 3;
                    http::bsd::sendNow(process->uid, http::error::e403.size, http::error::e403.data, false);
                    exc->state = -1;
                    exc->in = 0;
                    http::kickclient(process->uid);
                    delete[] process->request;
                    delete process;
                    continue;
                }
                exc->fd1 = rd.cookie;
                exc->fd2 = rd.get;
                exc->state = time(0);
                exc->in = 1;
                http::rproc::post(rd, process, ld);
                exc->state = -1;
                exc->in = 0;
                if (!rd.post)
                {
                    delete[] process->request;
                    delete process;
                    process->request = NULL;
                    if (rd.cookie)
                    {
                        delete rd.cookie;
                    }
                    if (rd.get)
                    {
                        delete rd.get;
                    }
                    if (rd.post)
                    {
                        delete rd.post;
                    }
                    http::unlockclient(process->uid);
                    continue;//will be disconnected
                }
            }
            ///POST checking end


            delete[] process->request;
            process->request = NULL;

            rd.remoteIP = http::client_ips[process->uid];

            nativehttp::data::pagedata result;
            exc->fd1 = rd.cookie;
            exc->fd2 = rd.get;
            exc->in = 2;
            exc->state = time(0);

            ld.uid = process->uid;

            uint8_t exstate = http::rproc::ex(result, &rd, ld);

            ///CLEANUP, sending
            if (exstate == 1)
            {
                exc->state = -1;
                exc->in = 0;
                if (rd.cookie)
                {
                    delete rd.cookie;
                }
                if (rd.get)
                {
                    delete rd.get;
                }
                if (rd.post)
                {
                    delete rd.post;
                }
                http::send(process->uid, http::error::e404.size, http::error::e404.data, false);
                http::unlockclient(process->uid);
                delete process;
                process = NULL;
                continue;
            }

            exc->state = -1;
            exc->in = 0;

            if (rd.cookie)
            {
                delete rd.cookie;
            }
            if (rd.get)
            {
                delete rd.get;
            }
            if (rd.post)
            {
                delete rd.post;
            }

            if(exstate != 2)
            {
                if (result.data)
                {
                    http::send(process->uid, result.size, result.data, true);
                }

                http::unlockclient(process->uid);
            }

            delete process;
            process = NULL;

            ///CLEANUP end

        }
        return NULL;
    }
}
