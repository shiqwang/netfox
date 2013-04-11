#include "Stdafx.h"
#include "Mmsystem.h"
#include "Resource.h"
#include "GameFrameDlg.h"
#include "GlobalOption.h"
#include "GameFrameControl.h"
#include "ClientKernelSink.h"

#include "FlyGameSound.h"

#define MESSAGE_CHOOSE_OBJ       WM_USER+52 

CFlyGameSound  g_myCFlyGameSound;
const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;
int g_PrintLogFile=1;
void WriteLog(CString strFileName, CString strText)
{
	//判断是否打印日志文件
	if ( g_PrintLogFile != 1)
		return;

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

BEGIN_MESSAGE_MAP(CGameFrameDlg, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BUTTON_HIDE_LIST, OnBnClickedHideList)
	ON_BN_CLICKED(IDC_BUTTON_YU_YIN, OnBnClickedYuYin)
	ON_BN_CLICKED(IDC_GAME_RULE, OnBnClickedRule)
	ON_BN_CLICKED(IDC_GAME_OPTION, OnBnClickedOption)
	ON_BN_CLICKED(IDC_QUIT_GAME, OnBnClickedQuitGame)	
	ON_MESSAGE(WM_PLAY_DT_SOUND,OnSendDTSound)

   ON_MESSAGE( MESSAGE_CHOOSE_OBJ,MyWndProc)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFrameDlg::CGameFrameDlg(CGameFrameView * pGameFrameView)  : CSkinDialog(IDD_GAME_FRAME), m_VorSplitter(VorSpliter)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCGameFrameDlg.log",strTime);

	strTemp.Format("into CGameFrameDlg");
	WriteLog(strFile, strTemp);

	isHaveSetupAV = 0;

	m_bHideList=false;
	pOneUserInfoDlg=NULL;

	//设置变量
	m_bReset=false;
	m_bRectify=false;
	m_bInitDialog=false;
	m_pKernelSink=NULL;
	m_pGameFrameControl=NULL;
	m_pGameFrameView=pGameFrameView;

	//配置变量
	m_bAllowSound=true;

	//创建钩子
	m_pKernelSink=new CClientKernelSink(*this);

	strTemp.Format("out CGameFrameDlg");
	WriteLog(strFile, strTemp);

	return;
}

//析构函数
CGameFrameDlg::~CGameFrameDlg()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnBnClickedQuitGame.log",strTime);

	strTemp.Format("%s", "析构函数()");
	WriteLog(strFile, strTemp);

	FlyAVClientSendUnRegMsg();

	strTemp.Format("%s", "FlyAVClientSendUnRegMsg()");
	WriteLog(strFile, strTemp);

	//FreeVideoDLL();

	strTemp.Format("%s", "FreeVideoDLL()");
	//WriteLog(strFile, strTemp);

	SafeDelete(m_pKernelSink);
	SafeDelete(m_pGameFrameControl);
	return;
}

//按钮消息
bool CGameFrameDlg::OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)
{
	return true;
}

//拆分条消息
bool CGameFrameDlg::OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}

//调整界面
void CGameFrameDlg::RectifyControl(int nWidth, int nHeight)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sRectifyControl.log",strTime);

	strTemp.Format("RectifyControl nWidth=%d  nHeight=%d", nWidth,  nHeight);
	WriteLog(strFile, strTemp);

	//状态判断
	if (m_bInitDialog==false) return;

	//变量定义
	CRect rcVorSplitter;
	const int nSplitterWidth=0;
	const int iXExc=GetXExcursionPos();
	const int iYExc=GetYExcursionPos();
	//	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//获取位置
	m_VorSplitter.GetWindowRect(&rcVorSplitter);
	int addInt = 50;//左移50
	//移动控件
	int iBaseXPos=rcVorSplitter.right;//左边最大值

	ScreenToClient(&rcVorSplitter);

	//调整拆分条
	if (m_bRectify==false)
	{
		m_bRectify=true;
		rcVorSplitter.left=nWidth*72/100;
		rcVorSplitter.right=rcVorSplitter.left+nSplitterWidth;
	}

	//设置拆分范围
	/*Doctor
	int nLessPos=nWidth*3/5,nMaxPos=nWidth*75/100;
	m_VorSplitter.SetSplitterRange(nLessPos,nMaxPos);
	if ((nWidth!=0)&&(nHeight!=0))
	{
	if (rcVorSplitter.left<nLessPos) 
	{
	rcVorSplitter.left=nLessPos;
	rcVorSplitter.right=rcVorSplitter.left+nSplitterWidth;
	}
	if (rcVorSplitter.right>nMaxPos)
	{
	rcVorSplitter.right=nMaxPos;
	rcVorSplitter.left=rcVorSplitter.right-nSplitterWidth;
	}
	}
	*/
	//锁定屏幕
	LockWindowUpdate();

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	DeferWindowPos(hDwp,m_pGameFrameView->GetSafeHwnd(),NULL,iXExc,iYExc,rcVorSplitter.left-iXExc,nHeight-iYExc,uFlags);
	DeferWindowPos(hDwp,m_VorSplitter,NULL,rcVorSplitter.left,iYExc,rcVorSplitter.Width(),nHeight-iYExc,uFlags);
	DeferWindowPos(hDwp,m_pGameFrameControl->GetSafeHwnd(),NULL,rcVorSplitter.right,iYExc,nWidth-rcVorSplitter.right,nHeight-iYExc,uFlags);


	//隐藏按钮  iBaseXPos-SPLIT_WIDE+addInt
	//功能按钮
	CRect rcButton;
	int nYPosButton=nHeight-30;
	int nBeginPos=nWidth-220,nButtonSpace=0;
	m_btGameOption.GetWindowRect(&rcButton);

	int myLeftInt,myLeftInt1, myLeftInt2;
	int btnW = 24;
	int btnW1 = 20;
	int btnW2 = rcButton.Width();

	if ( (nWidth-iBaseXPos) > 100)
	{
		myLeftInt = rcVorSplitter.right- btnW;//-SPLIT_WIDE+addInt;//展开
		myLeftInt1 =rcVorSplitter.right- btnW1 ;
		myLeftInt2 = rcVorSplitter.right- btnW2;//iBaseXPos- btnW2 ;
	}
	else
	{
		myLeftInt =rcVorSplitter.right- btnW ;
		myLeftInt1 =rcVorSplitter.right- btnW1 ;
		myLeftInt2 = rcVorSplitter.right- btnW2 ;//iBaseXPos- btnW2 ;
	}

	//	DeferWindowPos(hDwp,m_BtHideList,NULL,myLeftInt,TITLE_HIGHT,9,iHeight,uFlags);
	//	DeferWindowPos(hDwp,m_BtHideList,NULL,myLeftInt,TITLE_HIGHT,0,0,uFlags);
	strTemp.Format("m_BtHideList %d  %d iBaseXPos=%d", myLeftInt,  TITLE_HIGHT, iBaseXPos);
	WriteLog(strFile, strTemp);
	m_BtHideList.MoveWindow( myLeftInt, 180+TITLE_HIGHT,  btnW, 69);
	m_BtYuYin.MoveWindow(myLeftInt1, 275+TITLE_HIGHT, btnW1, 92);

	showDlgX=myLeftInt - 265;
	showDlgY=275 - 377/2;

	int ctrlXInt = myLeftInt2;

	int ctrlYInt = 20+TITLE_HIGHT;
	DeferWindowPos(hDwp,m_btGameRule,NULL, ctrlXInt, ctrlYInt,rcButton.Width(),rcButton.Height(),uFlags);

	ctrlYInt = 60+TITLE_HIGHT;
	DeferWindowPos(hDwp,m_btGameOption,NULL, ctrlXInt, ctrlYInt,rcButton.Width(),rcButton.Height(),uFlags);


	EndDeferWindowPos(hDwp);

	//重画界面
	Invalidate(FALSE);
	UpdateWindow();

	//解除锁定
	UnlockWindowUpdate();

	return;
}

//网络消息
bool CGameFrameDlg::OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	return false;
}

//发送函数
bool CGameFrameDlg::SendData(WORD wSubCmdID)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendSocketData(MDM_GF_GAME,wSubCmdID);
}

//发送函数
bool CGameFrameDlg::SendData(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendSocketData(MDM_GF_GAME,wSubCmdID,pData,wDataSize);
}

//发送准备
bool CGameFrameDlg::SendUserReady(void * pBuffer, WORD wDataSize)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);

	if ( isHaveSetupAV == 0 )
	{
		SetupVideoDLL();
	}
	else
	{
		SetMyselfAVClientPostion();
		FlyAVClientSendRegMsg();
	}
	return m_ClientKernelHelper->SendUserReady(pBuffer,wDataSize);
}

//设置定时器
bool CGameFrameDlg::SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SetGameTimer(wChairID,nTimerID,nElapse);
}

//删除定时器
bool CGameFrameDlg::KillGameTimer(UINT nTimerID)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->KillGameTimer(nTimerID);
}

//播放声音
bool CGameFrameDlg::PlayGameSound(LPCTSTR pszSoundName)
{
	if ((m_bAllowSound==true)&&(IsWindowVisible()))
	{
		::PlaySound(pszSoundName,AfxGetInstanceHandle(),SND_ASYNC|SND_NODEFAULT);
		return true;
	}

	return false;
}

//播放声音
bool CGameFrameDlg::PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName)
{
	if ((m_bAllowSound==true)&&(IsWindowVisible()))
	{
		//加载资源
		HRSRC hResour=FindResource(hInstance,pszSoundName,TEXT("WAVE"));
		if (hResour==NULL) return false;
		HGLOBAL hGlobal=LoadResource(hInstance,hResour);
		if (hGlobal==NULL) return false;

		//播放声音
		LPCTSTR pszMemory=(LPCTSTR)LockResource(hGlobal);
		::PlaySound(pszMemory,hInstance,SND_ASYNC|SND_MEMORY);

		//清理资源
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}

	return true;
}

//插入图片
bool CGameFrameDlg::InsertImage(CBitmap * pBitmap)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertImage(pBitmap);
}

//插入名字
bool CGameFrameDlg::InsertUserName(LPCTSTR pszUserName)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertUserName(pszUserName);
}

//系统消息
bool CGameFrameDlg::InsertSystemString(LPCTSTR pszString)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertSystemString(pszString,MS_NORMAL);
}

//特色消息
bool CGameFrameDlg::InsertShtickString(LPCTSTR pszString, COLORREF crColor, bool bNewLine)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertShtickString(pszString,crColor,MS_NORMAL,bNewLine);
}

//普通消息
bool CGameFrameDlg::InsertGeneralString(LPCTSTR pszString, COLORREF crColor, bool bNewLine)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertGeneralString(pszString,crColor,MS_NORMAL,bNewLine);
}

//激活界面
void CGameFrameDlg::ActiveGameFrame()
{
	BringWindowToTop(); 
	SetForegroundWindow();
	return;
}

//切换椅子
WORD CGameFrameDlg::SwitchViewChairID(WORD wChairID)
{
	//变量定义
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	const tagUserData * pMeUserData=m_ClientKernelHelper->GetMeUserInfo();
	const tagServerAttribute * pServerAttribute=m_ClientKernelHelper->GetServerAttribute();

	//转换椅子
	WORD wViewChairID=(wChairID+pServerAttribute->wChairCount-pMeUserData->wChairID);
	switch (pServerAttribute->wChairCount)
	{
	case 2: { wViewChairID+=1; break; }
	case 3: { wViewChairID+=1; break; }
	case 4: { wViewChairID+=2; break; }
	case 5: { wViewChairID+=2; break; }
	case 6: { wViewChairID+=3; break; }
	case 7: { wViewChairID+=3; break; }
	case 8: { wViewChairID+=4; break; }
	}
	return wViewChairID%pServerAttribute->wChairCount;
}

//是否启用
bool CGameFrameDlg::IsEnableSound()
{
	return m_bAllowSound;
}

//启用声音
void CGameFrameDlg::EnableSound(bool bEnable)
{
	//设置变量
	m_bAllowSound=bEnable;

	//读写注册
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("EnableSound"),m_bAllowSound);

	return;
}

//游戏状态
BYTE CGameFrameDlg::GetGameStatus()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return GS_FREE;
	return m_ClientKernelHelper->GetGameStatus();
}

//旁观模式
bool CGameFrameDlg::IsLookonMode()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return true;
	return m_ClientKernelHelper->IsLookonMode();
}

//允许旁观
bool CGameFrameDlg::IsAllowLookon()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return false;
	return m_ClientKernelHelper->IsAllowLookon();

}

//游戏状态
void CGameFrameDlg::SetGameStatus(BYTE bGameStatus)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return;
	m_ClientKernelHelper->SetGameStatus(bGameStatus);
	return;
}

//自己位置
WORD CGameFrameDlg::GetMeChairID()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;
	return m_ClientKernelHelper->GetMeChairID();
}

//时间位置
WORD CGameFrameDlg::GetTimeChairID()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;
	return m_ClientKernelHelper->GetTimeChairID();
}

//获取用户
const tagUserData * CGameFrameDlg::GetUserData(WORD wChairID)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return NULL;
	return m_ClientKernelHelper->GetUserInfo(wChairID);
}

//内核接口
void * CGameFrameDlg::GetClientKernel(const IID & Guid, DWORD dwQueryVer)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return NULL;
	return m_ClientKernelHelper->QueryInterface(Guid,dwQueryVer);
}

//控件绑定
void CGameFrameDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VOR_SPLITER, m_VorSplitter);
	DDX_Control(pDX, IDC_BUTTON_HIDE_LIST, m_BtHideList);
	DDX_Control(pDX, IDC_BUTTON_YU_YIN, m_BtYuYin);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_showSelfVideo);

	DDX_Control(pDX, IDC_STATIC_VIDEO_REMOTE, m_showRemoteVideo);

	DDX_Control(pDX, IDC_STATIC_VIDEO_REMOTE1, m_showRemoteVideo1);
	DDX_Control(pDX, IDC_STATIC_VIDEO_REMOTE2, m_showRemoteVideo2);

	DDX_Control(pDX, IDC_STATIC_VIDEO_REMOTE3, m_showRemoteVideo3);

	//	CLabelEx  ;  
	
	DDX_Control(pDX, IDC_STATIC_MYSELF, m_MyselfOpKuang);
	DDX_Control(pDX, IDC_STATIC_ONE, m_OpKuang);
	DDX_Control(pDX, IDC_STATIC_TWO, m_OpKuang1);
	DDX_Control(pDX, IDC_STATIC_THREE, m_OpKuang2);
	DDX_Control(pDX, IDC_STATIC_FOUR, m_OpKuang3);


	DDX_Control(pDX, IDC_GAME_OPTION, m_btGameOption);
	DDX_Control(pDX, IDC_QUIT_GAME, m_btQuitGame);
	DDX_Control(pDX, IDC_GAME_RULE, m_btGameRule);
}

//初始化函数
BOOL CGameFrameDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCGameFrameDlg.log",strTime);

	strTemp.Format("into OnInitDialog");
	WriteLog(strFile, strTemp);


	//设置属性
	ModifyStyle(WS_CAPTION|WS_BORDER,0,0);

	m_bAllowSound=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("EnableSound"),TRUE)?true:false;

	//创建控制
	m_pGameFrameControl=new CGameFrameControl(this);
	if (m_pGameFrameControl==NULL) throw TEXT("游戏框架创建失败");

	strTemp.Format("into OnInitDialog 2");
	WriteLog(strFile, strTemp);


	//创建组件
	if (m_MessageProxyHelper.CreateInstance()==false) throw TEXT("信息组件创建失败");
	m_MessageProxyHelper->SetRichEditHwnd(&m_pGameFrameControl->m_ChatMessage);

	//设置界面接口
	m_VorSplitter.SetSplitterSink(this);
	//	m_VorSplitter.InitSplitBar( );
	if(pOneUserInfoDlg==NULL)
	{
		pOneUserInfoDlg=new CDlgDTSound(this);
		pOneUserInfoDlg->Create(CDlgDTSound::IDD, this);
		if (pOneUserInfoDlg==NULL) throw TEXT("pOneUserInfoDlg创建失败");
	}

	//设置按钮
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_BtHideList.SetButtonImage(IDB_HIDE_LIST,hInstance,false);

	m_BtYuYin.SetButtonImage(IDB_YU_YIN,hInstance,false);

	m_btGameOption.SetButtonImage(IDB_BT_OPTION,hInstance,false);
	m_btQuitGame.SetButtonImage(IDB_BT_QUIT,hInstance,false);
	m_btQuitGame.ShowWindow(false);
	m_btGameRule.SetButtonImage(IDB_BT_GAME_RULE,hInstance,false);

	//创建组件
	if (m_UserFaceResHelper.CreateInstance()==false) throw TEXT("头像组件加载失败");
	if (m_ClientKernelHelper.CreateInstance()==false) throw TEXT("游戏框架内核模块加载失败");

	strTemp.Format("into OnInitDialog 3");
	WriteLog(strFile, strTemp);

	//创建游戏视图
	CRect rcGameView;
	m_pGameFrameControl->Create(IDD_FRAME_CONTROL,this);
	m_pGameFrameView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,this,10);
	if (m_pGameFrameView->SetUserFaceRes(m_UserFaceResHelper.GetInterface())==false) throw TEXT("头像资源接口设置失败");

	//初始化内核
	LPCTSTR lpszCmdLine=AfxGetApp()->m_lpCmdLine;
	bool bSuccess=m_ClientKernelHelper->InitClientKernel(lpszCmdLine,m_pKernelSink);
	if (bSuccess==false) throw TEXT("游戏框架逻辑模块初始化失败");
	bSuccess=m_ClientKernelHelper->SetMessageProxy(m_MessageProxyHelper.GetInterface());
	if (bSuccess==false) throw TEXT("获取消息输出模块失败");

	strTemp.Format("into OnInitDialog 4");
	WriteLog(strFile, strTemp);

	//初始化游戏
	m_pGameFrameControl->SetClientKernel(m_ClientKernelHelper.GetInterface());
	if (InitGameFrame()==false) throw TEXT("游戏框架初始化失败");

	//显示窗口
	m_bInitDialog=true;
	m_pGameFrameView->ShowWindow(SW_SHOW);
	m_pGameFrameControl->ShowWindow(SW_SHOW);

	//显示窗口
	MaxSizeWindow();
	ShowWindow(SW_SHOW);

	//	SetTimer( 1234, 3000, NULL);

	InitUserStatus();
	InitToolsBar();
	InitMyselfToolsBar();


	m_BtHideList.ShowWindow(true);
	m_BtYuYin.ShowWindow(true);

	strTemp.Format("into OnInitDialog 5");
	WriteLog(strFile, strTemp);

	return TRUE;
}

//消息解释
BOOL CGameFrameDlg::PreTranslateMessage(MSG * pMsg)
{
	//过滤消息
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//确定消息
void CGameFrameDlg::OnOK()
{
	return;
}

//取消消息
void CGameFrameDlg::OnCancel()
{
	//退出提示
	if ((m_ClientKernelHelper.GetInterface()!=NULL)&&(m_ClientKernelHelper->GetExitFlags()==false))
	{
		WORD wChairID=GetMeChairID();
		const tagUserData * pUserData=GetUserData(wChairID);
		if (pUserData!=NULL)
		{
			int iRet=AfxMessageBox(TEXT("你正在游戏中，强行退出将被扣分，确实要强退吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
			if (iRet!=IDYES) return;
		}
	}

	//保存配置
	g_GlobalOption.SaveOptionParameter();

	//销毁组件
	SafeDelete(m_pKernelSink);
	m_UserFaceResHelper.CloseInstance();
	m_ClientKernelHelper.CloseInstance();

	//关闭窗口
	DestroyWindow();

	return;
}

//位置改变
void CGameFrameDlg::OnWindowPosChanging(WINDOWPOS * lpwndpos)
{
	if (m_bInitDialog==false) lpwndpos->flags&=~SWP_SHOWWINDOW;
	__super::OnWindowPosChanging(lpwndpos);
}

//位置改变
void CGameFrameDlg::OnSize(UINT nType, int cx, int cy)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSize.log",strTime);

	strTemp.Format("OnSize %d  %d",  cx,  cy);
	WriteLog(strFile, strTemp);

	__super::OnSize(nType, cx, cy);
	CString showMsg;
	showMsg.Format("OnSize(UINT nType, int cx=%d, int cy=%d)",  cx,  cy);
	//MessageBox(showMsg);

	//调整拆分条
	if (m_VorSplitter.GetSafeHwnd()) 
	{
		//	UINT uSplitPos=__max(cx*17/24-SPLIT_WIDE,350);
		UINT uSplitPos = cx - INFO_WIDTH - BUTTON_WIDE - 1;
		showMsg.Format("uSplitPos=%d)",  uSplitPos);
		//MessageBox(showMsg);

		//	SafeMoveWindow(&m_VorSplitter,uSplitPos,GetTitleHight(),SPLIT_WIDE,cy-GetTitleHight());

		HDWP hDwp=BeginDeferWindowPos(32);
		DeferWindowPos(hDwp,m_VorSplitter,NULL,uSplitPos,0, 0, 0, uFlags);

		strTemp.Format("m_VorSplitter %d  %d",  uSplitPos,0);
		WriteLog(strFile, strTemp);

		EndDeferWindowPos(hDwp);
		m_VorSplitter.InitSplitBar(cx*3/5,cx-250,false);
		//调整控件
		RectifyControl(cx,cy);
	}


	return;

}
//定时器消息
void CGameFrameDlg::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);

	if ( nIDEvent == 1234)
	{
		//SetupVideoDLL();
		KillTimer( 1234);
	}


	return;
}

//设置自己的位置
void CGameFrameDlg::SetMyselfAVClientPostion()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sSetMyselfAVClientPostion.log",strTime);

	strTemp.Format("into SetMyselfAVClientPostion");
	WriteLog(strFile, strTemp);

	CString avServerIPStr, userIDStr, roomIDStr, tableIDStr, deskIDStr;

	char avServerIP[50];
	char *p;

	int roomID;
	int tableID;
	int deskID;

	WORD wChairID=GetMeChairID();
	const tagUserData * pUserInfo=GetUserData(wChairID);

	if (pUserInfo!=NULL)
	{
		userIDStr.Format("%d", pUserInfo->dwUserID);//
		deskID = wChairID;//椅子位置
		tableID = pUserInfo->wTableID;//桌子号码
	}
	else
	{
		return ;
	}
	//房间编号
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	const tagServerAttribute * pServer = m_ClientKernelHelper->GetServerAttribute();
	roomID = pServer->wServerID;

	avServerIPStr= Glb().m_CenterServerIPAddr;//"192.168.1.10";//采用中心服务器IP

	CString showMsg;
	showMsg.Format("userid=%s, roomid=%s, tableIDStr=%s, deskIDStr=%s,avServerIPStr=%s", userIDStr, roomIDStr,tableIDStr, deskIDStr, avServerIPStr );
	//AfxMessageBox(showMsg);

	//		strTemp.Format("into SetupVideoDLL %s", showMsg);
	//		WriteLog(strFile, strTemp);

	p= avServerIPStr.GetBuffer(avServerIPStr.GetLength());
	strcpy( avServerIP, p);

	FlySetAVClientPostionValue( 
		roomID, 
		tableID,
		deskID
		);

	strTemp.Format("%d	%d	%d", 				  roomID, 
		tableID,
		deskID);
	WriteLog(strFile, strTemp);

}
//////////////////////////////////////////////////////////////////////////
//功能：调用视频DLL
//输入：
//返回：
void CGameFrameDlg::SetupVideoDLL()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sSetupVideoDLL.log",strTime);

	strTemp.Format("into SetupVideoDLL");
	WriteLog(strFile, strTemp);

	myhinstDLL = AfxLoadLibrary("FlyGameAVClient.dll");

	if (myhinstDLL)
	{}
	else
	{
		AfxMessageBox("视频组件调用失败！");
		isHaveSetupAV = 2;//调用失败
		return ;
	}//End if

	//StopAVClient
	if ( myhinstDLL )
	{
		StopAVClient Proc;
		Proc = (StopAVClient) GetProcAddress( myhinstDLL, "StopAVClient");

		int		iTemp = Proc();
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");
	}//End if	

	if ( myhinstDLL )
	{
		CString avServerIPStr, userIDStr, roomIDStr, tableIDStr, deskIDStr;

		char avServerIP[50];
		char *p;

		WORD wChairID=GetMeChairID();
		const tagUserData * pUserInfo=GetUserData(wChairID);

		if (pUserInfo!=NULL)
		{
			userIDStr.Format("%d", pUserInfo->dwUserID);//
			deskIDStr.Format("%d", wChairID);//椅子位置
			tableIDStr.Format("%d", pUserInfo->wTableID );//桌子号码
		}

		//房间编号
		ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
		const tagServerAttribute * pServer = m_ClientKernelHelper->GetServerAttribute();
		roomIDStr.Format("%d", pServer->wServerID);

		CString myIPStr ;
		myIPStr = Glb().m_CenterServerIPAddr;
		myIPStr.Replace("http://", "");

		strTemp.Format("myIPStr %s", myIPStr);
		WriteLog(strFile, strTemp);

		avServerIPStr.Format("%s", myIPStr );//采用中心服务器IP

		CString showMsg;
		showMsg.Format("userid=%s, roomid=%s, tableIDStr=%s, deskIDStr=%s,avServerIPStr=%s", userIDStr, roomIDStr,tableIDStr, deskIDStr, avServerIPStr );
		//AfxMessageBox(showMsg);

		strTemp.Format("into SetupVideoDLL %s", showMsg);
		WriteLog(strFile, strTemp);

		p= avServerIPStr.GetBuffer(avServerIPStr.GetLength());
		strcpy( avServerIP, p);

		HWND myhWnd = m_showSelfVideo.m_hWnd;

		StartAVClient Proc;
		Proc = (StartAVClient) GetProcAddress( myhinstDLL, "StartAVClient");

		int		iTemp = Proc(myhWnd /*myCwnd this*/, 
			avServerIP,
			UDP_SOCKET_REG_PORT     , 
			atoi(userIDStr),
			atoi(roomIDStr), 
			atoi(tableIDStr),
			atoi(deskIDStr)
			);
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//AfxMessageBox("Not found dll ！");
	}//End if	

	HWND hRemoteWnd[REMOTE_VIDEO_MAX_NUM];
	hRemoteWnd[0] = 	m_showRemoteVideo.m_hWnd;
	hRemoteWnd[1] = 	m_showRemoteVideo1.m_hWnd;
	hRemoteWnd[2] = 	m_showRemoteVideo2.m_hWnd;
	hRemoteWnd[3] = 	m_showRemoteVideo3.m_hWnd;

	for (int i=0; i <REMOTE_VIDEO_MAX_NUM ; i++)
	{
		SetOneRemoteShowHWND( hRemoteWnd[i], i);
	}
	/*
	if ( myhinstDLL )
	{
	AVClientSendRegMsg Proc;
	Proc = (AVClientSendRegMsg) GetProcAddress( myhinstDLL, "AVClientSendRegMsg");

	int		iTemp = Proc();
	//	FreeLibrary( hinstDLL );
	}
	else
	{
	AfxMessageBox("Not found dll ！");

	}	

	if ( myhinstDLL )
	{
	GetOneTablePlayerList Proc;
	Proc = (GetOneTablePlayerList) GetProcAddress( myhinstDLL, "GetOneTablePlayerList");

	int		iTemp = Proc();
	//	FreeLibrary( hinstDLL );
	}
	else
	{
	AfxMessageBox("Not found dll ！");
	}	
	*/
	FlyAVClientSendRegMsg();
	FlyGetOneTablePlayerList();

	isHaveSetupAV = 1;//调用成功
}


void  CGameFrameDlg::SetOneRemoteShowHWND( HWND myhRemoteWnd, int deskInt)
{
	if ( myhinstDLL )
	{
		SetRemoteShowHWND Proc;
		Proc = (SetRemoteShowHWND) GetProcAddress( myhinstDLL, "SetRemoteShowHWND");

		int		iTemp = Proc(myhRemoteWnd, deskInt);
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");
	}	
}
//用户参数变化
void CGameFrameDlg::FlySetAVClientPostionValue( 
	int roomID, 
	int tableID,
	int deskID
	)
{
	if ( myhinstDLL )
	{
		SetAVClientPostionValue Proc;
		Proc = (SetAVClientPostionValue ) GetProcAddress( myhinstDLL, "SetAVClientPostionValue");

		int		iTemp = Proc(	   roomID, 
			tableID,
			deskID);
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");

	}	
}
//发送端口注册消息
void CGameFrameDlg::FlyAVClientSendRegMsg()
{
	if ( myhinstDLL )
	{
		AVClientSendRegMsg Proc;
		Proc = (AVClientSendRegMsg) GetProcAddress( myhinstDLL, "AVClientSendRegMsg");

		int		iTemp = Proc();
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");

	}	
}
void CGameFrameDlg::FlyAVClientSendUnRegMsg()
{
	if ( myhinstDLL )
	{
		AVClientSendUnRegMsg Proc;
		Proc = (AVClientSendUnRegMsg) GetProcAddress( myhinstDLL, "AVClientSendUnRegMsg");

		int		iTemp = Proc();
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");

	}//End if	
}
//视频操作接口	
//功能:控制本地发送
//输入:语音视频类型;开关类型
//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
void CGameFrameDlg::FlyOpNetSendMode(int avType, int opType)
{
	if ( myhinstDLL )
	{
		OpNetSendMode Proc;
		Proc = (OpNetSendMode) GetProcAddress( myhinstDLL, "OpNetSendMode");

		int		iTemp = Proc(avType,  opType);
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");
	}//End if	
}
//获得一个桌的人员LIST
void CGameFrameDlg::FlyGetOneTablePlayerList()
{
	if ( myhinstDLL )
	{
		GetOneTablePlayerList Proc;
		Proc = (GetOneTablePlayerList) GetProcAddress( myhinstDLL, "GetOneTablePlayerList");

		int		iTemp = Proc();
		//	FreeLibrary( hinstDLL );
	}
	else
	{
		//	AfxMessageBox("Not found dll ！");
	}//End if	

}
	//功能：初始化myself工具栏
void CGameFrameDlg::InitMyselfToolsBar()
{
	CRect myRect(100, 100, 100+80, 100+20);

	CLabelEx * myToolsList[1] ={ &m_MyselfOpKuang};

	for (int i=0; i < 1; i++)
	{
		myToolsList[i]->labelID = LABLE_ID_TYPE_MYSELF_TOOLS;//编号
		myToolsList[i]->my_hWnd = this->m_hWnd ;//

		myToolsList[i]->InitMyselfObjListData();//

		int	djUseNum = TOOLS_LABLE_MYSELF_USE_MAX_NUM;//可以用的个数
		for (int j=0; j<djUseNum ;j++)
		{
			myToolsList[i]->objList[j].objInt=j;
		}//End for


		myToolsList[i]->InitDrawKuang();
		myToolsList[i]->isActiveBool =  true;
		myToolsList[i]->SetBGBitmap(IDB_BITMAP_MYSELF_KUANG );//设置背景图
		myToolsList[i]->ShowWindow(false);
		myToolsList[i]->MoveWindow( myRect);//移动位置
	}//End for

	
}

//功能：初开化工具栏
void CGameFrameDlg::InitToolsBar()
{
	CRect myRect(100, 100, 100+120, 100+20);

	CLabelEx * myToolsList[TOOLS_LABLE_NUM] ={ &m_OpKuang, &m_OpKuang1, &m_OpKuang2, &m_OpKuang3};

	for (int i=0; i < TOOLS_LABLE_NUM; i++)
	{
		myToolsList[i]->labelID = LABLE_ID_TYPE_TOOLS+i;//编号
		myToolsList[i]->my_hWnd = this->m_hWnd ;//

		myToolsList[i]->InitObjListData();//

		int	djUseNum = TOOLS_LABLE_USE_MAX_NUM;//可以用的个数
		for (int j=0; j<djUseNum ;j++)
		{
			myToolsList[i]->objList[j].objInt=j;
		}//End for


		myToolsList[i]->InitDrawKuang();
		myToolsList[i]->isActiveBool =  true;
		myToolsList[i]->SetBGBitmap(IDB_BITMAP_KUANG );//设置背景图
		myToolsList[i]->ShowWindow(false);
		myToolsList[i]->MoveWindow( myRect);//移动位置
	}//End for



}
//场景语音
void CGameFrameDlg::OnBnClickedYuYin()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnBnClickedYuYin.log",strTime);

	strTemp.Format("into OnBnClickedYuYin()");
	WriteLog(strFile, strTemp);

	//AfxMessageBox("hello!");
	//CDlgDTSound myDlg;
	//myDlg.DoModal();
	if(pOneUserInfoDlg!=NULL)
	{
		strTemp.Format("pOneUserInfoDlg!=NULL");
		WriteLog(strFile, strTemp);

		int w,h,x,y;
		w=261;
		h=377;
		pOneUserInfoDlg->MoveWindow( showDlgX,showDlgY, w,h);
		if (pOneUserInfoDlg->IsWindowVisible())
		{
			strTemp.Format("ShowWindow(false)");
			WriteLog(strFile, strTemp);

			pOneUserInfoDlg->ShowWindow(false);
		}
		else
		{
			strTemp.Format("ShowWindow(true)");
			WriteLog(strFile, strTemp);
			pOneUserInfoDlg->ShowWindow(true);
		}
		//	AfxMessageBox("hello!");
	}
	else
	{
		AfxMessageBox("bad!");
	}

	strTemp.Format("out OnBnClickedYuYin()");
	WriteLog(strFile, strTemp);

}

//隐藏列表按钮
void CGameFrameDlg::OnBnClickedHideList()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnBnClickedHideList.log",strTime);

	strTemp.Format("%s", "OnBnClickedHideList()");
	WriteLog(strFile, strTemp);

	static bool bHideList=true;
	int cx,cy;
	CRect rect;
	GetClientRect(rect);
	cx=rect.Width();
	cy=rect.Height();
	if(bHideList)
	{
		if (m_VorSplitter.GetSafeHwnd()) 
		{
			UINT uSplitPos=cx-12;
			//		SafeMoveWindow(&m_VorSplitter,uSplitPos,GetTitleHight(),SPLIT_WIDE,cy-GetTitleHight());

			HDWP hDwp=BeginDeferWindowPos(32);
			DeferWindowPos(hDwp,m_VorSplitter,NULL,uSplitPos, 0,SPLIT_WIDE,cy, uFlags);
			EndDeferWindowPos(hDwp);
			RectifyControl(cx,cy);

			strTemp.Format("bHideList cx=%d,cy=%d", cx,cy);
			WriteLog(strFile, strTemp);
			//			FixControlStation(cx,cy);
		}
		//		m_BtExpression.ShowWindow(SW_HIDE);
		//		m_BtSend.ShowWindow(SW_HIDE);
	}
	else
	{
		if (m_VorSplitter.GetSafeHwnd()) 
		{
			//UINT uSplitPos=__max(cx*2/3-SPLIT_WIDE,350);
			//UINT uSplitPos=__max(cx*17/24-SPLIT_WIDE,350);
			UINT uSplitPos = cx - INFO_WIDTH - BUTTON_WIDE - 1;
			//		SafeMoveWindow(&m_VorSplitter,uSplitPos,GetTitleHight(),SPLIT_WIDE,cy-GetTitleHight());
			HDWP hDwp=BeginDeferWindowPos(32);
			DeferWindowPos(hDwp,m_VorSplitter,NULL,uSplitPos,0,SPLIT_WIDE,cy, uFlags);
			EndDeferWindowPos(hDwp);

			m_VorSplitter.InitSplitBar(cx*3/5,cx-250,false);
			//			FixControlStation(cx,cy);
			RectifyControl(cx,cy);
			strTemp.Format("cx=%d,cy=%d", cx,cy);
			WriteLog(strFile, strTemp);

		}
		//	m_BtExpression.ShowWindow(SW_SHOW);
		//	m_BtSend.ShowWindow(SW_SHOW);
	}
	bHideList=!bHideList;
	return;
}

//设置按钮
void CGameFrameDlg::OnBnClickedOption()
{
	OnGameOptionSet();
	return;
}

//游戏规则
void CGameFrameDlg::OnBnClickedRule()
{
	TCHAR szRuleUrl[128];
	//	_snprintf(szRuleUrl,sizeof(szRuleUrl),TEXT("%s/GameRule.asp?KindID=%ld"), HOME_WEB_URL, m_UserListView.m_wKindID);


	ShellExecute(NULL,TEXT("open"),szRuleUrl,NULL,NULL,SW_SHOWDEFAULT);

	return;
}

//退出按钮
void CGameFrameDlg::OnBnClickedQuitGame()
{

	PostMessage(WM_CLOSE,0,0);
	return;
}


//发送动态语音
LRESULT CGameFrameDlg::OnSendDTSound(WPARAM wparam, LPARAM lparam)
{
	CString showMsg;
	int typeInt = wparam;
	int soundInt = lparam;
	showMsg.Format("%d, %d", typeInt, soundInt);
	//AfxMessageBox(showMsg);
	//CString inStr = "PlaySound:boy/催促/boy (9).mp3;聊天内容!!!!";	
	CString strBuffer;
	SOUND_TEXT_ITEM  mySoundItemPtr;
	bool getBool = pOneUserInfoDlg->myCFlySoundText.GetOneSoundDataItem( typeInt, soundInt, &mySoundItemPtr);

	if ( getBool )
	{
		strBuffer.Format("%s%s;%s",
			DT_SOUND_STR,
			mySoundItemPtr.playSound,
			mySoundItemPtr.showInfo 
			);

		//AfxMessageBox(strBuffer);
		//发送聊天信息
		/*
		MSG_GR_RS_NormalTalk Talk;
		Talk.iLength=strBuffer.GetLength();
		Talk.dwSendID=0;//GetMeUserInfo()->dwUserID;
		Talk.dwTargetID=0;//dwTargerID;
		lstrcpy(Talk.szMessage,strBuffer);
		Talk.crColor=CGameImageLink::m_ColorResource.m_crTalkTxColor;
		SendGameData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_GAME_FRAME,ASS_GM_NORMAL_TALK,0);
		*/
		m_pGameFrameControl->SendChatMsg(strBuffer);
	}
	return 0;
}

void CGameFrameDlg::PlayMP3Sound(CString mp3FileStr)
{
	if ( mp3FileStr.GetLength() > 0 )
	{
		g_myCFlyGameSound.PlayMP3Sound(mp3FileStr);
	}
}

//功能：处理用户操作
void CGameFrameDlg::DealwithOp(int lableID, int opInt)
{

	switch(lableID)
	{
	case LABLE_ID_TYPE_TOOLS :
	case LABLE_ID_TYPE_TOOLS_1 :
	case LABLE_ID_TYPE_TOOLS_2 :
	case LABLE_ID_TYPE_TOOLS_3 :
		{
			DealwithToolsOp( lableID,  opInt);
		}
		break; 
	case LABLE_ID_TYPE_DJ_1 :
	case LABLE_ID_TYPE_DJ_2:
		{
			DealwithDJOp( lableID,  opInt);
		}
		break;
	case LABLE_ID_TYPE_MYSELF_TOOLS:
		{
			DealwithMyselfToolsOp(  opInt);		
		}
		break;

	}//End switch(lableID)
}
	//功能：处理myself tool用户操作
void CGameFrameDlg::DealwithMyselfToolsOp( int opInt)
{
	//toolbar
	//1.送礼物；2。加为好友；3。关开麦；4。关开视频；5。缩放视频；6。更多的；
	opInt+= 2;

	switch(opInt)
	{
		//3。关开麦
	case TOOLS_OP_TYPE_MIC :
		{
			OpOneMic(0, 1);

		}
		break; 
		//4。关开视频
	case TOOLS_OP_TYPE_VIDEO :
		{
			OpOneVideo(0, 1);

		}
		break; 
		//5。缩放视频
	case TOOLS_OP_TYPE_ZOOM_VIDEO :
		{}
		break; 
		//6。更多的
	case TOOLS_OP_TYPE_MORE :
		{
			ShowOneMoreMenu(0, 1);
		}
		break; 

	}	
}
//功能：处理tool用户操作
void CGameFrameDlg::DealwithToolsOp(int lableID, int opInt)
{
	//toolbar
	//1.送礼物；2。加为好友；3。关开麦；4。关开视频；5。缩放视频；6。更多的；
	int opUserInt ;
	//通过标签获取用户ID
	opUserInt = GetOneUserIdFromLableID(lableID);

	switch(opInt)
	{
		//1.送礼物
	case TOOLS_OP_TYPE_GIFT :
		{
			OpenGiftExe(lableID);
		}
		break; 
		//2。加为好友
	case TOOLS_OP_TYPE_ADD_FRIEND :
		{
			AddOneFriend(opUserInt);

		}
		break; 
		//3。关开麦
	case TOOLS_OP_TYPE_MIC :
		{
			OpOneMic(opUserInt);

		}
		break; 
		//4。关开视频
	case TOOLS_OP_TYPE_VIDEO :
		{
			OpOneVideo(lableID);

		}
		break; 
		//5。缩放视频
	case TOOLS_OP_TYPE_ZOOM_VIDEO :
		{}
		break; 
		//6。更多的
	case TOOLS_OP_TYPE_MORE :
		{
			ShowOneMoreMenu(opUserInt);
		}
		break; 

	}

}

//功能：处理DJ用户操作
void CGameFrameDlg::DealwithDJOp(int lableID, int opInt)
{/*
	CString userStr;
	CString djObjIDStr;
	CString ipStr;

	CString cmdLine;

	//自己名称
	UserInfoStruct * pSelfUserInfo=&m_pGameView->m_pUserItem[1]->GameUserInfo;
	userStr.Format("%s", pSelfUserInfo->szName);

	djObjIDStr.Format("%d", opInt);//obj id

	//服务器IP
	ipStr.Format("%s", Glb().m_CenterServerIPAddr);

	cmdLine.Format("-%s  -%s  -%s  -4", userStr, djObjIDStr, ipStr);
	ShellExecute( this->m_hWnd, "open", EX_TOOL_EXE, cmdLine, NULL, SW_SHOW );


*/

}

//功能：获取用户ID通过LABE
int CGameFrameDlg::GetOneUserIdFromLableID(int lableID)
{
	int getUserID = -1;

	getUserID =2019;

	return getUserID;

}


//1.送礼物
void CGameFrameDlg::OpenGiftExe(int lableID )
{/*
	//EXE
	CString userStr;
	CString getGiftUserStr;
	CString ipStr;

	CString cmdLine;

	//自己名称
	UserInfoStruct * pSelfUserInfo=&m_pGameView->m_pUserItem[1]->GameUserInfo;
	userStr.Format("%s", pSelfUserInfo->szName);
	//	AfxMessageBox(userStr);

	UserInfoStruct * pUserInfo=NULL;
	if (m_pGameView->m_pUserItem[lableID]==NULL) return ;

	pUserInfo=	&m_pGameView->m_pUserItem[lableID]->GameUserInfo;

	if ( pUserInfo == NULL)
		return ;

	//对方
	getGiftUserStr.Format("%s", pUserInfo->szName);
	//AfxMessageBox(getGiftUserStr);

	//自己不可以
	if (getGiftUserStr ==  userStr)
		return ;

	//服务器IP
	ipStr.Format("%s", Glb().m_CenterServerIPAddr);
	//AfxMessageBox(ipStr);

	cmdLine.Format("-%s  -%s  -%s  -3", userStr, getGiftUserStr, ipStr);
	ShellExecute( this->m_hWnd, "open", EX_TOOL_EXE, cmdLine, NULL, SW_SHOW );
*/
}

//2。加为好友

#define FLY_ADD_FRIEND   8001   //增加好友		
void CGameFrameDlg::AddOneFriend(int opUserInt )
{
/*
	HWND hWnd = ::FindWindow(NULL, FLY_QQ_WINDOW_NAME_STR  );

	if(hWnd!=NULL)
	{

		::SendMessage(hWnd, FLY_ADD_FRIEND, opUserInt, 0);
		showMaxWindow = false;


	}
	else
	{
		//AfxMessageBox("no find!");
	}	

*/
}



//3。关开麦
void CGameFrameDlg::OpOneMic(int opUserInt, int opType )
{
	if ( opType==0 )
	{
	}
	else
	{
	
	//关
		if ( OPEN_STATUS == mySelfStatusList.micStatus )
		{

			mySelfStatusList.micStatus  = CLOSE_STATUS;
			//

	//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
			 FlyOpNetSendMode( DATA_TYPE_AUDIO, CLOSE_STATUS);
		}
		else
		{

			mySelfStatusList.micStatus  = OPEN_STATUS;
			//
	//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
			 FlyOpNetSendMode( DATA_TYPE_AUDIO, OPEN_STATUS);
		}

	}

}



//4。关开视频
void CGameFrameDlg::OpOneVideo(int lableID , int opType)
{

	if ( opType==0 )
	{
	CStatic * myVideoLabelList[] = {&m_showRemoteVideo, &m_showRemoteVideo1, &m_showRemoteVideo2, &m_showRemoteVideo3};


	if ( OPEN_STATUS == myUserStatusList[lableID].videoStatus )
	{
		myVideoLabelList[lableID]->ShowWindow(false);
		myUserStatusList[lableID].videoStatus  = CLOSE_STATUS;
	}
	else
	{
		myVideoLabelList[lableID]->ShowWindow(true);
		myUserStatusList[lableID].videoStatus  = OPEN_STATUS;

	}
	}
	else
	{
	
//关
	if ( OPEN_STATUS == mySelfStatusList.videoStatus )
	{
		m_showSelfVideo.ShowWindow(false);
		mySelfStatusList.videoStatus  = CLOSE_STATUS;
		//

//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
		 FlyOpNetSendMode( DATA_TYPE_VIDEO, CLOSE_STATUS);
	}
	else
	{
		m_showSelfVideo.ShowWindow(true);
		mySelfStatusList.videoStatus  = OPEN_STATUS;
		//
//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
		 FlyOpNetSendMode( DATA_TYPE_VIDEO, OPEN_STATUS);
	}

	}


}



//5。缩放视频
void CGameFrameDlg::OpZoomVideo(int opUserInt, int opType)
{
	if ( opType==0 )
	{

	}
	//自己视频
	else
	{
		//大小,位置
	
	}
}

//6。更多的
void CGameFrameDlg::ShowOneMoreMenu(int opUserInt , int opType)
{


}

LRESULT CGameFrameDlg::MyWndProc(WPARAM wParam, LPARAM lParam)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sMyWndProc.log",strTime);



	int myInt = (int) wParam;//lable id
	int myInt2 = (int) lParam;//op Int

	CString showMsg;
	showMsg.Format("%d, %d", myInt, myInt2);
//	AfxMessageBox( showMsg);

	strTemp.Format("showMsg=%s", showMsg);
	WriteLog(strFile, strTemp);

	DealwithOp( myInt, myInt2);

	return 0;

	
}

	//功能：init
void CGameFrameDlg::InitUserStatus()
{
//USER_STATUS_ITEM  myUserStatusList[TOOLS_LABLE_NUM];
memset( (char *) myUserStatusList, 0, sizeof(myUserStatusList));
for (int i=0; i < TOOLS_LABLE_NUM; i++)
{
myUserStatusList[i].micStatus = OPEN_STATUS;
myUserStatusList[i].userId = 0;
myUserStatusList[i].videoStatus = OPEN_STATUS;
}

//myself
mySelfStatusList.micStatus = OPEN_STATUS;
mySelfStatusList.userId = 0;
mySelfStatusList.videoStatus = OPEN_STATUS;

}