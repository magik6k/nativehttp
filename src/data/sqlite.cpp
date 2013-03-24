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
#include "../nativehttp.h"
#include "queue.h"
#include <string.h>


void nativehttp::base::SQLite::open(const char* file,bool fast)
{
    sqlite3_open(file, &db);
    if(fast)
    {
        sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &emsg);
        sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &emsg);
    }
}

const char* nativehttp::base::SQLite::getLastError()
{
    return emsg;
}

nativehttp::base::SQLiteResult nativehttp::base::SQLite::exec(char* q)
{
    sqlite3_stmt *statement;
    unsigned int cols = 0;
    data::queue<char**> dat;


	if(sqlite3_prepare_v2(db, q, -1, &statement, 0) == SQLITE_OK)
	{
		cols = sqlite3_column_count(statement);
		int result = 0;
		while(true)
		{
			result = sqlite3_step(statement);

			if(result == SQLITE_ROW)
			{
			    char** row=new char*[cols];
				for(unsigned int col = 0; col < cols; col++)
				{
					char* s = (char*)sqlite3_column_text(statement, col);
					row[col] = new char[strlen(s)+1];
					strcpy(row[col],s);
				}
				dat.push(row);
				delete[] row;
			}
			else
			{
				break;
			}
		}

		sqlite3_finalize(statement);
	}

	char*** rtd=new char**[dat.size()];
    unsigned int ds=dat.size();
    for(unsigned int i=0;i<ds;i++)
    {
        rtd[i]=dat.front();
        dat.pop();
    }

    nativehttp::base::SQLiteResult rt;
    rt.__set(cols,ds,rtd);
    return rt;

}

void nativehttp::base::SQLite::transaction_start()
{
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &emsg);
}

void nativehttp::base::SQLite::transaction_done()
{
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &emsg);
}

void nativehttp::base::SQLiteResult::__set(unsigned int c,unsigned int r, char*** d)
{
    cols=c;
    rows=r;
    dt=d;
}

void nativehttp::base::SQLiteResult::free()
{
    for(unsigned int i=0;i<rows;i++)
    {
        for(unsigned int j=0;j<cols;j++)
        {
            delete[] dt[i][j];
        }
        delete[] dt[i];
    }
    if(dt)delete[] dt;
    dt=NULL;
}

char** nativehttp::base::SQLiteResult::operator[](int i)
{
    if(dt)return dt[i];
    return NULL;
}
