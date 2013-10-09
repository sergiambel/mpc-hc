// AdManager.h : Declaration of the AdManager

#pragma once
#include "resource.h"       // main symbols



#include "inApp_i.h"

#include <string>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// AdManager

class ATL_NO_VTABLE AdManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<AdManager, &CLSID_AdManager>,
	public IDispatchImpl<IAdManager, &IID_IAdManager, &LIBID_inAppLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	AdManager() 
		: m_bInitialized( false )
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ADMANAGER)


BEGIN_COM_MAP(AdManager)
	COM_INTERFACE_ENTRY(IAdManager)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		// Really important stuff... Initialize the FreeImage library.
		FreeImage_Initialise();
		return S_OK;
	}


	void FinalRelease()
	{
		// Uninitialize the FreeImage library.
		FreeImage_DeInitialise();
	}

public:
	STDMETHOD(init)(void);
	STDMETHOD(showAdBanner)(void);

	std::wstring		getApiKey();

protected:
	std::wstring		obtainApiKey();
	bool				storeApiKey( std::wstring& apiKey );

	static void			obtainContentThread( void* pCookie ); 

	bool				m_bInitialized;
	std::wstring		m_apiKey;
	std::wstring		m_contentUrl;
};

OBJECT_ENTRY_AUTO(__uuidof(AdManager), AdManager)
