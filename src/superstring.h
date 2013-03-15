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
#ifndef SUPERSTRING_H_INCLUDED
#define SUPERSTRING_H_INCLUDED

#include <string>
#include <vector>

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


#endif // SUPERSTRING_H_INCLUDED
