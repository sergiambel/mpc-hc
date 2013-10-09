#pragma once

#include <string>
#include <map>
#include "functors.h"

using namespace std;

class BaseAPI
{
protected:
	map<wstring, TFunctor*>			m_functors;			//Estructura de datos que contiene los
														//punteros a funciones. La key es 
														//una wstring con el nombre de la función.
	bool							m_functorsCreated;	//Cierto si se han registrado las funciones
														//en m_functors.

public:
	
	
	//Constructores / Destructores
	BaseAPI() { m_functorsCreated = false;}
	virtual ~BaseAPI()
	{ 
		map<wstring, TFunctor*>::iterator it;
		for(it = m_functors.begin(); it != m_functors.end(); it++)
		{
			delete(it->second);
			it->second = 0;
		}
		m_functors.clear();
		m_functorsCreated = false;
	}
	
	virtual void	CreateFunctors() = 0;

	map<wstring, TFunctor*> GetFunctors()
	{
		if(!m_functorsCreated)
		{
			CreateFunctors();
			m_functorsCreated = !m_functors.empty();
		}
		return m_functors;
	}
	
	Parameter ExecuteFunction(const wstring &functionName, const Parameters &p)
	{
		TFunctor* f = GetFunction(functionName);
		if(f)
		{
			return f->Call(0, p);
		}
		else
		{
			return Parameter();
		}
	}

	TFunctor* GetFunction(const wstring &functionName)
	{
		if(m_functors.find(functionName) != m_functors.end())
		{
			return m_functors[functionName];
		}
		else return 0;
	}
		
};