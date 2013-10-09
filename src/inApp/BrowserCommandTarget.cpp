#include "StdAfx.h"
#include "BrowserCommandTarget.h"

#include <comutil.h>
#include <string>

using namespace std;


BrowserCommandTarget::BrowserCommandTarget(void)
{
}


BrowserCommandTarget::~BrowserCommandTarget(void)
{
}


STDMETHODIMP BrowserCommandTarget::QueryInterface(REFIID riid, void **ppv)
{
	ATLTRACE( "Inside BrowserCommandTarget::QueryInterface\n" );
	*ppv = NULL;
	if ( ( IID_IOleCommandTarget == riid ) )
	{
		*ppv = this;
	}

	if ( NULL != *ppv )
	{
		ATLTRACE( "Inside BrowserCommandTarget::QueryInterface -> S_OK\n" );
		((LPUNKNOWN)*ppv)->AddRef();
		return S_OK;
	}

	ATLTRACE( "Inside BrowserCommandTarget::QueryInterface -> E_NOINTERFACE\n" );
	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) BrowserCommandTarget::AddRef(void)
{
	ATLTRACE( "Inside BrowserCommandTarget::AddRef(void)\n" );
	return 1;
}


STDMETHODIMP_(ULONG) BrowserCommandTarget::Release(void)
{
	ATLTRACE( "Inside BrowserCommandTarget::Release(void)\n" );
	return 1;
}	


STDMETHODIMP BrowserCommandTarget::QueryStatus( const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[  ], OLECMDTEXT *pCmdText )
{
	ATLTRACE( "Inside BrowserCommandTarget::QueryStatus\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserCommandTarget::Exec( const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut )
{
	ATLTRACE( "Inside BrowserCommandTarget::Exec\n" );

	// The only thing that should be done here is just avoid displaying script error warnings
	HRESULT		hr = OLECMDERR_E_NOTSUPPORTED;
	if ( ( pguidCmdGroup != NULL ) && IsEqualGUID( *pguidCmdGroup, CGID_DocHostCommandHandler ) && ( nCmdID == OLECMDID_SHOWSCRIPTERROR ) )
	{
		// Continue running scripts on the page.
		pvaOut->vt = VT_BOOL;
		pvaOut->boolVal = VARIANT_TRUE;
		hr = S_OK;
	}

	return (hr);
}
