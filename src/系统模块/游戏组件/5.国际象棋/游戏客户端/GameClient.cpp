#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//应用程序对象
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientApp::CGameClientApp()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCGameClientApp.log",strTime);

	strTemp.Format("into CGameClientApp()");
	WriteLog(strFile, strTemp);
}

//析构函数
CGameClientApp::~CGameClientApp() 
{
}

//////////////////////////////////////////////////////////////////////////
