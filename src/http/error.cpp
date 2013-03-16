#include "error.h"


namespace http
{
namespace error
{
pagedata load_error(string file, string head)
{
    pagedata rt;
    string epc="HTTP/1.1 "+head+"\r\n"+http::headers::standard+"Content-Length: ";
    FILE* f=fopen(file.c_str(),"r");
    if(f)
    {
        fseek(f,0,SEEK_END);
        rt.size = ftell(f);
        fseek(f,0,SEEK_SET);
        epc+=its(rt.size);
        epc+="\r\n\r\n";
        unsigned int rs=rt.size;
        rt.size+=epc.size()+3;//3 = "\r\n\0"
        rt.data=new char[rt.size];
        memcpy(rt.data,epc.c_str(),epc.size());
        fread(rt.data+epc.size(),1,rs,f);
        rt.data[rt.size-3]='\r';
        rt.data[rt.size-2]='\n';
        rt.data[rt.size-1]='\0';
        fclose(f);
    }
    else
    {
        log("ERROR","oppening error file failed.. "+file);
    }
    return rt;
}
}
}
