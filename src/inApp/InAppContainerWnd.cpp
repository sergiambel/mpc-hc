#include "StdAfx.h"
#include "InAppContainerWnd.h"
#include "AdManager.h"

#include "resource.h"

#include <algorithm>

#define CONTAINER_CLASSNAME	L"sftContainerClass"
#define SUBCLASS_ID			1
#define WM_FORCE_DESTROY ( WM_USER + 0x0100 )

InAppContainerWnd::InAppContainerWnd( AdManager* pAdManager ) :
	m_pAdManager( NULL ),
	m_hParent( NULL ),
	m_pScreenshot( NULL ),
	m_lParentStyle( NULL )
{
	registerClass();
	subscribeEvents();
}


InAppContainerWnd::~InAppContainerWnd( void )
{
	if ( m_pScreenshot != NULL )
	{
		FreeImage_Unload( m_pScreenshot );
	}
	detachWindow();
}


void InAppContainerWnd::setContentUrl( const std::wstring& contentUrl )
{
	m_contentUrl = contentUrl;
}


bool InAppContainerWnd::create( HWND hParent, RECT &rcWnd )
{
	// Create the container window.
	m_hWnd = CreateWindowEx( WS_EX_TOPMOST | WS_EX_COMPOSITED, CONTAINER_CLASSNAME, NULL, WS_CHILD, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, hParent, 0, AfxGetInstanceHandle(), 0);
	if ( !m_hWnd )
	{
		DWORD err = ::GetLastError();
		return false;
	}

	// Store the link between the HWND and InAppContainerWnd*
	attachWindow();

	// Subclass parent window and avoid it to be resizeable
	m_hParent = hParent;
	subclassWindow( hParent );

	// Take the screen shot of the current parent window state. 
	// If there is any animated element, it will get frozen with our approach.
	m_pScreenshot = takeScreenShot( m_hParent );
	hideChildWindows();
	disableMenuItems();

	// Create the browser window - Adjust it to be 80% in both directions
	RECT	rcBrowser = rcWnd;
	int		curWidth  = rcWnd.right - rcWnd.left;
	int		curHeight = rcWnd.bottom - rcWnd.top;
	int		newWidth  = (int) ( ( curWidth * 0.8f ) + 0.5f );
	int		newHeight = (int) ( ( curHeight * 0.8f ) + 0.5f );
	rcBrowser.left = rcWnd.left + ( ( curWidth - newWidth ) / 2 );
	rcBrowser.top = rcWnd.top + ( ( curHeight - newHeight ) / 2 );
	rcBrowser.right = rcBrowser.left + newWidth;
	rcBrowser.bottom = rcBrowser.top + newHeight;
	m_browser.setContentUrl( m_contentUrl );
	m_browser.create( m_hWnd, rcBrowser );

	// Create the close icon window
	RECT	rcClose;
	m_close.setIcon( IDB_CLOSE );
	rcClose.left = rcBrowser.right;
	rcClose.top = rcBrowser.top;
	rcClose.right = rcBrowser.right;
	rcClose.bottom = rcBrowser.top;
	m_close.create( m_hWnd, rcClose );

	// Finally make sure that our window shown and is the top most. It's somehow redundant but cool!
	ShowWindow( m_hWnd, SW_SHOW );
	BringWindowToTop( m_hWnd );

	return true;
}


void InAppContainerWnd::registerClass()
{
	WNDCLASSEX	wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = staticWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = AfxGetInstanceHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = CONTAINER_CLASSNAME;
	wc.hIconSm       = LoadIcon(NULL, NULL);
	::RegisterClassEx( &wc );
}


void InAppContainerWnd::subscribeEvents()
{
	WindowsAPI	*pAPI = m_browser.getWindowsAPI();
	pAPI->onWindowSize.Subscribe( CREATE_CALLBACK( InAppContainerWnd, onWindowSize ) );
	pAPI->onShowWindow.Subscribe( CREATE_CALLBACK( InAppContainerWnd, onShowWindow ) );
	pAPI->onCloseWindow.Subscribe( CREATE_CALLBACK( InAppContainerWnd, onCloseWindow ) );
	pAPI->onGetApiKey.Subscribe( CREATE_QUERY_CALLBACK( INAPP_RESULT_APIKEY, InAppContainerWnd, onGetApiKey ) );
	m_close.onClicked.Subscribe( CREATE_CALLBACK( InAppContainerWnd, onCloseWindow ) );
}


void InAppContainerWnd::subclassWindow( HWND hWnd )
{
	// Subclass the given parent window handle to avoid window size events.
	UINT_PTR uIdSubclass = SUBCLASS_ID;
	SetWindowSubclass( hWnd, parentWndProc, uIdSubclass, (DWORD_PTR) this );

	// Temporary remove the WS_MAXIMIZEBOX and WS_MINIMIZEBOX styles from the parent window.
	LONG	lStyle;
	m_lParentStyle = lStyle = GetWindowLong( hWnd, GWL_STYLE );
	lStyle = lStyle & ( ~ ( WS_MAXIMIZEBOX | WS_MINIMIZEBOX ) );
	SetWindowLong( hWnd, GWL_STYLE, lStyle );
}


FIBITMAP* InAppContainerWnd::takeScreenShot( HWND hWnd, const RECT* pRCCrop, bool applyBlur )
{
	// Obtain the input window client rect.
	RECT		rc;
	GetClientRect( hWnd, &rc );

	// Obtain the screen device context
	HDC		hDCScreen;
	hDCScreen = GetDC( NULL );
	if ( hDCScreen == NULL )
	{
		return NULL;
	}

	// Create a compatible device context from the screen one
	HDC		hDCMem;
	hDCMem = CreateCompatibleDC( hDCScreen );
	if ( hDCMem == NULL)
	{
		return NULL;
	}

	// Create the compatible bitmap where the screen shot will be hold
	HBITMAP	hBMP;
	hBMP = CreateCompatibleBitmap( hDCScreen, rc.right - rc.left, rc.bottom - rc.top );
	if ( hBMP == NULL )
	{
		return NULL;
	}

	// Take screen shot
	SelectObject( hDCMem, hBMP );
	PrintWindow( hWnd, hDCMem, PW_CLIENTONLY );

	// Release device contexts
	DeleteDC( hDCMem );
	ReleaseDC( NULL, hDCScreen );

	// Create the FreeImage bitmap object
	FIBITMAP	*pResultImage = createBitmap( hBMP ) ;
	DeleteObject( hBMP );
	if ( pResultImage != NULL )
	{
		if ( pRCCrop != NULL )
		{
			FIBITMAP	*pCropImage = FreeImage_Copy( pResultImage, pRCCrop->left, pRCCrop->top, pRCCrop->right, pRCCrop->bottom );
			FreeImage_Unload( pResultImage );
			if ( pCropImage != NULL )
			{
				pResultImage = pCropImage;
			}
			else
			{
				return NULL;
			}
		}

		// Blur the screen-shot 5 times
		if ( applyBlur )
		{
			FIBITMAP	*pGreyBitmap = FreeImage_ConvertToGreyscale( pResultImage );
			FreeImage_Unload( pResultImage );
			if ( pGreyBitmap != NULL )
			{
				pResultImage = pGreyBitmap;
			}
			else
			{
				return NULL;
			}
			applyBlurEffect( pResultImage );
			applyBlurEffect( pResultImage );
			applyBlurEffect( pResultImage );
			applyBlurEffect( pResultImage );
			applyBlurEffect( pResultImage );
		}
	}


	// Return the created image
	return pResultImage;
}


FIBITMAP* InAppContainerWnd::createBitmap( HBITMAP hBMP )
{
	if ( hBMP != NULL )
	{
		// Get informations about the bitmap
		BITMAP		bm;
		GetObject( hBMP, sizeof(BITMAP), (LPSTR) &bm );

		// Create the image
		FIBITMAP*	pBitmap;
		pBitmap = FreeImage_AllocateT( FIT_BITMAP, (WORD) bm.bmWidth, (WORD) bm.bmHeight, (WORD) bm.bmBitsPixel );

		// Obtain the number of used colors and update the palette if required
		unsigned int	nColors = FreeImage_GetColorsUsed( pBitmap );
		if ( ( bm.bmBitsPixel == 1 ) ||  ( bm.bmBitsPixel == 4 ) ||  ( bm.bmBitsPixel == 8 ) )
		{
			RGBQUAD			*pPal = FreeImage_GetPalette( pBitmap );
			for ( unsigned int i = 0; i < nColors; i++ )
			{
				pPal[i].rgbRed = i;
				pPal[i].rgbGreen = i;
				pPal[i].rgbBlue = i;
			}
		}

		// Create a device context for the bitmap and copy the pixels
		HDC		hDC = ::GetDC(NULL);
		int		result;
		result = GetDIBits(hDC,				// handle to DC
			hBMP,							// handle to bitmap
			0,								// first scan line to set
			FreeImage_GetHeight( pBitmap ),	// number of scan lines to copy
			FreeImage_GetBits( pBitmap ),	// array for bitmap bits
			FreeImage_GetInfo( pBitmap ),	// bitmap data buffer
			DIB_RGB_COLORS					// RGB 
		);

		// Release the device context
		ReleaseDC( NULL, hDC );

		if ( result == 0 )
		{
			FreeImage_Unload( pBitmap );
			return NULL;
		}


		// Restore BITMAPINFO members
		BITMAPINFOHEADER 	*pBMPInfo = FreeImage_GetInfoHeader( pBitmap );
		if ( pBMPInfo != NULL )
		{
			pBMPInfo->biClrUsed = nColors;
			pBMPInfo->biClrImportant = nColors;
		}

		// Finally return the created FIBITMAP object
		return pBitmap;
	}

	return NULL;
}


void InAppContainerWnd::applyBlurEffect( FIBITMAP* pImage )
{
	// Check that the image is in 8-bit colour mode
	if ( FreeImage_GetBPP( pImage ) != 8 )
	{
		return;
	}

	// Obtain image dimensions
	int		width = FreeImage_GetWidth( pImage );
	int		scanWidth = FreeImage_GetPitch( pImage );
	int		height = FreeImage_GetHeight( pImage );

	// Create temporary image
	FIBITMAP		*pTempImage;
	pTempImage = FreeImage_Copy( pImage, 0, 0, width, height );

	// Obtain the raw pixel pointers
	BYTE	*pCurBuffer = FreeImage_GetBits( pImage );
	BYTE	*pTmpBuffer = FreeImage_GetBits( pTempImage );

	if ( width > 1 && height > 1 )
	{
		// Iterate through all the pixels and apply the blurring effect
		for ( int y = 0; y < height; ++y )
		{
			for ( int x = 0; x < width; ++x )
			{
				// Define the blurring rect limits
				int		xmin = ( ( x != 0 ) ? -1 : 0 );
				int		xmax = ( ( x != width - 1 ) ? +1 : 0 );
				int		ymin = ( ( y != 0 ) ? -1 : 0 );
				int		ymax = ( ( y != height - 1 ) ? +1 : 0 );
				int		accum = 0;
				int		count = 0;

				for ( int yblur = ymin; yblur <= ymax; ++yblur )
				{
					for ( int xblur = xmin; xblur <= xmax; ++xblur )
					{
						if ( ( xblur == 0 ) && ( yblur == 0) )
						{
							continue;
						}
						accum += *( pTmpBuffer + ( scanWidth * ( y + yblur ) ) + ( x + xblur ) );
						++count;
					}
				}
				*( pCurBuffer + scanWidth * y + x ) = (BYTE) ( accum / count );
			}
		}
	}

	FreeImage_Unload( pTempImage );
}


void InAppContainerWnd::hideChildWindows()
{
	// Hide all the child windows.
	HWND hChild = GetWindow( m_hParent, GW_CHILD );
	while ( hChild != NULL )
	{
		if ( hChild != m_hWnd )
		{
			m_childWindowVisibleMap[ hChild ] = IsWindowVisible( hChild ); 
			ShowWindow( hChild, SW_HIDE );
		}
		hChild = GetWindow( hChild, GW_HWNDNEXT );
	}
}


void InAppContainerWnd::restoreChildWindows()
{
	// Show the child windows again
	std::for_each( m_childWindowVisibleMap.begin(), m_childWindowVisibleMap.end(), []( std::pair< HWND, BOOL > item ) 
	{
		if ( item.second )
		{
			ShowWindow( item.first, SW_SHOW );
		}
	});
}


void InAppContainerWnd::disableMenuItems()
{
	HMENU	hMenu = GetMenu( m_hParent );
	if ( hMenu != NULL )
	{
		int		nCount = GetMenuItemCount( hMenu );
		for (int i = 0; i < nCount; i++)
		{
			// Obtain and store the menu item state
			MENUITEMINFO	menuItemInfo = { 0 };
			menuItemInfo.cbSize = sizeof( MENUITEMINFO );
			menuItemInfo.fMask = MIIM_STATE;
			GetMenuItemInfo( hMenu, i, TRUE, &menuItemInfo);
			m_menuItemStateMap[ i ] = menuItemInfo.fState;

			// Disable the menu item
			EnableMenuItem( hMenu, i, MF_BYPOSITION | MF_DISABLED );
		}

		// Finally, redraws the menu bar
		DrawMenuBar( m_hParent );
	}
}


void InAppContainerWnd::restoreMenuItems()
{
	HMENU	hMenu = GetMenu( m_hParent );
	if ( hMenu != NULL )
	{
		// Basic initialization of the MENUITEMINFO struct.
		MENUITEMINFO	menuItemInfo = { 0 };
		menuItemInfo.cbSize = sizeof( MENUITEMINFO );
		menuItemInfo.fMask = MIIM_STATE;

		// Iterate through each one of the stored menus and update the state flag.
		std::for_each( m_menuItemStateMap.begin(), m_menuItemStateMap.end(), [&hMenu, &menuItemInfo]( std::pair< int, UINT > item ) 
		{
			menuItemInfo.fState = item.second;
			SetMenuItemInfo( hMenu, item.first, TRUE, &menuItemInfo );
		});

		// Finally, redraws the menu bar
		DrawMenuBar( m_hParent );
	}
}


LRESULT InAppContainerWnd::wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	LRESULT		lResult;
	switch ( msg )
	{
	case WM_PAINT:			lResult = onPaint();		break;
	case WM_DESTROY:		lResult = onDestroy();		break;
	case WM_FORCE_DESTROY:	lResult = onForceDestroy();	break;
	default:				lResult = ::DefWindowProc( hWnd, msg, wParam, lParam );
	}
	return lResult;
}


LRESULT CALLBACK InAppContainerWnd::staticWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	InAppContainerWnd		*pContainerWnd = findWindow( hWnd );
	return pContainerWnd != NULL ? pContainerWnd->wndProc( hWnd, msg, wParam, lParam ) : ::DefWindowProc( hWnd, msg, wParam, lParam );
}


LRESULT CALLBACK InAppContainerWnd::parentWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
	switch ( uMsg )
	{
	case WM_SIZING:
		{
			RECT &rc = *(LPRECT) lParam;
			RECT windowRect;
			GetWindowRect( hWnd, &windowRect );
			rc = windowRect;
			return FALSE;
		}

	case WM_SIZE:
		{
			return TRUE;
		}

	case WM_INITMENU:
		{
			return TRUE;
		}
	}
	return ::DefSubclassProc( hWnd, uMsg, wParam, lParam );
}


LRESULT InAppContainerWnd::onPaint()
{
	// Begin the paint process
	PAINTSTRUCT p;
	HDC hDC = BeginPaint(m_hWnd, &p);

/*	// If first time paint, hide the child windows
	if ( m_bFirstTime )
	{
		// Take the screen shot of the current parent window state. 
		// If there is any animated element, it will get frozen with our approach.
		m_pScreenshot = takeScreenShot( m_hParent );
		hideChildWindows();
		disableMenuItems();
		m_bFirstTime = false;
	}*/

	//Obtain the client area of the window
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// Draw the screen-shot bitmap
	if ( m_pScreenshot != NULL )
	{
		SetStretchBltMode(hDC, COLORONCOLOR);	
		StretchDIBits(hDC, 
			rcClient.left, rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 
			0, 0, 
			FreeImage_GetWidth( m_pScreenshot ), FreeImage_GetHeight( m_pScreenshot ),
			FreeImage_GetBits( m_pScreenshot ), FreeImage_GetInfo( m_pScreenshot ), 
			DIB_RGB_COLORS, 
			SRCCOPY
		);
	}

	//Terminates the paint process
	EndPaint(m_hWnd, &p);

	return 0L;
}


LRESULT InAppContainerWnd::onDestroy()
{
	ATLTRACE( "onDestroy\n" );

	// Remove the browser object
	m_browser.quit();

	// Show the child windows again
	restoreChildWindows();

	// And restore the menu items.
	restoreMenuItems();

	// Remove the Window Subclass
	UINT_PTR uIdSubclass = SUBCLASS_ID;
	RemoveWindowSubclass( m_hParent, parentWndProc, uIdSubclass );

	// Restore the parent window style
	SetWindowLong( m_hParent, GWL_STYLE, m_lParentStyle );
	RedrawWindow( m_hParent, NULL, NULL, RDW_INVALIDATE );

	// Self-destroy this InAppContainerWnd instance
	delete this;

	return 0L;
}


LRESULT InAppContainerWnd::onForceDestroy()
{
	ATLTRACE( "onForceDestroy\n" );
	DestroyWindow( m_hWnd );
	return 0;
}


void InAppContainerWnd::onWindowSize( void *from, const Parameters &params )
{
	// Obtain the client rect
	RECT		rcClient;
	GetClientRect( m_hWnd, &rcClient );

	// Obtain the desired SD window size
	int			width = params[0]->GetInt32Value();
	int			height = params[1]->GetInt32Value();

	// Recalculate the SD Browser Window rect
	RECT		rcBrowser;
	rcBrowser.left = ( ( rcClient.right - rcClient.left ) - width ) / 2;
	rcBrowser.top = ( ( rcClient.bottom - rcClient.top ) - height ) / 2;
	rcBrowser.right = rcBrowser.left + width;
	rcBrowser.bottom = rcBrowser.top + height;
	m_browser.moveWindow( rcBrowser );

	// Recalculate the Close Icon Window Rect
	RECT		rcClose;
	rcClose.left = rcBrowser.right;
	rcClose.top = rcBrowser.top;
	rcClose.right = rcBrowser.right;
	rcClose.bottom = rcBrowser.top;
	m_close.moveWindow( rcClose );
}


void InAppContainerWnd::onShowWindow( void *from, const Parameters &params )
{
	m_browser.showWindow( params[0]->GetBoolValue() );

	if ( m_pScreenshot != NULL )
	{
		RECT			rcWindow;
		rcWindow = m_close.getWindowRect( m_hWnd );
		m_close.setIconBackground( takeScreenShot( m_hWnd, &rcWindow, false ) );
	}
	m_close.showWindow( params[0]->GetBoolValue() );
}


void InAppContainerWnd::onCloseWindow( void *from, const Parameters &params )
{
	// Force the destruction of this window.
	ATLTRACE( "onCloseWindow\n" );
	PostMessage( m_hWnd, WM_FORCE_DESTROY, 0, 0 );
}


Parameter InAppContainerWnd::onGetApiKey( void *from, const Parameters &params )
{
	Parameter	p;
	CComBSTR	apiKey = m_pAdManager->getApiKey().c_str();
	p.SetBSTRValue( apiKey );
	return p;
}
