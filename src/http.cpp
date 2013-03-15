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
#include <queue>







/*
void http_kz()
{

    gstats.hits=0;
    int ach=time(NULL)/3600;
    SDLNet_Init();

    IPaddress tmp;
    SDLNet_ResolveHost(&tmp,NULL,cfg->get_int("port"));
    server=SDLNet_TCP_Open(&tmp);
    if(!server)
    {
        printf("INIT: %s\n", SDLNet_GetError());
        exit(1);
    }

    serverset = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(serverset,server);

    bool runs = true;

    while(runs)
    {
        if(ach!=time(NULL)/3600)
        {
            ach=time(NULL)/3600;
            if(gstats.loghours>0)
            {
                for(int i=gstats.loghours-2;i>-1;i--)
                {
                    gstats.hourly[i+1]=gstats.hourly[i];
                }
                if(gstats.hourly)gstats.hourly[0]=0;
            }
        }

        int active = SDLNet_CheckSockets(serverset,-1);

        for(int i=0; i<active; i++)
        {
            client = SDLNet_TCP_Accept(server);
            char qbuf[10240];
            int qlen;
            string q;

            qlen=SDLNet_TCP_Recv(client,qbuf,10240);
            if(qlen<10240)
            {
                qbuf[qlen]=0;
            }

            q=qbuf;
            //cout <<"---------\n"<<q.c_str()<<endl<<"---------\n";

            superstring req(q);
            req.add_token(token("GET",0));
            req.add_token(token("POST",1));

            methodid = req.tok().id;
            req.pos++;
            superstring pru(req.to(" HTTP"));
            turi=pru.to("?");
            rdata rd;
            rd.get=NULL;
            rd.post=NULL;
            rd.cookie=NULL;
            rd.uri=turi;
            string uriget=pru.to("#");
            if(!uriget.empty())
            {
                rd.get=new postgetdata(uriget);
            }
            req.clear_tokens();
            req.add_token(token("\r\n\r\n",0));
            req.add_token(token("Cookie: ",1));
            req.add_token(token("Host: ",2));
            req.add_token(token("Referer: ",3));
            req.add_token(token("User-Agent: ",4));
            req.add_token(token("Content-Length: ",5));

            int conlen=0;
            string tcookie="";
            while(req.pos<req.str.size())
            {
                switch(req.tok().id)
                {
                case 0://end of request/post data begin
                    if(methodid==1&&conlen>0)
                    {
                        string pd=req.to("\r\n");
                        if(conlen<=postmax&&conlen>pd.size())
                        {
                            char *postbuf=new char[conlen-pd.size()+1];
                            int torecv=conlen-pd.size();
                            int ar=0;
                            while(torecv>0)
                            {
                                int rca=SDLNet_TCP_Recv(client,postbuf+ar,torecv);
                                if(rca<=0)
                                {
                                    delete[] postbuf;
                                    SDLNet_TCP_Close(client);
                                    continue;
                                }
                                ar+=rca;
                                torecv-=rca;
                                (postbuf+ar)[0]=0;
                            }
                            pd+=postbuf;
                            delete[] postbuf;
                        }
                        rd.post=new postgetdata(pd);
                    }
                    break;
                case 1://cookie
                {
                    tcookie=req.to("\r\n");
                    break;
                }
                case 2://host
                {
                    rd.host=req.to("\r\n");
                    break;
                }
                case 3://referer
                {
                    rd.referer=req.to("\r\n");
                    break;
                }
                case 4://user-agent
                {
                    rd.userAgent=req.to("\r\n");
                    break;
                }
                case 5://content length
                {
                    conlen=strtol(req.to("\r\n").c_str(), NULL, 10);
                    break;
                }
                }
                if(req.pos<req.str.size())req.pos-=2;
            }

            rd.cookie=new cookiedata(tcookie);
            rd.remoteIP=SDLNet_TCP_GetPeerAddress(client)->host;
            pagedata sbuf=exec(turi,&rd);

            gstats.hits++;
            if(gstats.hourly){gstats.hourly[0]++;}

            if(sbuf.data)
            {
                SDLNet_TCP_Send(client,sbuf.data,sbuf.size);
                delete[] sbuf.data;
            }
            SDLNet_TCP_Close(client);
            if(!tcookie.empty())
            {
                delete rd.cookie;
            }
            if(rd.get!=NULL)
            {
                delete rd.get;
            }
        }
    }
}



*/








