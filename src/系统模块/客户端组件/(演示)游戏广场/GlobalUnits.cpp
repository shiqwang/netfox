#include "StdAfx.h"
#include "GlobalUnits.h"
#include "DlgControlBar.h"

//////////////////////////////////////////////////////////////////////////

//下载类型
#define DTP_GAME_CLIENT			1									//下载类型

//全局变量
CGlobalUnits					g_GlobalUnits;						//信息组件			
CGlobalAttemper					g_GlobalAttemper;					//全局调度

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUnits::CGlobalUnits(void)
{
	m_szDirSkin[0]=0;
	m_szDirWork[0]=0;
	m_dwPlazaVersion=VER_PLAZA_FRAME;
	memset(&m_GloblaUserData,0,sizeof(m_GloblaUserData));
	lstrcpyn(m_szStationPage,szStationPage,sizeof(m_szStationPage));
	
	return;
}

//析构函数
CGlobalUnits::~CGlobalUnits(void)
{
}

//初始化函数
bool CGlobalUnits::InitGlobalUnits()
{
	//设置随机种子
	srand((UINT)time(NULL));

	//设置工作目录
	GetModuleFileName(AfxGetInstanceHandle(),m_szDirWork,sizeof(m_szDirWork));
	int nModuleLen=lstrlen(m_szDirWork);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	m_szDirWork[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(m_szDirWork);

	//设置界面目录
	_snprintf(m_szDirSkin,sizeof(m_szDirSkin),TEXT("%s\\Skin"),m_szDirWork);

	//加载组件
	if (m_UserFaceRes.CreateInstance()==false) return false;
	if (m_DownLoadService.CreateInstance()==false) return false;
	if (m_CompanionManager.CreateInstance()==false) return false;
	if (m_GameRankManager.CreateInstance()==false) return false;

	//初始化组件
	m_CompanionManager->LoadCompanion();

	return true;
}

//拷贝字符
bool CGlobalUnits::CopyToClipboard(LPCTSTR pszString)
{
	//变量定义
	HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();

	//打开剪切板
	if (OpenClipboard(hWnd)==FALSE) return false;
	if (EmptyClipboard()==FALSE) { CloseClipboard(); return false; }

	//复制数据
	HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,CountString(pszString));
	if (hData==NULL) 
	{
		CloseClipboard();
		return false;
	}

	//复制数据
	LPSTR pszMemString=(LPSTR)GlobalLock(hData);
	lstrcpy(pszMemString,pszString);
	SetClipboardData(CF_TEXT,hData);

	//环境设置
	GlobalUnlock(hData);
	CloseClipboard();

	return true;
}

//获取序列
void CGlobalUnits::GetClientSerial(tagClientSerial & ClientSerial)
{
	return;
}

//设置主站地址
LPCTSTR CGlobalUnits::SetStationPage(LPCTSTR pszStationPage)
{
	ASSERT(pszStationPage!=NULL);
	lstrcpyn(m_szStationPage,pszStationPage,sizeof(m_szStationPage));
	return m_szStationPage;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalAttemper::CGlobalAttemper(void)
{
}

//析构函数
CGlobalAttemper::~CGlobalAttemper(void)
{
}

//显示提示
bool CGlobalAttemper::ShowStatusMessage(LPCTSTR pszMessage, CWnd * pNotifyWnd)
{
	return m_DlgStatus.ShowStatusMessage(pszMessage,pNotifyWnd);
}

//关闭状态
bool CGlobalAttemper::DestroyStatusWnd(CWnd * pNotifyWnd)
{
	return m_DlgStatus.DestroyStatusWnd(pNotifyWnd);
}

//下载游戏
bool CGlobalAttemper::DownLoadClient(LPCTSTR pszKindName, WORD wKindID, bool bDisplay)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sDownLoadClient.log",strTime);

	//构造数据
	tagDownLoadRequest DownLoadRequest;
	memset(&DownLoadRequest,0,sizeof(DownLoadRequest));
	DownLoadRequest.bDisplay=bDisplay;
	lstrcpyn(DownLoadRequest.szDescribe,pszKindName,CountArray(DownLoadRequest.szDescribe));
	_snprintf(DownLoadRequest.szFileName,sizeof(DownLoadRequest.szFileName),TEXT("%s.EXE"),pszKindName);
	_snprintf(DownLoadRequest.szLocalPath,sizeof(DownLoadRequest.szLocalPath),TEXT("%s\\DownLoad"),g_GlobalUnits.GetWorkDirectory());

	_snprintf(DownLoadRequest.szDownLoadUrl,sizeof(DownLoadRequest.szDownLoadUrl),TEXT("%s/GameDownload.asp?KindID=%ld&LocalVersion=0&PlazaVersion=%ld"), Glb().m_MainHomeUrl, wKindID,VER_PLAZA_FRAME);

	strTemp.Format("=%s", DownLoadRequest.szDownLoadUrl);
	theApp.WriteLog(strFile, strTemp);
	//投递请求
	DWORD dwDownLoadID=g_GlobalUnits.m_DownLoadService->AddDownLoadRequest(DTP_GAME_CLIENT,&DownLoadRequest);

	return true;
}

//////////////////////////////////////////////////////////////////////////
