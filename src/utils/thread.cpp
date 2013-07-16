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
#include <pthread.h>

namespace utils
{
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

        return pthread_create(thr, &at, fn, fnarg);
    }
    pthread_mutex_t* create_mutex()
    {
        pthread_mutex_t* tmp = new pthread_mutex_t;

        if(tmp)
            pthread_mutex_init(tmp, NULL);

        return tmp;
    }
}
