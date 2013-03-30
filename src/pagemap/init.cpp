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

#include "pagemap.h"
#include <string.h>

void page_mapper::page_mapper_init(string d)
{
    mapdir(d);
    for(unsigned int i=0; i<files.size(); i++)
    {
        page tmp=bscpageset(files[i]);
        if(tmp.type==-1)continue;

        if(is_dotso(files[i],strlen(files[i])))
        {
            load_so(tmp,files[i],d,NULL);
        }
        else if(is_dotnhp(files[i],strlen(files[i])))
        {
            load_nhp(tmp,files[i],d);
        }
        else
        {

        }
    }
}

