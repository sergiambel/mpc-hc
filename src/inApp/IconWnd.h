#pragma once

#include "GenericWnd.h"
#include "Events/Event.h"

#include <memory>

class IconWnd : public GenericWnd< IconWnd >
{
public:
									IconWnd(void);
	virtual							~IconWnd(void);

	bool							create( HWND hParent, const RECT &rcWnd );
	void							setIcon( UINT nID );
	void							setIconBackground( FIBITMAP *pBackground );

	void							moveWindow( const RECT& rcWnd );

	// Public events
	Event							onClicked;

protected:
	void							registerClass();

	LRESULT							wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK			staticWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	LRESULT							onPaint();
	LRESULT							onLButtonUp();

	FIBITMAP*						m_pIcon;
	FIBITMAP*						m_pIconDisplay;
	FIBITMAP*						m_pBackground;
};

