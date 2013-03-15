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

#include "../nativehttp.h"
#include "../protocol.h"
#include "executor.h"
#include "data.h"
#include "in.h"

namespace http
{


int executor(void* eid)
{
    while(true)
    {
        if(http::toexec.size()<=0)
        {
            SDL_Delay(1);
            continue;
        }
        if(http::toexec.front().taken)
        {
            SDL_Delay(1);
            continue;
        }
        http::toexec.front().taken=true;
        http::request process=http::toexec.front();
        http::toexec.pop();
        log("executor.cpp","execution start");

        rdata rd;
        rd.get=NULL;
        rd.post=NULL;
        rd.cookie=NULL;

        http::rproc::line0(process,rd);

        logid(process.method,"executor.cpp",rd.uri);

        if(process.method==0)
        {
            //400 Bad Request
            http::unlockclient(process.uid);
            continue;
        }
        if(process.method==3)
        {
            //501 Not Implemented
            http::unlockclient(process.uid);
            continue;
        }
        if(!process.http11)
        {
            //505 HTTP Version Not Supported
            http::unlockclient(process.uid);
            continue;
        }

        ///handle here rest of header!!

        log("executor.cpp","getting page");

        pagedata result;
        http::rproc::ex(result,&rd);



        if(result.data)
        {
            cout << result.data<<endl;
            delete[] result.data;
        }
        if(rd.cookie)
        {
            delete rd.cookie;
        }
        if(rd.get)
        {
            delete rd.get;
        }

        http::unlockclient(process.uid);
        log("executor.cpp","done");
    }
    return 1;
}

namespace rproc
{

void ex(pagedata& pd,rdata* rd)
{
    page pid = pmap.by_uri(rd->uri);

    switch(pid.type)
    {
    case page_native:
    {
        log("executor.cpp","native page");

        rd->ctype="text/html;charset="+charset;
        nativepage *npp = (nativepage*)pid.data;
        pagedata ts=npp->page(rd);

        string snd = "HTTP/1.1 200 OK\r\nContent-Type: "+rd->ctype+"\r\nContent-Length: ";
        snd+=its(ts.size);
        //snd+="\r\n";
        //snd+=rd->cookie->gethead();
        snd+="\r\n";
        string snd2="\r\n";

        pd.size=snd.size()+ts.size+snd2.size();
        pd.data=new char[pd.size];
        memcpy(pd.data,snd.c_str(),snd.size());
        memcpy(pd.data+snd.size(),ts.data,ts.size);
        memcpy(pd.data+snd.size()+ts.size,snd2.c_str(),snd2.size());
        delete[]ts.data;
    }
    break;
    case page_file:
    {
        string ct=mime->get_ctype((char*)pid.data);

        FILE* f=fopen((const char*)pid.data,"r");
        if(f)
        {
            fseek(f,0,SEEK_END);
            int size = ftell(f);
            rewind(f);
            string snd("HTTP/1.1 200 OK\r\n");
            snd += ct;
            snd += "\r\nContent-Length: ";
            snd += its(size);
            snd += "\r\n\r\n";
            char* b=new char[size];
            fread(b,1,size,f);
            string snd2="\r\n";
            pd.size=size+snd.size()+snd2.size();
            pd.data=new char[pd.size];
            memcpy(pd.data,snd.c_str(),snd.size());
            memcpy(pd.data+snd.size(),b,size);
            memcpy(pd.data+snd.size()+size,snd2.c_str(),snd2.size());
            delete[] b;
            fclose(f);
        }
        else
        {
            ///404 HERE  <<<<<<<<<
        }
    }
    break;
    default:
    log("executor.cpp","not found");
    ///404 here<<<<
    }

}

void line0(http::request& process,rdata& rd)
{
    superstring rss(superstring(process.request).to("\r\n"));
    rss.add_token(token(" ",0));
    rss.add_token(token("GET",1));
    rss.add_token(token("HEAD",3));
    rss.add_token(token("POST",3));
    rss.add_token(token("DELETE",3));
    rss.add_token(token("TRACE",3));
    rss.add_token(token("CONNECT",3));
    rss.add_token(token("OPTIONS",3));

    process.method=rss.tok().id;

    rss.clear_tokens();
    rss.pos++;
    superstring rawuri(rss.to(" "));
    process.http11=rss.check("HTTP/1.1");

    rd.uri=rawuri.to("?");
    string gu=rawuri.to("#");
    if(!gu.empty())
    {
        rd.get=new postgetdata(gu);
    }
}

}//namespace rproc


}//http namespace
