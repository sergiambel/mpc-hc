#include <regex>
#include <sstream>
#include <ios>
#include <iomanip>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include "StringUtils.h"


wstring StringUtils::StringToWString(const std::string& s)
{
	std::wstring temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}


string StringUtils::WStringToString(const std::wstring& s)
{
	string ret(s.begin(), s.end());
	ret.assign(s.begin(), s.end());
	return ret;
}

