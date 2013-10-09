#include "StdAfx.h"
#include "IconWnd.h"

#define ICON_CLASSNAME	L"sftIconClass"

IconWnd::IconWnd(void) : 
	m_pIcon( NULL ),	
	m_pIconDisplay( NULL ),
	m_pBackground( NULL )
{
	registerClass();
}


IconWnd::~IconWnd(void)
{
	if ( m_pIcon != NULL )
	{
		FreeImage_Unload( m_pIcon );
	}
	if ( m_pIconDisplay != NULL )
	{
		FreeImage_Unload( m_pIconDisplay );
	}
	if ( m_pBackground != NULL )
	{
		FreeImage_Unload( m_pBackground );
	}
}


bool IconWnd::create( HWND hParent, const RECT &rcWnd )
{
	// Update the rect with the bitmap size
	RECT	rcUpdatedWnd = rcWnd;
	if ( m_pIcon != NULL )
	{
		int		halfWidth = FreeImage_GetWidth( m_pIcon ) / 2;
		int		halfHeight = FreeImage_GetHeight( m_pIcon ) / 2;
		rcUpdatedWnd.left -= halfWidth;
		rcUpdatedWnd.right += halfWidth;
		rcUpdatedWnd.top -= halfHeight;
		rcUpdatedWnd.bottom += halfHeight;
	}

	// Create the icon window.
	m_hWnd = CreateWindowEx( WS_EX_TOPMOST | WS_EX_COMPOSITED, ICON_CLASSNAME, NULL, WS_CHILD, rcUpdatedWnd.left, rcUpdatedWnd.top, rcUpdatedWnd.right - rcUpdatedWnd.left, rcUpdatedWnd.bottom - rcUpdatedWnd.top, hParent, 0, AfxGetInstanceHandle(), 0);
	if ( !m_hWnd )
	{
		DWORD err = ::GetLastError();
		return false;
	}

	// Store the link between the HWND and InAppContainerWnd*
	attachWindow();

	// Finally make sure that our window shown and is the top most. It's somehow redundant but cool!
	BringWindowToTop( m_hWnd );
	return true;
}


void IconWnd::setIcon( UINT nID )
{
	// Remove the current icon image
	if ( m_pIcon != NULL )
	{
		FreeImage_Unload( m_pIcon );
		m_pIcon = NULL;
	}

	HINSTANCE	hInstance = AfxGetResourceHandle();
	if ( hInstance == NULL )
	{
		return;
	}

	HRSRC hRSRC = FindResource( hInstance, MAKEINTRESOURCE( nID ), L"PNG" );
	if (hRSRC == NULL) 
	{
		return;
	}

	DWORD dwSize = ::SizeofResource( hInstance, hRSRC );
	if ( !dwSize )
	{
		return;
	}

	HGLOBAL handle = ::LoadResource( hInstance, hRSRC );
	if (handle == NULL) 
	{
		return;
	}

	BYTE* pResourceData = static_cast<BYTE*>( ::LockResource( handle ) );
	if ( pResourceData != NULL )
	{
		// Create a FreeImage memory buffer and load the data from there
		FIMEMORY				*pMemIO	= FreeImage_OpenMemory( pResourceData, dwSize );
		FREE_IMAGE_FORMAT		fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileTypeFromMemory( pMemIO, 0 );
		if ( ( FIF_UNKNOWN != fif ) && FreeImage_FIFSupportsReading( fif ) )
		{
			m_pIcon = FreeImage_LoadFromMemory( fif, pMemIO, 0 );
		}
		FreeImage_CloseMemory( pMemIO );
	}

	FreeResource( handle );
}


void IconWnd::setIconBackground( FIBITMAP *pBackground )
{
	m_pBackground = FreeImage_ConvertTo24Bits( pBackground );
}


void IconWnd::moveWindow( const RECT& rcWnd )
{
	// Update the rect with the bitmap size
	RECT	rcUpdatedWnd = rcWnd;
	if ( m_pIcon != NULL )
	{
		int		halfWidth = FreeImage_GetWidth( m_pIcon ) / 2;
		int		halfHeight = FreeImage_GetHeight( m_pIcon ) / 2;
		rcUpdatedWnd.left -= halfWidth;
		rcUpdatedWnd.right += halfWidth;
		rcUpdatedWnd.top -= halfHeight;
		rcUpdatedWnd.bottom += halfHeight;
	}
	GenericWnd< IconWnd >::moveWindow( rcUpdatedWnd );
}


void IconWnd::registerClass()
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
	wc.lpszClassName = ICON_CLASSNAME;
	wc.hIconSm       = LoadIcon(NULL, NULL);
	::RegisterClassEx( &wc );
}


LRESULT IconWnd::wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	LRESULT		lResult;
	switch ( msg )
	{
	case WM_PAINT:		lResult = onPaint();		break;
	case WM_LBUTTONUP:	lResult = onLButtonUp();	break;
	default:			lResult = ::DefWindowProc( hWnd, msg, wParam, lParam );
	}
	return lResult;
}


LRESULT CALLBACK IconWnd::staticWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	IconWnd		*pIconWnd = findWindow( hWnd );
	return pIconWnd != NULL ? pIconWnd->wndProc( hWnd, msg, wParam, lParam ) : ::DefWindowProc( hWnd, msg, wParam, lParam );
}


LRESULT IconWnd::onPaint()
{
	// Begin the paint process
	PAINTSTRUCT p;
	HDC hDC = BeginPaint(m_hWnd, &p);

	//Obtain the client area of the window
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// Draw the screenshot bitmap
	if ( m_pIcon != NULL )
	{
		// Try to make the composition between the icon and its background.
		if ( m_pIconDisplay == NULL )
		{
			m_pIconDisplay = FreeImage_Composite( m_pIcon, FALSE, NULL, m_pBackground );
		}

		// Draw the composited bitmap.
		if ( m_pIconDisplay != NULL )
		{
			::SetStretchBltMode( hDC, COLORONCOLOR );	
			::StretchDIBits( hDC, 
							 rcClient.left, rcClient.top, 
							 rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 
							 0, 0, 
							 FreeImage_GetWidth( m_pIconDisplay ), FreeImage_GetHeight( m_pIconDisplay ),
							 FreeImage_GetBits( m_pIconDisplay ), 
							 FreeImage_GetInfo( m_pIconDisplay ), 
							 DIB_RGB_COLORS, SRCCOPY);
		}
	}

	//Terminates the paint process
	EndPaint(m_hWnd, &p);

	return 0L;
}


LRESULT IconWnd::onLButtonUp()
{
	onClicked( this, Parameters() );
	return 0L;
}

