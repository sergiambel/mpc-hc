#include "eventQuery.h"
#include <algorithm>

EventQuery::EventQuery()
{
	InitializeCriticalSection(&m_criticalSection);
}

EventQuery::~EventQuery()
{
	EnterCriticalSection(&m_criticalSection);
	FunctorQueryList::iterator it;
	for(it=m_functors.begin(); it!=m_functors.end(); it++)
	{
		if(*it)
		{
			EventQueryTFunctor* func = *it;
			delete func;
			*it = 0;
		}
	}
	m_functors.clear();
	LeaveCriticalSection(&m_criticalSection);
	DeleteCriticalSection(&m_criticalSection);
}

void EventQuery::Subscribe(EventQueryTFunctor* func)
{
	EnterCriticalSection(&m_criticalSection);
	m_functors.push_back(func);
	LeaveCriticalSection(&m_criticalSection);
}



void EventQuery::Unsubscribe(void* from)
{
	EnterCriticalSection(&m_criticalSection);
	if (!m_functors.empty())
	{
		bool exit = false;
		while(!exit)
		{
			exit = true;
			FunctorQueryList::iterator it;
			for(it=m_functors.begin(); it!=m_functors.end(); it++)
			{
				EventQueryTFunctor* func = *it;
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

EventQueryAnswers EventQuery::Execute(void* from, const Parameters &p)
{
	EventQueryAnswers answers;
	EnterCriticalSection(&m_criticalSection);
	if (!m_functors.empty())
	{
		FunctorQueryList::iterator it;
		for(it=m_functors.begin(); it!=m_functors.end(); it++)
		{
			EventQueryTFunctor* func = *it;
			Parameter answer = func->Call(from, p);
			//Only assign the answer if there is an actual answer.
			if(answer.GetDataType() != PARAM_TYPE_NONE)
			{
				answers[func->GetFunctorName()] = answer;
			}
		}
	}
	LeaveCriticalSection(&m_criticalSection);
	return answers;
}