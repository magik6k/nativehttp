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
#include "superstring.h"
#include <stdio.h>

token::token(string st, int i)
{
    s=st;
    id=i;
}

void superstring::set(string s)
{
    str=s;
}

superstring::superstring()
{
    pos=0;
}

superstring::superstring(string s)
{
    this->set(s);
    pos=0;
}

void superstring::operator()(string s)
{
    this->set(s);
}

void superstring::add_token(token t)
{
    tokens.push_back(t);
}

token superstring::tok()
{
    vector<int>d(tokens.size(),0);

    while(pos<str.size())
    {
        for(int i=0; i<tokens.size(); i++)
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
                return tokens[i];
            }
        }
        pos++;
    }
    return tokens[0];
}

token superstring::tok(string& opt)
{
    vector<int>d(tokens.size(),0);

    while(pos<str.size())
    {
        opt+=str[pos];
        for(int i=0; i<tokens.size(); i++)
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
                return tokens[i];
            }
        }
        pos++;
    }
    return tokens[0];
}

string superstring::to(string fend)
{
    rae=false;
    int c=0;
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
            return rt;
        }
    }
    rae=true;
    return rt;
}

string superstring::from(string start)
{
    int c=0;
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
    return rt;
}

int superstring::count(char c)
{
    int rt=0;
    for(; pos<str.size(); pos++)
    {
        if(str[pos]==c)
        {
            rt++;
        }
    }
    return rt;
}

string superstring::back_to(string fend)
{
    string nfe;
    for(int i=fend.size()-1; i>=0; i--)
    {
        nfe+=fend[i];
    }
    int c=0;
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
            return ert;
        }
    }
    string ert;
    for(int i=rt.size()-1; i>=0; i--)
    {
        ert+=rt[i];
    }
    return ert;
}

void superstring::change(string from, string to)
{
    string out;
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
    str=out;
}

string superstring::file(string fn)
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

void superstring::set_file(string fn)
{
    str=this->file(fn);
}

void superstring::append_file(string fn)
{
    str+=this->file(fn);
}

void superstring::clear_tokens()
{
    tokens.clear();
}

int superstring::check(string sch)
{

    int left=sch.size();
    int tpos=pos;
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
