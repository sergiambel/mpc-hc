#include "parameters.h"

Parameters::Parameters(int num)
{
	AddParameters(num);
}

//Constructor de copia
Parameters::Parameters(const Parameters& p)
{
	int numParams = (int) p.GetNumParameters();
	AddParameters(numParams);
	for(int i=0; i<numParams;i++)
	{
		m_parameters[i] = *p.GetParameter(i);
	}
}


Parameters& Parameters::operator=(const Parameters& p)
{
	m_parameters.clear();
	int numParams = (int) p.GetNumParameters();
	AddParameters(numParams);
	for(int i=0;i<numParams;i++)
	{
		m_parameters[i] = Parameter(*p.GetParameter(i));
	}
	return *this;
}



int Parameters::GetNumParameters() const
{
	return (int) m_parameters.size();
}

int Parameters::AddParameters(int params)
{
	for(int i = 0; i<params; i++)
	{
		AddParameter(Parameter());
	}
	return 0;
}

bool Parameters::AddParameter(const Parameter &param)
{
	m_parameters.insert(m_parameters.begin(), param);
	return true;
}

bool Parameters::AddParameterAtEnd(const Parameter &param)
{
	m_parameters.push_back(param);
	return true;
}

Parameter* Parameters::GetParameter(unsigned int position) const
{
	if(position < 0 || position >= m_parameters.size()) return 0;
	return (Parameter*)&m_parameters[position];
}

bool Parameters::RemoveParameter(unsigned int position)
{
	size_t pos = 0;
	if(position < 0 || position >= m_parameters.size()) return false;
	ParameterArray::iterator it = m_parameters.begin();

	for(it = m_parameters.begin() ; !(it == m_parameters.end()); it++)
	{ 
		pos = it - m_parameters.begin();
		if(pos ==  position)
		{
			m_parameters.erase(it);
			return true;
		}
	}
	return false;
}

void Parameters::Clear()
{
	m_parameters.clear();
}

void Parameters::Dump()
{
	for(unsigned int i=0;i<m_parameters.size(); i++)
	{
		m_parameters[i].Dump();
	}
}

wstring Parameters::DumpToString()
{
	wstring s = L"";
	
	for(unsigned int i=0;i<m_parameters.size(); i++)
	{
		if(i!=0)
		{
			s+= L", ";
		}
		s += m_parameters[i].DumpToString();
	}
	return s;
}

bool Parameters::operator ==(const Parameters& b) const
{
	//Check if the number of parameters is the same
	if(b.GetNumParameters() != m_parameters.size())
	{
		return false;
	}
	//Check each parameter and see if it has the same value
	for(unsigned int i=0; i<m_parameters.size(); i++)
	{
		if(*GetParameter(i) != *b.GetParameter(i))
		{
			return false;
		}
	}
	return true;
}