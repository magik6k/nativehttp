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

string pass = "root";
nativehttp::data::pagedata ajax(nativehttp::rdata* request);


extern "C"
{
	int onload()
	{
		nativehttp::data::Ccfg cfg(config);
		pass = cfg.get_var("pass");

		html_on = cfg.get_var("on");
		html_off = cfg.get_var("off");
		html_top = cfg.get_var("lay");
		logform = cfg.get_var("login_form");
		logformbad = cfg.get_var("badlogin_form");
		page_style = cfg.get_var("style");
		diagram_controls = cfg.get_var("diagram_controls");
		script = cfg.get_var("script");

        nh::data::superstring scrss(script);
        scrss.lock();
        scrss.change("[[hl]]",nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_hourly)));
		scrss.change("[[dl]]",nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_daily)));
		scrss.change("[[wl]]",nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_weekly)));
        script = scrss.str;

		nh::data::superstring htmss(html_top);//layout setup
		htmss.lock();
		htmss.change("[[srvVer]]", nh::server::version());
		htmss.change("[[style]]", page_style);
		page_style.clear();
		html_top = htmss.str;

		nh::data::superstring dcont(diagram_controls);//diagram controls setup
		dcont.lock().change("[[hl]]",nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_hourly)));
		dcont.change("[[dl]]",nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_daily)));
		dcont.change("[[wl]]",nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_weekly)));
        if(ns::stat::get(ns::stat::unit::len_hourly)>0)
        {
            dcont.remove("[[dhl]");
            dcont.remove("[dhl]]");
        }
        else dcont.remove("[[dhl]","[dhl]]");
        if(ns::stat::get(ns::stat::unit::len_daily)>0)
        {
            dcont.remove("[[ddl]");
            dcont.remove("[ddl]]");
        }
        else dcont.remove("[[ddl]","[ddl]]");
        if(ns::stat::get(ns::stat::unit::len_weekly)>0)
        {
            dcont.remove("[[dwl]");
            dcont.remove("[dwl]]");
        }
        else dcont.remove("[[dwl]","[dwl]]");
        diagram_controls = dcont.str;

		initated();
	}

	nativehttp::data::pagedata page(nativehttp::rdata* request)
	{
        if(request->get)
        {
            if(!request->get->get("ajax").empty())
            {
                return ajax(request);
            }
        }
		nh::data::superstring page(html_top);
		page.lock();

		if (!request->session)
		{
			page.change("[[content]]", "NativeStat needs Session module enabled!<br/>");
			page.change("[[script]]", "");
			return nativehttp::data::pagedata(page.str);
		}

		if (request->session->get("@__NtStS#") != "l")
		{
			if (!request->post)
			{
				page.change("[[content]]", logform);
				page.change("[[script]]", "");
				return nativehttp::data::pagedata(page.str);
			}
			else
			{
				if (request->post->get("scky") == pass)
				{
					request->session->set("@__NtStS#", "l");
				}
				else
				{
					page.change("[[content]]", logformbad);
					page.change("[[script]]", "");
					return nativehttp::data::pagedata(page.str);
				}
			}
		}

		if(!ns::stat::enabled())
		{
            page.change("[[content]]", "Stats disabled on this server");
            page.change("[[script]]", "");
            return nativehttp::data::pagedata(page.str);
		}

        page.change("[[script]]", script);

        string pg = "<h2>NativeStat</h2><hr/><b>Logging status:</b><br/>Hourly logging: ";
        if(ns::stat::get(ns::stat::unit::len_hourly)!=0)
        {
            pg += "<span style='color: #0a0'>"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_hourly))+" hours</span>";
        }
        else
        {
            pg += html_off;
        }

        pg += "<br/>Daily logging: ";

        if(ns::stat::get(ns::stat::unit::len_daily)!=0)
        {
            pg += "<span style='color: #0a0'>"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_daily))+" days</span>";
        }
        else
        {
            pg += html_off;
        }

        pg += "<br/>Weekly logging: ";

        if(ns::stat::get(ns::stat::unit::len_weekly)!=0)
        {
            pg += "<span style='color: #0a0'>"+nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::len_weekly))+" weeks</span>";
        }
        else
        {
            pg += html_off;
        }

        pg += "<hr/><h3>All-Time Statistics</h3><b>Network usage:</b><br/>Upload &#8593; - ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::upload));
        pg += " (HTTP headers: ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::http_head_size_sent));

        pg += ")<br/>Download &#8595; - ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::download));
        pg += " (HTTP headers: ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::http_head_size_received));

        pg += ")<br/>Total - ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::download)+ns::stat::get(ns::stat::unit::upload));
        pg += "(HTTP headers: ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::http_head_size_received)+ns::stat::get(ns::stat::unit::http_head_size_sent));

        pg += ")<br/>POST data size - ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::http_postdata_size));


        pg += "<br/></br><b>Acitvity:</b><br/>TCP Connections - ";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::connections));
        pg += "<br/>HTTP Requests -";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::requests));
        pg += "<br/>Sessions created(Visits) - ";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::sessions_created));
        if(ns::stat::get(ns::stat::unit::len_hourly)!=0)pg += "<br/>This hour max online - ";
        if(ns::stat::get(ns::stat::unit::len_hourly)!=0)pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::sessions_max_existing_hourly,0));


        pg += "<br/><br/><b>WebSockets:</b><br/>Upload &#8593; - ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::ws_upload));
        pg += "<br/>Download &#8595; - ";
        pg += nd::ss::str_from_size(ns::stat::get(ns::stat::unit::ws_download));
        pg += "<br/>Handshakes - ";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_handshakes));
        pg += "<br/>Messages/Frames sent - ";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_sent));
        pg += "/";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_frames_sent));
        pg += "<br/>Messages/Frames received - ";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_messages_received));
        pg += "/";
        pg += nd::ss::str_from_int64(ns::stat::get(ns::stat::unit::ws_frames_received));

        pg += "<br/><hr/><h4>Diagram settings</h4>";
        pg += diagram_controls;
        pg += "<br/><hr/><b>Diagram:</b> <span id='disp'>None</span><br/><div id='display'></div>";

		page.change("[[content]]", pg);

		return nativehttp::data::pagedata(page.str);
	}
}

