#include "stdafx.h"
#include "WindowsAPI.h"

WindowsAPI::WindowsAPI()
{

}


WindowsAPI::~WindowsAPI()
{

}


void WindowsAPI::CreateFunctors()
{
	ADD_FUNCTOR( L"waSetWindowSize", WindowsAPI, waSetWindowSize );
	ADD_FUNCTOR( L"waSetWindowTitle", WindowsAPI, waSetWindowTitle );
	ADD_FUNCTOR( L"waShowWindow", WindowsAPI, waShowWindow );
	ADD_FUNCTOR( L"waCloseWindow", WindowsAPI, waCloseWindow );
	ADD_FUNCTOR( L"waShowProgressBar", WindowsAPI, waShowProgressBar );
	ADD_FUNCTOR( L"waEnableDocumentComplete", WindowsAPI, waEnableDocumentComplete );
	ADD_FUNCTOR( L"waEnableClickFilter", WindowsAPI, waEnableClickFilter );
	ADD_FUNCTOR( L"waQuitApplication", WindowsAPI, waQuitApplication );
	ADD_FUNCTOR( L"waOpenURLOnDefaultBrowser", WindowsAPI, waOpenURLOnDefaultBrowser );
	ADD_FUNCTOR( L"waGetApiKey", WindowsAPI, waGetApiKey );
}


Parameter WindowsAPI::waSetWindowSize( void* sender, const Parameters& params )
{
	if ( ( params.GetNumParameters() == 2 ) &&
	     ( params[0]->GetDataType() == PARAM_TYPE_INT32 ) &&
		 ( params[1]->GetDataType() == PARAM_TYPE_INT32 ) )
	{
		onWindowSize( this, params );
	}
	return Parameter();
}


Parameter WindowsAPI::waSetWindowTitle( void* sender, const Parameters& params )
{
	return Parameter();
}


Parameter WindowsAPI::waShowWindow( void* sender, const Parameters& params )
{
	if ( ( params.GetNumParameters() == 1 ) &&
		( params[0]->GetDataType() == PARAM_TYPE_BOOL ) )
	{
		onShowWindow( this, params );
	}
	return Parameter();
}


Parameter WindowsAPI::waCloseWindow( void* sender, const Parameters& params )
{
	onCloseWindow( this, params );
	return Parameter();
}


Parameter WindowsAPI::waShowProgressBar( void* sender, const Parameters& params )
{
	return Parameter();
}


Parameter WindowsAPI::waEnableDocumentComplete( void* sender, const Parameters& params )
{
	return Parameter();
}


Parameter WindowsAPI::waEnableClickFilter( void* sender, const Parameters& params )
{
	return Parameter();
}


Parameter WindowsAPI::waQuitApplication( void* sender, const Parameters& params )
{
	onCloseWindow( this, params );
	return Parameter();
}


Parameter WindowsAPI::waOpenURLOnDefaultBrowser( void* sender, const Parameters& params )
{
	Parameter p;
	p.SetBoolValue(false);
	if ( ( params.GetNumParameters() == 1 ) &&
		 ( params[0]->GetDataType() == PARAM_TYPE_BSTR ) )
	{
		wstring				url = params[0]->GetBSTRValue();
		SHELLEXECUTEINFO	sei = { 0 };
		LPITEMIDLIST		pIDList = NULL;
		LPMALLOC			pMalloc = NULL;
		IShellFolder*		pIShellFolder = NULL;
		bool				result = false;

		// get pointer to Shell's interfaces (this is required for using PIDL list)
		SHGetMalloc( &pMalloc );
		SHGetDesktopFolder( &pIShellFolder );

		// Convert URL string into an PIDL list; we can't call SHParseDisplayName() here
		// because that is only supported in Windows XP and beyond, but we need support stuff
		// older than that.
		HRESULT hr = pIShellFolder->ParseDisplayName( NULL, NULL, (LPWSTR) url.c_str(), NULL, &pIDList, 0x0 );
		if ( SUCCEEDED( hr ) )
		{
			// Setup SHELLEXECUTEINFO structure
			sei.cbSize		= sizeof( sei );
			sei.fMask		= SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
			sei.nShow		= SW_SHOWNORMAL;
			sei.lpIDList	= ( LPVOID ) pIDList;	// must use mask SEE_MASK_INVOKEIDLIST
			result = ShellExecuteEx( &sei ) != 0;
		}

		p.SetBoolValue( result );
	}
	return p;
}


Parameter WindowsAPI::waGetApiKey( void* sender, const Parameters& params )
{
	EventQueryAnswers	answer = onGetApiKey( sender, params );
	return answer[ INAPP_RESULT_APIKEY ];
}
