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

#define NATIVEHTTP_API_VERSION 24
/*
NOTE, that API version macro MUST be written with following style:
#define[space - ASCII 32]NATIVEHTTP_API_VERSION[space - ASCII 32][Version - ASCII number(That Cpp compiller will see as normal integer)][newline]
*/

#include <string>
#include <vector>
#include <deque>
#include <stdio.h>
#include <sqlite3.h>

#define initated() return -(NATIVEHTTP_API_VERSION)
#define initerr(_rcode) return ((_rcode>0)?(_rcode):(-(_rcode)))+2

#define NH_SUCCES 0
#define NH_ERROR 1

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

		struct _chsetrule
		{
            string _from;
            string _to;
		};

		class superstring
		{
			private:

				vector<token>tokens;
				bool rae;

            protected:
				bool lck;
				unsigned int lpos;

			public:

				unsigned int pos;/** Position */
				string str;/** Container string, DO NOT OVERUSE THIS, the type MAY CHANGE in future API versions */


				/** Core functions*/
				~superstring();
				superstring();

				superstring& lock();
				superstring& lock(unsigned int lp);
				superstring& unlock();

				bool atbegin();
				bool atend();

				size_t length();
				size_t size();

				void clear();
				const char* c_str();

				superstring& go_begin();
				superstring& go_end();
				superstring& go_begin(int p);
				superstring& go_end(int p);
				superstring& go_pos(int p);

				char& operator[](unsigned int i);


				/** C++ string functions*/
				superstring(string s);
				void set(string s);
				void operator()(string s);
				superstring& operator+=(string in);

                string skip(string stb);
                string tochar(string fend);
                bool contain(string stb);
                bool contain_not(string stb);
                bool contain_only(string stb);
				string to(string fend);
				string back_to(string fend);
				string from(string start);
				unsigned int find(string str);

				int check(string sch);
				string get(unsigned int n);
				string get_begin(unsigned int n);
				string get_end(unsigned int n);

				superstring& change(string from, string to);
				superstring& remove(string from, string to);
				superstring& remove(string s);

                static int int_from_str(string in);

				/** SuperString functions*/
				void set(superstring s);
				void operator()(superstring s);
				superstring operator+=(superstring in);

                superstring skip(superstring stb);
                superstring tochar(superstring fend);
                bool contain(superstring stb);
                bool contain_not(superstring stb);
                bool contain_only(superstring stb);
				superstring to(superstring fend);
				superstring back_to(superstring fend);
				superstring from(superstring start);

				/** Integer functions*/
				void set(int s);
				void operator()(int s);
				superstring operator*=(unsigned int rc);

				static string str_from_int(int in);
				static superstring sst_from_int(int in);

				string from(unsigned int sp);
				string to(unsigned int ep);

				superstring& remove(unsigned int from, unsigned int to);

				/** 64bit integer functions */
                static string str_from_int64(int64_t in);
				static superstring sst_from_int64(int64_t in);
                static int64_t int64_from_sst(superstring in);
                static int64_t int64_from_str(string in);

				/** Size functions */
				static string str_from_size(int64_t in);
				static superstring sst_from_size(int64_t in);

				/** Double precission functions */
				static string str_from_double(double in);
				static superstring sst_from_double(double in);

				/** Token functions*/
				superstring& add_token(token t);
				void clear_tokens();
				token tok();
				token tok(string &opt);
				size_t num_tokens();


				/** Character functions */
				int count(const char c);

				/** IP functions */
                static unsigned int ip4_from_str(string str);
                static string str_from_ip4(unsigned int ip);

				/** File functions*/
				static string file(string fn);
				void set_file(string fn);
				void append_file(string fn);

                static int file(string fn, string content);
                static int str_to_file(string fn, string content);
                static int sst_to_file(string fn, superstring content);
                static int append_str_file(string fn, string append);
                static int append_sst_file(string fn, superstring append);

		};



        class superstringset : public superstring
        {
            private:
                vector<_chsetrule> chset_rules;

            public:

                superstringset& changeset_addRule(string from, string to);
                superstringset& changeset_clean();
                superstringset& changeset_removeRule(string from, string to);
                int changeset_numRules();
                superstringset& changeset_exec();
        };

		typedef superstring ss;
		typedef superstringset ssSet;
		typedef int clientid;

		struct cfgfil
		{

			string name;
			string cont;

		};

		class Ccfg
		{
			private:

				string file;
				vector<cfgfil>fileds;

			public:

				void parse_file(const char *f);
				string get_var(string name) const;
				int get_int(string name) const;
				vector<cfgfil> get_all() const;
				vector<cfgfil> get_prefix(string name) const;
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
				void __init(cookiedata *cd, nativehttp::data::clientid uid);
				void __mkinv(nativehttp::data::clientid uid);
				string get(string name);
				void set(string name, string value);
				bool is_valid();
                unsigned int get_ssid();
				static session* by_uid(nativehttp::data::clientid uid);
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
		void attach_uri(string uri, bool top);
	}

	namespace server
	{
		string version();
		void log(string lname, string value);
		void err(string lname, string value);
		void logid(int id, string lname, string value);

		const char* getPageDirectory();
		const char* getConfigFile();
		const char* getTempDir();
		const nativehttp::data::Ccfg* getConfig();

		data::clientid maxClients();

		bool is_ssl();

		namespace stat
		{

#define NH_STAT_YES     1
#define NH_STAT_NO      0
#define NH_STAT_ERROR   18446744073709551615ULL //2^64-1
			bool enabled();

            enum unit
            {
                len_hourly = 0,
                len_daily,
                len_weekly,

                upload,
                download,
                upload_hourly,
                download_hourly,
                upload_daily,
                download_daily,
                upload_weekly,
                download_weekly,

                requests,
                connections,
                requests_hourly,
                connections_hourly,
                requests_daily,
                connections_daily,
                requests_weekly,
                connections_weekly,

                requests_get,
                requests_post,

                ws_upload,
                ws_download,
                ws_handshakes,
                ws_frames_received,
                ws_frames_sent,
                ws_messages_received,
                ws_messages_sent,
                ws_upload_daily,
                ws_download_daily,
                ws_handshakes_daily,
                ws_messages_received_hourly,
                ws_messages_sent_hourly,
                ws_messages_received_daily,
                ws_messages_sent_daily,
                ws_messages_received_weekly,
                ws_messages_sent_weekly,

                http_head_size_received,
                http_head_size_sent,
                http_postdata_size,
                http_postdata_size_daily,
                http_head_size_received_daily,
                http_head_size_sent_daily,

                sessions_created,
                sessions_created_daily,
                sessions_created_weekly,
                sessions_max_existing_hourly
            };

			uint64_t get(unit value);
            uint64_t get(unit value, int column);

		}
	}

	namespace utils
	{
		extern "C" string decode_poststring(string str);
		extern "C" string getStackTrce();

	}

	namespace websock
	{
        typedef int (*cb_onConnect)(nativehttp::data::clientid);
        typedef int (*cb_onDisconnect)(nativehttp::data::clientid);
        typedef int (*cb_onBinMsg)(nativehttp::data::clientid, const void*, uint64_t);
        typedef int (*cb_onTxtMsg)(nativehttp::data::clientid, const char*);

        ///'add' function is returning AppID
        int add(const char* uri, const char* protocol_name, cb_onConnect onConnect, cb_onDisconnect onDisconnect, cb_onTxtMsg onTMsg, cb_onBinMsg onBMsg);

        int disconnect(nativehttp::data::clientid uid);

        int sendTxt(nativehttp::data::clientid uid, const char* data);
        int sendBin(nativehttp::data::clientid uid, const void* data, uint64_t size);

        int streamTxt(nativehttp::data::clientid uid, const char* data, bool finalize);
        int streamBin(nativehttp::data::clientid uid, const void* data, uint64_t size, bool finalize);

        ///AppID functions
        int onConnect(int appid, cb_onConnect onConnect);
        int onDisconnect(int appid, cb_onDisconnect onDisconnect);
        int onText(int appid, cb_onTxtMsg onTMsg);
        int onBinary(int appid, cb_onBinMsg onBMsg);

	}

}//nativehttp namespace

namespace nh = nativehttp;
namespace nd = nativehttp::data;
namespace nb = nativehttp::base;
namespace ns = nativehttp::server;
namespace nw = nativehttp::websock;

#endif // NATIVEHTTP_H_INCLUDED
