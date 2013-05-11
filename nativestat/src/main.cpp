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

#define config "./etc/nativehttp/nstat.cfg"

string pass="root";

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
        nativehttp::data::Ccfg cfg(config);
        pass=cfg.get_var("pass");
        initated();
    }
    nativehttp::data::pagedata page(nativehttp::rdata* request)
    {
        nh::data::superstring page(html_top);

        if(!request->session)
        {
            page.change("[[content]]","NativeStat needs Session module enabled!<br/>");
            return nativehttp::data::pagedata(page.str);
        }

        if(request->session->get("@__NtStS#")!="l")
        {
            if(!request->post)
            {
                page.change("[[content]]",logform);
                return nativehttp::data::pagedata(page.str);
            }
            else
            {
                if(request->post->get("scky")==pass)
                {
                    request->session->set("@__NtStS#","l");
                }
                else
                {
                    page.change("[[content]]",logformbad);
                    return nativehttp::data::pagedata(page.str);
                }
            }
        }

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
                if(nh::server::stat::hourly_length())
                {
                    unsigned long mxt=1;
                    for(unsigned long i=0;i<nh::server::stat::hourly_length();i++)
                    {
                        if(mxt<(nh::server::stat::hour_upload(i)+nh::server::stat::hour_download(i)))mxt=(nh::server::stat::hour_upload(i)+nh::server::stat::hour_download(i));
                    }

                    string diag = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" style=\"width: 100%; height: 300px;\"><rect width=\"100%\" height=\"100%\" style=\"fill:rgb(200,200,200);\"/>";
                    long long olt=(nh::server::stat::hour_upload(0)+nh::server::stat::hour_download(0));
                    long long olu=(nh::server::stat::hour_upload(0));
                    long long old=(nh::server::stat::hour_download(0));
                    for(unsigned long i=0;i<4;i++)
                    {
                        diag+="<line x1=\"0\" x2=\"100%\" y1=\""+nh::data::superstring::str_from_int((i+1)*20)+"%\" y2=\""+nh::data::superstring::str_from_int((i+1)*20)+"%\" style=\"stroke:rgb(50,50,50);stroke-width:0.5px\" />";
                        diag+="<text x=\"90%\"  y=\""+nh::data::superstring::str_from_int((i+1)*20)+"%\"> "+nh::data::superstring::str_from_int(mxt/(i+2)/1024)+"kb</text>";
                    }
                    for(unsigned long i=1;i<nh::server::stat::hourly_length();i++)
                    {
                        diag+="<line x1=\""+content.str_from_int(((i-1)*100)/nh::server::stat::hourly_length())+\
                            "%\" y1=\""+content.str_from_int(100-((olt*100)/mxt))+\
                            "%\" x2=\""+content.str_from_int(((i)*100)/nh::server::stat::hourly_length())+\
                            "%\" y2=\""+content.str_from_int(100-(((nh::server::stat::hour_upload(i)+nh::server::stat::hour_download(i))*100)/mxt))+\
                            "%\" style=\"stroke:rgb(220,80,0);stroke-width:1\" />";

                        diag+="<line x1=\""+content.str_from_int(((i-1)*100)/nh::server::stat::hourly_length())+\
                            "%\" y1=\""+content.str_from_int(100-((olu*100)/mxt))+\
                            "%\" x2=\""+content.str_from_int(((i)*100)/nh::server::stat::hourly_length())+\
                            "%\" y2=\""+content.str_from_int(100-(((nh::server::stat::hour_upload(i))*100)/mxt))+\
                            "%\" style=\"stroke:rgb(17,187,17);stroke-width:1\" />";

                        diag+="<line x1=\""+content.str_from_int(((i-1)*100)/nh::server::stat::hourly_length())+\
                            "%\" y1=\""+content.str_from_int(100-((old*100)/mxt))+\
                            "%\" x2=\""+content.str_from_int(((i)*100)/nh::server::stat::hourly_length())+\
                            "%\" y2=\""+content.str_from_int(100-(((nh::server::stat::hour_download(i))*100)/mxt))+\
                            "%\" style=\"stroke:rgb(187,17,17);stroke-width:1\" />";

                        olt=(nh::server::stat::hour_upload(i)+nh::server::stat::hour_download(i));
                        olu=(nh::server::stat::hour_upload(i));
                        old=(nh::server::stat::hour_download(i));
                    }
                    diag+="</svg>";

                    content.change("[[transfer_usage]]","<br/><b>Transfer used:</b> "+
                                   mkdtd(nh::server::stat::uploaded()/1024,nh::server::stat::downloaded()/1024,
                                         "UL: "+content.str_from_int(nh::server::stat::uploaded()/1024.f)+" kb",
                                         "DL: "+content.str_from_int(nh::server::stat::downloaded()/1024.f)+" kb",
                                         "11aa11","aa1111")+"<br/>Hourly max:"+content.str_from_int(mxt/1024)+" kb<br/>"+diag+"<br/>");
                }
                else
                {
                    content.change("[[transfer_usage]]","<br/><b>Transfer used:</b> "+
                                   mkdtd(nh::server::stat::uploaded(),nh::server::stat::downloaded(),
                                         "UL: "+content.str_from_int(nh::server::stat::uploaded()/1024)+" kb",
                                         "DL: "+content.str_from_int(nh::server::stat::downloaded()/1024)+" kb",
                                         "11aa11","aa1111")+"<br/>");
                }
            }
            else content.remove("[[transfer_usage]]");

            content.change("[[general_usage]]","<br/><b>Recived Connections: </b> <code>"+content.str_from_int(nh::server::stat::connections())+"</code>");
            if(nh::server::stat::hit_stats())
            {
                unsigned long mxt=1;
                for(unsigned long i=0;i<nh::server::stat::hourly_length();i++)
                {
                    if(mxt<(nh::server::stat::hour_hits(i)))mxt=(nh::server::stat::hour_hits(i));
                }
                string diag = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" style=\"width: 100%; height: 300px;\"><rect width=\"100%\" height=\"100%\" style=\"fill:rgb(200,200,200);\"/>";

                long long olh=(nh::server::stat::hour_hits(0));
                for(unsigned long i=0;i<4;i++)
                {
                    diag+="<line x1=\"0\" x2=\"100%\" y1=\""+nh::data::superstring::str_from_int((i+1)*20)+"%\" y2=\""+nh::data::superstring::str_from_int((i+1)*20)+"%\" style=\"stroke:rgb(50,50,50);stroke-width:0.5px\" />";
                    diag+="<text x=\"90%\"  y=\""+nh::data::superstring::str_from_int((i+1)*20)+"%\"> "+nh::data::superstring::str_from_int((mxt/5)*(4-i))+"</text>";
                }
                for(unsigned long i=1;i<nh::server::stat::hourly_length();i++)
                {
                    diag+="<line x1=\""+content.str_from_int(((i-1)*100)/nh::server::stat::hourly_length())+\
                        "%\" y1=\""+content.str_from_int(100-((olh*100)/mxt))+\
                        "%\" x2=\""+content.str_from_int(((i)*100)/nh::server::stat::hourly_length())+\
                        "%\" y2=\""+content.str_from_int(100-(((nh::server::stat::hour_hits(i))*100)/mxt))+\
                        "%\" style=\"stroke:rgb(187,17,17);stroke-width:1\" />";
                    olh=(nh::server::stat::hour_hits(i));
                }

                diag+="</svg>";

                content.change("[[hits]]","<br/><b>Total requests: </b> <code>"+content.str_from_int(nh::server::stat::hits())+"</code><br/><b>Hourly peak: </b><code>"+content.str_from_int(mxt)+"</code><br/>"+diag+"<br/>");
            }
            else content.remove("[[hits]]");

            if(nh::server::stat::method_stats())
            {
                content.change("[[methods]]","<br/><b>Request types:</b> "+
                               mkdtd(nh::server::stat::get_requests(),nh::server::stat::post_requests(),
                                     "GET: "+content.str_from_int(nh::server::stat::get_requests()),
                                     "POST: "+content.str_from_int(nh::server::stat::post_requests()),
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

