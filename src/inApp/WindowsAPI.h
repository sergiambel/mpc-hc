#pragma once

#include "Events/BaseAPI.h"
#include "Events/Event.h"
#include "Events/EventQuery.h"

class WindowsAPI : public BaseAPI
{
public:
					WindowsAPI();
	virtual			~WindowsAPI();

	virtual void	CreateFunctors();

	// Public events
	Event			onWindowSize;
	Event			onShowWindow;
	Event			onCloseWindow;
	EventQuery		onGetApiKey;

protected:
	Parameter		waSetWindowSize( void* sender, const Parameters& params );
	Parameter		waSetWindowTitle( void* sender, const Parameters& params );
	Parameter		waShowWindow( void* sender, const Parameters& params );
	Parameter		waCloseWindow( void* sender, const Parameters& params );
	Parameter		waShowProgressBar( void* sender, const Parameters& params );
	Parameter		waEnableDocumentComplete( void* sender, const Parameters& params );
	Parameter		waEnableClickFilter( void* sender, const Parameters& params );
	Parameter		waQuitApplication( void* sender, const Parameters& params );
	Parameter		waOpenURLOnDefaultBrowser( void* sender, const Parameters& params );
	Parameter		waGetApiKey( void* sender, const Parameters& params );
};
