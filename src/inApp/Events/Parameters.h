#pragma once
#include <vector>
#include "parameter.h"
using namespace std;

typedef vector<Parameter> ParameterArray;

class Parameters
{
private:
	ParameterArray			m_parameters;
	

public:

	Parameters() {}
	Parameters(const Parameters& p);
	Parameters(int num);
	~Parameters() {}

	Parameter*			operator[](int position) const {return GetParameter(position);}
	Parameters&			operator=(const Parameters& p);
	bool				operator==(const Parameters& p) const;
	int					AddParameters(int numParams);

	bool				AddParameter(const Parameter &param);
	bool				AddParameterAtEnd(const Parameter &param);
	Parameter*			GetParameter(unsigned int position) const;
	int					GetNumParameters() const;
	bool				RemoveParameter(unsigned int position);
	void				Clear();
	void				Dump();
	wstring				DumpToString();
	
};
