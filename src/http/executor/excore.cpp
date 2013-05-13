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
#include "protocol.h"
#include "../executor.h"
#include "../data.h"
#include "../stat.h"
#include "../net/net.h"

#ifdef NHDBG
#include "utils/memory.h"
size_t lastexecmem = 0;
#endif

namespace http
{
	namespace rproc
	{

		bool ex(nativehttp::data::pagedata &pd, nativehttp::rdata *rd)
		{
			page pid = pmap.by_uri(rd->uri.c_str());

			switch (pid.type)
			{
				case page_native:
					{
						rd->ctype = "text/html;charset=" + charset;
						rd->response = "200 OK";

						nativepage *npp = (nativepage*)pid.data;
#ifdef NHDBG
						size_t bm = getacmem() + getrsmem();
						unsigned int pgt = SDL_GetTicks();
#endif
						SDL_mutexP(http::mtx_exec2);
						nativehttp::data::pagedata ts = npp->page(rd);   //<<<execution of page
						SDL_mutexV(http::mtx_exec2);
#ifdef NHDBG
						unsigned int et = SDL_GetTicks() - pgt;
						if (!http::extmemstats)
						{
							cout << "[DBG:executor.cpp@http]Page execution allcocated: "
							     << (getacmem() + getrsmem() - bm) / 1024LL << "kb, time: " << et << "ms\n";
						}
						else
						{
							cout << "-------\n";
							cout << "Native Page execution stats:\n";
							cout << "File: " << pid.file << endl;
							cout << "Time of execution: " << et << "ms\n";

							cout << "Total Memory: ";
							utils::memory::printmemsize(getacmem() + getrsmem());
							cout << endl;

							cout << "Memory allocated by page execution: ";
							utils::memory::printmemsize(getacmem() + getrsmem() - bm);
							cout << endl;

							cout << "Memory change since init: ";
							utils::memory::printmemsize
							(int64_t(getacmem() + getrsmem()) - int64_t(http::init_memory));
							cout << endl;

							cout << "Memory change since LPE: ";
							utils::memory::printmemsize
							(int64_t(getacmem() + getrsmem()) - int64_t(lastexecmem));
							cout << endl;

							cout << "-------\n";
						}

						lastexecmem = getacmem() + getrsmem();
#endif
						string snd = "HTTP/1.1 " + rd->response + "\r\n" + http::headers::standard;
						snd += http::headers::alive + http::headers::alivetimeout;
						snd += "Content-Type: " + rd->ctype + "\r\nContent-Length: ";
						snd += its(ts.size);
						snd += "\r\n";
						snd += rd->cookie->gethead();
						snd += "\r\n";
						string snd2 = "\r\n";

						pd.size = snd.size() + ts.size + snd2.size();
						pd.data = new char[pd.size];
						memcpy(pd.data, snd.c_str(), snd.size());
						memcpy(pd.data + snd.size(), ts.data, ts.size);
						memcpy(pd.data + snd.size() + ts.size, snd2.c_str(), snd2.size());
						delete[]ts.data;
						return false;
					}
					break;
				case page_file:
					{

						FILE *f = fopen((const char*)pid.data, "r");
						if (f)
						{
							fseek(f, 0, SEEK_END);
							int size = ftell(f);
							fseek(f, 0, SEEK_SET);
							rewind(f);
							string snd("HTTP/1.1 200 OK\r\n");
							snd += http::headers::standard;
							snd += http::headers::alive + http::headers::alivetimeout;
							snd += mime->get_ctype((char*)pid.data);
							snd += "\r\nContent-Length: ";
							snd += its(size);
							snd += "\r\n\r\n";
							char *b = new char[size];
							fread(b, 1, size, f);
							string snd2 = "\r\n";
							pd.size = size + snd.size() + snd2.size();
							pd.data = new char[pd.size];
							memcpy(pd.data, snd.c_str(), snd.size());
							memcpy(pd.data + snd.size(), b, size);
							memcpy(pd.data + snd.size() + size, snd2.c_str(), snd2.size());
							delete[] b;
							fclose(f);
							return false;
						}
						else
						{
							return true;
						}
					}
					break;
				default:
					return true;
			}
			return true;

		}

	}//namespace rproc
}//http namespace
