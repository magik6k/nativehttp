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
#include "ws.h"
#include "http/net/net.h"
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <math.h>

namespace ws
{

    int Base64E(const unsigned char* in, int len, char** buffer) {
      BIO *bio, *b64;
      FILE* stream;
      int encodedSize = 4*ceil(double(len)/3);
      *buffer = (char *)malloc(encodedSize+1);

      stream = fmemopen(*buffer, encodedSize+1, "w");
      b64 = BIO_new(BIO_f_base64());
      bio = BIO_new_fp(stream, BIO_NOCLOSE);
      bio = BIO_push(b64, bio);
      BIO_write(bio, in, len);
      BIO_flush(bio);
      BIO_free_all(bio);
      fclose(stream);

      return (0); //success
    }

    void handshake(int uid, nativehttp::rdata &rd, http::rproc::lrqd &ld)
    {
        cout << "ACC: "<<ld.ws_acc_key<<endl;
        cout << "PROT: "<<ld.ws_prot<<endl;


        int fu = find_uri(rd.uri.c_str());

        cout << "URID: "<<fu<<endl;

        if(fu>-1)
        {
            cout << "PFU OK\n";
            if((*units)[fu].protocol == string(ld.ws_prot))
            {
                cout << "PROT OK";
                int ocs = NH_SUCCES;
                if((*units)[fu].on_connect)
                {
                    ocs = (*(*units)[fu].on_connect)(uid);
                }

                if(ocs == NH_SUCCES)
                {
                    unsigned char shabuf[20];

                    string th = ((string)ld.ws_acc_key+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

                    SHA1((const unsigned char*)th.c_str(), th.size(), shabuf);
                    char *B64 = NULL;
                    Base64E(shabuf,20,&B64);

                    cout << "OPTAC:"<<B64<<endl;

                    string rtn = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
                    rtn += B64;
                    rtn += "\r\nSec-WebSocket-Protocol: ";
                    rtn += ld.ws_prot;
                    rtn += "\r\n\r\n";

                    http::send(uid, rtn.size(), strdup(rtn.c_str()), true);

                    http::client_protocol[uid] = CLPROT_WEBSOCKETS;
                    http::unlockclient(uid);

                    rtn.clear();

                    delete[] B64;
                }
            }
        }

        delete[] ld.ws_acc_key;
        delete[] ld.ws_prot;

    }
}
