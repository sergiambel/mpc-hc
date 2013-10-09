// dllmain.h : Declaration of module class.

class CinAppModule : public ATL::CAtlDllModuleT< CinAppModule >
{
public :
	DECLARE_LIBID(LIBID_inAppLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_INAPP, "{FD91EAA7-F7AF-4690-9DDC-99E032091033}")
};

extern class CinAppModule _AtlModule;
