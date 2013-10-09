#pragma once

#include <mshtmhst.h>


class BrowserCommandTarget : public IOleCommandTarget
{
public:
								BrowserCommandTarget(void);
	virtual						~BrowserCommandTarget(void);

	// IUnknown interface methods
	STDMETHODIMP				QueryInterface( REFIID riid,void ** ppvObject );
	STDMETHODIMP_(ULONG)		AddRef( void );
	STDMETHODIMP_(ULONG)		Release( void );

	// IOleCommandTarget interface methods
	STDMETHODIMP				QueryStatus( const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[  ], OLECMDTEXT *pCmdText );
	STDMETHODIMP				Exec( const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut );
};

