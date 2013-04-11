#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE SocketModuleDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(SocketModuleDLL, hInstance)) return 0;
		new CDynLinkLibrary(SocketModuleDLL);
		if (!AfxSocketInit()) {	return 0; }
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(SocketModuleDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
