#pragma once

#include "Events/BaseAPI.h"

#include <map>
#include <string>

class BrowserExtension : public IDispatch
{
public:
									BrowserExtension(void);
	virtual							~BrowserExtension(void);

	void							registerAPI( BaseAPI* pAPI );

	// IUnknown interface methods
	STDMETHODIMP					QueryInterface( REFIID riid,void ** ppvObject );
	STDMETHODIMP_(ULONG)			AddRef( void );
	STDMETHODIMP_(ULONG)			Release( void );

	// IDispatch interface methods
	STDMETHODIMP					GetTypeInfoCount( UINT* pctinfo );
	STDMETHODIMP					GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo );
	STDMETHODIMP					GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId );
	STDMETHODIMP					Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr );

protected:
	bool							addFunction( const wstring& name, TFunctor* fPointer );
	bool							existFunction( const wstring& name );
	int								getFunctionId( const wstring& name );
	TFunctor*						getFunction( int id );

	ULONG							m_cRef;
	BaseAPI							*m_pAPI;

	std::map< std::wstring, int >	m_idFunctions;
	std::map< int, std::wstring >	m_namesFunctions;
	std::map< int, TFunctor* >		m_functors;
};

