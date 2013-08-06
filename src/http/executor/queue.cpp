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
#endif

int ts = 0;

namespace http
{
    namespace rproc
	{
        http::request* get_proc(http::Sexecutor *exc)
        {
            while(1)
            {
                pthread_mutex_lock(http::mtx_exec);
                if (http::toexec.size() <= 0)
                {
                    pthread_mutex_unlock(http::mtx_exec);
                    utils::sleep(1);
                    continue;
                }
                if (http::toexec.front(ts)->taken > 0)
                {
                    pthread_mutex_unlock(http::mtx_exec);
                    utils::sleep(1);
                    continue;
                }
                if (ts == 1)
                {
                    pthread_mutex_unlock(http::mtx_exec);
                    utils::sleep(1);
                    continue;
                }
                http::toexec.front()->taken = exc->id;
                http::request *process = http::toexec.front(ts);
                if (ts == 1)
                {
                    pthread_mutex_unlock(http::mtx_exec);
                    utils::sleep(1);
                    continue;
                }
                http::toexec.pop();
                pthread_mutex_unlock(http::mtx_exec);

#ifdef NHDBG
                if(http::log_detailed)nativehttp::server::log("DETAIL@execQ","Got request; user = "+nativehttp::data::superstring::str_from_int(process->uid)
                    +"; executor ="+nativehttp::data::superstring::str_from_int(exc->id)+";");
#endif

                if(process)return process;
                utils::sleep(1);
			}
        }
	}
}
