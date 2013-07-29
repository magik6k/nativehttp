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
#include "version.h"
#include "http/data.h"
#include "nativehttp.h"

namespace utils
{
    bool header_version_ok()
    {
        nativehttp::data::superstring nhf(http::nhpc_include_dir);
        nhf.go_end();
        if(!(nhf.check("/")||nhf.check("\\")))nhf.str+="/";
        nhf.str+=http::nhpc_nativehttph_dest;
        string tnfile = nhf.str;
        nhf.set_file(nhf.str);
        if(nhf.size()<=0)
        {
            nativehttp::server::err("version.cpp@utils","nativehttp.h DOES NOT EXIST");
        }
        nhf.to("#define NATIVEHTTP_API_VERSION ");
        if(nativehttp::data::superstring::int_from_str(nhf.lock().tochar("\r\n"))==NATIVEHTTP_API_VERSION)
            return true;
        else
        {
            nativehttp::server::err("version.cpp@utils","[ERROR]Version of nativehttp.h INVALID("+
                nhf.tochar("\r\n")+"!="+
                nativehttp::data::superstring::str_from_int(NATIVEHTTP_API_VERSION)+")");
            nativehttp::server::log("version.cpp@utils","[NOTE]The header file is set to: "+tnfile);
        }
        return false;
    }
}
