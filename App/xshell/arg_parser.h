#pragma once

#include <vector>
#include <string>
using namespace std;




class ArgParser
{
public:
	struct RECORD
	{
		vector<string> fields;
		string svalue;
		bool bDual;
		RECORD()
		{
			bDual=false;
		}
	};

	ArgParser(void);
	~ArgParser(void);

	void Register(const char* szFields,bool bDual=false);
	string Get(const char* szField);
	bool Parse(int argc, char* argv[]);
private:
	vector<RECORD>::iterator Find(const char* szField);
	bool IsEqual(const char* szField,const vector<string>& v);
private:
	vector<RECORD> m_params;
};

