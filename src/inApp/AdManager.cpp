// AdManager.cpp : Implementation of AdManager

#include "stdafx.h"
#include "AdManager.h"
#include "InAppContainerWnd.h"
#include "HTTPConnection/HTTPConnection.h"

#include <fstream>

#include "StringUtils/StringUtils.h"

// AdManager

STDMETHODIMP AdManager::init(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Build the handshake URL checking if we already have a API KEY.
	std::wstring	url = INAPP_HANDSHAKE_URL;
	m_apiKey = obtainApiKey();
	if ( !m_apiKey.empty() )
	{
		url += L"&key=" + m_apiKey;
	}

	// Do the request
	HTTPConnection	conn;
	std::string		response;
	if ( !conn.get( url, response ) )
	{
		return S_FALSE;
	}

	// Parse the JSON response
	Json::Value		jsonResult;
	Json::Reader	jsonReader;
	if ( !jsonReader.parse( response.c_str(), jsonResult ) )
	{
		return S_FALSE;
	}

	// Obtain the content_url
	if ( jsonResult.isObject() )
	{
		m_contentUrl = StringUtils::StringToWString( jsonResult[ "content_url" ].asString() );

		// Also obtain the API KEY if there isn't still a valid one
		if ( m_apiKey.empty() )
		{
			m_apiKey = StringUtils::StringToWString( jsonResult[ "raw_key" ].asString() );
			storeApiKey( m_apiKey );
		}
	}

	// Launch the thread that will obtain the content
	_beginthread( obtainContentThread, 0, this );


	// Initialization works fine
	m_bInitialized = true;
	return S_OK;
}


STDMETHODIMP AdManager::showAdBanner(void)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// First of all, check if the initialization works fine
	if ( !m_bInitialized )
	{
		return S_FALSE;
	}

	// Before going further..... check if there is Internet connection
	DWORD	dwResult;
	if ( !InternetGetConnectedState( &dwResult, 0 ) )
	{
		return S_FALSE;
	}

	CWnd	*pMainFrm = AfxGetMainWnd();

	// Init the OLE subsystem..... is it really required???
	HRESULT	hRes;
	hRes = ::OleInitialize( NULL );
	if ( FAILED( hRes ) )
	{
		return ERROR_INVALID_HANDLE;
	}

	// Get the main frame rect and calculate the browser rect
	CRect	rcWnd;
	pMainFrm->GetClientRect( rcWnd );

	// Create the browser window
	InAppContainerWnd	*pBrowserWnd = new InAppContainerWnd( this );
	pBrowserWnd->setContentUrl( m_contentUrl );
	pBrowserWnd->create( pMainFrm->GetSafeHwnd(), rcWnd );

	return S_OK;
}


// Obtain the API KEY from the registry
std::wstring AdManager::obtainApiKey()
{
	// Try to open the registry key that holds the API KEY value
	std::wstring	apiKey;
	HKEY			hKey = NULL;
	LONG			result;
	result = RegOpenKeyEx( HKEY_CURRENT_USER, INAPP_REG_MAINKEY, 0, KEY_READ, &hKey );
	if ( result != ERROR_SUCCESS )
	{
		return apiKey;
	}

	// Try to obtain the API KEY value
	DWORD	dwSize = 0;
	result = RegQueryValueEx( hKey, INAPP_REG_APIKEY, NULL, NULL, NULL, &dwSize );
	if ( result != ERROR_SUCCESS )
	{
		return apiKey;
	}

	// Allocate a temporary buffer to hold the data from the registry
	LPBYTE	pBuffer = new BYTE[ dwSize ];
	if ( pBuffer == NULL )
	{
		return apiKey;
	}

	// Init the buffer and obtain the value from the registry
	ZeroMemory( pBuffer, dwSize );
	result = RegQueryValueEx( hKey, INAPP_REG_APIKEY, NULL, NULL, pBuffer, &dwSize );
	if ( result == ERROR_SUCCESS )
	{
		apiKey = reinterpret_cast< wchar_t* >( pBuffer );
	}

	// Deallocate the buffer...
	delete [] pBuffer;

	// ...close the registry...
	RegCloseKey( hKey );

	// ...and finally return the API KEY.
	return apiKey;
}


std::wstring AdManager::getApiKey()
{
	return m_apiKey;
}


bool AdManager::storeApiKey( std::wstring& apiKey )
{
	// Try to open or create the registry key that holds the API KEY value.
	HKEY			hKey = NULL;
	LONG			result;
	result = RegOpenKeyEx( HKEY_CURRENT_USER, INAPP_REG_MAINKEY, 0, KEY_WRITE, &hKey );
	if ( result != ERROR_SUCCESS )
	{
		return false;
	}

	// Try to obtain the API KEY value.
	DWORD	dwSize = 0;
	result = RegSetValueEx( hKey, INAPP_REG_APIKEY, NULL, REG_SZ, reinterpret_cast< const BYTE* >( apiKey.c_str() ), apiKey.length() * sizeof( wchar_t ) );
	if ( result != ERROR_SUCCESS )
	{
		return false;
	}

	// Close the registry.
	RegCloseKey( hKey );

	return true;
}


void AdManager::obtainContentThread( void* pCookie )
{
	AdManager*	pThis = reinterpret_cast< AdManager* >( pCookie );
	if ( pThis != NULL )
	{
		TCHAR		tempFile[ MAX_PATH ];
		GetTempPath( MAX_PATH, tempFile );
		PathAppend( tempFile, L"adContent.html" );

		// Do the request
		HTTPConnection	conn;
		std::string		response;
		if ( conn.get( pThis->m_contentUrl, response ) )
		{
			std::ofstream	ofs( tempFile );
			ofs.write( response.c_str(), response.length() );
			ofs.close();

			pThis->m_contentUrl = tempFile;
		}
	}
}
