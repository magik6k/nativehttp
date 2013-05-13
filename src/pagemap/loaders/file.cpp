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

#include "../pagemap.h"
#include <string.h>

void page_mapper::load_file(page &tmp, const char *f, string dir)
{
	tmp.type = page_file;
	tmp.data = new char[strlen(f) + 1];
	memcpy(tmp.data, f, strlen(f));
	((char*)tmp.data)[strlen(f)] = '\0';
	base->push_back(tmp);
	nativehttp::data::superstring pgac(f);

	string furi = '/' + pgac.from(dir);
	char *tfu = new char[furi.size() + 1];
	memcpy(tfu, furi.c_str(), furi.size());
	tfu[furi.size()] = '\0';

	urimp tmu = {tfu, int(base->size()) - 1};
	uris.push_back(tmu);
}
