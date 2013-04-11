#include "Stdafx.h"
#include "AtlBase.h"
#include "CenterServerDlg.h"

#include "../../Include/FlyPublicDefine.h"
BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CCenterServerDlg, CDialog)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_INIT_SERVICE, OnBnClickedInitService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
END_MESSAGE_MAP()


//判断是否过期
int IsTimeOut()
{
	int resultInt = 0;
        CTime tm = CTime::GetCurrentTime();
        CString strTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
CString myTimeStr = tm.Format(_T("%Y%m"));//200703

	if ( (atoi(myTimeStr) > MAX_MIN_TIME) && (atoi(myTimeStr) < MAX_USE_TIME))
	{
		//AfxMessageBox("过期！");
	}
	else
	{
			resultInt=1;	
	}
/*
//AfxMessageBox(myTimeStr);
if ( atoi(myTimeStr) > MAX_LIMITED_TIME    )
{
resultInt=1;
}
*/
	return resultInt;
}

//
//CSystemOptionDlg
//
CSystemOptionDlg::CSystemOptionDlg() : CDialog(IDD_SYSTEM_OPTION)
{
}

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
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_NAME))->LimitText(31);

	//加载参数
	CInitParamter InitParamter;
	InitParamter.LoadInitParamter();

	//设置控件
	SetDlgItemInt(IDC_LISTEN_PORT,InitParamter.m_wListenPort,FALSE);
	SetDlgItemInt(IDC_MAX_CONNECT,InitParamter.m_wMaxConnect,FALSE);

	//信息数据库
	SetDlgItemInt(IDC_SERVER_DATABASE_PORT,InitParamter.m_wServerDataBasePort,FALSE);
	SetDlgItemText(IDC_SERVER_DATABASE_USER,InitParamter.m_szServerDataBaseUser);
	SetDlgItemText(IDC_SERVER_DATABASE_PASS,InitParamter.m_szServerDataBasePass);
	SetDlgItemText(IDC_SERVER_DATABASE_NAME,InitParamter.m_szServerDataBaseName);

	//信息数据库地址
	DWORD dwDataBaseIP=inet_addr(InitParamter.m_szServerDataBaseAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szServerDataBaseAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	return TRUE;
}

//确定函数
void CSystemOptionDlg::OnOK()
{
	//如果超时，退出
	if ( IsTimeOut())
	{
		return;
	}

	//获取输入
	CInitParamter InitParamter;
	InitParamter.m_wListenPort=GetDlgItemInt(IDC_LISTEN_PORT);
	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);

	//信息数据库
	InitParamter.m_wServerDataBasePort=GetDlgItemInt(IDC_SERVER_DATABASE_PORT);
	GetDlgItemText(IDC_SERVER_DATABASE_USER,InitParamter.m_szServerDataBaseUser,sizeof(InitParamter.m_szServerDataBaseUser));
	GetDlgItemText(IDC_SERVER_DATABASE_PASS,InitParamter.m_szServerDataBasePass,sizeof(InitParamter.m_szServerDataBasePass));
	GetDlgItemText(IDC_SERVER_DATABASE_NAME,InitParamter.m_szServerDataBaseName,sizeof(InitParamter.m_szServerDataBaseName));

	//信息数据库地址
	DWORD dwDataBaseIP=INADDR_NONE;
	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szServerDataBaseAddr,sizeof(InitParamter.m_szServerDataBaseAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//保存设置
	InitParamter.SaveInitParamter(false);

	__super::OnOK();
}


//
//CCenterServerDlg
//
CCenterServerDlg::CCenterServerDlg() : CDialog(IDD_CENTERSERVER_DIALOG)
{
	m_bAskQuit=true;
}

CCenterServerDlg::~CCenterServerDlg()
{
}

//控件子类化
void CCenterServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_EVENT, m_RichEditCtrl);
}

//初始化函数
BOOL CCenterServerDlg::OnInitDialog()
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
		m_CenterService.SetEventService(pIUnknownEx);
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
void CCenterServerDlg::OnOK()
{
	return;
}

//取消函数
void CCenterServerDlg::OnCancel()
{
	//获取状态
	bool bRuning=m_CenterService.IsService();

	//询问退出
	if ((m_bAskQuit==true)&&(bRuning==true))
	{
		CString strMessage=TEXT("中心服务器正在运行中，确实要退出吗？");
		int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (iRetCode!=IDYES) 
		{
			return;
		}
	}

	//停止服务
	m_bAskQuit=true;
	if (bRuning==true) 
	{
		m_CenterService.StopService();
	}

	__super::OnCancel();
}

//消息解释
BOOL CCenterServerDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) 
	{
		return TRUE;
	}
	
	return __super::PreTranslateMessage(pMsg);
}

//启动服务
void CCenterServerDlg::OnBnClickedStartService()
{
	if (m_CenterService.StartService()==true)
	{
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
		if (m_EventService.GetInterface()) 
		{
			m_EventService->ShowEventNotify(TEXT("中心服务启动成功"),Level_Normal);
		}
	}

	return;
}

//停止服务
void CCenterServerDlg::OnBnClickedStopService()
{
	if (m_CenterService.StopService()==true)
	{
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
		if (m_EventService.GetInterface()) 
		{
			m_EventService->ShowEventNotify(TEXT("中心服务停止成功"),Level_Normal); 
		}
	}

	return;
}

//系统配置
void CCenterServerDlg::OnBnClickedInitService()
{
	CSystemOptionDlg SystemOptionDlg;
	SystemOptionDlg.DoModal();

	return;
}


//-----------------------------------------------
//					the end
//-----------------------------------------------
