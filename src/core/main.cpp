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
#include <iostream>
#include "protocol.h"
#include "http/init.h"
#include "http/manager.h"
#include "data/queue.h"
#include "http/net/net.h"
#include "ws/ws.h"
#include <sys/prctl.h>
#include <signal.h>

page_mapper pmap;
mimec *mime = NULL;
nativehttp::data::Ccfg *cfg;
string charset;

using namespace std;

int main(int argc, char *argv[])
{

#ifdef NHDBG
	size_t bim = (getacmem());
	cout << "[DBG:main.cpp@core]before-init mem: " << bim << "b\n";
#endif

	cout << "NativeHTTP DEV\nBy magik6000\nfor more info visit nativehttp.org\n";
	cout << "pre-init\n";

	prctl(PR_SET_NAME, "nativehttp", 0, 0, 0);

	bool dmnz = false;
	bool gr = true;
	string conf = "/etc/nativehttp/config.cfg";

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0])
		{
			switch (argv[i][1])
			{
				case 'd':
					dmnz = true;
					break;
				case 'c':
					if (i + 1 < argc)
					{
						if (argv[i + 1][0] != '-')
						{
							conf = argv[i + 1];
							cout << "Using non-standard configuration file: " << conf << endl;
							i++;
						}
						else
						{
							cout << "ERROR: " << "Wrong config file name: " << argv[i + 1] << "\n";
							exit(1);
						}
					}
					else
					{
						cout << "ERROR: " << "No config file specified\n";
						exit(1);
					}
					break;
				case 'k':
					gr = false;
					break;
				default:
					cout << "ERROR: " << "Unknown option: " << argv[i] << endl;;
					exit(1);
			}
		}
		else
		{
			cout << "ERROR: " << "Invalid option: " << argv[i] << endl;
			exit(1);
		}
	}

	if (gr)goroot();

	signal(SIGSEGV, http::manager::sig);
	signal(SIGILL, http::manager::sig);
	signal(SIGFPE, http::manager::sig);
	signal(SIGABRT, http::manager::sig);

    nativehttp::server::log("INIT", "Loading configuration, setting up server");
	cfg = new nativehttp::data::Ccfg(conf);

	http::datainit();
	ws::init();

	charset = cfg->get_var("charset");
	default_mime = cfg->get_var("default_content_type");
	if (cfg->get_int("instantso"))
	{
		setbuf(stdout, NULL);
	}
	mime = new mimec;
	nativehttp::server::log("INIT", "Mapping server directory, loading native pages");
	pmap.preinit();
	pmap.page_mapper_init(cfg->get_var("files_location"));
	if (dmnz)deamonize();
	nativehttp::server::log("INIT", "Starting HTTP system");

	http::sdlinit();
	http::executorinit();
	http::netstart();
	http::startsystem();

	nativehttp::server::log("INIT", "Ready");

	if (cfg->get_int("use_ssl"))
	{
		prctl(PR_SET_NAME, "nh-recv-ssl", 0, 0, 0);
		http::ssl::reciver();
	}
	else
	{
		prctl(PR_SET_NAME, "nh-recv-bsd", 0, 0, 0);
		http::bsd::reciver();
	}

	return 0;
}


string its(int n)
{
	string tmp, ret;
	if (n < 0)
	{
		ret = "-";
		n = -n;
	}
	do
	{
		tmp += n % 10 + 48;
		n -= n % 10;
	}
	while (n /= 10);
	for (int i = tmp.size() - 1; i >= 0; i--)
		ret += tmp[i];
	return ret;
}
