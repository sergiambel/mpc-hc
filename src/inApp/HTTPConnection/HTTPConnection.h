#pragma once

#include <map>
#include <string>

class HTTPConnection
{
public:
											HTTPConnection();
	virtual									~HTTPConnection();

	void									setTimeout( long value )					{ timeout = value; }
	long									getTimeout()								{ return timeout; }

	void									setUserAgent( const std::wstring& value )	{ userAgent = value; }
	std::wstring							getUserAgent()								{ return userAgent; }

	void									addExtraHeader( const std::wstring& property, const std::wstring& value );
	void									clearExtraHeaders();

	bool									get( const std::wstring& url, std::string& response );
	bool									post( const std::wstring& url, std::wstring& data, std::string& response );

protected:
	// Helper methods	
	std::wstring							obtainExtraHeaders();
	bool									splitUrl( const std::wstring& url, std::wstring& server, std::wstring& page, bool& isHttps );


	// Timeout, in seconds
	long									timeout;

	// User agent to be used in the connections
	std::wstring							userAgent;

	// Additional headers map
	std::map< std::wstring, std::wstring >	extraHeaders;
};
