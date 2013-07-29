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
#include "utils/memory.h"

struct nbfhead
{
	char fid[4];
	int v;
	unsigned int tables;
};

namespace mutl = utils::memory;

///nbase class

class nbfstring
{
	public:
		string str;
		int writestr(FILE *fp)
		{
			size_t strs = str.size();
			fwrite(&strs, sizeof(size_t), 1, fp);
			fwrite(str.c_str(), 1, strs, fp);
			return strs;
		}
		int readstr(FILE *fp)
		{
			size_t strs = 0;
			fread(&strs, sizeof(size_t), 1, fp);
			str.resize(strs);
			fread((char*)str.c_str(), 1, strs, fp);
			return strs;
		}
		nbfstring()
		{

		}
		nbfstring(string s)
		{
			str = s;
		}
};

nativehttp::base::nbase::nbase(string file)
{
	bfn = file;
	FILE *bf = fopen(file.c_str(), "rb");
	if (bf)
	{
		nbfhead head;
		fread(&head, sizeof(nbfhead), 1, bf);
		if (string(head.fid) == "nbB" && head.v == NB_VERSION)
		{
			for (unsigned int i = 0; i < head.tables; i++)
			{
				nbfstring tabname;
				tabname.readstr(bf);
				int cln = 0;
				fread(&cln, sizeof(int), 1, bf);
				string *fcols = mutl::alloc<string>(cln);
				if (fcols)
				{
					for (int j = 0; j < cln; j++)
					{
						nbfstring cn;
						cn.readstr(bf);
						fcols[j] = cn.str;
					}
					this->createtable(tabname.str, fcols, cln);

					size_t rnum = 0;
					fread(&rnum, sizeof(size_t), 1, bf);
					for (unsigned int j = 0; j < rnum; j++)
					{
						for (int k = 0; k < cln; k++)
						{
							nbfstring nbds;
							nbds.readstr(bf);
							fcols[k] = nbds.str;
						}
						this->insert(tabname.str, fcols);
					}
					delete[] fcols;
				}
			}
		}
		fclose(bf);
	}
}

int nativehttp::base::nbase::istable(string name)
{
	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data[i]->name == name)
		{
			return NB_SUCCES;
		}
	}
	return NB_ERROR;
}

int nativehttp::base::nbase::createtable(string name, string *columns, int cnum)
{
	nativehttp::base::nbtable *tmtable = new nbtable(name, columns, cnum);
	data.push_back(tmtable);
	return NB_SUCCES;
}

int nativehttp::base::nbase::insert(string tabname, string *rowdata)
{
	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data[i]->name == tabname)
		{
			return data[i]->ins(rowdata);
		}
	}
	return NB_ERROR;
}

nativehttp::base::nbresult nativehttp::base::nbase::operator()(string table)
{
	nativehttp::base::nbresult rt;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data[i]->name == table)
		{
			for (unsigned int j = 0; j < data[i]->data.size(); j++)
			{
				rt.res.push_back(data[i]->data[j]->data);
			}
			break;
		}
	}
	return rt;
}

nativehttp::base::nbresult nativehttp::base::nbase::operator()(string table, nativehttp::base::nbfilter flt, void *fltdata)
{
	nativehttp::base::nbresult rt;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data[i]->name == table)
		{
			for (unsigned int j = 0; j < data[i]->data.size(); j++)
			{
				if ((*flt)(data[i]->data[j]->data, fltdata))
				{
					rt.res.push_back(data[i]->data[j]->data);
				}
			}
			break;
		}
	}
	return rt;
}

int nativehttp::base::nbase::del(string tabname, nativehttp::base::nbfilter flt, void *fltdata)
{
	int o = -1;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		if (data[i]->name == tabname)
		{
			o = 0;
			if (flt)
			{
				for (int j = data[i]->data.size() - 1; j >= 0; j--)
				{
					if ((*flt)(data[i]->data[j]->data, fltdata))
					{
						o++;
						data[i]->data.erase(data[i]->data.begin() + j);
					}
				}
			}
			else
			{
				for (int j = data[i]->data.size() - 1; j >= 0; j--)
				{
					o++;
					data[i]->data.erase(data[i]->data.begin() + j);
				}
			}
			break;
		}
	}
	return o;
}

///nbase class end
///nbtable class&functions
struct nbblt
{
	string f;
	nativehttp::base::nbase *p;
};
vector<nbblt>nbopls;

extern "C" nativehttp::base::nbase *nbase_open(string file)
{
	for (unsigned int i = 0; i < nbopls.size(); i++)
	{
		if (nbopls[i].f == file)
		{
			return nbopls[i].p;
		}
	}
	nbblt ttm = {file, new nativehttp::base::nbase(file)};
	if (ttm.p)
	{
		nbopls.push_back(ttm);
	}
	return ttm.p;
}

nativehttp::base::nbtable::nbtable(string n, string *cl, int cnb)
{
	name = n;
	cn = cnb;
	cols = mutl::alloc<string>(cnb);
	for (int i = 0; i < cnb; i++)cols[i] = cl[i];
}

nativehttp::base::nbtable::~nbtable()
{
	if (cols)
	{
		delete[] cols;
	}
}

int nativehttp::base::nbtable::ins(string *in)
{
	nbrow *trow = new nativehttp::base::nbrow;
	trow->data = mutl::alloc<string>(cn);
	for (int i = 0; i < cn; i++)
	{
		trow->data[i].resize(in[i].size());
		trow->data[i] = in[i];
	}
	data.push_back(trow);
	return NB_SUCCES;
}

int nativehttp::base::nbase::save()
{
	FILE *bf = fopen(bfn.c_str(), "wb");
	if (!bf)return NB_ERROR;

	nbfhead head = {{'n', 'b', 'B', 0}, NB_VERSION, data.size()};
	fwrite(&head, sizeof(nbfhead), 1, bf);

	for (unsigned int i = 0; i < data.size(); i++)
	{
		nbfstring tabname(data[i]->name);   ///table head - nbstr(name), colnum, str[cols], rownum
		tabname.writestr(bf);
		fwrite(&data[i]->cn, sizeof(int), 1, bf);
		for (int j = 0; j < data[i]->cn; j++)
		{
			nbfstring cnst(data[i]->cols[j]);
			cnst.writestr(bf);
		}
		size_t rows = data[i]->data.size();
		fwrite(&rows, sizeof(size_t), 1, bf);
		for (size_t j = 0; j < rows; j++)   /// rows - [str[rowfil]]
		{
			for (int k = 0; k < data[i]->cn; k++)
			{
				nbfstring dstr(data[i]->data[j]->data[k]);
				dstr.writestr(bf);
			}
		}
	}

	fclose(bf);
	return NB_SUCCES;
}

///nbtable class end
///nbrow class

nativehttp::base::nbrow::~nbrow()
{
	if (data)
	{
		delete[] data;
	}
}

///nbrow class end
///nbresult class

string *nativehttp::base::nbresult::operator[](int rowid)
{
	return res[rowid];
}

int nativehttp::base::nbresult::count()
{
	return res.size();
}

int nativehttp::base::nbresult::filter(nbfilter flt, void *fltdata)
{
	int o = 0;
	vector<string*>nvec;
	for (unsigned int i = 0; i < res.size(); i++)
	{
		if ((*flt)(res[i], fltdata))
		{
			nvec.push_back(res[i]);
		}
		else o++;
	}
	res = nvec;
	return o;
}

///nbresult class end
