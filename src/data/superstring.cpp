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
#include <stdio.h>

#define SSLOCK if(lck)pos=lpos

nativehttp::data::token::token(string st, int i)
{
    s=st;
    id=i;
}

void nativehttp::data::superstring::set(string s)
{
    str=s;
}

nativehttp::data::superstring::superstring()
{
    pos=0;
    lck=false;
    lpos=0;
}

nativehttp::data::superstring::superstring(string s)
{
    this->set(s);
    pos=0;
    lck=false;
    lpos=0;
}

void nativehttp::data::superstring::operator()(string s)
{
    this->set(s);
}

void nativehttp::data::superstring::add_token(token t)
{
    tokens.push_back(t);
}

nativehttp::data::token nativehttp::data::superstring::tok()
{
    vector<unsigned int>d(tokens.size(),0);

    while(pos<str.size())
    {
        for(unsigned int i=0; i<tokens.size(); i++)
        {
            if(str[pos]==tokens[i].s[d[i]])
            {
                d[i]++;
            }
            else
            {
                d[i]=0;
            }
            if(d[i]>=tokens[i].s.size())
            {
                pos++;
                SSLOCK;
                return tokens[i];
            }
        }
        pos++;
    }
    SSLOCK;
    return tokens[0];
}

nativehttp::data::token nativehttp::data::superstring::tok(string& opt)
{
    vector<unsigned int>d(tokens.size(),0);

    while(pos<str.size())
    {
        opt+=str[pos];
        for(unsigned int i=0; i<tokens.size(); i++)
        {
            if(str[pos]==tokens[i].s[d[i]])
            {
                d[i]++;
            }
            else
            {
                d[i]=0;
            }
            if(d[i]>=tokens[i].s.size())
            {
                pos++;
                SSLOCK;
                return tokens[i];
            }
        }
        pos++;
    }
    SSLOCK;
    return tokens[0];
}

string nativehttp::data::superstring::to(string fend)
{
    rae=false;
    unsigned int c=0;
    string rt;
    string prt;
    while(pos<str.size())
    {
        if(str[pos]==fend[c])
        {
            prt+=str[pos];
            c++;
        }
        else
        {
            if(!prt.empty())
            {
                rt+=prt;
                prt.clear();
                c=0;
            }
            rt+=str[pos];
        }
        pos++;
        if(c>=fend.size())
        {
            SSLOCK;
            return rt;
        }
    }
    rae=true;
    SSLOCK;
    return rt;
}

string nativehttp::data::superstring::from(string start)
{
    unsigned int c=0;
    string rt;
    while(pos<str.size())
    {
        if(str[pos]==start[c])
        {
            c++;
        }
        else
        {
            if(c>0)
            {
                c=0;
            }
        }
        pos++;
        if(c>=start.size())
        {
            break;
        }
    }
    while(pos<str.size())
    {
        rt+=str[pos];
        pos++;
    }
    SSLOCK;
    return rt;
}

int nativehttp::data::superstring::count(char c)
{
    int rt=0;
    for(; pos<str.size(); pos++)
    {
        if(str[pos]==c)
        {
            rt++;
        }
    }
    SSLOCK;
    return rt;
}

string nativehttp::data::superstring::back_to(string fend)
{
    string nfe;
    for(int i=fend.size()-1; i>=0; i--)
    {
        nfe+=fend[i];
    }
    unsigned int c=0;
    string rt;
    string prt;
    while(pos>0)
    {
        if(str[pos]==nfe[c])
        {
            prt+=str[pos];
            c++;
        }
        else
        {
            if(!prt.empty())
            {
                rt+=prt;
                prt.clear();
                c=0;
            }
            rt+=str[pos];
        }
        pos--;
        if(c>=nfe.size())
        {
            string ert;
            for(int i=rt.size()-1; i>=0; i--)
            {
                ert+=rt[i];
            }
            SSLOCK;
            return ert;
        }
    }
    string ert;
    for(int i=rt.size()-1; i>=0; i--)
    {
        ert+=rt[i];
    }
    SSLOCK;
    return ert;
}

void nativehttp::data::superstring::change(string from, string to)
{
    string out;
    bool ls=lck;
    if(lck)
    {
        lck=false;
    }
    while(pos<str.size())
    {
        string ctg=this->to(from);
        if(!ctg.empty())
        {
            out+=ctg;
            if(!rae)
            {
                out+=to;
            }
        }
    }
    lck=ls;
    str=out;
    SSLOCK;
}

void nativehttp::data::superstring::remove(string from, string to)
{
    string out;
    bool ls=lck;
    if(lck)
    {
        lck=false;
    }
    while(pos<str.size())
    {

        string ctg=this->to(from);
        if(!ctg.empty())
        {
            out+=ctg;
            if(!rae)
            {
                this->to(to);
            }
        }
    }
    lck=ls;
    str=out;
    SSLOCK;
}

void nativehttp::data::superstring::remove(string s)
{
    string out;
    bool ls=lck;
    if(lck)
    {
        lck=false;
    }
    while(pos<str.size())
    {
        string ctg=this->to(s);
        out+=ctg;
    }
    lck=ls;
    str=out;
    SSLOCK;
}

string nativehttp::data::superstring::file(string fn)
{
    FILE* f=fopen(fn.c_str(),"r");
    if(f)
    {
        fseek(f,0,SEEK_END);
        int size = ftell(f);
        rewind(f);
        string tm;
        tm.resize(size);
        fread(&tm[0],1,size,f);
        fclose(f);
        return tm;
    }
    return "";
}

void nativehttp::data::superstring::set_file(string fn)
{
    str=this->file(fn);
}

void nativehttp::data::superstring::append_file(string fn)
{
    str+=this->file(fn);
}

void nativehttp::data::superstring::clear_tokens()
{
    tokens.clear();
}

int nativehttp::data::superstring::check(string sch)
{

    unsigned int left=sch.size();
    unsigned int tpos=pos;
    while(left>0&&tpos<str.size())
    {
        if(str[tpos]==sch[sch.size()-left])
        {
            left--;
        }
        else
        {
            return 0;
        }
        tpos++;
    }


    if(left<=0)
    {
        return 1;
    }
    return 0;
}

void nativehttp::data::superstring::lock()
{
    lck=true;
    lpos=pos;
}

void nativehttp::data::superstring::lock(unsigned int lp)
{
    lck=true;
    lpos=lp;
}

void nativehttp::data::superstring::unlock()
{
    lck=false;
    lpos=0;
}

#include <iostream>
string nativehttp::data::superstring::from_int(int in)
{
    string tmp, ret;
    if(in < 0)
    {
        ret = "-";
        in = -in;
    }
    do
    {
        tmp += in % 10 + 48;
        in -= in % 10;
    }
    while(in /= 10);
    for(int i = tmp.size()-1; i >= 0; i--)
        ret += tmp[i];

    return ret;
}
int nativehttp::data::superstring::from_string(string in)
{
    int tmp = 0;
    unsigned int i = 0;
    bool m = false;
    if(in[0] == '-')
    {
        m = true;
        i++;
    }
    for(; i < in.size(); i++)
        tmp = 10 * tmp + in[i] - 48;
    return m ? -tmp : tmp;
}

