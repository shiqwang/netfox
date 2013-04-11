#include "Stdafx.h"
#include "Mmsystem.h"
#include "Resource.h"
#include "GameParrentDlg.h"
#include "GlobalOption.h"
//#include "GameFrameControl.h"
#include "ClientKernelSink.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameParrentDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameParrentDlg::CGameParrentDlg(CGameParrentView * pGameFrameView)
{
	//设置变量
	m_bReset=false;
	m_bRectify=false;
	m_bInitDialog=false;
	m_pKernelSink=NULL;
//	m_pGameFrameControl=NULL;
	m_pGameFrameView=pGameFrameView;

	//配置变量
	m_bAllowSound=true;

	//创建钩子
	m_pKernelSink=new CClientKernelSink(*this);

	return;
}

//析构函数
CGameParrentDlg::~CGameParrentDlg()
{
	SafeDelete(m_pKernelSink);
//	SafeDelete(m_pGameFrameControl);
	return;
}

//按钮消息
bool CGameParrentDlg::OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)
{
	return true;
}

//拆分条消息
bool CGameParrentDlg::OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}

//调整界面
void CGameParrentDlg::RectifyControl(int nWidth, int nHeight)
{
	//状态判断
	if (m_bInitDialog==false) return;

	//改对话框要改
	//变量定义
	const int iXExc=0;//GetXExcursionPos();
	const int iYExc=0;//GetYExcursionPos();
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//锁定屏幕
	LockWindowUpdate();

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(3);
	DeferWindowPos(hDwp,m_pGameFrameView->GetSafeHwnd(),NULL,iXExc,iYExc,nWidth-2*iXExc,nHeight-iYExc,uFlags);
	EndDeferWindowPos(hDwp);

	//重画界面
	Invalidate(FALSE);
	UpdateWindow();

	//解除锁定
	UnlockWindowUpdate();

	return;
}

//网络消息
bool CGameParrentDlg::OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	return false;
}

//发送函数
bool CGameParrentDlg::SendData(WORD wSubCmdID)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendSocketData(MDM_GF_GAME,wSubCmdID);
}

//发送函数
bool CGameParrentDlg::SendData(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendSocketData(MDM_GF_GAME,wSubCmdID,pData,wDataSize);
}

//发送准备
bool CGameParrentDlg::SendUserReady(void * pBuffer, WORD wDataSize)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendUserReady(pBuffer,wDataSize);
}

//设置定时器
bool CGameParrentDlg::SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SetGameTimer(wChairID,nTimerID,nElapse);
}

//删除定时器
bool CGameParrentDlg::KillGameTimer(UINT nTimerID)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->KillGameTimer(nTimerID);
}

//播放声音
bool CGameParrentDlg::PlayGameSound(LPCTSTR pszSoundName)
{
	if ((m_bAllowSound==true)&&(IsWindowVisible()))
	{
		::PlaySound(pszSoundName,AfxGetInstanceHandle(),SND_ASYNC|SND_NODEFAULT);
		return true;
	}

	return false;
}

//播放声音
bool CGameParrentDlg::PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName)
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
bool CGameParrentDlg::InsertImage(CBitmap * pBitmap)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertImage(pBitmap);
}

//插入名字
bool CGameParrentDlg::InsertUserName(LPCTSTR pszUserName)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertUserName(pszUserName);
}

//系统消息
bool CGameParrentDlg::InsertSystemString(LPCTSTR pszString)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertSystemString(pszString,MS_NORMAL);
}

//特色消息
bool CGameParrentDlg::InsertShtickString(LPCTSTR pszString, COLORREF crColor, bool bNewLine)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertShtickString(pszString,crColor,MS_NORMAL,bNewLine);
}

//普通消息
bool CGameParrentDlg::InsertGeneralString(LPCTSTR pszString, COLORREF crColor, bool bNewLine)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertGeneralString(pszString,crColor,MS_NORMAL,bNewLine);
}

//激活界面
void CGameParrentDlg::ActiveGameFrame()
{
	BringWindowToTop(); 
	SetForegroundWindow();
	return;
}

//切换椅子
WORD CGameParrentDlg::SwitchViewChairID(WORD wChairID)
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
bool CGameParrentDlg::IsEnableSound()
{
	return m_bAllowSound;
}

//启用声音
void CGameParrentDlg::EnableSound(bool bEnable)
{
	//设置变量
	m_bAllowSound=bEnable;

	//读写注册
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("EnableSound"),m_bAllowSound);

	return;
}

//游戏状态
BYTE CGameParrentDlg::GetGameStatus()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return GS_FREE;
	return m_ClientKernelHelper->GetGameStatus();
}

//旁观模式
bool CGameParrentDlg::IsLookonMode()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return true;
	return m_ClientKernelHelper->IsLookonMode();
}

//允许旁观
bool CGameParrentDlg::IsAllowLookon()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return false;
	return m_ClientKernelHelper->IsAllowLookon();
}

//游戏状态
void CGameParrentDlg::SetGameStatus(BYTE bGameStatus)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return;
	m_ClientKernelHelper->SetGameStatus(bGameStatus);
	return;
}

//自己位置
WORD CGameParrentDlg::GetMeChairID()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;
	return m_ClientKernelHelper->GetMeChairID();
}

//时间位置
WORD CGameParrentDlg::GetTimeChairID()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;
	return m_ClientKernelHelper->GetTimeChairID();
}

//获取用户
const tagUserData * CGameParrentDlg::GetUserData(WORD wChairID)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return NULL;
	return m_ClientKernelHelper->GetUserInfo(wChairID);
}

//内核接口
void * CGameParrentDlg::GetClientKernel(const IID & Guid, DWORD dwQueryVer)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return NULL;
	return m_ClientKernelHelper->QueryInterface(Guid,dwQueryVer);
}

//控件绑定
void CGameParrentDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_VOR_SPLITER, m_VorSplitter);
}

//初始化函数
BOOL CGameParrentDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置属性
	ModifyStyle(WS_CAPTION|WS_BORDER,0,0);

	m_bAllowSound=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("EnableSound"),TRUE)?true:false;

	//创建控制
//	m_pGameFrameControl=new CGameFrameControl(this);
//	if (m_pGameFrameControl==NULL) throw TEXT("游戏框架创建失败");

	//创建组件
	if (m_MessageProxyHelper.CreateInstance()==false) throw TEXT("信息组件创建失败");
//	m_MessageProxyHelper->SetRichEditHwnd(&m_pGameFrameControl->m_ChatMessage);

	//设置界面接口
//	m_VorSplitter.SetSplitterSink(this);

	//创建组件
	if (m_UserFaceResHelper.CreateInstance()==false) throw TEXT("头像组件加载失败");
	if (m_ClientKernelHelper.CreateInstance()==false) throw TEXT("游戏框架内核模块加载失败");

	//创建游戏视图
	CRect rcGameView;
//	m_pGameFrameControl->Create(IDD_FRAME_CONTROL,this);
	m_pGameFrameView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,this,10);
	if (m_pGameFrameView->SetUserFaceRes(m_UserFaceResHelper.GetInterface())==false) throw TEXT("头像资源接口设置失败");

	//初始化内核
	LPCTSTR lpszCmdLine=AfxGetApp()->m_lpCmdLine;
	bool bSuccess=m_ClientKernelHelper->InitClientKernel(lpszCmdLine,m_pKernelSink);
	if (bSuccess==false) throw TEXT("游戏框架逻辑模块初始化失败");
	bSuccess=m_ClientKernelHelper->SetMessageProxy(m_MessageProxyHelper.GetInterface());
	if (bSuccess==false) throw TEXT("获取消息输出模块失败");

	//初始化游戏
//	m_pGameFrameControl->SetClientKernel(m_ClientKernelHelper.GetInterface());
	if (InitGameFrame()==false) throw TEXT("游戏框架初始化失败");

	//显示窗口
	m_bInitDialog=true;
	m_pGameFrameView->ShowWindow(SW_SHOW);
//	m_pGameFrameControl->ShowWindow(SW_SHOW);

	//显示窗口
	//if (GetSystemMetrics(SM_CXSCREEN)>1024)
	//{
	//	MoveWindow(0,0,1024,748,FALSE);
	//	CenterWindow(NULL);
	//}
	if (GetSystemMetrics(SM_CXSCREEN)>800)
	{
		MoveWindow(0,0,800,600,FALSE);
		CenterWindow(NULL);
	}
	else ShowWindow( SW_MAXIMIZE);//MaxSizeWindow();
	ShowWindow(SW_SHOW);

	return TRUE;
}

//消息解释
BOOL CGameParrentDlg::PreTranslateMessage(MSG * pMsg)
{
	//过滤消息
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//确定消息
void CGameParrentDlg::OnOK()
{
	return;
}

//取消消息
void CGameParrentDlg::OnCancel()
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
void CGameParrentDlg::OnWindowPosChanging(WINDOWPOS * lpwndpos)
{
	if (m_bInitDialog==false) lpwndpos->flags&=~SWP_SHOWWINDOW;
	//__super::OnWindowPosChanging(lpwndpos);
}

//位置改变
void CGameParrentDlg::OnSize(UINT nType, int cx, int cy)
{
//	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;

}

//////////////////////////////////////////////////////////////////////////
