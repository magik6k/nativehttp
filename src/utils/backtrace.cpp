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
#include <execinfo.h>
#include <unistd.h>

#include "backtrace.h"
#include "nativehttp.h"
#include "http/data.h"

namespace utils
{
    namespace debug
    {
        void print_bt()
        {
            if(http::backtrace_len > 0)
            {
                void **addrbuf = new void*[http::backtrace_len];
                if(!addrbuf)
                {
                    nativehttp::server::err("Backtrace","Could not allocate address buffor(Out of memory?)");
                }

                char **strcont;

                nativehttp::server::err("Backtrace","-----BACKTRACE BEGIN-----");

                int n = backtrace(addrbuf, http::backtrace_len);
                nativehttp::server::err("Backtrace","Lenght: "+nativehttp::data::superstring::str_from_int(n));

                strcont = backtrace_symbols(addrbuf, n);

                if(!strcont)
                {
                    nativehttp::server::err("Backtrace","-----Error-----");
                }
                else
                {
                    for (int i = 0; i < n; i++)
                        nativehttp::server::err("Backtrace",nativehttp::data::superstring::str_from_int(i)+": "+string(strcont[i]));

                    free(strcont);
                    delete[] addrbuf;
                }

                nativehttp::server::err("Backtrace","-----BACKTRACE END-----");
            }
        }
    }
}
