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

#include "manager.h"
#include "nativehttp.h"
#include "http/data.h"
#include "http/stat.h"
#include "utils/thread.h"
#include "http/net/net.h"
#include "http/executor.h"


namespace manager
{
    void quit(int sig)
    {
        if (sig == SIGINT || sig == SIGTERM)
        {
            nativehttp::server::log("manager.cpp", "Saving statistics");
            http::statdata::save();

            nativehttp::server::log("manager.cpp", "Quitting");
            exit(0);
        }
    }

    void sig(int sig)
    {
        if (sig == 11 || sig == 4 || sig == 8)
        {
            if (pthread_equal(*http::theard_mg, pthread_self()) != 0)
            {
                nativehttp::server::err("manager.cpp", "[Warning]Internal crash");

                switch (mstate)
                {
                    case mgr_timeouts:
                        {
                            nativehttp::server::log("manager.cpp", "Disabling manager module Timeout");
                            http::manager::postto = -1;
                            http::manager::execto = -1;
                            break;
                        }
                    case mgr_fsref:
                        {
                            nativehttp::server::log("manager.cpp", "Disabling manager module APR");   //automatical page reloader - APR
                            http::manager::apr = false;
                            break;
                        }
                    case mgr_stat:
                        {
                            nativehttp::server::log("manager.cpp", "Disabling manager module Stat");
                            http::statdata::internal::managersafe = false;
                            break;
                        }
                    case mgr_sessions:
                        {
                            nativehttp::server::log("manager.cpp", "Disabling manager module Session");
                            mgrsess = false;
                            break;
                        }
                    case mgr_wait:
                        {
                            nativehttp::server::log("manager.cpp", "Disabling manager");
                            pthread_cancel(pthread_self());
                            return;
                        }
                }


                int tmkm = utils::create_thread(http::theard_mg, manager::manager, NULL, 256 * 1024);
                if (tmkm != 0)nativehttp::server::err("manager.cpp", "Manager failed to restart");

                pthread_cancel(pthread_self());
            }
            if (pthread_equal(*http::theard_nc, pthread_self()) != 0)
            {
                nativehttp::server::err("manager.cpp", "ANC module crash");
                pthread_cancel(pthread_self());
            }
            if (pthread_equal(*http::theard_fs, pthread_self()) != 0)
            {
                nativehttp::server::err("manager.cpp", "File sender module crash");
                pthread_cancel(pthread_self());
            }
            for (int i = 0; i < http::Nsend; i++)
            {
                if (pthread_equal(*(http::theard_sd[i]), pthread_self()) != 0)
                {
                    nativehttp::server::err("WARNING", "Sender theard crashed, rescuing");
                    pthread_mutex_unlock(http::mtx_snd);   //may be needed

                    pthread_t *kth = http::theard_sd[i];

                    int tmks = utils::create_thread(http::theard_sd[i], http::bsd::sender, NULL, 16 * 1024);
                    if (tmks != 0)nativehttp::server::err("manager.cpp", "Sender failed to start");

                    pthread_cancel(*kth);
                    delete kth;

                }
            }

            for (int i = 0; i < http::Nexec; i++)
            {
                if (pthread_equal(*(http::execUnits[i].etheard), pthread_self()) != 0)
                {
                    nativehttp::server::err("WARNING", "Execution theard crashed, rescuing");
                    http::execUnits[i].state = -1;
                    http::execUnits[i].in = 0;
                    pthread_mutex_unlock(http::mtx_exec2);
                    pthread_mutex_unlock(http::mtx_exec);

                    pthread_t *kth = http::execUnits[i].etheard;

                    pthread_t *tt = new pthread_t;
                    int tms = utils::create_thread(tt, http::executor, &(http::execUnits[i]), 256 * 1024);

                    http::execUnits[i].etheard = tt;
                    if (tms != 0)nativehttp::server::err("manager.cpp", "Executor failed to start");

                    pthread_cancel(*kth);
                    delete kth;//this will never happen
                }
            }


            //wtf, here?
            nativehttp::server::err("WARNING", "Unknown thread has crashed, ignoring");
            pthread_cancel(pthread_self());

        }
    }
}

