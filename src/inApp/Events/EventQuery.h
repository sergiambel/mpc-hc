#ifndef _EVENT_QUERY_H
#define _EVENT_QUERY_H

#include "functors.h"
#include "parameters.h"
#include <list>
#include <map>

using namespace std;

#define CREATE_QUERY_CALLBACK(answerId, className, function) \
	new EventQueryTSpecificFunctor<className> (answerId, this, &className::function)

#define CREATE_QUERY_REMOTE_CALLBACK(answerId, className, function, object) \
	new EventQueryTSpecificFunctor<className> (answerId, object, &className::function)

#define REDIRECT_QUERY_EVENT(eventName, callbackName)			\
	public: EventQuery			eventName;						\
	protected:													\
	Parameter callbackName(void* from, const Parameters &p)		\
{																\
	eventName(this, p);											\
}																\
	public:														\

class EventQueryTFunctor
{
protected:
	wstring	m_functorName;
public:

	//Returns the pointer of the class containing the function
	virtual void* GetPointerToObject() = 0;

	// two possible functions to call member function. virtual cause derived
	// classes will use a pointer to an object and a pointer to a member function
	// to make the function call
	virtual Parameter operator()(void* sender, const Parameters &p)=0;  // call using operator
	virtual Parameter Call(void* sender, const Parameters &p) = 0; // {return Parameter();}       // call using function
	virtual wstring   GetFunctorName() = 0;
};

// derived template class
template <class TClass> class EventQueryTSpecificFunctor : public EventQueryTFunctor
{
private:
	Parameter (TClass::*m_fpt)(void*, const Parameters &);		// pointer to member function
	TClass*		m_pt2Object;									// pointer to object

public:

	// constructor - takes pointer to an object and pointer to a member and stores
	// them in two private variables
	EventQueryTSpecificFunctor(wstring name, TClass* _pt2Object, Parameter(TClass::*_fpt)(void*, const Parameters &))
	{ 
		m_pt2Object = _pt2Object;  
		m_fpt=_fpt; 
		m_functorName = name;
	};

	//Destructor
	~EventQueryTSpecificFunctor() 
	{
		m_pt2Object = 0;
	}

	//Returns the pointer of the class containing the function
	virtual void* GetPointerToObject() 
	{
		return (void*) m_pt2Object;
	}

	// override operator "()"
	virtual Parameter operator()(void* sender, const Parameters &p)
	{ 
		return (*m_pt2Object.*m_fpt)(sender, p); // execute member function
	};              

	// override function "Call"
	Parameter Call(void* sender, const Parameters &p)
	{ 
		return (*m_pt2Object.*m_fpt)(sender, p);  // execute member function
	};            

	inline wstring GetFunctorName()
	{
		return m_functorName;
	}
};


typedef list<EventQueryTFunctor*> FunctorQueryList;
typedef map<wstring, Parameter> EventQueryAnswers;

class EventQuery
{
private:

	//List of functors subscribed to this event
	FunctorQueryList			m_functors;
	//Critical section to manage the data structs of this event
	CRITICAL_SECTION	m_criticalSection;

public:

	//Constructor
	EventQuery();
	//Destructor
	~EventQuery();

	//Adds a function as a listener of this event
	void			Subscribe(EventQueryTFunctor* functor);
	//Removes all the listeners from a specific class
	void			Unsubscribe(void* from);
	//Execute a call to all the listeners
	EventQueryAnswers Execute(void *from, const Parameters &params);
	//Execute a call to all the listeners
	EventQueryAnswers operator()(void* from, const Parameters &params)
	{
		return Execute(from, params);
	}
};

#endif