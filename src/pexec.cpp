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
#include "protocol.h"
#include <string.h>

string e404="HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 18\r\n\r\n<h1>NOT FOUND</h1>\r\n";


string its(int n)
{
    string tmp, ret;
    if(n < 0)
    {
        ret="-";
        n=-n;
    }
    do
    {
        tmp+=n%10+48;
        n-=n%10;
    }
    while(n/=10);
    for(int i=tmp.size()-1; i>=0; i--)
        ret+=tmp[i];
    return ret;
}


pagedata exec(string uri,rdata* rd)
{
    pagedata ep;
    page pid = pmap.by_uri(uri);
    //cout << "exec "<<uri.c_str()<<" ("<<pid.type<<")\n";
    switch(pid.type)
    {
    case page_native:
    {
        rd->ctype="text/html;charset="+charset;
        nativepage *npp = (nativepage*)pid.data;
        pagedata ts=npp->page(rd);

        string snd = "HTTP/1.1 200 OK\r\nContent-Type: "+rd->ctype+"\r\nContent-Length: ";
        snd+=its(ts.size);
        snd+="\r\n";
        snd+=rd->cookie->gethead();
        snd+="\r\n";
        string snd2="\r\n";

        pagedata spd;
        spd.size=snd.size()+ts.size+snd2.size();
        spd.data=new char[spd.size];
        memcpy(spd.data,snd.c_str(),snd.size());
        memcpy(spd.data+snd.size(),ts.data,ts.size);
        memcpy(spd.data+snd.size()+ts.size,snd2.c_str(),snd2.size());
        delete[]ts.data;
        return spd;
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
            pagedata spd;
            spd.size=size+snd.size()+snd2.size();
            spd.data=new char[spd.size];
            memcpy(spd.data,snd.c_str(),snd.size());
            memcpy(spd.data+snd.size(),b,size);
            memcpy(spd.data+snd.size()+size,snd2.c_str(),snd2.size());
            delete[] b;
            fclose(f);
            return spd;
        }
        else
        {
            ep(e404);
            return ep;///404 HERE  <<<<<<<<<
        }
    }
    break;
    }
    ep(e404);
    return ep;///404 HERE <<<<<<<<<

}



