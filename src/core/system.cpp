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
#include "protocol.h"
#include "http/data.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <SDL/SDL_net.h>
#include <netinet/tcp.h>
#define SOCKET	int
extern "C"
{
	extern DECLSPEC void SDLCALL SDLNet_SetLastError(int err);
	extern DECLSPEC int SDLCALL SDLNet_GetLastError(void);
}

bool deamonized = false;

void deamonize()
{
	nativehttp::server::log("system.cpp", "demonizing");
	pid_t pid, sid;
	pid = fork();
	if(pid < 0)
	{
		exit(EXIT_FAILURE);
	}
	if(pid > 0)
	{
		exit(EXIT_SUCCESS);
	}
	umask(0);
	sid = setsid();
	if(sid < 0)
	{
		exit(EXIT_FAILURE);
	}
	deamonized = true;
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void goroot()
{
	if((chdir("/")) < 0)
	{
		exit(EXIT_FAILURE);
	}
}

size_t getacmem()
{
	long long r = 0LL;
	FILE *fp = NULL;
	if(!(fp = fopen("/proc/self/statm", "r")))
		return size_t(0LL);
	if(fscanf(fp, "%*s%*s%*s%*s%*s%lld", &r) != 1)
	{
		fclose(fp);
		return size_t(0LL);
	}
	fclose(fp);
	return size_t(r * sysconf(_SC_PAGESIZE));
}

size_t getrsmem()
{
	long long r = 0LL;
	FILE *fp = NULL;
	if(!(fp = fopen("/proc/self/statm", "r")))
		return size_t(0LL);
	if(fscanf(fp, "%*s%lld", &r) != 1)
	{
		fclose(fp);
		return size_t(0LL);
	}
	fclose(fp);
	return size_t(r * sysconf(_SC_PAGESIZE));
}


