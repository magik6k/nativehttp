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

#include "pagemap.h"
#include <iostream>
#include <string.h>

void page_mapper::mapdir(string d)
{
    for(unsigned int i=0;i<files.size();i++){delete[] files[i];files[i]=NULL;}
    files.clear();
    nativehttp::data::queue<string>todo;
    todo.push(d);

    while(todo.size()>0)
    {
        DIR *dp;
        if((dp  = opendir(todo.front().c_str())) == NULL)
        {
            cout <<"error: "<< errno << ", for directory " << todo.front().c_str() << endl;
        }
        else
        {
            struct dirent *de;
            while ((de = readdir(dp)) != NULL)
            {
                struct stat fi;
                if (lstat( (todo.front()+string(de->d_name)).c_str(), &fi)<0)
                {
                    cout << "error(pm.statact) \n";
                }
                else
                {
                    if(S_ISDIR(fi.st_mode))
                    {
                        if(de->d_name[0]!='.')
                        {
                            todo.push((todo.front()+string(de->d_name)+'/').c_str());
                        }
                    }
                    else
                    {
                        if(string(de->d_name)[string(de->d_name).size()-1]!='~')
                        {
                            string ts=(todo.front()+string(de->d_name));
                            char* b=new char[ts.size()+1];
                            if(b)
                            {
                                strcpy(b,ts.c_str());
                                b[ts.size()]='\0';
                                files.push_back(b);
                            }
                            else
                            {
                                nativehttp::server::log("pagemap.cpp","mapper error[0]");
                            }
                        }

                    }
                }
                //cout << (string(de->d_name)) << endl;
            }
            closedir(dp);
        }
        todo.pop();
    }
}
