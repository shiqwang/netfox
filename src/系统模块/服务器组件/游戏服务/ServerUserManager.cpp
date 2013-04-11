#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserItem::CServerUserItem(void)
{
	InitServerUserItem();
}

//析构函数
CServerUserItem::~CServerUserItem(void)
{
}

//接口查询
void * __cdecl CServerUserItem::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//用户胜率
WORD __cdecl CServerUserItem::GetUserWinRate()
{
	//效验参数
	ASSERT(m_bAcitve==true);

	//计算胜率
	tagUserScore * pUserScore=&m_ServerUserData.UserScoreInfo;
	LONG lPlayCount=pUserScore->lWinCount+pUserScore->lLostCount+pUserScore->lDrawCount+pUserScore->lFleeCount;
	if (lPlayCount!=0L) return (WORD)(pUserScore->lWinCount*10000L/lPlayCount);

	return 0;
}

//用户输率
WORD __cdecl CServerUserItem::GetUserLostRate()
{
	//效验参数
	ASSERT(m_bAcitve==true);

	//计算输率
	tagUserScore * pUserScore=&m_ServerUserData.UserScoreInfo;
	LONG lPlayCount=pUserScore->lWinCount+pUserScore->lLostCount+pUserScore->lDrawCount+pUserScore->lFleeCount;
	if (lPlayCount!=0L) return (WORD)(pUserScore->lLostCount*10000L/lPlayCount);

	return 0;
}

//用户和率
WORD __cdecl CServerUserItem::GetUserDrawRate()
{
	//效验参数
	ASSERT(m_bAcitve==true);

	//计算和率
	tagUserScore * pUserScore=&m_ServerUserData.UserScoreInfo;
	LONG lPlayCount=pUserScore->lWinCount+pUserScore->lLostCount+pUserScore->lDrawCount+pUserScore->lFleeCount;
	if (lPlayCount!=0L) return (WORD)(pUserScore->lDrawCount*10000L/lPlayCount);

	return 0;
}

//用户逃率
WORD __cdecl CServerUserItem::GetUserFleeRate()
{
	//效验参数
	ASSERT(m_bAcitve==true);

	//计算逃率
	tagUserScore * pUserScore=&m_ServerUserData.UserScoreInfo;
	LONG lPlayCount=pUserScore->lWinCount+pUserScore->lLostCount+pUserScore->lDrawCount+pUserScore->lFleeCount;
	if (lPlayCount!=0L) return (WORD)(pUserScore->lFleeCount*10000L/lPlayCount);

	return 0;
}

//设置规则
bool __cdecl CServerUserItem::SetUserRule(tagUserRule & UserRule)
{
	//效验参数
	ASSERT(m_bAcitve==true);
	if (m_bAcitve==false) return false;

	//设置规则
	m_UserRule=UserRule;

	return true;
}

//修改积分
bool __cdecl CServerUserItem::WriteScore(tagScoreInfo & ScoreInfo, DWORD dwPlayTimeCount)
{
	//效验参数
	ASSERT(m_bAcitve==true);
	if (m_bAcitve==false) return false;

	//修改积分
	switch (ScoreInfo.ScoreKind)
	{
	case enScoreKind_Win: 
		{
			m_ScoreModify.lWinCount++;
			m_ServerUserData.UserScoreInfo.lWinCount++;
			break;
		}
	case enScoreKind_Lost: 
		{
			m_ScoreModify.lLostCount++;
			m_ServerUserData.UserScoreInfo.lLostCount++;
			break;
		}
	case enScoreKind_Draw:
		{
			m_ScoreModify.lDrawCount++;
			m_ServerUserData.UserScoreInfo.lDrawCount++;
			break;
		}
	case enScoreKind_Flee:
		{
			m_ScoreModify.lFleeCount++;
			m_ServerUserData.UserScoreInfo.lFleeCount++;
			break;
		}
	}

	//修改积分
	m_ScoreModify.lExperience++;
	m_ScoreModify.lScore+=ScoreInfo.lScore;
	m_ServerUserData.UserScoreInfo.lExperience++;
	m_ServerUserData.UserScoreInfo.lScore+=ScoreInfo.lScore;

	//游戏时间
	m_dwPlayTimeCount+=dwPlayTimeCount;

	return true;
}

//设置状态
bool __cdecl CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//效验状态
	ASSERT(m_bAcitve==true);
	if (m_bAcitve==false) return false;

	//设置变量
	m_ServerUserData.wTableID=wTableID;
	m_ServerUserData.wChairID=wChairID;
	m_ServerUserData.cbUserStatus=cbUserStatus;

	return true;
}

//初始化
void CServerUserItem::InitServerUserItem()
{
	m_bAcitve=false;
	m_dwClientIP=0L;
	m_szPassword[0]=0;
	m_wUserIndex=0xFFFF;
	m_dwPlayTimeCount=0L;
	m_dwLogonTime=(LONG)time(NULL);
	memset(&m_UserRule,0,sizeof(m_UserRule));
	memset(&m_ScoreBorn,0,sizeof(m_ScoreBorn));
	memset(&m_ScoreModify,0,sizeof(m_ScoreModify));
	memset(&m_ServerUserData,0,sizeof(m_ServerUserData));

	return;
}

//积分信息
bool __cdecl CServerUserItem::GetUserScoreInfo(tagUserScore & ScoreResultInfo, tagUserScore & ScoreModifyInfo)
{
	//效验状态
	ASSERT(m_bAcitve==true);
	if (m_bAcitve==false) return false;

	//设置变量
	ScoreModifyInfo=m_ScoreModify;
	ScoreResultInfo=m_ServerUserData.UserScoreInfo;

	return true;;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager(void)
{
}

//析构函数
CServerUserManager::~CServerUserManager(void)
{
	//释放用户
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++)
	{
		pServerUserItem=m_UserItemStore[i];
		ASSERT(pServerUserItem!=NULL);
		SafeDelete(pServerUserItem);
	}
	for (INT_PTR i=0;i<m_OnLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OnLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		SafeDelete(pServerUserItem);
	}
	for (INT_PTR i=0;i<m_OffLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OffLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		SafeDelete(pServerUserItem);
	}
	m_UserItemStore.RemoveAll();
	m_OnLineUserItem.RemoveAll();
	m_OffLineUserItem.RemoveAll();

	return;
}

//接口查询
void * __cdecl CServerUserManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//重置用户
bool __cdecl CServerUserManager::ResetUserManager()
{
	//恢复数组
	m_UserItemStore.Append(m_OnLineUserItem);
	m_UserItemStore.Append(m_OffLineUserItem);
	m_OnLineUserItem.RemoveAll();
	m_OffLineUserItem.RemoveAll();
	
	return true;
}

//切换用户
bool __cdecl CServerUserManager::SwitchOnLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//寻找用户
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_OnLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OnLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==pIServerUserItem)
		{
			//设置用户
			pServerUserItem->m_dwClientIP=dwClientIP;
			pServerUserItem->m_wUserIndex=wUserIndex;

			return true;
		}
	}
	ASSERT(FALSE);

	return true;
}

//激活用户
bool __cdecl CServerUserManager::ActiveOffLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetUserStatus()==US_OFFLINE);
	if (pIServerUserItem==NULL) return false;

	//寻找用户
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_OffLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OffLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==pIServerUserItem)
		{
			//设置用户
			pServerUserItem->m_dwClientIP=dwClientIP;
			pServerUserItem->m_wUserIndex=wUserIndex;

			//注销断线
			m_OffLineUserItem.RemoveAt(i);
			m_OnLineUserItem.Add(pServerUserItem);

			return true;
		}
	}
	ASSERT(FALSE);

	return false;
}

//激活用户
IServerUserItem * __cdecl CServerUserManager::ActiveUserItem(tagServerUserData & ServerUserData, DWORD dwClientIP, WORD wUserIndex, TCHAR szPassword[PASS_LEN])
{
	//获取空闲对象
	CServerUserItem * pServerUserItem=NULL;
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];
		ASSERT(pServerUserItem!=NULL);
		m_UserItemStore.RemoveAt(nItemPostion);
	}

	//创建新对象
	if (pServerUserItem==NULL)
	{
		try
		{
			pServerUserItem=new CServerUserItem;
			if (pServerUserItem==NULL) return NULL;
		}
		catch (...) { return NULL; }
	}

	//加入用户
	ASSERT(pServerUserItem!=NULL);
	m_OnLineUserItem.Add(pServerUserItem);
	pServerUserItem->InitServerUserItem();

	//设置用户
	pServerUserItem->m_bAcitve=true;
	pServerUserItem->m_wUserIndex=wUserIndex;
	pServerUserItem->m_dwClientIP=dwClientIP;
	pServerUserItem->m_dwLogonTime=(LONG)time(NULL);
	pServerUserItem->m_ScoreBorn=ServerUserData.UserScoreInfo;
	CopyMemory(&pServerUserItem->m_ServerUserData,&ServerUserData,sizeof(ServerUserData));
	lstrcpyn(pServerUserItem->m_szPassword,szPassword,CountArray(pServerUserItem->m_szPassword));

	return pServerUserItem;
}

//枚举用户
IServerUserItem * __cdecl CServerUserManager::EnumOnLineUser(WORD wEnumIndex)
{
	if (wEnumIndex>=m_OnLineUserItem.GetCount()) return NULL;
	return m_OnLineUserItem[wEnumIndex];
}

//枚举用户
IServerUserItem * __cdecl CServerUserManager::EnumOffLineUser(WORD wEnumIndex)
{
	if (wEnumIndex>=m_OffLineUserItem.GetCount()) return NULL;
	return m_OffLineUserItem[wEnumIndex];
}

//删除用户
bool __cdecl CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CServerUserItem * pServerUserItem=NULL;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//寻找用户
	if (pIServerUserItem->GetUserIndex()!=0xFFFF)
	{
		for (INT_PTR i=0;i<m_OnLineUserItem.GetCount();i++)
		{
			pServerUserItem=m_OnLineUserItem[i];
			ASSERT(pServerUserItem!=NULL);
			if (pServerUserItem==pIServerUserItem)
			{
				pServerUserItem->m_bAcitve=false;
				m_OnLineUserItem.RemoveAt(i);
				m_UserItemStore.Add(pServerUserItem);
				return true;
			}
		}
	}
	else
	{
		for (INT_PTR i=0;i<m_OffLineUserItem.GetCount();i++)
		{
			pServerUserItem=m_OffLineUserItem[i];
			ASSERT(pServerUserItem!=NULL);
			if (pServerUserItem==pIServerUserItem)
			{
				pServerUserItem->m_bAcitve=false;
				m_OffLineUserItem.RemoveAt(i);
				m_UserItemStore.Add(pServerUserItem);
				return true;
			}
		}
	}

	ASSERT(FALSE);

	return false;
}

//注册断线
bool __cdecl CServerUserManager::RegOffLineUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CServerUserItem * pServerUserItem=NULL;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//寻找用户
	ASSERT(pUserData->cbUserStatus!=US_OFFLINE);
	for (INT_PTR i=0;i<m_OnLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OnLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==pIServerUserItem)
		{
			pServerUserItem->m_wUserIndex=0xFFFF;
			m_OnLineUserItem.RemoveAt(i);
			m_OffLineUserItem.Add(pServerUserItem);
			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//删除断线
bool __cdecl CServerUserManager::UnRegOffLineUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CServerUserItem * pServerUserItem=NULL;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//寻找用户
	for (INT_PTR i=0;i<m_OffLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OffLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem==pIServerUserItem)
		{
			m_OffLineUserItem.RemoveAt(i);
			m_OnLineUserItem.Add(pServerUserItem);
			return true;
		}
	}

	return false;
}

//查找用户
IServerUserItem * __cdecl CServerUserManager::SearchOnLineUser(DWORD dwUserID)
{
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_OnLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OnLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem->m_ServerUserData.dwUserID==dwUserID) return pServerUserItem;
	}

	return NULL;
}

//查找用户
IServerUserItem * __cdecl CServerUserManager::SearchOffLineUser(DWORD dwUserID)
{
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_OffLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OffLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		if (pServerUserItem->m_ServerUserData.dwUserID==dwUserID) return pServerUserItem;
	}

	return NULL;
}

//查找用户
IServerUserItem * __cdecl CServerUserManager::SearchOnLineUser(LPCTSTR pszAccounts)
{
	CString strAccounts=pszAccounts,strAccountsTemp;
	strAccounts.MakeLower();
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_OnLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OnLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		strAccountsTemp=pServerUserItem->m_ServerUserData.szAccounts;
		strAccountsTemp.MakeLower();
		if (strAccounts==strAccountsTemp) return pServerUserItem;
	}

	return NULL;
}

//查找用户
IServerUserItem * __cdecl CServerUserManager::SearchOffLineUser(LPCTSTR pszAccounts)
{
	CString strAccounts=pszAccounts,strAccountsTemp;
	strAccounts.MakeLower();
	CServerUserItem * pServerUserItem=NULL;
	for (INT_PTR i=0;i<m_OffLineUserItem.GetCount();i++)
	{
		pServerUserItem=m_OffLineUserItem[i];
		ASSERT(pServerUserItem!=NULL);
		strAccountsTemp=pServerUserItem->m_ServerUserData.szAccounts;
		strAccountsTemp.MakeLower();
		if (strAccounts==strAccountsTemp) return pServerUserItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////