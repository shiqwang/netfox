#include "Stdafx.h"
#include <AfxDllx.h>

//组件头文件
#include <WinSock2.h>
#include "ServerKernel.h"

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE ServerKernelDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ServerKernelDLL,hInstance)) return 0;
		new CDynLinkLibrary(ServerKernelDLL);

		//初始化 COM
		CoInitialize(NULL);

		//初始化 SOCKET
		WSADATA WSAData;
		WORD wVersionRequested=MAKEWORD(2,2);
		int iErrorCode=WSAStartup(wVersionRequested,&WSAData);
		if (iErrorCode!=0) return 0;
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		CoUninitialize();
		AfxTermExtensionModule(ServerKernelDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
