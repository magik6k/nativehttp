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
#ifndef PAGEMAP_H_INCLUDED
#define PAGEMAP_H_INCLUDED
#include "nativehttp.h"
#include "data/vector.h"
#include "data/queue.h"
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dlfcn.h>

#define is_dotso(_pn,_sl) (_pn[_sl-3]=='.'&&_pn[_sl-2]=='s'&&_pn[_sl-1]=='o')
#define is_dotnhp(_pn,_sl) (_pn[_sl-4]=='.'&&_pn[_sl-3]=='n'&&_pn[_sl-2]=='h'&&_pn[_sl-1]=='p')

#define page_none -1
#define page_native 0
#define page_file 1

namespace nativehttp
{
namespace data
{
typedef int (*Tonload)();
typedef pagedata(*Tpage)(rdata*);
}
}

struct nativepage
{
    void* handle;

    nativehttp::data::Tonload onload;
    nativehttp::data::Tpage page;
};

struct page
{
    int type;
    void *data;
    char* file;
    time_t timestamp;
};

struct urimp
{
    char* u;
    int sid;
};

class page_mapper
{
private:
    vector<char*>files;
    deque<urimp>uris;
    int acp;
    unsigned int adui;

    void mapdir(string d);
    page bscpageset(char* f);

    void load_so(page& tmp, const char* f, string dir);

public:
    void preinit();
    void adduri(string u,bool top);
    data::vector<page>* base;
    void page_mapper_init(string d);
    void refresh(string d);
    page by_uri(const char* u);

};

extern nativehttp::data::Ccfg *cfg;

#endif
