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
#include <sys/time.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include "time.h"

struct timeval start;

namespace utils
{
    void init_time()
    {
        gettimeofday(&start,NULL);
    }
    uint64_t get_time()
    {
        struct timeval tmp;
        gettimeofday(&tmp,NULL);

        return ((uint64_t(tmp.tv_sec)*1000LL)+(uint64_t(tmp.tv_usec)/1000LL))-((uint64_t(start.tv_sec)*1000LL)+(uint64_t(start.tv_usec)/1000LL));
    }
    void sleep(int t)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(t));
    }
}

