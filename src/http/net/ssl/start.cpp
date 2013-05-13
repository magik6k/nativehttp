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
#include "../net.h"
#include "nativehttp.h"
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef NHDBG
#include "protocol.h"
#include <iostream>
#endif
namespace http
{
	namespace ssl
	{
		SSL_METHOD *meth;
		SSL_CTX *ctx;

		void init()
		{
#ifdef NHDBG
			size_t bm = getrsmem();
#endif
			SSL_library_init();
			SSL_load_error_strings();
			meth = (SSL_METHOD*) SSLv3_method();
			ctx = SSL_CTX_new(meth);
			if (!ctx)
			{
				nativehttp::server::log("ssl.cpp", "CTX init failed");
				ERR_print_errors_fp(stderr);
				exit(1);
			}
			if (SSL_CTX_use_certificate_file(ctx, cfg->get_var("ssl_crt_file").c_str(), SSL_FILETYPE_PEM) <= 0)
			{
				nativehttp::server::log("ssl.cpp", "certificate loading failed");
				ERR_print_errors_fp(stderr);
				exit(1);
			}
			if (SSL_CTX_use_PrivateKey_file(ctx, cfg->get_var("ssl_key_file").c_str(), SSL_FILETYPE_PEM) <= 0)
			{
				nativehttp::server::log("ssl.cpp", "key loading failed");
				ERR_print_errors_fp(stderr);
				exit(1);
			}
			if (!SSL_CTX_check_private_key(ctx))
			{
				nativehttp::server::log("ssl.cpp", "key does not match certificate");
				exit(1);
			}
#ifdef NHDBG
			cout << "[DBG:ssl.cpp@http]SSL data mem: " << (getrsmem() - bm) / 1024.f << "kb\n";
#endif
		}

	}
}
