#include "event.h"
#include <algorithm>

Event::Event()
{
	InitializeCriticalSection(&m_criticalSection);
}

Event::~Event()
{
	wstring name = L"";
	EnterCriticalSection(&m_criticalSection);
	FunctorList::iterator it;
	for(it=m_functors.begin(); it!=m_functors.end(); it++)
	{
		if(*it)
		{
			EventTFunctor* func = *it;
			delete func;
			*it = 0;
		}
	}
	m_functors.clear();
	LeaveCriticalSection(&m_criticalSection);
	DeleteCriticalSection(&m_criticalSection);
}

void Event::Subscribe(EventTFunctor* func)
{
	EnterCriticalSection(&m_criticalSection);
	m_functors.push_back(func);
	LeaveCriticalSection(&m_criticalSection);
}



void Event::Unsubscribe(void* from)
{
	EnterCriticalSection(&m_criticalSection);
	if (!m_functors.empty())
	{
		bool exit = false;
		while(!exit)
		{
			exit = true;
			FunctorList::iterator it;
			for(it=m_functors.begin(); it!=m_functors.end(); it++)
			{
				EventTFunctor* func = *it;
				if(func->GetPointerToObject() == from)
				{
					//erase this one, and start again
					delete func;
					m_functors.erase(it);
					exit = false;
					break;
				}
			}
		}
	}
	LeaveCriticalSection(&m_criticalSection);
}

void Event::Execute(void* from, const Parameters &p)
{
	EnterCriticalSection(&m_criticalSection);
	if (!m_functors.empty())
	{
		FunctorList::iterator it;
		for(it=m_functors.begin(); it!=m_functors.end(); it++)
		{
			EventTFunctor* func = *it;
			func->Call(from, p);
		}
	}
	LeaveCriticalSection(&m_criticalSection);
}