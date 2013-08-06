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
#include "thread.h"
#include "time.h"
#include <pthread.h>

namespace utils
{

    struct thr_t
    {
        void* data;
        void *(*fn) (void *);
    };

    void* thread_runner(void* fn)
    {
        void* rt = (((thr_t*)fn)->fn)(((thr_t*)fn)->data);
        delete (thr_t*)fn;
        nativehttp::server::log("thread.cpp@utils","A thread has quit");
        return rt;
    }

    int create_thread(pthread_t* thr, void *(*fn) (void *), void* fnarg, unsigned int heap)
    {
        pthread_attr_t at;
        if (pthread_attr_init(&at) != 0)
        {
            nativehttp::server::log("thread.cpp@utils", "ERROR: attr setup failed");
            return -1;
        }
        if (pthread_attr_setstacksize(&at, heap) != 0)
        {
            nativehttp::server::log("thread.cpp@utils", "ERROR: Setting heap size failed");
            return -1;
        }
        thr_t* dp = new thr_t;
        dp->fn = fn;
        dp->data = fnarg;
        return pthread_create(thr, &at, thread_runner, (void*)dp);
    }
    pthread_mutex_t* create_mutex()
    {
        pthread_mutex_t* tmp = new pthread_mutex_t;

        if(tmp)
            pthread_mutex_init(tmp, NULL);

        if(!tmp)
        {
            nativehttp::server::err("thread.cpp@utils", "ERROR: Creating mutex failed(Out of memory?)");
        }

        return tmp;
    }

    condex* create_condex()
    {
        condex* cdx = new condex;

        if(condex_init(cdx))
        {
            nativehttp::server::err("thread.cpp@utils", "ERROR: Creating condex failed(Out of memory?)");
        }
        return cdx;
    }

    int condex_init(condex* cdx)
    {
        if(cdx == NULL)return EINVAL;
        cdx->mtx = PTHREAD_MUTEX_INITIALIZER;
        cdx->smtx = PTHREAD_MUTEX_INITIALIZER;
        cdx->cnd = PTHREAD_COND_INITIALIZER;
        return 0;
    }

    int condex_send_begin(condex* cdx)
    {
                                    nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE");
        if(cdx == NULL)return EINVAL;
                                    nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE");
        int e = pthread_mutex_trylock(&cdx->mtx);
                                    nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE");
        if(e == 0) return 0;
                                    nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE");

            int LIMIT = 15;


        while(e != 0&&LIMIT>0)
        {
            if(e == EBUSY)
            {
                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE-T 0");
                utils::sleep(100);
                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE-T 1");
                --LIMIT;
                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE-T 2");
            }
            else if(e == EINVAL||e == EAGAIN)
            {
                nativehttp::server::err("DBG:send_begin@condex@thread.cpp","Invalid Condex");
                return EINVAL;
            }
            else if(e == EDEADLK)
            {
                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE-DDL");
                return 0;
            }
            nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE-RT 0");
            e = pthread_mutex_trylock(&cdx->mtx);
            nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CDE-RT 1");
        }

#ifdef NHDBG
        nativehttp::server::err("DBG:send_begin@condex@thread.cpp","Condex timed out");
#endif

        return e;
    }

    int condex_send_end(condex* cdx)
    {
        if(cdx == NULL)return EINVAL;
        int e = pthread_cond_signal(&cdx->cnd);
        if(e!=0)return e;
        return pthread_mutex_unlock(&cdx->mtx);
    }


    int condex_recv_begin(condex* cdx)
    {
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 1");
        if(cdx == NULL)
        {
#ifdef NHDBG
            nativehttp::server::err("DBG:condex@thread.cpp","NULL Condex");
#endif
            return EINVAL;
        }
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 2");
        int e = pthread_mutex_lock(&cdx->mtx);
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 3");
        if(e!=0)
        {
#ifdef NHDBG
            nativehttp::server::err("DBG:condex@thread.cpp","Mutex lock fail: "+nativehttp::data::superstring::str_from_int(e));
#endif
            return e;
        }
rewait:
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 4");
        e = pthread_cond_wait(&cdx->cnd, &cdx->mtx);
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 5");
        if(e)
        {
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 6-1");
#ifdef NHDBG
            nativehttp::server::err("DBG:condex@thread.cpp","Cond wait fail fail: "+nativehttp::data::superstring::str_from_int(e));
#endif
            return e;
        }
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 6-2");
        if(!pthread_mutex_trylock(&cdx->smtx))return 0;
                                                nativehttp::server::log("DEVELOPMENT-!!!!!@cd","CRB 7");
        pthread_mutex_unlock(&cdx->mtx);
        goto rewait;
    }

    int condex_recv_end(condex* cdx)
    {
        if(cdx == NULL)return EINVAL;
        pthread_mutex_unlock(&cdx->smtx);
        return pthread_mutex_unlock(&cdx->mtx);
    }

}
