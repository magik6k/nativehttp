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
#ifndef FILESENDER_H_INCLUDED
#define FILESENDER_H_INCLUDED
#include <sys/types.h>
#include <aio.h>

#define FSS_Init 1
#define FSS_Sending 2
#define FSS_Reading 3
#define FSS_Done 4

namespace http
{
	void *fsender(void *unused);

	struct fsrq
	{
        int uid;
        const char* file;

        int64_t rngs;
        int64_t rnge;
	};

	struct fsrq_run
	{
        int uid;
        const char* file;

        int64_t rngs;
        int64_t rnge;

        int state;

        int fd;
        aiocb aio;
        size_t apos;

        int64_t fsize;
        void* buf;

	};

}

#endif // FILESENDER_H_INCLUDED
