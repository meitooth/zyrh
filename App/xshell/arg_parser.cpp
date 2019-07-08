#include "arg_parser.h"
#include <regex>


ArgParser::ArgParser(void)
{
}

ArgParser::~ArgParser(void)
{
}
std::vector<std::string> split(const string& input, const string& regex) {
	// passing -1 as the submatch index parameter performs splitting
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}
void ArgParser::Register(const char* szFields,bool bDual)
{
	RECORD r;
	vector<string> ss;

	ss = split(szFields, ",");
	//std::split(ss,szFields,boost::is_any_of(","));

	for (vector<string>::const_iterator itr=ss.begin();itr!=ss.end();itr++)
	{
		r.fields.push_back(*itr);
	}
	r.bDual=bDual;
	m_params.push_back(r);
}

string ArgParser::Get(const char* szField)
{
	vector<RECORD>::const_iterator itr=Find(szField);
	if(itr!=m_params.end())
	{
		return itr->svalue;
	}

	return "";
}



bool ArgParser::Parse(int argc, char* argv[])
{
	for (int i=1;i<argc;i++)
	{
		vector<RECORD>::iterator itr=Find(argv[i]);
		if(itr==m_params.end())
		{
			return false;
		}
		if(itr->bDual)
		{
			if(i+1>=argc)
			{
				return false;
			}
			itr->svalue=argv[i+1];
			++i;
		}
		else
		{
			itr->svalue="True";
		}
	}

	return true;
}





bool ArgParser::IsEqual(const char* szField,const vector<string>& v)
{
	for (vector<string>::const_iterator itr=v.begin();itr!=v.end();itr++)
	{
		if(*itr==szField)
			return true;
	}

	return false;
}

vector<ArgParser::RECORD>::iterator ArgParser::Find(const char* szField)
{
	for (vector<RECORD>::iterator itr=m_params.begin();itr!=m_params.end();itr++)
	{
		if(IsEqual(szField,itr->fields))
		{
			return itr;
		}
	}
	return m_params.end();
}
