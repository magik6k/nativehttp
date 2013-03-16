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
#ifndef NATIVEHTTP_H_INCLUDED
#define NATIVEHTTP_H_INCLUDED
#include <string>
#include <vector>
#include <deque>
#include <stdio.h>
using namespace std;

#ifndef SUPERSTRING_H_INCLUDED
#define SUPERSTRING_H_INCLUDED

using namespace std;

struct token
{
    int id;
    string s;
    token(string st, int i);
};

class superstring
{
private:

    vector<token>tokens;
    bool rae;

public:

    int pos;
    string str;

    void set(string s);

    superstring();
    superstring(string s);
    void operator()(string s);

    void add_token(token t);
    void clear_tokens();
    token tok();
    token tok(string& opt);

    string to(string fend);
    string back_to(string fend);

    string from(string start);

    void change(string from, string to);

    int count(char c);

    string file(string fn);
    void set_file(string fn);
    void append_file(string fn);

    int check(string sch);


};

#endif

struct cfgfil
{

    string name;
    string cont;

};

class Ccfg
{
private:

    string flie;
    vector<cfgfil>fileds;

public:

    void parse_file(const char* f);
    string get_var(string name);
    int get_int(string name);
    Ccfg(string f)
    {
        parse_file(f.c_str());
    }
    ~Ccfg()
    {
        fileds.empty();
    }

};

struct pagedata
{
    pagedata(string s);
    void operator()(string s);
    pagedata();
    char *data;
    size_t size;
};

struct cfil
{
    string name;
    string value;
};

class cookiedata
{
private:
    vector<cfil>data;
    string sets;
public:
    cookiedata(string strcookie);
    string get(string name);
    void set(string name, string value);
    void set(string name, string value, string attributes);
    string gethead();
};

class postgetdata
{
private:
    vector<cfil>data;
public:
    postgetdata(string dstr);
    string get(string name);
};

struct sstat
{
    int hits;
    int* hourly;
    int loghours;
};

struct rdata
{
    cookiedata* cookie;
    postgetdata* get;
    postgetdata* post;
    string response;
    string userAgent;
    string referer;
    string host;
    string uri;
    unsigned int remoteIP;
    string ctype;
};

#define NB_VERSION 1

#define NB_ERROR 0
#define NB_SUCCES 1
typedef bool (*nbfilter)(string*,void*);



class nbrow
{
public:
    string* data;
    ~nbrow();
};

class nbtable
{
public:
    int cn;
    string* cols;
    vector<nbrow*>data;
    string name;
    nbtable(string n, string* cl, int cnb);
    ~nbtable();
    int ins(string* in);
};

class nbresult
{
public:
    vector<string*>res;
    int count();
    int filter(nbfilter flt, void* fltdata);

    string* operator[](int rowid);
};

class nbase
{
private:
    vector<nbtable*>data;
    string bfn;
public:
    nbase(string file);
    int createtable(string name, string* columns, int cnum);
    int insert(string tabname, string* rowdata);
    int del(string tabname, nbfilter flt, void* fltdata);
    int save();
    int istable(string name);
    nbresult operator()(string table);
    nbresult operator()(string table, nbfilter flt, void* fltdata);
};

extern "C" nbase* nbase_open(string file);

///init ONLY functions(use in onload funcion)
extern "C" void attach_uri(string uri,bool top);
///end of init functions

extern "C" string version();
extern "C" string decode_poststring(string str);
extern "C" sstat *get_stats();

extern "C" void log(string lname, string value);
extern "C" void logid(int id, string lname, string value);

#endif // NATIVEHTTP_H_INCLUDED
