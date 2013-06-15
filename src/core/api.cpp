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
#include "http/data.h"
#include <string.h>

ofstream logfile;

namespace nativehttp
{
	data::pagedata::pagedata(string s)
	{
		data = new char[s.size()];
		size = s.size();
		memcpy(data, s.c_str(), size);
	}
	void data::pagedata::operator()(string s)
	{
		if (data)delete[]data;
		data = new char[s.size()];
		size = s.size();
		memcpy(data, s.c_str(), size);
	}
	data::pagedata::pagedata()
	{
		data = NULL;
		size = 0;
	}

	extern "C" void init::attach_uri(string uri, bool top)
	{
		pmap.adduri(uri, top);
	}

	extern "C" string server::version()
	{
		return "NativeHTTP Alpha " + data::superstring::str_from_int(NATIVEHTTP_API_VERSION);
	}

	extern "C" void server::log(string lname, string value)
	{
		if (!deamonized)
		{
            if(http::logger_colors)
                cout << "[\x1b[36m" << lname.c_str() << "\x1b[0m]" << value.c_str() << endl;
                else
                cout << "[" << lname.c_str() << "]" << value.c_str() << endl;
		}
		if (logfile.is_open())logfile << "[" << lname.c_str() << "]" << value.c_str() << endl;
	}

	extern "C" void server::logid(int id, string lname, string value)
	{
		if (!deamonized)
		{
            if(http::logger_colors)
                cout << "[\x1b[36m" << lname.c_str() << "\x1b[0m](" << id << ")" << value.c_str() << endl;
                else
                cout << "[" << lname.c_str() << "](" << id << ")" << value.c_str() << endl;
		}
		if (logfile.is_open())logfile << "[" << lname.c_str() << "](" << id << ")" << value.c_str() << endl;
	}

    extern "C" data::clientid maxClients()
    {
        return http::maxConnections;
    }

    extern "C" bool server::is_ssl()
    {
        return http::onssl;
    }

}
