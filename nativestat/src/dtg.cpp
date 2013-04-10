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
#include "assets.h"
#include "dtg.h"

string mkdtd(unsigned long v1, unsigned long v2, string s1, string s2, string col1, string col2)
{
    if(v1+v2==0)v1++;
    nh::data::superstring rt(ddbase);
    rt.lock();
    rt.change("[[c1]]",s1);
    rt.change("[[c2]]",s2);
    rt.change("[[csslftu]]","text-align:left; float: left; width: 50%;");
    rt.change("[[cssrgtu]]","text-align:right; margin-left: 50%; width: 50%;");
    rt.change("[[csslft]]","height: 16px; text-align:left; float: left; width: "+rt.from_int((v1*100)/(v1+v2))+"%; background-color: #"+col1+";");
    rt.change("[[cssrgt]]","height: 16px; text-align:right; margin-left:"+rt.from_int((v1*100)/(v1+v2))+"%; width: "+rt.from_int((v2*100)/(v1+v2))+"%; background-color: #"+col2+";");
    return rt.str;
}
