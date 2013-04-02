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
#include "assets.h"
#include "dtg.h"

extern "C"
{
    int onload()
    {
        nh::data::superstring htmss(html_top);
        htmss.lock();
        htmss.change("[[srvVer]]",nh::server::version());
        htmss.change("[[style]]",page_style);
        page_style.clear();
        html_top=htmss.str;
        return 1;
    }
    nativehttp::data::pagedata page(nativehttp::rdata* request)
    {
        nh::data::superstring page(html_top);
        nh::data::superstring content(basic_content);

        page.lock();
        content.lock();

        content.change("[[state]]",(nh::server::stat::stats()?html_on:html_off));

        if(nh::server::stat::stats())
        {
            content.remove("[[toggle]");
            content.remove("[toggle]]");
            content.change("[[transferlogging]]",(nh::server::stat::transfer_stats()?html_on:html_off));
            content.change("[[hitlogging]]",(nh::server::stat::hit_stats()?html_on:html_off));
            content.change("[[methodlogging]]",(nh::server::stat::method_stats()?html_on:html_off));

            if(nh::server::stat::transfer_stats())
            {
                content.change("[[transfer_usage]]","<br/><b>Transfer used:</b> "+
                               mkdtd(nh::server::stat::uploaded(),nh::server::stat::downloaded(),
                                     "UL: "+content.from_int(nh::server::stat::uploaded()/1024)+" kb",
                                     "DL: "+content.from_int(nh::server::stat::downloaded()/1024)+" kb",
                                     "11aa11","aa1111")+"<br/>");
            }
            else content.remove("[[transfer_usage]]");

            content.change("[[general_usage]]","<br/><b>Recived Connections: </b> <code>"+content.from_int(nh::server::stat::connections())+"</code>");
            if(nh::server::stat::hit_stats())
            {
                content.change("[[hits]]","<br/><b>Requests: </b> <code>"+content.from_int(nh::server::stat::hits())+"</code><br/><br/>");
            }
            else content.remove("[[hits]]");

            if(nh::server::stat::method_stats())
            {
                content.change("[[methods]]","<br/><b>Request types:</b> "+
                               mkdtd(nh::server::stat::get_requests(),nh::server::stat::post_requests(),
                                     "GET: "+content.from_int(nh::server::stat::get_requests()),
                                     "POST: "+content.from_int(nh::server::stat::post_requests()),
                                     "11aa11","aa1111")+"<br/>");
            }
            else content.remove("[[methods]]");

        }
        else
        {
            content.remove("[[toggle]","[toggle]]");
        }

        page.change("[[content]]",content.str);
        return nativehttp::data::pagedata(page.str);
    }
}

