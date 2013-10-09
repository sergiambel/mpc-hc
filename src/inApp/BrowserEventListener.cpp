#include "StdAfx.h"
#include "BrowserEventListener.h"

#include "exdispid.h"


BrowserEventListener::BrowserEventListener(void) :
	m_cRef( 0 )
{
	m_extension.registerAPI( &m_api );
}


BrowserEventListener::~BrowserEventListener(void)
{
	ATLTRACE( "Inside BrowserEventListener::~BrowserEventListener\n" );
}


WindowsAPI* BrowserEventListener::getWindowsAPI()
{
	ATLTRACE( "Inside BrowserEventListener::getWindowsAPI()\n" );
	return &m_api;
}


STDMETHODIMP BrowserEventListener::QueryInterface(REFIID riid, void **ppv)
{
	ATLTRACE( "Inside BrowserEventListener::QueryInterface\n" );
	*ppv = NULL;
	if ( IID_IDocHostUIHandler == riid )
	{
		*ppv = this;
	}
	else if  ( IID_IOleCommandTarget == riid )
	{
		*ppv = &m_cmdTarget;
	}


	if ( NULL != *ppv )
	{
		ATLTRACE( "Inside BrowserEventListener::QueryInterface -> S_OK\n" );
		((LPUNKNOWN)*ppv)->AddRef();
		return S_OK;
	}

	ATLTRACE( "Inside BrowserEventListener::QueryInterface -> E_NOINTERFACE\n" );
	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) BrowserEventListener::AddRef(void)
{
	ATLTRACE( "Inside BrowserEventListener::AddRef(void)\n" );
	return 1;
}


STDMETHODIMP_(ULONG) BrowserEventListener::Release(void)
{
	ATLTRACE( "Inside BrowserEventListener::Release(void)\n" );
	return 1;
}	


STDMETHODIMP BrowserEventListener::ShowContextMenu( DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit )
{
	ATLTRACE( "Inside BrowserEventListener::ShowContextMenu\n" );
	return ( dwID == CONTEXT_MENU_CONTROL ) ? E_NOTIMPL : S_OK;
}


STDMETHODIMP BrowserEventListener::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	ATLTRACE( "Inside BrowserEventListener::GetHostInfo\n" );
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	return S_OK;
}


STDMETHODIMP BrowserEventListener::ShowUI( DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc )
{
	ATLTRACE( "Inside BrowserEventListener::ShowUI\n" );
	return S_OK;
}


STDMETHODIMP BrowserEventListener::HideUI(void)
{
	ATLTRACE( "Inside BrowserEventListener::HideUI\n" );
	return S_OK;
}


STDMETHODIMP BrowserEventListener::UpdateUI(void)
{
	ATLTRACE( "Inside BrowserEventListener::UpdateUI\n" );
	return S_OK;
}


STDMETHODIMP BrowserEventListener::EnableModeless( BOOL fEnable )
{
	ATLTRACE( "Inside BrowserEventListener::EnableModeless\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserEventListener::OnDocWindowActivate( BOOL fActivate )
{
	ATLTRACE( "Inside BrowserEventListener::OnDocWindowActivate\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserEventListener::OnFrameWindowActivate( BOOL fActivate )
{
	ATLTRACE( "Inside BrowserEventListener::OnFrameWindowActivate\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserEventListener::ResizeBorder( LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fRameWindow )
{
	ATLTRACE( "Inside BrowserEventListener::ResizeBorder\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserEventListener::TranslateAccelerator( LPMSG lpMsg, const GUID __RPC_FAR *pguidCmdGroup, DWORD nCmdID )
{
	ATLTRACE( "Inside BrowserEventListener::TranslateAccelerator\n" );

	//disable F5
	if(lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(VK_F5) < 0)
		return S_OK;
	else if (lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(VK_F1) < 0)
		return S_OK;
	else if (lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(VK_F12) < 0)
		return S_OK;

	if(GetKeyState(VK_CONTROL) & 0x8000)
	{
		//disable ctrl + O
		if(lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(0x4F) < 0)
			return S_OK;
		//disable ctrl + P
		if(lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(0x50) < 0)
			return S_OK;
		//disable ctrl + N
		if(lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(0x4E) < 0)
			return S_OK;
		//disable ctrl + C
		if(lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(0x43) < 0)
			return S_OK;
		//disable ctrl + L
		if(lpMsg->message == WM_KEYDOWN && GetAsyncKeyState(0x4C) < 0)
			return S_OK;
	}

	//disable back space
	if(lpMsg->wParam == VK_BACK)
		return S_OK;
	else if(lpMsg->wParam == VK_DOWN)
		return S_OK;
	else if(lpMsg->wParam == VK_UP)
		return S_OK;
	else if(lpMsg->wParam == VK_LEFT)
		return S_OK;
	else if(lpMsg->wParam == VK_RIGHT)
		return S_OK;

	return S_FALSE;
}


STDMETHODIMP BrowserEventListener::GetOptionKeyPath( BSTR* pbstrKey, DWORD dw )
{
	ATLTRACE( "Inside BrowserEventListener::GetOptionKeyPath\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserEventListener::GetDropTarget( IDropTarget __RPC_FAR *pDropTarget, IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget )
{
	ATLTRACE( "Inside BrowserEventListener::GetDropTarget\n" );
	if ( ppDropTarget != NULL )
	{
		*ppDropTarget  = pDropTarget;
	}
	return S_OK;
}


STDMETHODIMP BrowserEventListener::GetExternal( IDispatch __RPC_FAR *__RPC_FAR *ppDispatch )
{
	ATLTRACE( "Inside BrowserEventListener::GetExternal\n" );
	m_extension.QueryInterface( IID_IDispatch, (LPVOID*) ppDispatch );
	return S_OK;
}


STDMETHODIMP BrowserEventListener::TranslateUrl( DWORD dwTranslate, OLECHAR __RPC_FAR *pchURLIn, OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut )
{
	ATLTRACE( "Inside BrowserEventListener::TranslateUrl\n" );
	return S_OK;
}


STDMETHODIMP BrowserEventListener::FilterDataObject( IDataObject __RPC_FAR *pDO, IDataObject __RPC_FAR *__RPC_FAR *ppDORet )
{
	ATLTRACE( "Inside BrowserEventListener::FilterDataObject\n" );
	return E_NOTIMPL;
}

/*
STDMETHODIMP BrowserEventListener::QueryStatus( const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[  ], OLECMDTEXT *pCmdText )
{
	ATLTRACE( "Inside BrowserEventListener::QueryStatus\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserEventListener::Exec( const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut )
{
	ATLTRACE( "Inside BrowserEventListener::Exec\n" );
	return E_NOTIMPL;
}
*/