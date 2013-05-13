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
#ifndef EXECUTOR_H_INCLUDED
#define EXECUTOR_H_INCLUDED
#include <SDL/SDL.h>
#include "nativehttp.h"
#include "data.h"

namespace http
{
	void *executor(void *eid);

	namespace rproc
	{

		struct lrqd
		{
			unsigned int clen;
			char d501;
		};

		void post(nativehttp::rdata &rd, http::request *process, http::rproc::lrqd &ld);
		bool ex(nativehttp::data::pagedata &pd, nativehttp::rdata *rd);
		void line0(http::request *process, nativehttp::rdata &rd, http::rproc::lrqd &ld);
		void header(http::request *process, nativehttp::rdata &rd, http::rproc::lrqd &ld);
	}
}

#endif // EXECUTOR_H_INCLUDED
