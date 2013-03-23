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
#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

#include <SDL/SDL_net.h>
#include <string>
#include <vector>
#include <iostream>
#include <deque>
#include <map>
#include "data/vector.h"
#include "superstring.h"
#include "nativehttp.h"

using namespace std;

string parse_nhp(string nhp);
void deamonize();
string its(int n);

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
    vector<string>files;
    deque<urimp>uris;
    int acp;
    unsigned int adui;
public:
    void preinit();
    void adduri(string u,bool top);
    data::vector<page>* base;
    void page_mapper_init(string d);
    void refresh(string d);
    page by_uri(const char* u);

};


nativehttp::data::pagedata exec(string uri,nativehttp::rdata* rd);

class mimec
{
private:
    map<string,string> mimes;
public:
    mimec();
    string get_ctype(string fn);
};



extern page_mapper pmap;
extern string charset;
extern mimec *mime;
extern nativehttp::data::Ccfg *cfg;
extern int postmax;
extern string default_mime;
extern bool deamonized;

#endif // PROTOCOL_H_INCLUDED
