// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "inApp_i.h"
#include "dllmain.h"

CinAppModule _AtlModule;

class CinAppApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CinAppApp, CWinApp)
END_MESSAGE_MAP()

CinAppApp theApp;

BOOL CinAppApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CinAppApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
