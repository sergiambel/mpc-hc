#pragma once

#include <map>

template <typename base>
class GenericWnd
{
public:
										GenericWnd( void );
	virtual								~GenericWnd( void );

	typedef std::map< HWND, base * >	WindowMap;

	HWND								getHWND();

	RECT								getWindowRect( HWND hRefWindow = NULL );
	void								moveWindow( const RECT& rcWnd );
	void								showWindow( bool show );
	void								destroyWindow();

protected:
	void								attachWindow();
	void								detachWindow();
	static base*						findWindow( HWND hWnd );

	HWND								m_hWnd;

private:
	static WindowMap					m_windowMap;
};


template < typename base > std::map< HWND, base * > GenericWnd< base >::m_windowMap;


template < typename base >
GenericWnd< base >::GenericWnd( void ) :
	m_hWnd( NULL )
{
}
	

template < typename base >
GenericWnd< base >::~GenericWnd( void )
{
}


template < typename base >
HWND GenericWnd< base >::getHWND()
{
	return m_hWnd;
}


template < typename base >
RECT GenericWnd< base >::getWindowRect( HWND hRefWindow )
{
	RECT	rcWindow;
	::GetWindowRect( m_hWnd, &rcWindow);
	if ( hRefWindow != NULL )
	{
		POINT	ptLT = { rcWindow.left, rcWindow.top };
		POINT	ptRB = { rcWindow.right, rcWindow.bottom };
		::ScreenToClient( hRefWindow, &ptLT );
		::ScreenToClient( hRefWindow, &ptRB );
		rcWindow.left	= ptLT.x;
		rcWindow.top	= ptLT.y;
		rcWindow.right	= ptRB.x;
		rcWindow.bottom = ptRB.y;
	}
	return rcWindow;
}


template < typename base >
void GenericWnd< base >::moveWindow( const RECT& rcWnd )
{
	::MoveWindow( m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, TRUE );
}


template < typename base >
void GenericWnd< base >::showWindow( bool show )
{
	::ShowWindow( m_hWnd, show ? SW_SHOW : SW_HIDE );
}

template < typename base >
void GenericWnd< base >::destroyWindow()
{
	::DestroyWindow( m_hWnd );
}


template < typename base >
void GenericWnd< base >::attachWindow()
{
	m_windowMap[ m_hWnd ] = static_cast< base* >( this );
}


template < typename base >
void GenericWnd< base >::detachWindow()
{
	m_windowMap.erase( m_hWnd );
}


template < typename base >
base* GenericWnd< base >::findWindow( HWND hWnd )
{
	WindowMap::iterator		it;
	base*					result = NULL;
	if ( it = m_windowMap.find( hWnd ), it != m_windowMap.end() )
	{
		result = static_cast< base* >( it->second );
	}
	return result;
}