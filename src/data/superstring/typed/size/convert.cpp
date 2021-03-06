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


string nativehttp::data::superstring::str_from_size(int64_t in)
{
    bool m = false;
    if(in<0){in = -in;m = true;}
	float tcn = 0;
	string end;
	if (in >= 1099511627776LL)
	{
		tcn = double(in) / 1024.f / 1024.f / 1024.f / 1024.f;
		end = " TiB";
	}
	else if (in >= 1073741824LL)
	{
		tcn = double(in) / 1024.f / 1024.f / 1024.f;
		end = " GiB";
	}
	else if (in >= 1048576LL)
	{
		tcn = double(in) / 1024.f / 1024.f;
		end = " MiB";
	}
	else if (in >= 1024LL)
	{
		tcn = double(in) / 1024.f;
		end = " KiB";
	}
	else
	{
		tcn = in;
		end = " B";
	}

	string ret = nativehttp::data::superstring::str_from_int64(tcn);
	if (int(10.f * (tcn - float(int(tcn)))))
	{
		ret += ".";
		ret += nativehttp::data::superstring::str_from_int64(int(10.f * (tcn - float(int(tcn)))));
	}
	ret += end;

	return m?("-"+ret):ret;
}

nativehttp::data::superstring nativehttp::data::superstring::sst_from_size(int64_t in)
{
	return nativehttp::data::superstring(nativehttp::data::superstring::str_from_size(in));
}
