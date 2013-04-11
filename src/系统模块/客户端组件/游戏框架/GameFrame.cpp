#include "Stdafx.h"
#include <AfxDLLx.h>
#include "GameFrame.h"

static AFX_EXTENSION_MODULE GameFrameDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL Ö÷º¯Êý
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(GameFrameDLL, hInstance)) return 0;
		new CDynLinkLibrary(GameFrameDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(GameFrameDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
