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
#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED
#include <stdlib.h>
#include "nativehttp.h"
using namespace std;

namespace data
{
namespace session
{
struct session
{
    unsigned int svid;
    time_t started;
};

class sbmain
{
    private:
        nativehttp::data::cfgfil** keys;
        size_t fileds;
        size_t getkeyid(string kn);
        size_t createkey(string kn);
        size_t getfreekeyid();
        void alloc_keys();
    public:
        sbmain();
        void setkey(string kn, string kv);
        string& getkey(string kn);
};

extern time_t sess_life;
extern session* storage;
}
}

#endif
