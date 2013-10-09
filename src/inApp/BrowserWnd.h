#pragma once

#include "GenericWnd.h"
#include "BrowserEventListener.h"

#include <memory>

class BrowserWnd : 
	public GenericWnd< BrowserWnd >,
	public IOleClientSite, 
	public IOleInPlaceSite,
	public IOleInPlaceFrame,
	public IStorage,
	public DWebBrowserEvents2
{
public:
	BrowserWnd(void);
	virtual										~BrowserWnd(void);

	void										setContentUrl( const std::wstring& contentUrl );

	bool										create( HWND hParent, RECT &rcWnd );
	WindowsAPI*									getWindowsAPI();

	Parameter									callJSFunction( const wstring &name, const Parameters &params );

	void										quit();

	// IUnknown interface methods
	STDMETHODIMP								QueryInterface( REFIID riid,void ** ppvObject );
	STDMETHODIMP_(ULONG)						AddRef( void );
	STDMETHODIMP_(ULONG)						Release( void );

	// IOleClientSite interface methods
	STDMETHODIMP								SaveObject();
	STDMETHODIMP								GetMoniker( DWORD dwAssign,DWORD dwWhichMoniker,IMoniker ** ppmk );
	STDMETHODIMP								GetContainer( LPOLECONTAINER FAR* ppContainer );
	STDMETHODIMP								ShowObject();
	STDMETHODIMP								OnShowWindow( BOOL fShow );
	STDMETHODIMP								RequestNewObjectLayout();

	// IOleWindow interface methods
	STDMETHODIMP								GetWindow( HWND FAR* lphwnd );
	STDMETHODIMP								ContextSensitiveHelp( BOOL fEnterMode );

	// IOleInPlaceSite methods interface methods
	STDMETHODIMP								CanInPlaceActivate();
	STDMETHODIMP								OnInPlaceActivate();
	STDMETHODIMP								OnUIActivate();
	STDMETHODIMP								GetWindowContext( LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo );
	STDMETHODIMP								Scroll( SIZE scrollExtent );
	STDMETHODIMP								OnUIDeactivate( BOOL fUndoable );
	STDMETHODIMP								OnInPlaceDeactivate();
	STDMETHODIMP								DiscardUndoState();
	STDMETHODIMP								DeactivateAndUndo();
	STDMETHODIMP								OnPosRectChange( LPCRECT lprcPosRect );

	// IOleInPlaceUIWindow interface methods
	STDMETHODIMP								GetBorder(LPRECT lprectBorder);
	STDMETHODIMP								RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
	STDMETHODIMP								SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
	STDMETHODIMP								SetActiveObject(IOleInPlaceActiveObject *pActiveObject,LPCOLESTR pszObjName);

	// IOleInPlaceFrame interface methods
	STDMETHODIMP								InsertMenus(HMENU hmenuShared,LPOLEMENUGROUPWIDTHS lpMenuWidths);
	STDMETHODIMP								SetMenu(HMENU hmenuShared,HOLEMENU holemenu,HWND hwndActiveObject);
	STDMETHODIMP								RemoveMenus(HMENU hmenuShared);
	STDMETHODIMP								SetStatusText(LPCOLESTR pszStatusText);
	STDMETHODIMP								EnableModeless(BOOL fEnable);
	STDMETHODIMP								TranslateAccelerator(  LPMSG lpmsg,WORD wID);

	// IStorage interface methods
	STDMETHODIMP								CreateStream(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStream ** ppstm);
	STDMETHODIMP								OpenStream(const WCHAR * pwcsName,void * reserved1,DWORD grfMode,DWORD reserved2,IStream ** ppstm);
	STDMETHODIMP								CreateStorage(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStorage ** ppstg);
	STDMETHODIMP								OpenStorage(const WCHAR * pwcsName,IStorage * pstgPriority,DWORD grfMode,SNB snbExclude,DWORD reserved,IStorage ** ppstg);
	STDMETHODIMP								CopyTo(DWORD ciidExclude,IID const * rgiidExclude,SNB snbExclude,IStorage * pstgDest);
	STDMETHODIMP								MoveElementTo(const OLECHAR * pwcsName,IStorage * pstgDest,const OLECHAR* pwcsNewName,DWORD grfFlags);
	STDMETHODIMP								Commit(DWORD grfCommitFlags);
	STDMETHODIMP								Revert(void);
	STDMETHODIMP								EnumElements(DWORD reserved1,void * reserved2,DWORD reserved3,IEnumSTATSTG ** ppenum);
	STDMETHODIMP								DestroyElement(const OLECHAR * pwcsName);
	STDMETHODIMP								RenameElement(const WCHAR * pwcsOldName,const WCHAR * pwcsNewName);
	STDMETHODIMP								SetElementTimes(const WCHAR * pwcsName,FILETIME const * pctime,FILETIME const * patime,FILETIME const * pmtime);
	STDMETHODIMP								SetClass(REFCLSID clsid);
	STDMETHODIMP								SetStateBits(DWORD grfStateBits,DWORD grfMask);
	STDMETHODIMP								Stat(STATSTG * pstatstg,DWORD grfStatFlag);

	// DWebBrowserEvents2 interface methods
	STDMETHODIMP								GetTypeInfoCount( UINT *pctinfo );
	STDMETHODIMP								GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo );
	STDMETHODIMP								GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId );
	STDMETHODIMP								Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );


protected:
	void										registerClass();

	bool										createBrowser();
	bool										registerEventListener();
	bool										unregisterEventListener();
	bool										disableDragAndDrop();

	bool										getDocumnent( CComPtr< IHTMLDocument2 >& pDocument );
	bool										getJavascript( CComPtr< IDispatch >& pDispatch );
	DISPID										getJSFunctionId( CComPtr< IDispatch >& pJS, const wstring &name );
	void										prepareJSFunctionParams( const Parameters &params, DISPPARAMS &dispparams );

	LRESULT										wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK						staticWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	void										onDocumentComplete( DISPPARAMS *pDispParams );

	std::wstring								m_contentUrl;
	CComPtr< IWebBrowser2 >						m_pWebBrowser;
	std::unique_ptr< BrowserEventListener >		m_pEventListener;
	DWORD										m_dwCookie;
};

