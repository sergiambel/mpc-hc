#pragma once

#include "GenericWnd.h"
#include "BrowserWnd.h"
#include "IconWnd.h"

#include <map>


// Forward class declaration
class AdManager;


class InAppContainerWnd : public GenericWnd< InAppContainerWnd >
{
public:
								InAppContainerWnd( AdManager* pAdManager );
	virtual						~InAppContainerWnd( void );

	void						setContentUrl( const std::wstring& contentUrl );
	bool						create( HWND hParent, RECT &rcWnd );

protected:
	void						registerClass();
	void						subscribeEvents();

	void						subclassWindow( HWND hWnd );
	FIBITMAP*					takeScreenShot( HWND hWnd, const RECT* pRCCrop = NULL, bool applyBlur = true );
	FIBITMAP*					createBitmap( HBITMAP hBMP );
	void						applyBlurEffect( FIBITMAP* pImage );

	void						hideChildWindows();
	void						restoreChildWindows();

	void						disableMenuItems();
	void						restoreMenuItems();

	LRESULT						wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK		staticWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	static LRESULT CALLBACK		parentWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData );

	LRESULT						onPaint();
	LRESULT						onDestroy();
	LRESULT						onForceDestroy();

	// Event functions.
	void						onWindowSize( void *from, const Parameters &params );
	void						onShowWindow( void *from, const Parameters &params );
	void						onCloseWindow( void *from, const Parameters &params );
	Parameter					onGetApiKey( void *from, const Parameters &params );

	// Store a link pointer with the AdManager
	AdManager*					m_pAdManager;

	// Store the content url.
	std::wstring				m_contentUrl;

	// Parent window and screen-shot of the parent window.
	HWND						m_hParent;
	FIBITMAP*					m_pScreenshot;

	// Preserve the parent window style.
	LONG						m_lParentStyle;

	// Store the visibility flag of the children windows for restore them after showing the banner.
	std::map< HWND, BOOL >		m_childWindowVisibleMap;

	// Store the status of each single menu item.
	std::map< int, UINT >		m_menuItemStateMap;

	// These are the children windows created by this container: Browser and Close icon windows.
	BrowserWnd					m_browser;
	IconWnd						m_close;
};

