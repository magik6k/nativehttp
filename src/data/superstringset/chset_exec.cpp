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
#include <stdio.h>
#include "data/queue.h"

#define SSLOCK if(lck)pos=lpos

namespace nativehttp
{
    namespace data
    {
        superstringset& superstringset::changeset_exec()
        {
            vector<int>detect(chset_rules.size(),0);

            superstring out;

            for(;pos<str.size();pos++)
            {
                out.str += str[pos];
                for(unsigned int i=0;i<detect.size();i++)
                {
                    if(str[pos] == chset_rules[i]._from[detect[i]])
                    {
                        detect[i]++;
                        if(detect[i] == chset_rules[i]._from.size())
                        {
                            out.remove(out.size()-chset_rules[i]._from.size(),out.size());
                            for(unsigned int f=0;f<detect.size();f++)
                            {
                                detect[i] = 0;
                            }
                            out.str += chset_rules[i]._to;
                            break;
                        }
                    }
                }
            }

            str = out.str;

            SSLOCK;
            return *this;
        }
    }
}
