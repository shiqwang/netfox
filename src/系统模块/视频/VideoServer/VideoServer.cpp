// VideoServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MyUserManage.h"
#include "..\Include\Common.h"
#include "..\Include\IniFile.h"
#include "UserProcess.h"

extern ROOM_ID_LIST m_RoomIDList;
extern CRITICAL_SECTION g_CheckCritical; //临界变量

int _tmain(int argc, _TCHAR* argv[])
{
	//检测句柄
	if( CheckInstance( _T("VideoServer") ) )
		exit(0);
	
	InitializeCriticalSection( &g_CheckCritical );
	//配置文件
	TCHAR path[MAX_PATH];
	GetProgPath( path, _T("VideoServer.ini")  );
	CIniFile IniFile;
	IniFile.Load( path );

	//坚听端口
	unsigned int Port = IniFile.GetInt( _T("SETUP"), _T("PORT") );


	CUdpManage UdpManage;
	UdpManage.Init( 1, 1 );
	UdpManage.CreateSocket<CMyUserManage>( Port );

	//消息循环
	MSG msg;
	while(true)
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	m_RoomIDList.DeleteAllData();
	DeleteCriticalSection( &g_CheckCritical ); //删掉临界变量

	return 0;
}

