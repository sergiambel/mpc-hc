#pragma once

#include <mshtmhst.h>

#include "WindowsAPI.h"
#include "BrowserCommandTarget.h"
#include "BrowserExtension.h"

class BrowserEventListener : public IDocHostUIHandler
{
public:
								BrowserEventListener(void);
	virtual						~BrowserEventListener(void);

	WindowsAPI*					getWindowsAPI();

	// IUnknown interface methods
	STDMETHODIMP				QueryInterface( REFIID riid,void ** ppvObject );
	STDMETHODIMP_(ULONG)		AddRef( void );
	STDMETHODIMP_(ULONG)		Release( void );

	// IDocHostUIHandler interface methods
	STDMETHODIMP				ShowContextMenu( DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved );
	STDMETHODIMP				GetHostInfo( DOCHOSTUIINFO *pInfo );
	STDMETHODIMP				ShowUI( DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc );
	STDMETHODIMP				HideUI( void );
	STDMETHODIMP				UpdateUI( void );
	STDMETHODIMP				EnableModeless( BOOL fEnable );
	STDMETHODIMP				OnDocWindowActivate( BOOL fActivate );
	STDMETHODIMP				OnFrameWindowActivate( BOOL fActivate );
	STDMETHODIMP				ResizeBorder( LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow );
	STDMETHODIMP				TranslateAccelerator( LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID );
	STDMETHODIMP				GetOptionKeyPath( LPOLESTR *pchKey, DWORD dw );
	STDMETHODIMP				GetDropTarget( IDropTarget *pDropTarget, IDropTarget **ppDropTarget );
	STDMETHODIMP				GetExternal( IDispatch **ppDispatch );
	STDMETHODIMP				TranslateUrl( DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut );
	STDMETHODIMP				FilterDataObject( IDataObject *pDO, IDataObject **ppDORet );

protected:
	ULONG						m_cRef;
	WindowsAPI					m_api;
	BrowserExtension			m_extension;
	BrowserCommandTarget		m_cmdTarget;
};

