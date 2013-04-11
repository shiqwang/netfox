#include "Stdafx.h"
#include "GlobalUnits.h"
#include "ShortMessage.h"
#include "RoomViewItem.h"

#define CONTROL_SPACE						2							//控件间距
#define PIC_BT_WIDE							20							//按钮宽度
#define PIC_BT_HIGHT						20							//按钮高度
#define BT_BUTTON_WIDE						65							//按钮宽度
#define BT_BUTTON_HIGHT						21							//按钮高度

BEGIN_MESSAGE_MAP(CShortMessage, CSkinDialogEx)
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnExpression)
	ON_BN_CLICKED(IDC_SEND_SHORT, OnBnClickedSendShort)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_EXPRESSION, OnBnClickedExpression)
END_MESSAGE_MAP()

//构造函数
CShortMessage::CShortMessage(CWnd * pParentWnd) : CSkinDialogEx(IDD_SHORT_MESSAGE)
{
	m_pParentWnd=NULL;
	m_dwTargetUserID=0L;
	m_pParentWnd=pParentWnd;
	m_pTargetUserItem=NULL;
}

//析构函数
CShortMessage::~CShortMessage()
{
}

//控件绑定函数
void CShortMessage::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEND_SHORT, m_BtSend);
	DDX_Control(pDX, IDC_EXPRESSION, m_BtExpression);
	DDX_Control(pDX, IDC_CLOSE, m_BtClose);
	DDX_Control(pDX, IDC_SEND_MESSAGE, m_SendMessage);
	DDX_Control(pDX, IDC_RECV_MESSAGE, m_RecvMessage);
}

//初始化函数
BOOL CShortMessage::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	if (m_pTargetUserItem!=NULL)
	{
		TCHAR szTitle[256];
		_snprintf(szTitle,sizeof(szTitle),TEXT("[ %s ] － [ %s ]"),m_pTargetUserItem->GetUserData()->szName,m_strRoomName);
		SetWindowText(szTitle);
	}

	//信息代理
	if (m_MessageProxyHelper.CreateInstance()==false) return false;
	if (m_MessageProxyHelper->SetRichEditHwnd(&m_RecvMessage)==false) return false;

	m_BtExpression.SetButtonImage(IDB_BT_EXPESSION,AfxGetInstanceHandle(),false);

	//设置控件
	m_RecvMessage.SetBackgroundColor(FALSE,RGB(255,255,255));
	m_SendMessage.LimitText(MAX_CHAT_LEN);
	m_SendMessage.SetFocus();

	return FALSE;
}

//设置信息
void CShortMessage::SetTalkInfo(IUserItem * pTargerUserItem, TCHAR * szRoomName)
{
	//设置变量
	bool bCleanMessage=((m_dwTargetUserID!=0L)&&(m_dwTargetUserID!=pTargerUserItem->GetUserData()->dwUserID));
	m_strRoomName=szRoomName;
	m_pTargetUserItem=pTargerUserItem;
	m_strTargerName=m_pTargetUserItem->GetUserData()->szName;
	m_dwTargetUserID=m_pTargetUserItem->GetUserData()->dwUserID;

	//设置控件
	if (GetSafeHwnd()!=NULL)
	{
		//设置标题
		TCHAR szTitle[256];
		_snprintf(szTitle,sizeof(szTitle),TEXT("[ %s ] － [ %s ]"),m_strTargerName,m_strRoomName);
		SetWindowText(szTitle);

		//清理屏幕
		if (bCleanMessage==true)
		{
			m_MessageProxyHelper->CleanScreen();
			m_SendMessage.SetWindowText(TEXT(""));
		}
		m_SendMessage.SetFocus();

		//重画控件
		Invalidate(FALSE);
	}

	return;
}

//确定按钮
void CShortMessage::OnOK()
{
	OnBnClickedSendShort();
}

//退出按钮
void CShortMessage::OnBnClickedClose()
{
	DestroyWindow();
	return;
}

//取消按钮
void CShortMessage::OnCancel()
{
	DestroyWindow();

	return;
}

//发送按钮
void CShortMessage::OnBnClickedSendShort()
{
	//判断信息
	m_SendMessage.SetFocus();
	if (m_SendMessage.GetWindowTextLength()==0)	return;
	
	try
	{
		//获取时间
		long int dwNowTime=(long int)time(NULL);

		//判断目标对象
		ASSERT(m_dwTargetUserID!=0L);
		if ((m_pTargetUserItem==NULL)||(m_dwTargetUserID!=m_pTargetUserItem->GetUserData()->dwUserID))
		{
			m_pTargetUserItem=((CRoomViewItem *)m_pParentWnd)->m_ClientUserManager.SearchUserByID(m_dwTargetUserID);
			if (m_pTargetUserItem==NULL) m_MessageProxyHelper->InsertSystemString(TEXT("目标玩家已经离开，信息发送失败！"),MS_NORMAL);
			return;
		}

		//获取消息
		CString strBuffer;
		m_SendMessage.GetWindowText(strBuffer);

		//过滤消息
		if (strBuffer.Find(TEXT("{\\rtf"))!=-1) throw TEXT("不能发送非法字符，发送失败！");
		for (int i=strBuffer.GetLength();i>0;i--)
			if ((strBuffer[i-1]!=TEXT('\n'))&&(strBuffer[i-1]!=TEXT('\r'))&&(strBuffer[i-1]>=-1)&&(strBuffer[i-1]<30)) throw TEXT("不能发送非法字符，发送失败！");

		//发送消息
		CMD_GR_Wisper Wisper;
		Wisper.dwTargetUserID=m_dwTargetUserID;
		Wisper.crFontColor=g_GlobalOption.m_crChatTX;
		Wisper.dwSendUserID=g_GlobalUnits.GetGolbalUserData().dwUserID;
		lstrcpyn(Wisper.szChatMessage,strBuffer,CountArray(Wisper.szChatMessage));
		Wisper.wChatLength=CountString(Wisper.szChatMessage);

		//发送命令
		WORD wSendSize=sizeof(Wisper)-sizeof(Wisper.szChatMessage)+Wisper.wChatLength;
		((CRoomViewItem *)m_pParentWnd)->m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_WISPER,&Wisper,wSendSize);

		//设置参数
		m_SendMessage.SetWindowText(TEXT(""));
	}
	catch (TCHAR * szErrorMessage) { m_MessageProxyHelper->InsertSystemString(szErrorMessage,MS_NORMAL); }
	catch (...) { m_MessageProxyHelper->InsertSystemString(TEXT("未知异常错误，消息发送失败！"),MS_NORMAL); }

	return;
}

//消息解释
BOOL CShortMessage::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		if ((::GetAsyncKeyState(VK_LCONTROL)&0xFFF0)||(::GetAsyncKeyState(VK_RCONTROL)&0xFFF0))
		{
			OnBnClickedSendShort();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

//收到消息
void CShortMessage::OnRecvMessage(TCHAR * szSendName, TCHAR * szMessage, COLORREF crColor, bool bMeMessage)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//消息头
	TCHAR szTitle[128]=TEXT("");
	_snprintf(szTitle,CountArray(szTitle),TEXT("%s %02d:%02d:%02d\n   "),szSendName,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
	m_MessageProxyHelper->InsertGeneralString(szTitle,(bMeMessage==true)?RGB(0,128,64):RGB(0,0,255),MS_NORMAL,false);

	//插入消息
	m_MessageProxyHelper->InsertShtickString(szMessage,crColor,MS_NORMAL,true);
	m_MessageProxyHelper->InsertGeneralString(TEXT(""),crColor,MS_NORMAL,true);

	return;
}

//获取位置
void CShortMessage::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	lpMMI->ptMinTrackSize.x=300;
	lpMMI->ptMinTrackSize.y=250;
	return;
}

//表情消息
LRESULT CShortMessage::OnExpression(WPARAM wparam, LPARAM lparam)
{
	CExpressionItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wparam);
	if (pExpItem!=NULL)
	{
		CString strBuffer;
		m_SendMessage.GetWindowText(strBuffer);
		strBuffer+=pExpItem->m_szTrancelate;
		m_SendMessage.SetWindowText(strBuffer);
		m_SendMessage.SetFocus();
		m_SendMessage.SetSel(strBuffer.GetLength(),strBuffer.GetLength());	
	}
	return 0;
}

//表情按钮
void CShortMessage::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) m_ExpressWnd.CreateExpression(this);

	//移动窗口
	CRect ButtonRect;
	CSize ExpressSize;
	m_ExpressWnd.GetFixSize(ExpressSize);
	m_BtExpression.GetWindowRect(&ButtonRect);
	m_ExpressWnd.MoveWindow(ButtonRect.left,ButtonRect.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();

	return;
}
