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
#include "manager.h"
#include "http/data.h"
#include "http/executor.h"
#include "http/stat.h"
#include "data/session/session.h"
#include "http/net/net.h"
#include "utils/thread.h"

namespace manager
{
    manager::managestate mstate = mgr_wait;
    bool mgrsess = true;

    void *manager(void *unused)
    {
        prctl(PR_SET_NAME, "nh-manager", 0, 0, 0);

        if (http::manager::rate == -1)return 0;
        while (1)
        {
            mstate = manager::mgr_timeouts;
            manager::timeouts();

            mstate = manager::mgr_fsref;
            manager::fsrefresh();

            mstate = manager::mgr_stat;
            http::statdata::manage();

            if (http::usesessions && mgrsess)
            {
                mstate = manager::mgr_sessions;
                sdata::session::storage.mng();
            }

            mstate = manager::mgr_wait;
            manager::wait();

        }
        return NULL;
    }

    void fsrefresh()
    {
        if (http::manager::apr)pmap.refresh(http::manager::fileloc);
    }

    void timeouts()
    {
        for (int i = 0; i < http::Nexec; i++)
        {
            if (http::execUnits[i].state != -1)
            {
                switch (http::execUnits[i].in)
                {
                    case 1:
                    case 3:
                        if (time(0) - http::execUnits[i].in > http::manager::postto && http::manager::postto != -1)
                        {
                            if (http::execUnits[i].fd1)delete(nativehttp::data::postgetdata*)http::execUnits[i].fd1;
                            if (http::execUnits[i].fd2)delete(nativehttp::data::postgetdata*)http::execUnits[i].fd2;
                            //SDL_KillThread(http::execUnits[i].etheard);
                            http::execUnits[i].in = 0;
                            http::execUnits[i].state = -1;
                            http::execUnits[i].fd1 = NULL;
                            http::execUnits[i].fd2 = NULL;
                            //SDL_CreateThread(http::executor,&(http::execUnits[i]));
                        }
                        break;
                    case 2:
                        if (time(0) - http::execUnits[i].in > http::manager::execto && http::manager::execto != -1)
                        {

                            nativehttp::server::logid(i, "manager.cpp", "PAGE EXECUTION TIMEOUT");
                            if (http::execUnits[i].fd1)delete(nativehttp::data::postgetdata*)http::execUnits[i].fd1;
                            if (http::execUnits[i].fd2)delete(nativehttp::data::postgetdata*)http::execUnits[i].fd2;
                            //SDL_KillThread(http::execUnits[i].etheard);
                            http::execUnits[i].in = 0;
                            http::execUnits[i].state = -1;
                            http::execUnits[i].fd1 = NULL;
                            http::execUnits[i].fd2 = NULL;
                            //SDL_CreateThread(http::executor,&(http::execUnits[i]));
                        }
                        break;
                }
            }
        }
    }
    void wait()
    {
        utils::sleep(http::manager::rate * 1000);
    }

}

