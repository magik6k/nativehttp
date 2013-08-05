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
#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <pthread.h>

namespace utils
{
    struct condex
    {
        pthread_mutex_t mtx;
        pthread_cond_t cnd;
    };

    int condex_init(condex* cdx);

    int condex_send_begin(condex* cdx);
    int condex_send_end(condex* cdx);

    int condex_recv_begin(condex* cdx);
    int condex_recv_end(condex* cdx);

    int create_thread(pthread_t* thr, void *(*fn) (void *), void* fnarg, unsigned int heap);
    pthread_mutex_t* create_mutex();


}

#endif // THREAD_H_INCLUDED
