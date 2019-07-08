//
//  stringz.cpp
//  ftpz
//
//  Created by 胖胖的ALEX on 2017/10/25.
//
#include "stdafx.h"
#include "stringz.h"

int trim_z(std::string& s)
{
	if (s.empty())
	{
		return 0;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);

	return 1;
}

int ltrim_z(char* s)
{
	_strrev(s);
	rtrim_z(s);
	_strrev(s);
	return 1;
}

int rtrim_z(char* s)
{
	int s_len = strlen(s);
	int x = 0;
	for (int i = s_len - 1; i >= 0; i--)
	{
		if (i == s_len - 1 && s[i] == ' ')
		{
			s[i] = '\0';
			break;
		}
		x = 1;
		break;
	}
	if (x == 0)
		rtrim_z(s);
	return 1;
}

int trim_z(char* s)
{
	rtrim_z(s);
	ltrim_z(s);
	return 1;
}

std::vector<char*> split_z(char* pch, char* sp)
{
	pch = std::strtok(pch, sp);

	std::vector<char*> vec;
	int i = 0;
	while (pch != NULL)
	{
		ltrim_z(pch);
		vec.push_back(pch);
		pch = strtok(NULL, sp);
		i++;
	}
	return vec;
}

int rtrim_n_z(char* s)
{
	int s_len = strlen(s);
	int x = 0;
	for (int i = s_len - 1; i >= 0; i--)
	{
		if (i == s_len - 1 && s[i] == '\n')
		{
			s[i] = '\0';
			break;
		}
		x = 1;
		break;
	}
	if (x == 0)
		rtrim_n_z(s);
	return 1;
}

int ltrim_n_z(char* s)
{
	_strrev(s);
	int s_len = strlen(s);
	int x = 0;
	for (int i = s_len - 1; i >= 0; i--)
	{
		if (i == s_len - 1 && s[i] == '\n')
		{
			s[i] = '\0';
			break;
		}
		x = 1;
		break;
	}
	_strrev(s);
	if (x == 0)
		ltrim_n_z(s);
	return 1;
}

int trim_n_z(char* s)
{
	rtrim_n_z(s);
	ltrim_n_z(s);
	return 1;
}