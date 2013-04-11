#include "Stdafx.h"
#include "AtlBase.h"
#include "LogonServerDlg.h"
#include "../../Include/FlyPublicDefine.h"
    
//判断是否过期
int IsTimeOut()
{
	int resultInt = 0;
	CTime tm = CTime::GetCurrentTime();
	CString strTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
	CString myTimeStr = tm.Format(_T("%Y%m"));//200703
	//AfxMessageBox(myTimeStr);


	if ( (atoi(myTimeStr) > MAX_MIN_TIME) && (atoi(myTimeStr) < MAX_USE_TIME))
	{
		//AfxMessageBox("过期！");
	}
	else
	{
			resultInt=1;	
	}
/*
	if ( atoi(myTimeStr) > 200712)
	{
	resultInt=1;
	}
	*/

	return resultInt;
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CLogonServerDlg, CDialog)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_INIT_SERVICE, OnBnClickedInitService)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSystemOptionDlg::CSystemOptionDlg() : CDialog(IDD_SYSTEM_OPTION)
{
}

//析构函数
CSystemOptionDlg::~CSystemOptionDlg()
{
}

//控件子类化
void CSystemOptionDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CSystemOptionDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//限制输入
	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_MAIN_PAGE))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_NAME))->LimitText(31);

	//加载参数
	CInitParamter InitParamter;
	InitParamter.LoadInitParamter();

	//设置控件
	SetDlgItemInt(IDC_LISTEN_PORT,InitParamter.m_wListenPort,FALSE);
	SetDlgItemInt(IDC_MAX_CONNECT,InitParamter.m_wMaxConnect,FALSE);

	//登录数据库
	SetDlgItemInt(IDC_USER_DATABASE_PORT,InitParamter.m_wUserDataBasePort,FALSE);
	SetDlgItemText(IDC_USER_DATABASE_USER,InitParamter.m_szUserDataBaseUser);
	SetDlgItemText(IDC_USER_DATABASE_PASS,InitParamter.m_szUserDataBasePass);
	SetDlgItemText(IDC_USER_DATABASE_NAME,InitParamter.m_szUserDataBaseName);

	//登录数据库地址
	DWORD dwDataBaseIP=inet_addr(InitParamter.m_szUserDataBaseAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szUserDataBaseAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_USER_DATABASE_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	//主站地址
	if (InitParamter.m_szMainPage[0]==0) SetDlgItemText(IDC_MAIN_PAGE,szStationPage);
	else SetDlgItemText(IDC_MAIN_PAGE,InitParamter.m_szMainPage);

	//中心服务器
	dwDataBaseIP=inet_addr(InitParamter.m_szCenterServerAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szCenterServerAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_CENTER_SERVER_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	return TRUE;
}

//确定函数
void CSystemOptionDlg::OnOK()
{
	//获取输入
	CInitParamter InitParamter;
	InitParamter.m_wListenPort=GetDlgItemInt(IDC_LISTEN_PORT);
	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);

	//登录数据库 
	InitParamter.m_wUserDataBasePort=GetDlgItemInt(IDC_USER_DATABASE_PORT);
	GetDlgItemText(IDC_USER_DATABASE_USER,InitParamter.m_szUserDataBaseUser,sizeof(InitParamter.m_szUserDataBaseUser));
	GetDlgItemText(IDC_USER_DATABASE_PASS,InitParamter.m_szUserDataBasePass,sizeof(InitParamter.m_szUserDataBasePass));
	GetDlgItemText(IDC_USER_DATABASE_NAME,InitParamter.m_szUserDataBaseName,sizeof(InitParamter.m_szUserDataBaseName));

	//登录数据库地址
	DWORD dwDataBaseIP=INADDR_NONE;
	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_USER_DATABASE_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szUserDataBaseAddr,sizeof(InitParamter.m_szUserDataBaseAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//主站地址
	GetDlgItemText(IDC_MAIN_PAGE,InitParamter.m_szMainPage,sizeof(InitParamter.m_szMainPage));
	if (lstrcmp(InitParamter.m_szMainPage,szStationPage)==0) InitParamter.m_szMainPage[0]=0;

	//中心服务器
	dwDataBaseIP=INADDR_NONE;
	pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_CENTER_SERVER_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szCenterServerAddr,sizeof(InitParamter.m_szCenterServerAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//保存设置
	InitParamter.SaveInitParamter(false);

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerDlg::CLogonServerDlg() : CDialog(IDD_LOGONSERVER_DIALOG)
{
	m_bAskQuit=true;
}

//析构函数
CLogonServerDlg::~CLogonServerDlg()
{
}

//控件子类化
void CLogonServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_EVENT, m_RichEditCtrl);
}

//初始化函数
BOOL CLogonServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置组件
	if (m_EventService.CreateInstance()==true)
	{
		IUnknownEx * pIUnknownEx=(IUnknownEx *)m_EventService->QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
		m_EventService->SetRichEditHwnd(m_RichEditCtrl.GetSafeHwnd());
		m_LogonService.SetEventService(pIUnknownEx);
	}
	m_RichEditCtrl.SetBackgroundColor(FALSE,RGB(220,220,220));

	//如果超时，退出
	if ( IsTimeOut())
	{
		__super::OnCancel();
	}


	return TRUE;
}

//确定消息
void CLogonServerDlg::OnOK()
{
	return;
}

//取消函数
void CLogonServerDlg::OnCancel()
{
	//获取状态
	bool bRuning=m_LogonService.IsService();

	//询问退出
	if ((m_bAskQuit==true)&&(bRuning==true))
	{
		CString strMessage=TEXT("登录服务器正在运行中，确实要退出吗？");
		int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (iRetCode!=IDYES) return;
	}

	//停止服务
	m_bAskQuit=true;
	if (bRuning==true) m_LogonService.StopService();

	__super::OnCancel();
}

//消息解释
BOOL CLogonServerDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//启动服务
void CLogonServerDlg::OnBnClickedStartService()
{
	//如果超时，退出
	if ( IsTimeOut())
	{
		return ;
	}

	if (m_LogonService.StartService()==true)
	{
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
		if (m_EventService.GetInterface()) m_EventService->ShowEventNotify(TEXT("登录服务启动成功"),Level_Normal);
	}

	return;
}

//停止服务
void CLogonServerDlg::OnBnClickedStopService()
{
	if (m_LogonService.StopService()==true)
	{
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
		if (m_EventService.GetInterface()) m_EventService->ShowEventNotify(TEXT("登录服务停止成功"),Level_Normal); 
	}

	return;
}

//系统配置
void CLogonServerDlg::OnBnClickedInitService()
{
	//如果超时，退出
	if ( IsTimeOut())
	{
		return;
	}

	CSystemOptionDlg SystemOptionDlg;
	SystemOptionDlg.DoModal();

	return;
}

//////////////////////////////////////////////////////////////////////////

