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

cookiedata::cookiedata(string strcookie)
{
    superstring cke(strcookie);
    while(cke.pos<cke.str.size())
    {
        cfil ctm;
        ctm.name=cke.to("=");
        ctm.value=cke.to("; ");
        data.push_back(ctm);
    }
}

string cookiedata::get(string name)
{
    for(int i=0; i<data.size(); i++)
    {
        if(data[i].name==name)
        {
            return data[i].value;
        }
    }
    return "";
}

void cookiedata::set(string name, string value)
{
    sets+="Set-Cookie: "+name+"="+value+"\r\n";
}

void cookiedata::set(string name, string value, string attributes)
{
    sets+="Set-Cookie: "+name+"="+value+"; "+attributes+"\r\n";
}

string cookiedata::gethead()
{
    return sets;
}
