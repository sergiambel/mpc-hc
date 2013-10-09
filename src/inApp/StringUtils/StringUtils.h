#pragma once

#pragma comment(lib, "comsupp.lib")

#include <algorithm>
#include <vector>
#include <string>
#include <comutil.h>
#include <time.h>
#include <atlbase.h>

using namespace std;

class StringUtils
{
private:
	
	
public:
	static wstring	StringToWString(const std::string& s);
	static string	WStringToString(const std::wstring& s);
};
