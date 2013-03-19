#include <iostream>
#include <SDL/SDL_net.h>
#include <string>
#include <vector>
using namespace std;


int tests=0;
string addr;
string uri;
int port=0;
IPaddress ip;
int i=0;
string req;
vector<TCPsocket> s;
vector<int> ids;
vector<SDL_Thread*>thr;
int err=0;
int tsth(void* sid)
{
char td[4096];
s[*(int*)sid]=SDLNet_TCP_Open(&ip);
    for(;i<tests;i++)
    {
        bi:
        if(!s[*(int*)sid])
        {
            //i--;
            //cout << "bugged near "<<i<<endl;
            s[*(int*)sid]=SDLNet_TCP_Open(&ip);
            err++;
            goto bi;
        }
        else
        {
            SDLNet_TCP_Send(s[*(int*)sid],req.c_str(),req.size());
            if(SDLNet_TCP_Recv(s[*(int*)sid],&td,4096)<1)
            {
                i--;
                //cout << "error near "<<i<<endl;
                SDLNet_TCP_Close(s[*(int*)sid]);
                s[*(int*)sid]=SDLNet_TCP_Open(&ip);
                err++;
                goto bi;
            }
        }

        //s[*(int*)sid]=NULL;

    }
    SDLNet_TCP_Close(s[*(int*)sid]);
    return 0;
}


int main()
{
    SDL_Init(SDL_INIT_TIMER);
    SDLNet_Init();
    int th=0;
    cout << "tests: ";
    cin >> tests;
    cout << "theards: ";
    cin >> th;
    cout <<"host: ";
    cin >> addr;
    cout<<"port: ";
    cin >> port;
    cout <<"rest of URI: ";
    cin >> uri;
    cout << "translating host: ";

    SDLNet_ResolveHost(&ip,addr.c_str(),port);
    cout <<int(((Uint8*)&ip.host)[0])<<'.'<<int(((Uint8*)&ip.host)[1])<<'.'<<int(((Uint8*)&ip.host)[2])<<'.'<<int(((Uint8*)&ip.host)[3])<<endl;
    req="GET ";
    req+=uri;
    req+=" HTTP/1.1\r\nHost: ";
    req+=addr;
    //req+="\r\nUser-Agent: Mozilla/5.0 (X11; Linux i686) AppleWebKit/536.11 (KHTML, like Gecko) Ubuntu/12.04 Chromium/20.0.1132.47 Chrome/20.0.1132.47 Safari/536.11\r\nAccept-Charset: ISO-8859-2,utf-8;q=0.7,*;q=0.3\r\n\r\n";
    req+="\r\nAccept-Charset: ISO-8859-2,utf-8;q=0.7,*;q=0.3\r\n\r\n";
    cout << "starting\n";
    int start=SDL_GetTicks();
    s.resize(th,NULL);
    ids.resize(th);
    thr.resize(th);
    for(int j=0;j<th;j++)
    {
        ids[j]=j;
        thr[j]=SDL_CreateThread(tsth,&ids[j]);
    }
    for(;i<tests;)
    {
        SDL_Delay(7500);
        cout << "refresh theards(done "<<i<<", err: "<<err<<")\n";
        for(int j=0;j<th;j++)
        {
            //SDL_KillThread(thr[j]);
            //SDLNet_TCP_Close(s[j]);
            //thr[j]=SDL_CreateThread(tsth,&ids[j]);
        }

    }
    int end=SDL_GetTicks();
    cout << "done in "<<double(end-start)/1000.f<<"s\n";

    return 0;
}
