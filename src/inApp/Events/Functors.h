#pragma once
#include "Parameters.h"

#define ADD_FUNCTOR(desc, className, function) \
	m_functors[desc] = new TSpecificFunctor<className> (this, &className::function);


class TFunctor
   {
   protected:
	   int m_numParams;

   public:

	   
	
      // two possible functions to call member function. virtual cause derived
      // classes will use a pointer to an object and a pointer to a member function
      // to make the function call
       virtual Parameter operator()(void* sender, const Parameters &p)=0;		// call using operator
	   virtual Parameter Call(void* sender, const Parameters &p) = 0;			// call using function
   };

   // derived template class
template <class TClass> class TSpecificFunctor : public TFunctor
{
private:
	Parameter (TClass::*fpt)(void*, const Parameters &);   // pointer to member function
	TClass* pt2Object;                  // pointer to object
public:

	// constructor - takes pointer to an object and pointer to a member and stores
	// them in two private variables
	TSpecificFunctor(TClass* _pt2Object, Parameter(TClass::*_fpt)(void*, const Parameters &))
	{ 
		pt2Object = _pt2Object;  
		fpt=_fpt; 
	};

	~TSpecificFunctor() {}
	// override operator "()"
	virtual Parameter operator()(void* sender, const Parameters &p)
	{ return (*pt2Object.*fpt)(sender, p);};              // execute member function

	// override function "Call"
	virtual Parameter Call(void* sender, const Parameters &p)
	{ return (*pt2Object.*fpt)(sender, p);};             // execute member function
	};
