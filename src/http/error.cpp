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
#include "error.h"


namespace http
{
	namespace error
	{
		nativehttp::data::pagedata load_error(string file, string head)
		{
			nativehttp::data::pagedata rt;
			string epc = "HTTP/1.1 " + head + "\r\n" + http::headers::standard + "Content-Length: ";
			FILE *f = fopen(file.c_str(), "r");
			if (f)
			{
				fseek(f, 0, SEEK_END);
				rt.size = ftell(f);
				fseek(f, 0, SEEK_SET);
				epc += its(rt.size);
				epc += "\r\n\r\n";
				unsigned int rs = rt.size;
				rt.size += epc.size() + 3; //3 = "\r\n\0"
				rt.data = new char[rt.size];
				memcpy(rt.data, epc.c_str(), epc.size());
				fread(rt.data + epc.size(), 1, rs, f);
				rt.data[rt.size - 3] = '\r';
				rt.data[rt.size - 2] = '\n';
				rt.data[rt.size - 1] = '\0';
				fclose(f);
			}
			else
			{
				nativehttp::server::log("ERROR", "oppening error file failed.. " + file);
			}
			return rt;
		}
	}
}
