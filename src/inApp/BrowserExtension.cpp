#include "StdAfx.h"
#include "BrowserExtension.h"
#include "VariantUtils.h"

#include <algorithm>
#include <map>

using namespace std;

BrowserExtension::BrowserExtension(void) :
	m_cRef( 0 ),
	m_pAPI( NULL )
{
}


BrowserExtension::~BrowserExtension(void)
{
	ATLTRACE( "Inside BrowserExtension::~BrowserExtension\n" );
}


void BrowserExtension::registerAPI( BaseAPI* pAPI )
{
	map< wstring, TFunctor* >				functions;
	functions = pAPI->GetFunctors();

	for_each( functions.begin(), functions.end(), [this](pair< wstring, TFunctor* > item) {
		addFunction( item.first, item.second );
	});
}


STDMETHODIMP BrowserExtension::QueryInterface( REFIID riid, void **ppv )
{
	ATLTRACE( "Inside BrowserExtension::QueryInterface\n" );
	*ppv = NULL;
	if ( IID_IDispatch == riid )
	{
		*ppv = this;
	}

	if ( NULL != *ppv )
	{
		((LPUNKNOWN)*ppv)->AddRef();
		ATLTRACE( "Inside BrowserExtension::QueryInterface -> S_OK\n" );
		return S_OK;
	}
	ATLTRACE( "Inside BrowserExtension::QueryInterface -> E_NOINTERFACE\n" );
	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) BrowserExtension::AddRef( void )
{
	ATLTRACE( "Inside BrowserExtension::AddRef\n" );
	return ++m_cRef;
}


STDMETHODIMP_(ULONG) BrowserExtension::Release( void )
{
	ATLTRACE( "Inside BrowserExtension::Release\n" );
	return --m_cRef;
}


STDMETHODIMP BrowserExtension::GetTypeInfoCount( UINT* pctinfo )
{
	ATLTRACE( "Inside BrowserExtension::GetTypeInfoCount\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserExtension::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo )
{
	ATLTRACE( "Inside BrowserExtension::GetTypeInfo\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserExtension::GetIDsOfNames( REFIID riid, OLECHAR** rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId )
{
	ATLTRACE( "Inside BrowserExtension::GetIDsOfNames\n" );
	HRESULT hr = S_OK;
	for ( UINT idx = 0; idx < cNames; ++idx )
	{
		if ( existFunction( rgszNames[ idx ] ) )
		{
			rgDispId[ idx ] = getFunctionId( rgszNames[ idx ] );
		}
		else 
		{
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[ idx ] = DISPID_UNKNOWN;
		}
	}
	return hr;
}


STDMETHODIMP BrowserExtension::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr )
{
	ATLTRACE( "Inside BrowserExtension::Invoke\n" );
	// Build the array of parameters
	Parameters		params;
	int				numArgs = pDispParams->cArgs;
	for (int i =0; i<numArgs; i++)
	{
		Parameter p = VariantUtils::BuildParameter(&pDispParams->rgvarg[i]);
		params.AddParameter(p);
	}

	// Obtain the functor object
	TFunctor*		functor = getFunction( dispIdMember );
	if ( functor == NULL )
	{
		return E_NOTIMPL;
	}

	// Perform the call and obtain the result value
	Parameter result = functor->Call(this, params);

	// Resend the result back
	if ( ( pVarResult != NULL ) && ( result.GetDataType() != PARAM_TYPE_NONE ) )
	{
		VariantUtils::CreateVariant( pVarResult, &result );
	}
	return S_OK;
}


bool BrowserExtension::addFunction( const wstring &name, TFunctor* fPointer )
{
	if ( !existFunction( name ) )
	{
		int		idFunction = (int) m_idFunctions.size() + 1;
		m_idFunctions[ name ] = idFunction;
		m_namesFunctions[ idFunction ] = name;
		m_functors[ idFunction ] = fPointer;
		return true;
	}
	return false;
}


bool BrowserExtension::existFunction( const wstring& name )
{
	return m_idFunctions.find( name ) != m_idFunctions.end() ? true : false;
}


int BrowserExtension::getFunctionId( const wstring& name )
{
	return existFunction( name ) ? m_idFunctions[ name ] : -1;
}


TFunctor* BrowserExtension::getFunction( int id )
{
	auto iter = m_functors.find( id );
	return ( iter != m_functors.end() ) ? iter->second : 0;
}
