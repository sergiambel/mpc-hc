#include "StdAfx.h"
#include "BrowserWnd.h"
#include "VariantUtils.h"

#include <atlbase.h>
#include <ExDispid.h>

#define BROWSER_CLASSNAME	L"sftBrowserClass"
#define NOTIMPLEMENTED		return E_NOTIMPL

using namespace ATL;

BrowserWnd::BrowserWnd( void )
{
	registerClass();
	m_pEventListener.reset( new BrowserEventListener() );
}


BrowserWnd::~BrowserWnd( void )
{
	ATLTRACE( "Destroy BrowserWnd!\n" );
	unregisterEventListener();
}


void BrowserWnd::setContentUrl( const std::wstring& contentUrl )
{
	m_contentUrl = contentUrl;
}


bool BrowserWnd::create( HWND hParent, RECT &rcWnd )
{
	OleInitialize( NULL );

	// Create the browser window.
	m_hWnd = CreateWindowEx( WS_EX_TOPMOST, BROWSER_CLASSNAME, NULL, WS_CHILD | WS_BORDER, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, hParent, 0, AfxGetInstanceHandle(), 0);
	if ( !m_hWnd )
	{
		DWORD err = ::GetLastError();
		return false;
	}

	// Store the link between the HWND and BrowserWnd*
	attachWindow();

	if ( createBrowser() && registerEventListener() && disableDragAndDrop() )
	{
		// Navigate to the starting url
		CComBSTR	url = m_contentUrl.c_str();
		m_pWebBrowser->Navigate( url, NULL, NULL, NULL, NULL );
	}
	return true;
}


WindowsAPI* BrowserWnd::getWindowsAPI()
{
	return m_pEventListener->getWindowsAPI();
}


Parameter BrowserWnd::callJSFunction( const wstring &name, const Parameters &params )
{
	// Obtains the current document Javascript.
	Parameter				result;
	CComPtr< IDispatch >	pJS;
	if ( !getJavascript( pJS ) )
	{
		return result;
	}

	// Obtain the ID of the requested function name.
	DISPID					dispid = getJSFunctionId( pJS, name );
	if ( dispid == -1 )
	{
		return result;
	}

	// Prepare the function parameters
	DISPPARAMS				dispparams =  { 0 };
	prepareJSFunctionParams( params, dispparams );

	// Init the rest of required elements (exception context, result value, error value).
	EXCEPINFO				excepInfo = { 0 };
	UINT					nArgErr = (UINT)-1;
	CComVariant				vaResult;

	// Call the JS function.
	HRESULT		hr;
	hr = pJS->Invoke( dispid, IID_NULL, 0, DISPATCH_METHOD, &dispparams, &vaResult, &excepInfo, &nArgErr );
	if ( SUCCEEDED( hr ) )
	{
		VariantUtils::BuildParameter( &result, &vaResult );
	}

	return result;
}


void BrowserWnd::quit()
{
	if ( m_pWebBrowser != NULL )
	{
		m_pWebBrowser->Quit();
	}
}



void BrowserWnd::registerClass()
{
	WNDCLASSEX	wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;;
	wc.lpfnWndProc   = staticWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = AfxGetInstanceHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = BROWSER_CLASSNAME;
	wc.hIconSm       = LoadIcon(NULL, NULL);
	::RegisterClassEx( &wc );
}


bool BrowserWnd::createBrowser()
{
	// Create the WebBrowser object
	HRESULT		hr;
	hr = CoCreateInstance( CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (LPVOID*) &m_pWebBrowser );
	if ( FAILED( hr ) )
	{
		return false;
	}

	// Query WebBrowser for IOleObject pointer
	CComPtr< IOleObject >  pOleObject;
	hr = m_pWebBrowser->QueryInterface( IID_IOleObject, (LPVOID*) &pOleObject );
	if ( FAILED( hr ) )
	{
		return false;
	}

	// Set client site
	hr = pOleObject->SetClientSite( this );
	if ( FAILED( hr ) )
	{
		return false;
	}

	// In-place activate the WebBrowser control
	RECT rcClient;
	GetClientRect( m_hWnd, &rcClient );
	hr = pOleObject->DoVerb( OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_hWnd, &rcClient );
	if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}


bool BrowserWnd::registerEventListener()
{
	// Register container to intercept WebBrowser events
	HRESULT									hr;
	CComPtr< IUnknown >						pHandler = NULL;
	CComPtr< IConnectionPointContainer >	pCPC = NULL;
	CComPtr< IConnectionPoint >				pCP = NULL;
	hr = QueryInterface( IID_IUnknown, (LPVOID*) &pHandler );
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = m_pWebBrowser->QueryInterface( IID_IConnectionPointContainer, (LPVOID*) &pCPC);
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = pCPC->FindConnectionPoint( DIID_DWebBrowserEvents2, &pCP );
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = pCP->Advise( pHandler, &m_dwCookie );
	if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}


bool BrowserWnd::unregisterEventListener()
{
	// Register container to intercept WebBrowser events
	HRESULT									hr;
	CComPtr< IConnectionPointContainer >	pCPC = NULL;
	CComPtr< IConnectionPoint >				pCP = NULL;
	hr = m_pWebBrowser->QueryInterface( IID_IConnectionPointContainer, (LPVOID*) &pCPC);
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = pCPC->FindConnectionPoint( DIID_DWebBrowserEvents2, &pCP );
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = pCP->Unadvise( m_dwCookie );
	if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}


bool BrowserWnd::disableDragAndDrop()
{
	return SUCCEEDED( m_pWebBrowser->put_RegisterAsDropTarget( VARIANT_FALSE ) );
}


bool BrowserWnd::getDocumnent( CComPtr< IHTMLDocument2 >& pDocument )
{
	CComPtr< IDispatch >	pDispatch;
	HRESULT					hr;
	hr = m_pWebBrowser->get_Document( &pDispatch );
	if ( FAILED( hr ) )
	{
		return false;
	}
	pDocument = pDispatch;
	return ( pDocument != NULL )? true : false;
}


bool BrowserWnd::getJavascript( CComPtr< IDispatch >& pDispatch )
{
	// Obtains the current document
	CComPtr< IHTMLDocument2 >	pHTMLDocument;
	if ( !getDocumnent( pHTMLDocument ) )
	{
		return false;
	}

	// Obtains the current script from the document
	return SUCCEEDED( pHTMLDocument->get_Script( &pDispatch ) ) ? true : false;
}


DISPID BrowserWnd::getJSFunctionId( CComPtr< IDispatch >& pJS, const wstring &name )
{
	CComBSTR				bstrMember( name.c_str() );
	DISPID					dispid = 0;
	return SUCCEEDED( pJS->GetIDsOfNames( IID_NULL, &bstrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid ) ) ? dispid : -1;
}


void BrowserWnd::prepareJSFunctionParams( const Parameters &params, DISPPARAMS &dispparams )
{
	// Does the function call contains parameters
	dispparams.cArgs = params.GetNumParameters();
	if ( params.GetNumParameters() != 0 )
	{
		// Create an array of VARIANTS, one for each parameter and initialize them.
		dispparams.rgvarg = new VARIANT[ dispparams.cArgs ];
		for( unsigned int i = 0; i < dispparams.cArgs; i++ )
		{
			VariantUtils::CreateVariant( &dispparams.rgvarg[i], params.GetParameter( i ) );
		}
	}
}


LRESULT BrowserWnd::wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return ::DefWindowProc( hWnd, msg, wParam, lParam );
}


LRESULT CALLBACK BrowserWnd::staticWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	BrowserWnd		*pBrowserWnd = findWindow( hWnd );
	return pBrowserWnd != NULL ? pBrowserWnd->wndProc( hWnd, msg, wParam, lParam ) : ::DefWindowProc( hWnd, msg, wParam, lParam );
}


// IUnknown interface methods
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP BrowserWnd::QueryInterface( REFIID riid,void ** ppvObject )
{
	ATLTRACE( "Inside BrowserWnd::QueryInterface\n" );
	*ppvObject = NULL;
	if ( ( riid == IID_IUnknown ) || ( riid == IID_IOleClientSite ) )
	{
		*ppvObject = ( IOleClientSite* ) this;
	}
	else if ( riid == IID_IOleInPlaceSite )
	{
		*ppvObject = ( IOleInPlaceSite* ) this;
	}
	else if ( riid == IID_IOleInPlaceFrame )
	{
		*ppvObject = ( IOleInPlaceFrame* ) this;
	}
	else if ( riid == IID_IStorage )
	{
		*ppvObject = ( IStorage* ) this;
	}
	else if ( riid == IID_IDocHostUIHandler )
	{
		return m_pEventListener->QueryInterface( riid, ppvObject );
	}
	else if ( riid == DIID_DWebBrowserEvents2 )
	{
		*ppvObject = ( DWebBrowserEvents2* ) this;
	}
	else
	{
		ATLTRACE( "Inside BrowserWnd::QueryInterface -> E_NOINTERFACE\n" );
		return E_NOINTERFACE;
	}

	((LPUNKNOWN)*ppvObject)->AddRef();
	ATLTRACE( "Inside BrowserWnd::QueryInterface -> S_OK\n" );
	return S_OK;
}


STDMETHODIMP_(ULONG) BrowserWnd::AddRef( void )
{
	ATLTRACE( "Inside BrowserWnd::AddRef\n" );
	return 1;
}


STDMETHODIMP_(ULONG) BrowserWnd::Release( void )
{
	ATLTRACE( "Inside BrowserWnd::Release\n" );
	return 1;
}


// IOleClientSite
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP BrowserWnd::SaveObject()
{
	ATLTRACE( "Inside BrowserWnd::SaveObject\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::GetMoniker( DWORD dwAssign,DWORD dwWhichMoniker,IMoniker ** ppmk )
{
	ATLTRACE( "Inside BrowserWnd::GetMoniker\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::GetContainer( LPOLECONTAINER FAR* ppContainer )
{
	ATLTRACE( "Inside BrowserWnd::GetContainer\n" );
	// We are a simple object and don't support a container.
	*ppContainer = NULL;
	return E_NOINTERFACE;
}


STDMETHODIMP BrowserWnd::ShowObject()
{
	ATLTRACE( "Inside BrowserWnd::ShowObject\n" );
	return NOERROR;
}


STDMETHODIMP BrowserWnd::OnShowWindow( BOOL fShow )
{
	ATLTRACE( "Inside BrowserWnd::OnShowWindow\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::RequestNewObjectLayout()
{
	ATLTRACE( "Inside BrowserWnd::RequestNewObjectLayout\n" );
	NOTIMPLEMENTED;
}


// IOleWindow interface methods
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP BrowserWnd::GetWindow( HWND FAR* lphwnd )
{
	ATLTRACE( "Inside BrowserWnd::GetWindow\n" );
	*lphwnd = getHWND();
	return S_OK;
}


STDMETHODIMP BrowserWnd::ContextSensitiveHelp( BOOL fEnterMode )
{
	ATLTRACE( "Inside BrowserWnd::ContextSensitiveHelp\n" );
	NOTIMPLEMENTED;
}


// IOleInPlaceSite methods interface methods
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP BrowserWnd::CanInPlaceActivate()
{
	ATLTRACE( "Inside BrowserWnd::CanInPlaceActivate\n" );
	// Yes we can
	return S_OK;
}


STDMETHODIMP BrowserWnd::OnInPlaceActivate()
{
	ATLTRACE( "Inside BrowserWnd::OnInPlaceActivate\n" );
	// Why disagree.
	return S_OK;
}


STDMETHODIMP BrowserWnd::OnUIActivate()
{
	ATLTRACE( "Inside BrowserWnd::OnUIActivate\n" );
	return S_OK;
}


STDMETHODIMP BrowserWnd::GetWindowContext(	LPOLEINPLACEFRAME FAR* ppFrame,	
	LPOLEINPLACEUIWINDOW FAR* ppDoc,
	LPRECT prcPosRect,
	LPRECT prcClipRect,
	LPOLEINPLACEFRAMEINFO lpFrameInfo )
{
	ATLTRACE( "Inside BrowserWnd::GetWindowContext\n" );
	*ppFrame = this;
	*ppDoc = NULL;
	GetClientRect( getHWND(), prcPosRect );
	GetClientRect( getHWND(), prcClipRect );

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = getHWND();
	lpFrameInfo->haccel = NULL;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}


STDMETHODIMP BrowserWnd::Scroll( SIZE scrollExtent )
{
	ATLTRACE( "Inside BrowserWnd::Scroll\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::OnUIDeactivate( BOOL fUndoable )
{
	ATLTRACE( "Inside BrowserWnd::OnUIDeactivate\n" );
	return S_OK;
}


STDMETHODIMP BrowserWnd::OnInPlaceDeactivate()
{
	ATLTRACE( "Inside BrowserWnd::OnInPlaceDeactivate\n" );
	return S_OK;
}


STDMETHODIMP BrowserWnd::DiscardUndoState()
{
	ATLTRACE( "Inside BrowserWnd::DiscardUndoState\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::DeactivateAndUndo()
{
	ATLTRACE( "Inside BrowserWnd::DeactivateAndUndo\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::OnPosRectChange( LPCRECT lprcPosRect )
{
	ATLTRACE( "Inside BrowserWnd::OnPosRectChange\n" );
	return S_OK;
}


// IOleInPlaceUIWindow
STDMETHODIMP BrowserWnd::GetBorder(LPRECT lprectBorder)
{
	ATLTRACE( "Inside BrowserWnd::GetBorder\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	ATLTRACE( "Inside BrowserWnd::RequestBorderSpace\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	ATLTRACE( "Inside BrowserWnd::SetBorderSpace\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::SetActiveObject(IOleInPlaceActiveObject *pActiveObject,LPCOLESTR pszObjName)
{
	ATLTRACE( "Inside BrowserWnd::SetActiveObject\n" );
	return S_OK;
}

// IOleInPlaceFrame
STDMETHODIMP BrowserWnd::InsertMenus(HMENU hmenuShared,LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	ATLTRACE( "Inside BrowserWnd::InsertMenus\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::SetMenu(HMENU hmenuShared,HOLEMENU holemenu,HWND hwndActiveObject)
{
	ATLTRACE( "Inside BrowserWnd::SetMenu\n" );
	return S_OK;
}

STDMETHODIMP BrowserWnd::RemoveMenus(HMENU hmenuShared)
{
	ATLTRACE( "Inside BrowserWnd::RemoveMenus\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::SetStatusText(LPCOLESTR pszStatusText)
{
	ATLTRACE( "Inside BrowserWnd::SetStatusText\n" );
	return S_OK;
}

STDMETHODIMP BrowserWnd::EnableModeless(BOOL fEnable)
{
	ATLTRACE( "Inside BrowserWnd::EnableModeless\n" );
	return S_OK;
}

STDMETHODIMP BrowserWnd::TranslateAccelerator(  LPMSG lpmsg,WORD wID)
{
	ATLTRACE( "Inside BrowserWnd::TranslateAccelerator\n" );
	return S_OK;
}


// IStorage 
STDMETHODIMP BrowserWnd::CreateStream(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStream ** ppstm)
{
	ATLTRACE( "Inside BrowserWnd::CreateStream\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::OpenStream(const WCHAR * pwcsName,void * reserved1,DWORD grfMode,DWORD reserved2,IStream ** ppstm)
{
	ATLTRACE( "Inside BrowserWnd::OpenStream\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::CreateStorage(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStorage ** ppstg)
{
	ATLTRACE( "Inside BrowserWnd::CreateStorage\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::OpenStorage(const WCHAR * pwcsName,IStorage * pstgPriority,DWORD grfMode,SNB snbExclude,DWORD reserved,IStorage ** ppstg)
{
	ATLTRACE( "Inside BrowserWnd::OpenStorage\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::CopyTo(DWORD ciidExclude,IID const * rgiidExclude,SNB snbExclude,IStorage * pstgDest)
{
	ATLTRACE( "Inside BrowserWnd::CopyTo\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::MoveElementTo(const OLECHAR * pwcsName,IStorage * pstgDest,const OLECHAR* pwcsNewName,DWORD grfFlags)
{
	ATLTRACE( "Inside BrowserWnd::MoveElementTo\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::Commit(DWORD grfCommitFlags)
{
	ATLTRACE( "Inside BrowserWnd::Commit\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::Revert(void)
{
	ATLTRACE( "Inside BrowserWnd::Revert\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::EnumElements(DWORD reserved1,void * reserved2,DWORD reserved3,IEnumSTATSTG ** ppenum)
{
	ATLTRACE( "Inside BrowserWnd::EnumElements\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::DestroyElement(const OLECHAR * pwcsName)
{
	ATLTRACE( "Inside BrowserWnd::DestroyElement\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::RenameElement(const WCHAR * pwcsOldName,const WCHAR * pwcsNewName)
{
	ATLTRACE( "Inside BrowserWnd::RenameElement\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::SetElementTimes(const WCHAR * pwcsName,FILETIME const * pctime,FILETIME const * patime,FILETIME const * pmtime)
{
	ATLTRACE( "Inside BrowserWnd::SetElementTimes\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::SetClass(REFCLSID clsid)
{
	ATLTRACE( "Inside BrowserWnd::SetClass\n" );
	return S_OK;
}

STDMETHODIMP BrowserWnd::SetStateBits(DWORD grfStateBits,DWORD grfMask)
{
	ATLTRACE( "Inside BrowserWnd::SetStateBits\n" );
	NOTIMPLEMENTED;
}

STDMETHODIMP BrowserWnd::Stat(STATSTG * pstatstg,DWORD grfStatFlag)
{
	ATLTRACE( "Inside BrowserWnd::Stat\n" );
	NOTIMPLEMENTED;
}


STDMETHODIMP BrowserWnd::GetTypeInfoCount( UINT *pctinfo )
{
	ATLTRACE( "Inside BrowserWnd::GetTypeInfoCount\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserWnd::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	ATLTRACE( "Inside BrowserWnd::GetTypeInfo\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserWnd::GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
{
	ATLTRACE( "Inside BrowserWnd::GetIDsOfNames\n" );
	return E_NOTIMPL;
}


STDMETHODIMP BrowserWnd::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
{
	ATLTRACE( "Inside BrowserWnd::Invoke\n" );
	switch ( dispIdMember )
	{
	case DISPID_DOCUMENTCOMPLETE:
		ATLTRACE( "onDocumentComplete event!\n" );
		onDocumentComplete( pDispParams );
		break;
	}

	return E_NOTIMPL;
}


void BrowserWnd::onDocumentComplete( DISPPARAMS *pDispParams )
{
	// Verify the the input parameters
	if ( ( pDispParams == NULL ) || 
		 ( pDispParams->cArgs != 2 ) ||
		 ( pDispParams->rgvarg[0].vt != (VARTYPE) ( VT_BYREF | VT_VARIANT ) ) )
		return;

	// Obtain the document URL
	wstring		url = pDispParams->rgvarg[0].pvarVal->bstrVal;

	callJSFunction( L"waOnDocumentComplete", Parameters(0) );
}
