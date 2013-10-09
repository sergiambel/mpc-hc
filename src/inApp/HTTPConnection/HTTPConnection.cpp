#include "HTTPConnection.h"

#include <Windows.h>
#include <WinInet.h>
#include <algorithm>
#include <vector>

HTTPConnection::HTTPConnection()
	: timeout( 3 )
{

}


HTTPConnection::~HTTPConnection()
{

}


void HTTPConnection::addExtraHeader( const std::wstring& property, const std::wstring& value )
{
	extraHeaders[property] = value;
}


void HTTPConnection::clearExtraHeaders()
{
	extraHeaders.clear();
}


bool HTTPConnection::get( const std::wstring& url, std::string& response )
{
	bool			result = false;

	// Obtain the extra headers
	std::wstring		headers = obtainExtraHeaders();

	// Split the incoming URL
	std::wstring		server;
	std::wstring		page;
	bool			isHttps = false;
	if ( !splitUrl( url, server, page, isHttps ) )
	{
		return result;
	}

	// Open the internet connection
	HINTERNET	hInet1 = InternetOpen( userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( hInet1 ==NULL )
	{
		return result;
	}

	// Connect to the server
	HINTERNET	hInet2 = InternetConnect( hInet1, server.c_str() , isHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
	if ( hInet2 == NULL) 
	{
		InternetCloseHandle( hInet1 );
		return result;
	}

	// Setup the request flags
	DWORD dwRequestFlags = INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
	if ( isHttps ) 
	{
		dwRequestFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_SECURE;
	}

	// Perform the request to the page
	HINTERNET	hInet3 = HttpOpenRequest( hInet2, L"GET", page.c_str(), NULL, url.c_str(), NULL, dwRequestFlags, NULL );
	if ( hInet3 == NULL )
	{
		InternetCloseHandle( hInet2 );
		InternetCloseHandle( hInet1 );
		return result;
	}

	// Add the additional request headers
	HttpAddRequestHeaders( hInet3, headers.c_str(), headers.length(), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD );

	// Do required additional stuff on HTTPS connections
	if ( isHttps )
	{
		DWORD	dwResult;
		DWORD	dwSize = sizeof( DWORD );
		InternetQueryOption( hInet3, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID) &dwResult, &dwSize );
		dwResult |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_SECURE;
		InternetSetOption( hInet3, INTERNET_OPTION_SECURITY_FLAGS, &dwResult, sizeof(DWORD) );
	}

	// Send the request
	if ( !HttpSendRequest( hInet3, headers.c_str(), headers.length(), NULL, 0 ) )
	{
		InternetCloseHandle( hInet3 );
		InternetCloseHandle( hInet2 );
		InternetCloseHandle( hInet1 );
		return result;
	}

	char				temp[8192];
	std::vector< char >	buffer;
	DWORD				dwRead = 0;
	while ( InternetReadFile( hInet3, temp, 8192, &dwRead ) && ( dwRead >0 ) )
	{
		buffer.insert( buffer.end(), temp, temp + dwRead );
	}

	if ( !buffer.empty() )
	{
		buffer.push_back(0);
		response = buffer.data();
		result = true;
	}

	InternetCloseHandle( hInet3 );
	InternetCloseHandle( hInet2 );
	InternetCloseHandle( hInet1 );
	return result;
}


bool HTTPConnection::post( const std::wstring& url, std::wstring& data, std::string& response )
{
	return false;

}



std::wstring HTTPConnection::obtainExtraHeaders()
{
	std::wstring		result;
	std::for_each( extraHeaders.begin(), extraHeaders.end(), [ &result ]( std::pair< const std::wstring, std::wstring > entry) 
	{
		result += entry.first + L":" + entry.second + L"\r\n";
	});
	return result;
}


bool HTTPConnection::splitUrl( const std::wstring& url, std::wstring& server, std::wstring& page, bool& isHttps )
{
	// Check if the current input url is HTTP or HTTPS
	std::wstring	prefix = L"https://";
	size_t			prefixPos = url.find( prefix );
	if ( prefixPos == std::wstring::npos )
	{
		prefix = L"http://";
		prefixPos = url.find( prefix );
		if ( prefixPos == std::wstring::npos )
		{
			return false;
		}
		isHttps = false;
	}
	else
	{
		isHttps = true;
	}

	// Extract the server name and page
	size_t			splitPos = url.find( '/', prefixPos + prefix.length() );
	if ( splitPos == std::wstring::npos )
	{
		server = url.substr( prefixPos + prefix.length() );
		page = L"";
	}
	else
	{
		server = url.substr( prefixPos + prefix.length(), splitPos - prefixPos - prefix.length() );
		page = url.substr( splitPos + 1 );
	}

	return true;
}
