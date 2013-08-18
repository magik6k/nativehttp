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

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED
#include <sys/types.h>
#include <new>
#include "backtrace.h"
#include "nativehttp.h"
namespace utils
{
	namespace memory
	{
#ifdef NHDBG
		void printmemsize(int64_t mem);
#endif

        template<class T>T* alloc(size_t amount)
        {
            if(amount<=0)
            {
                nativehttp::server::err("alloc@memory.h","Could not allocate 0 or less elements");
                utils::debug::print_bt();
                exit(1);
            }
            T* tmptr = new (std::nothrow) T[amount];
            if(!tmptr)
            {
                nativehttp::server::err("alloc@memory.h","Allocation failed(out of memory?)");
                nativehttp::server::err("alloc@memory.h","Requested allocation: "+nd::ss::str_from_size(amount*sizeof(T)));
                utils::debug::print_bt();
                exit(1);
            }
            return tmptr;
        }
	}
}

#endif // MEMORY_H_INCLUDED
