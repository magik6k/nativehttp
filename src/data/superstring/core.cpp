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

#define SSLOCK if(lck)pos=lpos


nativehttp::data::superstring::~superstring()
{
	str.clear();
}

nativehttp::data::superstring::superstring()
{
	pos = 0;
	lck = false;
	lpos = 0;
}

void nativehttp::data::superstring::lock()
{
	lck = true;
	lpos = pos;
}

void nativehttp::data::superstring::lock(unsigned int lp)
{
	lck = true;
	lpos = lp;
}

void nativehttp::data::superstring::unlock()
{
	lck = false;
	lpos = 0;
}

const char* nativehttp::data::superstring::c_str()
{
    return str.c_str();
}

size_t nativehttp::data::superstring::size()
{
    return str.size();
}

size_t nativehttp::data::superstring::length()
{
    return str.length();
}

void nativehttp::data::superstring::clear()
{
    str.clear();
    tokens.clear();
}
