#include "Stdafx.h"
#include "GameFrame.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define BORDER_WIDTH						1							//边框宽度
#define TITLE_HEIGHT						81							//标题高度
#define MIN_CLIENT_WIDTH					800							//最小宽度
#define MIN_CLIENT_HEIGHT					500							//最小高度

//按钮标示
#define IDC_BT_MIN							1000						//最小按钮
#define IDC_BT_CLOSE						1001						//关闭按钮
#define IDC_BT_BUTTON_1						1002						//功能按钮
#define IDC_BT_BUTTON_2						1003						//功能按钮
#define IDC_BT_BUTTON_3						1004						//功能按钮
#define IDC_BT_BUTTON_4						1005						//功能按钮
#define IDC_BT_BUTTON_5						1006						//功能按钮
//#define IDC_BT_BUTTON_6						1007						//功能按钮
//#define IDC_BT_BUTTON_7						1008						//功能按钮

//消息定义
#define WM_SETUP_FINISH						WM_USER+100					//安装完成

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrame, CFrameWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SETTINGCHANGE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BT_MIN, OnBnClickedMin)
	ON_BN_CLICKED(IDC_BT_CLOSE, OnBnClickedClose)
	ON_MESSAGE(WM_SETUP_FINISH,OnMessageSetupFinish)
	ON_BN_CLICKED(IDC_BT_BUTTON_1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BT_BUTTON_2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BT_BUTTON_3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BT_BUTTON_4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BT_BUTTON_5, OnBnClickedButton5)
	//ON_BN_CLICKED(IDC_BT_BUTTON_6, OnBnClickedButton6)
	//ON_BN_CLICKED(IDC_BT_BUTTON_7, OnBnClickedButton7)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFrame::CGameFrame()
{
	//设置变量
	m_pAdBrower=NULL;

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageL.SetLoadInfo(IDB_FRAME_L,hInstance);
	m_ImageR.SetLoadInfo(IDB_FRAME_R,hInstance);
	m_ImageTL.SetLoadInfo(IDB_FRAME_TL,hInstance);
	m_ImageTM.SetLoadInfo(IDB_FRAME_TM,hInstance);
	m_ImageTR.SetLoadInfo(IDB_FRAME_TR,hInstance);

	return;
}

//析构函数
CGameFrame::~CGameFrame()
{
}

//消息解释
BOOL CGameFrame::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//建立消息
int CGameFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置属性
	ModifyStyle(WS_CAPTION|WS_BORDER,0,0);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置标题
	CString strTitle;
	strTitle.LoadString(AfxGetInstanceHandle(),IDS_MAIN_TITLE);
	SetWindowText(strTitle);

	//创建广告
	m_pAdBrower = new CHtmlBrower;
	m_pAdBrower->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,20,NULL);
//	m_pAdBrower->Navigate(TEXT("http://61.187.55.150/AD/FrameAD.htm"));]
	CString urlStr;
	urlStr.Format("%sAD/FrameAD.htm", Glb().m_MainHomeUrl);
	m_pAdBrower->Navigate(TEXT(urlStr));


	m_pAdBrower->EnableBrowser(true);

	//创建按钮
	m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_MIN);
	m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_CLOSE);
	m_btButton1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_1);
	m_btButton2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_2);
	m_btButton3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_3);
	m_btButton4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_4);
	m_btButton5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_5);
	//m_btButton6.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_6);
	//m_btButton7.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_7);

	//设置图片
	m_btMin.SetButtonImage(IDB_FRAME_MIN,AfxGetInstanceHandle(),false);
	m_btClose.SetButtonImage(IDB_FRAME_CLOSE,AfxGetInstanceHandle(),false);
	m_btButton1.SetButtonImage(IDB_FRAME_BT_BUTTON_1,AfxGetInstanceHandle(),false);
	m_btButton2.SetButtonImage(IDB_FRAME_BT_BUTTON_2,AfxGetInstanceHandle(),false);
	m_btButton3.SetButtonImage(IDB_FRAME_BT_BUTTON_3,AfxGetInstanceHandle(),false);
	m_btButton4.SetButtonImage(IDB_FRAME_BT_BUTTON_4,AfxGetInstanceHandle(),false);
	m_btButton5.SetButtonImage(IDB_FRAME_BT_BUTTON_5,AfxGetInstanceHandle(),false);
	//m_btButton6.SetButtonImage(IDB_FRAME_BT_BUTTON_6,AfxGetInstanceHandle(),false);
	//m_btButton7.SetButtonImage(IDB_FRAME_BT_BUTTON_7,AfxGetInstanceHandle(),false);

	//建立提示控件
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BT_MIN),TEXT("最小化游戏广场"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BT_CLOSE),TEXT("退出游戏广场"));

	//创建控件
	m_DlgControlBar.Create(IDD_CONTROL_BAR,this);
	m_DlgGamePlaza.Create(IDD_GAME_PLAZA,this);
	m_DlgControlBar.ActivePlazaViewItem();
	m_DlgControlBar.ShowWindow(SW_SHOW);

	//显示窗口
	CRect rcRect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcRect,SPIF_SENDCHANGE);
	SetWindowPos(&wndTop,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_DRAWFRAME|SWP_SHOWWINDOW);
	

	return 0;
}

//绘画消息
void CGameFrame::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载资源
	int nXPos=0,nYPos=0;
	CImageHandle ImageLHandle(&m_ImageL);
	CImageHandle ImageRHandle(&m_ImageR);
	CImageHandle ImageTLHandle(&m_ImageTL);
	CImageHandle ImageTMHandle(&m_ImageTM);
	CImageHandle ImageTRHandle(&m_ImageTR);

	//绘画上边
	m_ImageTL.BitBlt(dc,0,0);
	for (nXPos=m_ImageTL.GetWidth();nXPos<rcClient.Width()-m_ImageTR.GetWidth();nXPos+=m_ImageTM.GetWidth())
	{
		m_ImageTM.BitBlt(dc,nXPos,0);
	}
	m_ImageTR.BitBlt(dc,rcClient.Width()-m_ImageTR.GetWidth(),0);

	//绘画左右
	nXPos=rcClient.Width()-m_ImageR.GetWidth();
	for (int nYPos=m_ImageTL.GetHeight();nYPos<rcClient.Height();nYPos+=m_ImageL.GetHeight()) 
	{
		m_ImageL.BitBlt(dc,0,nYPos);
		m_ImageR.BitBlt(dc,nXPos,nYPos);
	}

	return;
}

//绘画背景
BOOL CGameFrame::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//设置改变
void CGameFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags, lpszSection);

	//调整框架大小
	CRect rcClient;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcClient,SPIF_SENDCHANGE);
	MoveWindow(&rcClient,TRUE);

	return;
}

//关闭消息
void CGameFrame::OnClose()
{
	//关闭提示
	if (g_GlobalUnits.GetGolbalUserData().dwUserID!=0L)
	{
		int nCode=AfxMessageBox(TEXT("您确实要关闭游戏广场吗？"),MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION);
		if (nCode!=IDYES) return;
	}

	//隐藏界面
	ShowWindow(SW_HIDE);

	//关闭房间
	g_pControlBar->CloseAllRoomViewItem();

	//保存配置
	g_GlobalOption.SaveOptionParameter();
	g_GlobalUnits.m_CompanionManager->SaveCompanion();

	__super::OnClose();
}

//功能按钮
void CGameFrame::OnBnClickedButton1()
{
	m_DlgControlBar.WebBrowse(TEXT(Glb().m_MainHomeUrl),true);

	return;
}

//功能按钮 帐号充值
void CGameFrame::OnBnClickedButton2()
{
	CString urlStr;
	urlStr.Format("%s%s", Glb().m_MainHomeUrl, Glb().m_CZUrl);

	m_DlgControlBar.WebBrowse(TEXT(urlStr),true);

	return;
}

//功能按钮  我的家园
void CGameFrame::OnBnClickedButton3()
{
	CString urlStr;
	urlStr.Format("%s%s", Glb().m_MainHomeUrl, Glb().m_MemberUrl);

	m_DlgControlBar.WebBrowse(TEXT(urlStr),true);
	return;
}

//功能按钮
void CGameFrame::OnBnClickedButton4()
{
//	m_DlgControlBar.WebBrowse(TEXT(HOME_WEB_URL),true);
	return;
}

//功能按钮 bbs
void CGameFrame::OnBnClickedButton5()
{
	CString urlStr;
	urlStr.Format("%s%s", Glb().m_MainHomeUrl, Glb().m_BBSUrl);

	m_DlgControlBar.WebBrowse(TEXT(urlStr),true);

	return;
}



//获取最大位置
void CGameFrame::OnGetMinMaxInfo(MINMAXINFO FAR * lpMMI) 
{
	lpMMI->ptMinTrackSize.x=MIN_CLIENT_WIDTH;
	lpMMI->ptMinTrackSize.y=MIN_CLIENT_HEIGHT;
	__super::OnGetMinMaxInfo(lpMMI);
}

//位置消息
void CGameFrame::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);

	//设置控件
	m_DlgControlBar.SetViewItemRect(BORDER_WIDTH,TITLE_HEIGHT,cx-2*BORDER_WIDTH,cy-TITLE_HEIGHT-CONTROL_BAR_HEIGHT);

	//锁定屏幕
	LockWindowUpdate();

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(20);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//其他控件

	int minBtnX = cx-70;
	int closeBtnX = minBtnX + 32;
	DeferWindowPos(hDwp,m_btMin,NULL, minBtnX,7,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btClose,NULL,closeBtnX,7,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_pAdBrower->GetSafeHwnd(),NULL,161,17,279,48,uFlags);
	m_pAdBrower->ShowWindow(false);

	DeferWindowPos(hDwp,m_DlgControlBar,NULL,0,cy-CONTROL_BAR_HEIGHT,cx,CONTROL_BAR_HEIGHT,uFlags);

	//导航按钮
	CRect rcButton;
	int nBeginPos=320;//460;
	m_btButton1.GetWindowRect(&rcButton);
//	int nSpace=(cx-nBeginPos-rcButton.Width()*7-10)/6;

	int btnY = 5;
	int btnX = nBeginPos;
	int btnW = rcButton.Width();
	int nSpace= btnW/8;

	DeferWindowPos(hDwp,m_btButton1,NULL,btnX,btnY,0,0,uFlags|SWP_NOSIZE);

	btnX+=(btnW+nSpace);
	DeferWindowPos(hDwp,m_btButton2,NULL,btnX,btnY,0,0,uFlags|SWP_NOSIZE);
	btnX+=(btnW+nSpace);
	DeferWindowPos(hDwp,m_btButton3,NULL,btnX,btnY,0,0,uFlags|SWP_NOSIZE);
	btnX+=(btnW+nSpace);
	DeferWindowPos(hDwp,m_btButton4,NULL,btnX,btnY,0,0,uFlags|SWP_NOSIZE);
	btnX+=(btnW+nSpace);
	DeferWindowPos(hDwp,m_btButton5,NULL,btnX,btnY,0,0,uFlags|SWP_NOSIZE);
/*
	m_btButton1.ShowWindow(false);
	m_btButton2.ShowWindow(false);
	m_btButton3.ShowWindow(false);
	m_btButton4.ShowWindow(false);
	m_btButton5.ShowWindow(false);
*/
	//DeferWindowPos(hDwp,m_btButton6,NULL,nBeginPos+rcButton.Width()*5+nSpace*5,25,0,0,uFlags|SWP_NOSIZE);
	//DeferWindowPos(hDwp,m_btButton7,NULL,nBeginPos+rcButton.Width()*6+nSpace*6,25,0,0,uFlags|SWP_NOSIZE);

	//移动控件
	EndDeferWindowPos(hDwp);

	//解除锁定
	UnlockWindowUpdate();

	return;
}

//最小按钮
void CGameFrame::OnBnClickedMin()
{
	ShowWindow(SW_MINIMIZE);
	return;
}

//关闭按钮
void CGameFrame::OnBnClickedClose()
{
	g_pControlBar->CloseCurrentViewItem();
	return;
}

//安装完成
LRESULT CGameFrame::OnMessageSetupFinish(WPARAM wParam, LPARAM lParam)
{
	g_GlobalUnits.m_ServerListManager.UpdateGameKind((WORD)wParam);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
