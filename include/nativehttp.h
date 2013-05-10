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
#include <sqlite3.h>

using namespace std;

namespace nativehttp
{

	namespace data
	{

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
			bool lck;
			unsigned int lpos;

		public:

			unsigned int pos;
			string str;

			void set(string s);
			void set(superstring s);

			~superstring();
			superstring();
			superstring(string s);

			void operator()(string s);
			void operator()(superstring s);
			superstring operator+=(superstring in);
			superstring operator*=(unsigned int rc);


			void add_token(token t);
			void clear_tokens();
			token tok();
			token tok(string &opt);
			size_t num_tokens();

			string to(string fend);
			string back_to(string fend);

			string from(string start);

			void change(string from, string to);
			void remove(string from, string to);
			void remove(string s);

			int count(const char c);

			int check(string sch);

			void lock();
			void lock(unsigned int lp);
			void unlock();

			static string file(string fn);
			void set_file(string fn);
			void append_file(string fn);

			static string from_int(int in);
			static int from_string(string in);

			const char* c_str();

			size_t length();
			size_t size();

			void clear();

		};

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

			void parse_file(const char *f);
			string get_var(string name);
			int get_int(string name);
			Ccfg(string f)
			{
				parse_file(f.c_str());
			}
			~Ccfg()
			{
				fileds.clear();
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

		class session
		{
		private:
			unsigned int ssid;
			bool valid;
		public:
			void __init(cookiedata *cd);
			string get(string name);
			void set(string name, string value);
		};

	}//data namespace

	struct rdata
	{
		nativehttp::data::cookiedata *cookie;
		nativehttp::data::postgetdata *get;
		nativehttp::data::postgetdata *post;
		nativehttp::data::session *session;
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

	namespace base
	{

		typedef bool (*nbfilter)(string*, void*);

		class nbrow
		{
		public:
			string *data;
			~nbrow();
		};

		class nbtable
		{
		public:
			int cn;
			string *cols;
			vector<nbrow*>data;
			string name;
			nbtable(string n, string *cl, int cnb);
			~nbtable();
			int ins(string *in);
		};

		class nbresult
		{
		public:
			vector<string*>res;
			int count();
			int filter(nbfilter flt, void *fltdata);

			string *operator[](int rowid);
		};

		class nbase
		{
		private:
			vector<nbtable*>data;
			string bfn;
		public:
			nbase(string file);
			int createtable(string name, string *columns, int cnum);
			int insert(string tabname, string *rowdata);
			int del(string tabname, nbfilter flt, void *fltdata);
			int save();
			int istable(string name);
			nbresult operator()(string table);
			nbresult operator()(string table, nbfilter flt, void *fltdata);
		};

		extern "C" nbase *nbase_open(string file);

		typedef class SQLite SQLite;

		class SQLiteResult
		{
		private:
			unsigned int cols;
			unsigned int rows;
			char *** dt;
			nativehttp::base::SQLite *from;
		public:

			SQLiteResult();
			void __set(unsigned int c, unsigned int r, char *** d, nativehttp::base::SQLite *clr);

			void free();

			unsigned int numRows();
			char **operator[](int);

		};

		enum SQLite_ctype
		{
			SLC_NULL,
			SLC_INTEGER,
			SLC_REAL,
			SLC_TEXT,
			SLC_BLOB
		};

		struct SQLiteCol
		{
			SQLiteCol();
			SQLiteCol(const char *n, SQLite_ctype t);
			const char *name;
			SQLite_ctype type;
		};

		class SQLite
		{
		private:

			sqlite3 *db;
			char *emsg;

		public:

			const char *getLastError();
			void open(const char *file, bool fast);
			SQLiteResult exec(const char *q);

			void transaction_start();
			void transaction_done();

			void create_table(const char *name, unsigned int cols, ...);
			void create_table(const char *name, unsigned int cols, SQLiteCol *cl);

			bool isTable(const char *name);

		};

		extern "C" SQLite *sqlite_open(string file, bool fast);

	}//base namespace

	namespace init
	{
		extern "C" void attach_uri(string uri, bool top);
	}

	namespace server
	{
		extern "C" string version();
		extern "C" void log(string lname, string value);
		extern "C" void logid(int id, string lname, string value);
		namespace stat
		{
			extern "C" bool stats();
			extern "C" bool transfer_stats();
			extern "C" bool hit_stats();
			extern "C" bool method_stats();

			extern "C" unsigned long hits();
			extern "C" unsigned long connections();
			extern "C" unsigned long uploaded();
			extern "C" unsigned long downloaded();

			extern "C" unsigned long get_requests();
			extern "C" unsigned long post_requests();

			extern "C" unsigned long hourly_length();

			extern "C" unsigned long hour_hits(unsigned long hid);
			extern "C" unsigned long hour_connections(unsigned long hid);
			extern "C" unsigned long hour_upload(unsigned long hid);
			extern "C" unsigned long hour_download(unsigned long hid);

		}
	}

	namespace utils
	{
		extern "C" string decode_poststring(string str);
	}

}//nativehttp namespace

namespace nh = nativehttp;

#endif // NATIVEHTTP_H_INCLUDED
