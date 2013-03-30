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
#include "pagemap/pagemap.h"

using namespace std;

void deamonize();

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
