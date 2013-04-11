#include "Stdafx.h"
#include "SystemOption.h"
#include "ServerItemView.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define REG_GLOBAL_OPTIONS			TEXT("GlobalOption")				//全局配置

//////////////////////////////////////////////////////////////////////////

CGlobalOption						g_GlobalOption;						//全局配置

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerOption::CServerOption(WORD wServerID, CListServer * pListServer)
	: m_wServerID(wServerID),m_pListServer(pListServer)
{
	//携带密码
	m_bPassword=false;
	m_szPassword[0]=0;

	return;
}

//析构函数
CServerOption::~CServerOption()
{
}

//加载设置
void CServerOption::LoadOptionParameter()
{
	return;
}

//保存设置
void CServerOption::SaveOptionParameter()
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameOption::CGameOption(WORD wKindID, CListKind * pListKind) 
	: m_wKindID(wKindID),m_pListKind(pListKind)
{
	//积分限制
	m_bLimitScore=false;
	m_lMaxScore=2147483647;
	m_lLessScore=-2147483646;

	//延时限制
	m_wNetDelay=500;
	m_bLimitDelay=false;

	//逃跑限制
	m_wFleeRate=5000;
	m_bLimitFlee=false;

	//胜率限制
	m_wWinRate=0;
	m_bLimitWin=false;

	return;
}

//析构函数
CGameOption::~CGameOption()
{
}

//加载设置
void CGameOption::LoadOptionParameter()
{
	//变量定义
	CWinApp * pApp=AfxGetApp();
	tagGameKind * pGameKind=m_pListKind->GetItemInfo();
	LPCTSTR pszRegKeyName=pGameKind->szKindName;

	//积分限制
	m_lMaxScore=pApp->GetProfileInt(pszRegKeyName,TEXT("MaxScore"),m_lMaxScore);
	m_lLessScore=pApp->GetProfileInt(pszRegKeyName,TEXT("LessScore"),m_lLessScore);
	m_bLimitScore=pApp->GetProfileInt(pszRegKeyName,TEXT("LimitScore"),m_bLimitScore)?true:false;

	//延时限制
	m_wNetDelay=pApp->GetProfileInt(pszRegKeyName,TEXT("NetDelay"),m_wNetDelay);
	m_bLimitDelay=pApp->GetProfileInt(pszRegKeyName,TEXT("LimitDelay"),m_bLimitDelay)?true:false;

	//逃跑限制
	m_wFleeRate=pApp->GetProfileInt(pszRegKeyName,TEXT("FleeRate"),m_wFleeRate);
	m_bLimitFlee=pApp->GetProfileInt(pszRegKeyName,TEXT("LimitFlee"),m_bLimitFlee)?true:false;

	//胜利限制
	m_wWinRate=pApp->GetProfileInt(pszRegKeyName,TEXT("WinRate"),m_wWinRate);
	m_bLimitWin=pApp->GetProfileInt(pszRegKeyName,TEXT("LimitWin"),m_bLimitWin)?true:false;

	return;
}

//保存设置
void CGameOption::SaveOptionParameter()
{
	//变量定义
	CWinApp * pApp=AfxGetApp();
	tagGameKind * pGameKind=m_pListKind->GetItemInfo();
	LPCTSTR pszRegKeyName=pGameKind->szKindName;

	//积分限制
	pApp->WriteProfileInt(pszRegKeyName,TEXT("LimitScore"),m_bLimitScore);
	pApp->WriteProfileInt(pszRegKeyName,TEXT("MaxScore"),m_lMaxScore);
	pApp->WriteProfileInt(pszRegKeyName,TEXT("LessScore"),m_lLessScore);

	//延时限制
	pApp->WriteProfileInt(pszRegKeyName,TEXT("LimitDelay"),m_bLimitDelay);
	pApp->WriteProfileInt(pszRegKeyName,TEXT("NetDelay"),m_wNetDelay);

	//逃跑限制
	pApp->WriteProfileInt(pszRegKeyName,TEXT("FleeRate"),m_wFleeRate);
	pApp->WriteProfileInt(pszRegKeyName,TEXT("LimitFlee"),m_bLimitFlee);

	//胜利限制
	pApp->WriteProfileInt(pszRegKeyName,TEXT("WinRate"),m_wWinRate);
	pApp->WriteProfileInt(pszRegKeyName,TEXT("LimitWin"),m_bLimitWin);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalOption::CGlobalOption()
{
	//全局配置
	m_bExpandedList=true;
	m_enAcountsRule=enAcountsRule_Accounts;

	//房间设置
	m_bLimitDetest=true;
	m_bCheckSameIP=false;

	//消息设置
	m_bSaveWisper=true;
	m_bShowInOutMessage=true;
	m_bOnlyFriendMessage=false;
	m_bHideDetestMessage=true;

	//其他配置
	m_bAutoSit=true;
	m_InviteMode=enInviteMode_All;

	//颜色设置
	m_crChatTX=RGB(0,0,0);
	m_crMsgName=RGB(0,0,200);
	m_crMsgNormal=RGB(0,0,0);
	m_crMsgSysHead=RGB(200,0,0);
	m_crMsgSysString=RGB(0,0,200);
	m_crServerListTX=RGB(0,0,0);
	m_crServerListBK=RGB(200,200,200);

	return;
}

//析构函数
CGlobalOption::~CGlobalOption()
{
	//删除配置
	CGameOption * pGameOption=NULL;
	for (INT_PTR i=0;i<m_GameOptionArray.GetCount();i++)
	{
		pGameOption=m_GameOptionArray[i];
		ASSERT(pGameOption!=NULL);
		SafeDelete(pGameOption);
	}
	m_GameOptionArray.RemoveAll();

	//删除配置
	CServerOption * pServerOption=NULL;
	for (INT_PTR i=0;i<m_ServerOptionArray.GetCount();i++)
	{
		pServerOption=m_ServerOptionArray[i];
		ASSERT(pServerOption!=NULL);
		SafeDelete(pServerOption);
	}
	m_ServerOptionArray.RemoveAll();

	return;
}

//加载设置
void CGlobalOption::LoadOptionParameter()
{
	//变量定义
	CWinApp * pApp=AfxGetApp();

	//全局配置
	m_bExpandedList=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("ExpandedList"),m_bExpandedList)?true:false;
	m_enAcountsRule=(enAcountsRule)pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("AcountsRule"),m_enAcountsRule);

	//房间设置
	m_bLimitDetest=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("LimitDetest"),m_bLimitDetest)?true:false;
	m_bCheckSameIP=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("AutoHideServerList"),m_bCheckSameIP)?true:false;

	//消息设置
	m_bSaveWisper=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("SaveWisper"),m_bSaveWisper)?true:false;
	m_bShowInOutMessage=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("ShowInOutMessage"),m_bShowInOutMessage)?true:false;
	m_bOnlyFriendMessage=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("OnlyFriendMessage"),m_bOnlyFriendMessage)?true:false;
	m_bHideDetestMessage=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("HideDetestMessage"),m_bHideDetestMessage)?true:false;

	//其他配置
	m_bAutoSit=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("AutoSit"),m_bAutoSit)?true:false;
	m_InviteMode=(enInviteMode)pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("InviteMode"),m_InviteMode);

	//颜色设置
	m_crChatTX=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("ChatTX"),m_crChatTX);
	m_crMsgName=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgName"),m_crMsgName);
	m_crMsgNormal=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgNormal"),m_crMsgNormal);
	m_crMsgSysHead=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgSysHead"),m_crMsgSysHead);
	m_crMsgSysString=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgSysString"),m_crMsgSysString);
	m_crServerListTX=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("ServerListTX"),m_crServerListTX);
	m_crServerListBK=pApp->GetProfileInt(REG_GLOBAL_OPTIONS,TEXT("ServerListBK"),m_crServerListBK);

	//游戏配置
	CGameOption * pGameOption=NULL;
	for (INT_PTR i=0;i<m_GameOptionArray.GetCount();i++)
	{
		pGameOption=m_GameOptionArray[i];
		ASSERT(pGameOption!=NULL);
		pGameOption->LoadOptionParameter();
	}

	//房间配置
	CServerOption * pServerOption=NULL;
	for (INT_PTR i=0;i<m_ServerOptionArray.GetCount();i++)
	{
		pServerOption=m_ServerOptionArray[i];
		ASSERT(pServerOption!=NULL);
		pServerOption->LoadOptionParameter();
	}

	return;
}

//保存设置
void CGlobalOption::SaveOptionParameter()
{
	//变量定义
	CWinApp * pApp=AfxGetApp();

	//全局配置
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("ExpandedList"),m_bExpandedList);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("AcountsRule"),m_enAcountsRule);

	//房间设置
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("LimitDetest"),m_bLimitDetest);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("AutoHideServerList"),m_bCheckSameIP);

	//消息设置
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("SaveWisper"),m_bSaveWisper);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("ShowInOutMessage"),m_bShowInOutMessage);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("OnlyFriendMessage"),m_bOnlyFriendMessage);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("HideDetestMessage"),m_bHideDetestMessage);

	//其他配置
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("AutoSit"),m_bAutoSit);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("InviteMode"),m_InviteMode);

	//颜色设置
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("ChatTX"),m_crChatTX);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgName"),m_crMsgName);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgNormal"),m_crMsgNormal);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgSysHead"),m_crMsgSysHead);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("MsgSysString"),m_crMsgSysString);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("ServerListTX"),m_crServerListTX);
	pApp->WriteProfileInt(REG_GLOBAL_OPTIONS,TEXT("ServerListBK"),m_crServerListBK);

	//游戏配置
	CGameOption * pGameOption=NULL;
	for (INT_PTR i=0;i<m_GameOptionArray.GetCount();i++)
	{
		pGameOption=m_GameOptionArray[i];
		ASSERT(pGameOption!=NULL);
		pGameOption->SaveOptionParameter();
	}

	//房间配置
	CServerOption * pServerOption=NULL;
	for (INT_PTR i=0;i<m_ServerOptionArray.GetCount();i++)
	{
		pServerOption=m_ServerOptionArray[i];
		ASSERT(pServerOption!=NULL);
		pServerOption->SaveOptionParameter();
	}

	return;
}

//游戏配置
CGameOption * CGlobalOption::GetGameOption(CListKind * pListKind)
{
	//获取参数
	ASSERT(pListKind!=NULL);
	tagGameKind * pGameKind=pListKind->GetItemInfo();
	
	//寻找现存
	CGameOption * pGameOption=NULL;
	for (INT_PTR i=0;i<m_GameOptionArray.GetCount();i++)
	{
		pGameOption=m_GameOptionArray[i];
		ASSERT(pGameOption!=NULL);
		if (pGameOption->m_wKindID==pGameKind->wKindID) return pGameOption;
	}

	//创建对象
	try
	{
		pGameOption=new CGameOption(pGameKind->wKindID,pListKind);
		if (pGameOption==NULL) return NULL;
		pGameOption->LoadOptionParameter();
		m_GameOptionArray.Add(pGameOption);
		return pGameOption;
	}
	catch (...) {}

	return NULL;
}

//房间设置
CServerOption * CGlobalOption::GetServerOption(CListServer * pListServer)
{
	//获取参数
	ASSERT(pListServer!=NULL);
	tagGameServer * pGameServer=pListServer->GetItemInfo();
	
	//寻找现存
	CServerOption * pServerOption=NULL;
	for (INT_PTR i=0;i<m_ServerOptionArray.GetCount();i++)
	{
		pServerOption=m_ServerOptionArray[i];
		ASSERT(pServerOption!=NULL);
		if (pServerOption->m_wServerID==pGameServer->wServerID) return pServerOption;
	}

	//创建对象
	try
	{
		pServerOption=new CServerOption(pGameServer->wServerID,pListServer);
		if (pServerOption==NULL) return NULL;
		pServerOption->LoadOptionParameter();
		m_ServerOptionArray.Add(pServerOption);
		return pServerOption;
	}
	catch (...) {}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
