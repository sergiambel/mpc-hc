#ifndef _EVENT_H
#define _EVENT_H

#include "functors.h"
#include "parameters.h"
#include <list>

using namespace std;

#define CREATE_DELEGATE CREATE_CALLBACK

#define CREATE_CALLBACK(className, function) \
	new EventTSpecificFunctor<className> (this, &className::function)

#define CREATE_REMOTE_CALLBACK(className, function, object) \
	new EventTSpecificFunctor<className> (object, &className::function)


//Redirects an event to anyone subscribed to 'eventName'
#define REDIRECT_EVENT(eventName, callbackName)			\
	public: Event				eventName;				\
	protected:											\
	void callbackName(void* from, const Parameters &p)	\
	{													\
		eventName(this, p);								\
	}													\
	public:												\

//Redirects an event to anyone subscribed to 'eventName'
//only if the content of the parameter [position] matches
//the value of 'filter'. It must be an INT32 value.
#define FILTER_INT32_EVENT(eventName, callbackName, position, filter )			\
	public: Event				eventName;										\
	protected:																	\
	void callbackName(void* from, const Parameters &p)							\
{																				\
	if( p.GetNumParameters() >= position &&										\
		p[position]->GetInt32Value() == (INT32) filter )						\
	{																			\
		eventName(this, p);														\
	}																			\
}																				\
	public:																		\



class EventTFunctor
{

public:

	//Returns the pointer of the class containing the function
	virtual void* GetPointerToObject() = 0;

	// two possible functions to call member function. virtual cause derived
	// classes will use a pointer to an object and a pointer to a member function
	// to make the function call
	virtual void operator()(void* sender, const Parameters &p)=0;  // call using operator
	virtual void Call(void* sender, const Parameters &p) = 0; // {return Parameter();}       // call using function
};

// derived template class
template <class TClass> class EventTSpecificFunctor : public EventTFunctor
{
private:
  void (TClass::*fpt)(void*, const Parameters &);   // pointer to member function
  TClass* pt2Object;						// pointer to object

public:
	
	// constructor - takes pointer to an object and pointer to a member and stores
	// them in two private variables
	EventTSpecificFunctor(TClass* _pt2Object, void(TClass::*_fpt)(void*, const Parameters &))
	{ 
		pt2Object = _pt2Object;  fpt=_fpt; 
	};

	//Destructor
	~EventTSpecificFunctor() 
	{
		pt2Object = 0;
	}

	//Returns the pointer of the class containing the function
	virtual void* GetPointerToObject() 
	{
		return (void*) pt2Object;
	}

	// override operator "()"
	virtual void operator()(void* sender, const Parameters &p)
	{ 
		return (*pt2Object.*fpt)(sender, p);
	};              // execute member function

	// override function "Call"
	void Call(void* sender, const Parameters &p)
	{ 
		return (*pt2Object.*fpt)(sender, p);
	};             // execute member function
  
};
 

typedef list<EventTFunctor*> FunctorList;

class Event
{
private:

	//List of functors subscribed to this event
	FunctorList			m_functors;
	//Critical section to manage the data structs of this event
	CRITICAL_SECTION	m_criticalSection;

public:
	
	//Constructor
	Event();
	//Destructor
	~Event();
	//Return the number of subscribers
	size_t			GetNumSubscribers() { return m_functors.size(); }
	//Adds a function as a listener of this event
	void			Subscribe(EventTFunctor* functor);
	//Removes all the listeners from a specific class
	void			Unsubscribe(void* from);
	//Execute a call to all the listeners
	void			Execute(void *from, const Parameters &params);
	//Execute a call to all the listeners
	void			operator()(void* from, const Parameters &params)
	{
		Execute(from, params);
	}
};

#endif