#include "Stdafx.h"
#include "Resource.h"
#include "GameFrameApp.h"
#include "GlobalOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrameApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFrameApp::CGameFrameApp()
{
	m_pGameFrame=NULL;
}

//析构函数
CGameFrameApp::~CGameFrameApp()
{
}

//初始化函数
BOOL CGameFrameApp::InitInstance()
{
	__super::InitInstance();

	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCGameFrameApp.log",strTime);

	strTemp.Format("into InitInstance");
	WriteLog(strFile, strTemp);

	//内部初始化
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	strTemp.Format("into InitInstance 1");
	WriteLog(strFile, strTemp);

	//初始化界面库
	IUnknownEx * pIUnknownEx=(IUnknownEx *)m_SkinRecordFile.QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
	CSkinAttribute::LoadSkinAttribute(pIUnknownEx);

	//加载配置
	g_GlobalOption.LoadOptionParameter();

	strTemp.Format("into InitInstance 2");
	WriteLog(strFile, strTemp);

	try
	{
		//创建主窗口
		m_pGameFrame=GetGameFrameDlg();
		ASSERT(m_pGameFrame!=NULL);
		if (m_pGameFrame==NULL) throw TEXT("游戏主窗口创建失败");

		//设置主窗口
		m_pMainWnd=m_pGameFrame;
		AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
		m_pGameFrame->Create(IDD_GAME_FRAME,NULL);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	catch (LPCTSTR pszMesssage)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("由于 [ %s ] 游戏程序即将退出。"),pszMesssage);
		AfxMessageBox(strBuffer,MB_ICONSTOP);
		if (m_pGameFrame!=NULL) SafeDelete(m_pGameFrame);
		return FALSE;
	}

	return TRUE;
}

//退出消息
int CGameFrameApp::ExitInstance()
{
	//删除窗口
	if (m_pGameFrame!=NULL) SafeDelete(m_pGameFrame);
	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
