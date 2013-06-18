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


string nativehttp::data::superstring::file(string fn)
{
	FILE *f = fopen(fn.c_str(), "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		rewind(f);
		string tm;
		tm.resize(size);
		fread(&tm[0], 1, size, f);
		fclose(f);
		return tm;
	}
	return "";
}

void nativehttp::data::superstring::set_file(string fn)
{
	str = this->file(fn);
	pos = 0;
}

void nativehttp::data::superstring::append_file(string fn)
{
	str += this->file(fn);
}

int nativehttp::data::superstring::file(string fn, string content)
{
    FILE *f = fopen(fn.c_str(), "w");
	if (f)
	{
		fwrite(content.c_str(), 1, content.size(), f);
		fclose(f);
		return NH_SUCCES;
	}
	return NH_ERROR;
}

int nativehttp::data::superstring::str_to_file(string fn, string content)
{
	return nativehttp::data::superstring::file(fn,content);
}

int nativehttp::data::superstring::sst_to_file(string fn, nativehttp::data::superstring content)
{
	return nativehttp::data::superstring::file(fn,content.str);
}

int nativehttp::data::superstring::append_str_file(string fn, string append)
{
	FILE *f = fopen(fn.c_str(), "a");
	if (f)
	{
		fwrite(append.c_str(), 1, append.size(), f);
		fclose(f);
		return NH_SUCCES;
	}
	return NH_ERROR;
}

int nativehttp::data::superstring::append_sst_file(string fn, nativehttp::data::superstring append)
{
	return nativehttp::data::superstring::append_str_file(fn,append.str);
}
