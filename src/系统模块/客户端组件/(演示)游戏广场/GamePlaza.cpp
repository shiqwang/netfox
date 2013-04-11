#include "Stdafx.h"
#include "GamePlaza.h"
#include "GameFrame.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGamePlazaApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//应用程序对象说明
CGamePlazaApp theApp;

//构造函数
CGamePlazaApp::CGamePlazaApp()
{
}

//初始化函数
BOOL CGamePlazaApp::InitInstance()
{
	__super::InitInstance();

	//判断是否重入
#ifndef ADMIN_PLAZA
#ifndef _DEBUG
	bool bExistIGame=false;
	CMutex Mutex(FALSE,MAIN_DLG_CLASSNAME,NULL);
	if (Mutex.Lock(0)==FALSE) bExistIGame=true;
	CWnd * pIGameWnd=CWnd::FindWindow(MAIN_DLG_CLASSNAME,NULL);
	if (pIGameWnd!=NULL) 
	{
		bExistIGame=true;
		if (pIGameWnd->IsIconic()) pIGameWnd->ShowWindow(SW_RESTORE);
		pIGameWnd->SetActiveWindow();
		pIGameWnd->BringWindowToTop();
		pIGameWnd->SetForegroundWindow();
	}
	if (bExistIGame==true) return FALSE;
#endif
#endif

	DWORD d=MAKELONG(MAKEWORD(6,0),MAKEWORD(1,0));

	//程序初始化
	AfxOleInit();
	AfxInitRichEdit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szSystemRegKey);

	//加载界面库
	CSkinAttribute::LoadSkinAttribute(GET_OBJECT_INTERFACE(m_SkinRecordFile,IUnknownEx));

	//加载配置
	g_GlobalOption.LoadOptionParameter();

	//全局信息
	if (g_GlobalUnits.InitGlobalUnits()==false)
	{
		AfxMessageBox(TEXT("游戏广场初始化失败，程序即将退出！"),MB_ICONSTOP);
		return FALSE;
	}

	//注册窗口类
	WNDCLASS WndClasss;
	memset(&WndClasss,0,sizeof(WndClasss));
	WndClasss.lpfnWndProc=::DefWindowProc;
	WndClasss.hInstance=AfxGetInstanceHandle();
	WndClasss.lpszClassName=MAIN_DLG_CLASSNAME;
	if (!AfxRegisterClass(&WndClasss)) AfxThrowResourceException();

	//建立窗口
	const DWORD dwStyte=WS_SYSMENU|WS_MINIMIZEBOX|WS_OVERLAPPED|WS_CLIPCHILDREN;
	CGameFrame * pMainFrame=new CGameFrame();
	pMainFrame->Create(MAIN_DLG_CLASSNAME,TEXT(""),dwStyte,CRect(0,0,0,0));
	m_pMainWnd=pMainFrame;

	return TRUE;
}

void CGamePlazaApp::WriteLog(CString strFileName, CString strText)
{
	try
	{
        CTime tm = CTime::GetCurrentTime();
        CString strTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
		//BOOL bFull = FALSE;
		CStdioFile file;
		if( file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite) != 0)
		{
			file.SeekToEnd();
			file.WriteString(strTime);
			file.WriteString(strText);
			file.WriteString(_T("\n\n"));
			//if(file.GetLength() > 2000000)
			//	bFull = TRUE;
			file.Close();
		}
		/*
		if(!bFull) return;
		if( file.Open(strFileName, CFile::modeCreate|CFile::modeReadWrite) != 0)
		{
		file.SeekToEnd();
		file.WriteString(strTime);
		file.WriteString(strText);
		file.WriteString(_T("\n"));
		file.Close();
		}
		*/
	}
	catch(...)
	{
	}
}

//////////////////////////////////////////////////////////////////////////

