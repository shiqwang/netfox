#include "StdAfx.h"
#include "TableFrame.h"

//////////////////////////////////////////////////////////////////////////
extern void WriteLog(CString strFileName, CString strText);
//定时器 ID
#define IDI_OFFLINE					(IDI_MAX_TIME_ID+1)					//断线定时器

//////////////////////////////////////////////////////////////////////////

//管理权限定义
#define QX_CAN_AUTO_SIT				0x00000001L			//系统自动排位
#define QX_CAN_NO_SAME_IP			0x00000002L			//禁止同IP
#define QX_CAN_NO_CHAT				0x00000004L			//禁止聊天

//系统自动排位
bool CanAutoSit(LONG lMasterRight) { return ((lMasterRight&QX_CAN_AUTO_SIT)!=0); }

//禁止同IP
bool CanNoSameIP(LONG lMasterRight) { return ((lMasterRight&QX_CAN_NO_SAME_IP)!=0); }

//禁止聊天
bool CanNoChat(LONG lMasterRight) { return ((lMasterRight&QX_CAN_NO_CHAT)!=0); }

//构造函数
CTableFrame::CTableFrame()
{
	//属性变量
	m_wChairCount=0;
	m_wTableID=INVALID_TABLE;

	//状态变量
	m_bLocked=false;
	m_bStarted=false;
	m_dwTimeStart=0L;
	m_dwTableOwnerID=0L;
	m_bGameStatus=GS_FREE;
	m_szPassword[0]=0;

	//分数信息
	m_lGameTaxScore=0L;
	m_bWriteScore=false;
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	memset(m_dwPlayerID,0,sizeof(m_dwPlayerID));

	//用户信息
	memset(m_pIUserItem,0,sizeof(m_pIUserItem));
	memset(m_bAllowLookon,0,sizeof(m_bAllowLookon));
	memset(m_wOffLineCount,0,sizeof(m_wOffLineCount));

	//配置变量
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//接口变量
	m_pITableFrameSink=NULL;
	m_pITableFrameEvent=NULL;
	m_pIGameServiceFrame=NULL;

	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	if (m_pITableFrameSink!=NULL)
	{
		m_pITableFrameSink->Release();
		m_pITableFrameSink=NULL;
	}
	return;
}

//接口查询
void * __cdecl CTableFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ITableFrameManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameManager,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool __cdecl CTableFrame::InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter)
{
	//效验参数
	ASSERT(pTableFrameParameter!=NULL);

	//设置变量
	m_wTableID=wTableID;
	m_pGameServiceAttrib=pTableFrameParameter->pGameServiceAttrib;
	m_pGameServiceOption=pTableFrameParameter->pGameServiceOption;
	m_wChairCount=pTableFrameParameter->pGameServiceAttrib->wChairCount;

	//查询接口
	m_pIGameServiceFrame=GET_OBJECTPTR_INTERFACE(pTableFrameParameter->pIGameServiceFrame,IGameServiceFrame);
	ASSERT(m_pIGameServiceFrame!=NULL);
	if (m_pIGameServiceFrame==NULL) return false;

	//创建桌子
	IGameServiceManager * pIGameServiceManager=GET_OBJECTPTR_INTERFACE(pTableFrameParameter->pIGameServiceManager,IGameServiceManager);
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink==NULL) return false;

	//设置接口
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->InitTableFrameSink(pIUnknownEx)==false) return false;

	//扩展接口
	m_pITableFrameEvent=GET_OBJECTPTR_INTERFACE(m_pITableFrameSink,ITableFrameEvent);

	return true;
}

//游戏状态
bool __cdecl CTableFrame::IsUserPlaying(IServerUserItem * pIServerUserItem)
{
	//游戏状态
	if (m_bStarted==false) return false;

	//用户状态
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((cbUserStatus!=US_PLAY)&&(cbUserStatus!=US_OFFLINE)) return false;

	//逻辑判断
	WORD wChairID=pIServerUserItem->GetChairID();
	return m_pITableFrameSink->IsUserPlaying(wChairID);
}

//离开动作
bool __cdecl CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into PerformStandUpAction");
	WriteLog(strFile, strTemp);

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<m_wChairCount);

	//变量定义
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	WORD wChairID=pUserData->wChairID;
	bool bLookonUser=(pIServerUserItem->GetUserStatus()==US_LOOKON);

	//用户处理
	if (bLookonUser==true)
	{
		IServerUserItem * pILookonUserItem=NULL;
		for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
		{
			pILookonUserItem=m_LookonUserItemPtr[i];
			ASSERT(pILookonUserItem!=NULL);
			if (pILookonUserItem==pIServerUserItem)
			{
				//起立处理
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqStandUp(wChairID,pIServerUserItem,false);

				//设置用户
				m_LookonUserItemPtr.RemoveAt(i);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
				m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

				//起立处理
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserStandUp(wChairID,pIServerUserItem,true);

				return true;
			}
		}
	}
	else
	{
		//起立请求
		if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqStandUp(wChairID,pIServerUserItem,false);

		//结束游戏
		if (IsUserPlaying(pIServerUserItem)==true)
		{
			m_pITableFrameSink->OnEventGameEnd(pUserData->wChairID,pIServerUserItem,GER_USER_LEFT);
		}

		//离开判断
		if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

		//设置用户
		m_pIUserItem[pUserData->wChairID]=NULL;
		pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
		m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

		//设置桌主
		if (m_dwTableOwnerID==pUserData->dwUserID) m_dwTableOwnerID=0L;

		//更新密码
		WORD wUserCount=0;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) wUserCount++;
		}
		if (wUserCount==0)
		{
			m_bLocked=false;
			m_szPassword[0]=0;
			m_pIGameServiceFrame->SendTableStatus(m_wTableID);
		}

		//坐下处理
		if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserStandUp(wChairID,pIServerUserItem,false);

		//变量定义
		bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
		bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

		strTemp.Format("GAME_GENRE_MATCH 1");
		WriteLog(strFile, strTemp);

		//开始判断
		if ((bControlStart==false)&&(StartVerdict()==true))
		{
			StartGame();
			return true;
		}//End if

		return true;
	}

	return true;
}

//旁观动作
bool __cdecl CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//变量定义
	DWORD dwMasterRight=pIServerUserItem->GetUserData()->dwUserRight;

	//权限判断
	if (CUserRight::CanLookon(dwMasterRight)==false)
	{
		//发送消息
		LPCTSTR pszMessage=TEXT("抱歉，你没有进行旁观游戏的权限，若需要帮助，请联系游戏客服咨询！");
		SendSitFailedPacket(pIServerUserItem,pszMessage);

		return false;
	}

	//关闭查询
	if ((dwMasterRight==0L)&&(m_pIGameServiceFrame->IsShallClose()==true))
	{
		LPCTSTR pszDescribe=TEXT("由于此游戏房间即将暂停服务，玩家不允许再进入游戏桌！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//禁止查询
	if ((dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowEnterGame()==false))
	{
		LPCTSTR pszDescribe=TEXT("抱歉，此游戏桌现在不允许玩家进入！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//位置判断
	if (m_pIUserItem[wChairID]==NULL)
	{
		LPCTSTR pszDescribe=TEXT("所请求旁观的位置已经没有玩家了，不能旁观！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//密码效验
	if (m_bLocked==true)
	{
		const tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
		if ((pUserRule->bPassword==false)||(lstrcmp(m_szPassword,pUserRule->szPassword)!=0))
		{
			LPCTSTR pszDescribe=TEXT("游戏桌携带密码错误，不能旁观游戏！");
			SendSitFailedPacket(pIServerUserItem,pszDescribe);
			return false;
		}
	}

	//状态判断
	if ((m_bStarted==false)&&(dwMasterRight==0L))
	{
		LPCTSTR pszDescribe=TEXT("游戏还没有开始，暂时不能旁观！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//旁观处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqSitDown(wChairID,pIServerUserItem,true);

	//设置玩家
	m_LookonUserItemPtr.Add(pIServerUserItem);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

	//旁观处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserSitDown(wChairID,pIServerUserItem,true);

	return true;
}

//坐下动作
bool __cdecl CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into PerformSitDownAction(wChairID=%d");
	WriteLog(strFile, strTemp);
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//变量定义
	const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();;
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),* pTableUserRule=NULL;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData(),* pTableUserData=NULL;

	//关闭查询
	if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsShallClose()==true))
	{
		LPCTSTR pszDescribe=TEXT("由于此游戏房间即将暂停服务，玩家不允许再进入游戏桌！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//禁止查询
	if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowEnterGame()==false))
	{
		LPCTSTR pszDescribe=TEXT("抱歉，此游戏桌现在不允许玩家进入！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//位置判断
	if (m_pIUserItem[wChairID]!=NULL)
	{
		TCHAR szDescribe[128]=TEXT("");
		pTableUserData=m_pIUserItem[wChairID]->GetUserData();
		_snprintf(szDescribe,sizeof(szDescribe),TEXT("椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！"),pTableUserData->szAccounts);
		SendSitFailedPacket(pIServerUserItem,szDescribe);
		return false;
	}//End if

	//游戏状态
	if (m_bStarted==true)
	{
		LPCTSTR pszDescribe=TEXT("游戏已经开始了，暂时不能进入游戏桌！");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}//End if

	//比赛判断
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		strTemp.Format("GAME_GENRE_MATCH 2");
		WriteLog(strFile, strTemp);
		//比赛权限
		
		if (CUserRight::IsMatchUser(pUserData->dwUserRight)==false)
		{
			TCHAR szDescribe[128]=TEXT("");
			lstrcpyn(szDescribe,TEXT("这是游戏比赛房间，你不是比赛选手，不能坐到此位置上！ "),CountArray(szDescribe));
			SendSitFailedPacket(pIServerUserItem,szDescribe);
			return false;
		}//End if

		strTemp.Format("最低分%d,用户分数%d", m_pGameServiceOption->lLessScore, pUserData->UserScoreInfo.lScore);
		WriteLog(strFile, strTemp);

		//积分限制
		if (m_pGameServiceOption->lLessScore!=0L)
		{
			if (pUserData->UserScoreInfo.lScore<m_pGameServiceOption->lLessScore)
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("加入游戏至少需要 %ld 的游戏积分，你的积分不够，不能加入！"),m_pGameServiceOption->lLessScore);
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}
		}//End if

		if (m_pGameServiceOption->lHighScore!=0L)
		{
			if (pUserData->UserScoreInfo.lScore>m_pGameServiceOption->lHighScore)
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的游戏积分高于 %ld，不能加入游戏！"),m_pGameServiceOption->lHighScore);
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}
		}//End if

		//地址效验
		DWORD dwUserIP=pIServerUserItem->GetClientIP();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==dwUserIP))
			{
				if (pUserRule->bCheckSameIP)
				{
					LPCTSTR pszDescribe=TEXT("你设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与你 IP 地址相同的玩家，不能加入游戏！");
					SendSitFailedPacket(pIServerUserItem,pszDescribe);
					return false;
				}
				else
				{
					if (CanNoSameIP(m_pGameServiceOption->cbFZB ))
					{
						LPCTSTR pszDescribe=TEXT("此桌设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与你 IP 地址相同的玩家，不能加入游戏！");
						SendSitFailedPacket(pIServerUserItem,pszDescribe);
						return false;
					}
				}//End if
			}//End if
		}//End if

		if (CanNoSameIP(m_pGameServiceOption->cbFZB ))
		{
			for (WORD i=0;i<m_wChairCount-1;i++)
			{
				if (m_pIUserItem[i]!=NULL)
				{
					for (WORD j=i+1;j<m_wChairCount;j++)
					{
						if ((m_pIUserItem[j]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==m_pIUserItem[j]->GetClientIP()))
						{
							LPCTSTR pszDescribe=TEXT("你设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
							SendSitFailedPacket(pIServerUserItem,pszDescribe);
							return false;
						}
					}//End if
				}//End if
			}//End if
		}

		//对局局数
		if ((m_pGameServiceOption->cbLimitDraw==TRUE)&&(m_pGameServiceOption->lMatchDraw>0))
		{
			LONG lPlayCount=pUserData->UserScoreInfo.lWinCount+pUserData->UserScoreInfo.lLostCount+pUserData->UserScoreInfo.lDrawCount;
			if (lPlayCount>=m_pGameServiceOption->lMatchDraw)
			{
				TCHAR szDescribe[128]=TEXT("");
				lstrcpyn(szDescribe,TEXT("恭喜你，你的比赛局数已经完成了，不需要再继续比赛，请耐心等待赛果公布！ "),CountArray(szDescribe));
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}//End if
		}//End if
	}//End if
	else
	{
		//权限判断
		if (CUserRight::CanPlay(pUserData->dwUserRight)==false)
		{
			//发送消息
			LPCTSTR pszMessage=TEXT("抱歉，你没有进行游戏的权限，若需要帮助，请联系游戏客服咨询！");
			SendSitFailedPacket(pIServerUserItem,pszMessage);

			return true;
		}//End if

		//积分限制
		if (m_pGameServiceOption->lLessScore!=0L)
		{
			if (pUserData->UserScoreInfo.lScore<m_pGameServiceOption->lLessScore)
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("加入游戏至少需要 %ld 的游戏积分，你的积分不够，不能加入！"),m_pGameServiceOption->lLessScore);
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}
		}//End if
		if (m_pGameServiceOption->lHighScore!=0L)
		{
			if (pUserData->UserScoreInfo.lScore>m_pGameServiceOption->lHighScore)
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的游戏积分高于 %ld，不能加入游戏！"),m_pGameServiceOption->lHighScore);
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}
		}//End if

		//密码效验
		if (m_bLocked==true)
		{
			if ((pUserRule->bPassword==false)||(lstrcmp(m_szPassword,pUserRule->szPassword)!=0))
			{
				LPCTSTR pszDescribe=TEXT("携带密码错误，不能加入游戏！");
				SendSitFailedPacket(pIServerUserItem,pszDescribe);
				return false;
			}
		}//End if

		//积分范围
		WORD wWinRate=pIServerUserItem->GetUserWinRate();
		WORD wFleeRate=pIServerUserItem->GetUserFleeRate();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取规则
				pTableUserRule=m_pIUserItem[i]->GetUserRule();

				//逃率效验
				if ((pTableUserRule->bLimitFlee)&&(wFleeRate>pTableUserRule->wFleeRate))
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}

				//胜率效验
				if ((pTableUserRule->bLimitWin)&&(wWinRate<pTableUserRule->wWinRate))
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}

				//延时效验
				if ((pTableUserRule->bLimitDelay)&&(pUserData->wNetDelay>pTableUserRule->wNetDelay))
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的网络延时太高，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}

				//积分效验
				if (pTableUserRule->bLimitScore==true)
				{
					if (pUserScore->lScore>pTableUserRule->lMaxScore)
					{
						TCHAR szDescribe[128]=TEXT("");
						_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的积分太高，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
						SendSitFailedPacket(pIServerUserItem,szDescribe);
						return false;
					}
					if (pUserScore->lScore<pTableUserRule->lLessScore)
					{
						TCHAR szDescribe[128]=TEXT("");
						_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的积分太低，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
						SendSitFailedPacket(pIServerUserItem,szDescribe);
						return false;
					}
				}
			}
		}//End if

		//地址效验
		bool bCheckSameIP=pUserRule->bCheckSameIP;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				pTableUserRule=m_pIUserItem[i]->GetUserRule();
				if (pTableUserRule->bCheckSameIP==true) 
				{
					bCheckSameIP=true;
					break;
				}
			}
		}//End for

		//防作弊
//		if (bCheckSameIP==true  ||  m_pIGameServiceFrame->IsFZBRoom() || CanNoSameIP(m_pGameServiceOption->cbFZB ))
		if (bCheckSameIP==true  ||   CanNoSameIP(m_pGameServiceOption->cbFZB ))
		{
			DWORD dwUserIP=pIServerUserItem->GetClientIP();
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==dwUserIP))
				{
					if (pUserRule->bCheckSameIP)
					{
						LPCTSTR pszDescribe=TEXT("你设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与你 IP 地址相同的玩家，不能加入游戏！");
						SendSitFailedPacket(pIServerUserItem,pszDescribe);
						return false;
					}
					else
					{
						LPCTSTR pszDescribe=TEXT("此桌设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与你 IP 地址相同的玩家，不能加入游戏！");
						SendSitFailedPacket(pIServerUserItem,pszDescribe);
						return false;
					}//End if
				}//End if
			}//End for 
			for (WORD i=0;i<m_wChairCount-1;i++)
			{
				if (m_pIUserItem[i]!=NULL)
				{
					for (WORD j=i+1;j<m_wChairCount;j++)
					{
						if ((m_pIUserItem[j]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==m_pIUserItem[j]->GetClientIP()))
						{
							LPCTSTR pszDescribe=TEXT("你设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
							SendSitFailedPacket(pIServerUserItem,pszDescribe);
							return false;
						}//End if
					}//Enf for
				}//Enf if
			}//End for
		}
	}

	//坐下处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqSitDown(wChairID,pIServerUserItem,false);

	//更新密码
	WORD wUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL) wUserCount++;
	}
	if (wUserCount==0)
	{
		m_bLocked=pUserRule->bPassword;
		m_dwTableOwnerID=pUserData->dwUserID;
		if (m_bLocked==true) lstrcpyn(m_szPassword,pUserRule->szPassword,CountArray(m_szPassword));
	}//End if

	//设置玩家
	m_pIUserItem[wChairID]=pIServerUserItem;
	pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

	//坐下处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserSitDown(wChairID,pIServerUserItem,false);

	return true;
}

//断线事件
bool __cdecl CTableFrame::OnUserOffLine(WORD wChairID)
{
	//效验状态
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);

	//设置状态
	m_wOffLineCount[wChairID]++;
	m_pIUserItem[wChairID]->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[wChairID]);

	//断线处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserOffLine(wChairID,m_pIUserItem[wChairID]);

	//设置定时器
	WORD wTimerID=IDI_OFFLINE+wChairID;
	WPARAM wBindParam=m_pIUserItem[wChairID]->GetUserID();
	m_pIGameServiceFrame->SetTableTimer(m_wTableID,wTimerID,90000L,1,wBindParam);

	return true;
}

//重进事件
bool __cdecl CTableFrame::OnUserReConnect(WORD wChairID)
{
	//效验状态
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);

	//重入处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqReConnect(wChairID,m_pIUserItem[wChairID]);

	//删除定时器
	m_pIGameServiceFrame->KillTableTimer(m_wTableID,IDI_OFFLINE+wChairID);

	//设置用户
	m_pIUserItem[wChairID]->SetUserStatus(US_PLAY,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[wChairID]);

	//重入处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReConnect(wChairID,m_pIUserItem[wChairID]);

	return true;
}

//请求断线
bool __cdecl CTableFrame::OnUserReqOffLine(WORD wChairID)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into OnUserReqOffLine (%d", wChairID);
	WriteLog(strFile, strTemp);

	//效验参数
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	//断线处理
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqOffLine(wChairID,m_pIUserItem[wChairID]);

	//比赛过虑s
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) return m_wOffLineCount[wChairID]<5;

	return m_wOffLineCount[wChairID]<3;
}

//定时器事件
bool __cdecl CTableFrame::OnEventTimer(WORD wTimerID, WPARAM wBindParam)
{
	//桌子定时器
	if (wTimerID>IDI_MAX_TIME_ID)
	{
		switch (wTimerID)
		{
		case IDI_OFFLINE:
		case IDI_OFFLINE+1:
		case IDI_OFFLINE+2:
		case IDI_OFFLINE+3:
		case IDI_OFFLINE+4:
		case IDI_OFFLINE+5:
		case IDI_OFFLINE+6:
		case IDI_OFFLINE+7:		//断线定时器
			{
				//效验参数
				WORD wChairID=wTimerID-IDI_OFFLINE;
				ASSERT(wChairID<m_wChairCount);

				//效验状态
				if (m_bStarted==false) return false;

				//断线处理
				if (m_pIUserItem[wChairID]!=NULL)
				{
					tagServerUserData * pUserData=m_pIUserItem[wChairID]->GetUserData();
					if (pUserData->dwUserID!=wBindParam) return false;
					if (pUserData->cbUserStatus!=US_OFFLINE) return false;
					m_pITableFrameSink->OnEventGameEnd(wChairID,m_pIUserItem[wChairID],GER_USER_LEFT);

					return true;
				}

				return false;
			}
		}

		return false;
	}

	//游戏定时器
	return m_pITableFrameSink->OnTimerMessage(wTimerID,wBindParam);
}

//游戏事件处理
bool __cdecl CTableFrame::OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
}

//框架事件处理
bool __cdecl CTableFrame::OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into OnEventSocketFrame");
	WriteLog(strFile, strTemp);

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	bool bSuccess=m_pITableFrameSink->OnFrameMessage(wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
	if (bSuccess==false)
	{
		switch (wSubCmdID)
		{
		case SUB_GF_INFO:	//游戏信息
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GF_Info));
				if (wDataSize<sizeof(CMD_GF_Info)) return false;

				//变量定义
				CMD_GF_Info * pInfo=(CMD_GF_Info *)pDataBuffer;
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);

				//效验状态
				ASSERT(pUserData->wChairID<m_wChairCount);
				if (pUserData->wChairID>=m_wChairCount) return false;

				//设置变量
				if (bLookonUser==false) m_bAllowLookon[pUserData->wChairID]=pInfo->bAllowLookon?true:false;

				//发送配置
				CMD_GF_Option Option;
				Option.bGameStatus=m_bGameStatus;
				Option.bAllowLookon=m_bAllowLookon[pUserData->wChairID]?TRUE:FALSE;
				m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_OPTION,&Option,sizeof(Option));

				//发送场景
				bool bSendSecret=((bLookonUser==false)||(m_bAllowLookon[pUserData->wChairID]==true));
				return m_pITableFrameSink->SendGameScene(pUserData->wChairID,pIServerUserItem,m_bGameStatus,bSendSecret);
			}
		case SUB_GF_USER_READY:	//用户同意
			{
				//变量定义
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);

				//状态效验
				ASSERT(bLookonUser==false);
				ASSERT(m_pIUserItem[pUserData->wChairID]==pIServerUserItem);
				if (bLookonUser==true) return false;
				if (pUserData->cbUserStatus>=US_PLAY) return true;

				//同意处理
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqReady(pUserData->wChairID,m_pIUserItem[pUserData->wChairID]);

				//事件处理
				if ((m_pITableFrameEvent!=NULL)&&(m_pITableFrameEvent->OnEventUserReqReady(
					pUserData->wChairID,pIServerUserItem)==false)) return true;

				//设置变量
				pUserData->cbUserStatus=US_READY;

				//同意处理
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReady(pUserData->wChairID,m_pIUserItem[pUserData->wChairID]);

				//变量定义
				bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
				bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

				strTemp.Format("into GAME_GENRE_MATCH");
				WriteLog(strFile, strTemp);

				//开始判断
				if ((bControlStart==false)&&(StartVerdict()==true))
				{
					StartGame();
					return true;
				}//End if

				//发送状态
				m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

				return true;
			}
		case SUB_GF_USER_CHAT:	//用户聊天
			{
				//效验参数
				CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pDataBuffer;
				ASSERT(wDataSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)));
				ASSERT(wDataSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
				if (wDataSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
				if (wDataSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

				//变量定义
				bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
				bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

				strTemp.Format("into GAME_GENRE_MATCH 2");
				WriteLog(strFile, strTemp);

				//命令过虑
				const tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//防作弊房间
		//		if (   m_pIGameServiceFrame->IsFZBRoom() || m_pGameServiceOption->cbFZB )
				if (   CanNoChat(m_pGameServiceOption->cbFZB ) )
				{
					LPCTSTR pszMessage=TEXT("抱歉，本游戏房间为防作弊房间不允许发送聊天信息！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;				
				}//End if

				if (pUserData->dwMasterRight!=0L)
				{
					//比赛开始
					if ((bMatchServer==true)&&(bControlStart=true)&&(lstrcmp(pUserChat->szChatMessage,TEXT("/:StartGame"))==0))
					{
						if (StartVerdict()==true) StartGame();
						return true;
					}

					//游戏解散
					if (lstrcmp(pUserChat->szChatMessage,TEXT("/:DismissGame"))==0)
					{
						DismissGame();
						return true;
					}
				}//End if

				//比赛旁观
				if (pUserData->dwMasterRight==0L)
				{
					if ((bMatchServer==true)&&(pIServerUserItem->GetUserStatus()==US_LOOKON))
					{
						LPCTSTR pszMessage=TEXT("为了不影响比赛选手比赛，旁观的用户禁止发言！");
						SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
						return true;
					}
				}

				//权限判断
				if (CUserRight::CanGameChat(pIServerUserItem->GetUserData()->dwUserRight)==false)
				{
					LPCTSTR pszMessage=TEXT("你暂时没有发送房间消息的权限，只能与管理员私聊！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;
				}

				//状态查询
				if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowGameChat()==false))
				{
					LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不允许发送聊天信息！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;
				}

				//游戏玩家
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if (m_pIUserItem[i]!=NULL)
						m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_FRAME,SUB_GF_USER_CHAT,pUserChat,wDataSize);
				}

				//旁观玩家
				for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
				{
					m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i],MDM_GF_FRAME,SUB_GF_USER_CHAT,pUserChat,wDataSize);
				}

				return true;
			}
		}
	}

	return bSuccess;
}

//枚举用户
IServerUserItem * __cdecl CTableFrame::EnumLookonUserItem(WORD wIndex)
{
	if (wIndex>=m_LookonUserItemPtr.GetCount()) return NULL;
	return m_LookonUserItemPtr[wIndex];
}

//获取用户
IServerUserItem * __cdecl CTableFrame::GetServerUserItem(WORD wChairID)
{
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;
	return m_pIUserItem[wChairID];
}

//设置定时器
bool __cdecl CTableFrame::SetGameTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	//效验参数
	ASSERT(wTimerID<=IDI_MAX_TIME_ID);
	if (wTimerID>IDI_MAX_TIME_ID) return false;

	//设置定时器
	return m_pIGameServiceFrame->SetTableTimer(m_wTableID,wTimerID,dwElapse,dwRepeat,wBindParam);
}

//删除定时器
bool __cdecl CTableFrame::KillGameTimer(WORD wTimerID)
{
	//效验参数
	ASSERT(wTimerID<=IDI_MAX_TIME_ID);
	if (wTimerID>IDI_MAX_TIME_ID) return false;

	//删除定时器
	return m_pIGameServiceFrame->KillTableTimer(m_wTableID,wTimerID);
}

//发送数据
bool __cdecl CTableFrame::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
	return true;
}

//发送数据
bool __cdecl CTableFrame::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	return true;
}

//发送数据
bool __cdecl CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//群发用户
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) 
			{
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_GAME,wSubCmdID);
			}
		}
		return true;
	}

	//单一发送
	ASSERT(wChairID<m_wChairCount);
	if ((wChairID<m_wChairCount)&&(m_pIUserItem[wChairID]!=NULL))
	{
		m_pIGameServiceFrame->SendData(m_pIUserItem[wChairID],MDM_GF_GAME,wSubCmdID);
		return true;
	}

	return false;
}

//发送数据
bool __cdecl CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//群发用户
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) 
			{
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_GAME,wSubCmdID,pData,wDataSize);
			}
		}
		return true;
	}

	//单一发送
	ASSERT(wChairID<m_wChairCount);
	if ((wChairID<m_wChairCount)&&(m_pIUserItem[wChairID]!=NULL))
	{
		m_pIGameServiceFrame->SendData(m_pIUserItem[wChairID],MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		return true;
	}

	return false;
}

//发送数据
bool __cdecl CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	tagServerUserData * pUserData=NULL;
	IServerUserItem * pIServerUserItem=NULL;
	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		pIServerUserItem=m_LookonUserItemPtr[i];
		ASSERT(pIServerUserItem!=NULL);
		if (wChairID<m_wChairCount)
		{
			pUserData=pIServerUserItem->GetUserData();
			if (pUserData->wChairID==wChairID) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
		}
		else m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
	}

	return true;
}

//发送数据
bool __cdecl CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	tagServerUserData * pUserData=NULL;
	IServerUserItem * pIServerUserItem=NULL;
	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		pIServerUserItem=m_LookonUserItemPtr[i];
		ASSERT(pIServerUserItem!=NULL);
		if (wChairID<m_wChairCount)
		{
			pUserData=pIServerUserItem->GetUserData();
			if (pUserData->wChairID==wChairID) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}
		else m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//发送房间消息
bool __cdecl CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//构造数据包
	CMD_GR_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountString(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);

	return true;
}

//发送游戏消息
bool __cdecl CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//构造数据包
	CMD_GF_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountString(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);

	return true;
}

//解散游戏
bool __cdecl CTableFrame::DismissGame()
{
	//状态判断
	if (m_bStarted==false) return false;

	//解散游戏
	m_pITableFrameSink->OnEventGameEnd(INVALID_CHAIR,NULL,GER_DISMISS);

	return true;
}

//结束游戏
bool __cdecl CTableFrame::ConcludeGame()
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into ConcludeGame");
	WriteLog(strFile, strTemp);

	//设置变量
	m_bStarted=false;
	m_dwTimeStart=0L;
	m_lGameTaxScore=0L;
	m_bWriteScore=false;
	m_bGameStatus=GS_FREE;
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	memset(m_dwPlayerID,0,sizeof(m_dwPlayerID));
	memset(m_wOffLineCount,0,sizeof(m_wOffLineCount));

	//重置桌子
	m_pITableFrameSink->RepositTableFrameSink();

	//设置用户
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			pUserData=m_pIUserItem[i]->GetUserData();
			if (pUserData->cbUserStatus!=US_OFFLINE) pUserData->cbUserStatus=US_SIT;
		}
	}

	//断线处理
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=m_pIUserItem[i];
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE) continue;
			
			//删除定时器
			WORD wTimerID=IDI_OFFLINE+i;
			m_pIGameServiceFrame->KillTableTimer(m_wTableID,wTimerID);

			//用户离开
			PerformStandUpAction(pIServerUserItem);

			//清理用户资料
			m_pIGameServiceFrame->DeleteUserItem(pIServerUserItem);
		}
	}

	//赛局判断
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)&&
		(m_pGameServiceOption->cbLimitDraw==TRUE)&&(m_pGameServiceOption->lMatchDraw>0))
	{
		strTemp.Format("into GAME_GENRE_MATCH 3");
		WriteLog(strFile, strTemp);

		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//比赛判断
				LONG lPlayCount=pUserData->UserScoreInfo.lWinCount+pUserData->UserScoreInfo.lLostCount+pUserData->UserScoreInfo.lDrawCount;
				if (lPlayCount>=m_pGameServiceOption->lMatchDraw)
				{
					TCHAR szDescribe[256]=TEXT("");
					lstrcpyn(szDescribe,TEXT("恭喜你，你的比赛局数已经完成了，不需要再继续比赛，请耐心等待赛果公布！ "),CountArray(szDescribe));
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);
				}//End if
			}//End if
		}//End for
	}//End if

	//检测积分
	LONG lHighScore=m_pGameServiceOption->lHighScore;
	LONG lLessScore=m_pGameServiceOption->lLessScore;
	if ((lLessScore!=0L)||(lHighScore!=0))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//判断积分
				if ((lLessScore!=0L)&&(pUserData->UserScoreInfo.lScore<lLessScore))
				{
					TCHAR szDescribe[256]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的游戏积分少于 %ld，不能继续游戏！"),lLessScore);
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);
				}
				if ((lHighScore!=0L)&&(pUserData->UserScoreInfo.lScore>lHighScore))
				{
					TCHAR szDescribe[256]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("你的游戏积分高于 %ld，不能继续游戏！"),lHighScore);
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);
				}
			}
		}
	}

	//关闭判断
	if ((m_pIGameServiceFrame->IsAllowEnterGame()==false)||(m_pIGameServiceFrame->IsShallClose()==true))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//用户判断
				if (pUserData->dwMasterRight==0L)
				{
					//用户起立
					LPCTSTR pszMessage=TEXT("请注意，游戏房间即将关闭或者不允许玩家进入，请你离开游戏桌子！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);

					continue;
				}
			}
		}
	}

	//发送状态
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);

	return true;
}

//发送场景
bool __cdecl CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{
	//系统消息
	TCHAR szMessage[256]=TEXT("");
	//比赛房间
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		_snprintf(szMessage,sizeof(szMessage),TEXT("亲爱的用户，欢迎您来到“%s”比赛游戏房间，祝比赛选手赛出水平，取得好的成绩！"),m_pGameServiceAttrib->szKindName);
		SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
	}
	else
	{
		_snprintf(szMessage,sizeof(szMessage),TEXT("亲爱的用户，欢迎您来到“%s”，欢迎你多提宝贵建议！"),m_pGameServiceAttrib->szKindName);
		SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
	}

	//发送场景
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SCENE,pData,wDataSize);

	return true;
}

//写入积分
bool __cdecl CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, LONG lTaxScore)
{
	//效验状态
	ASSERT(m_bStarted==true);
	ASSERT(m_bWriteScore==false);
	if (m_bStarted==false) return false;
	if (m_bWriteScore==true) return false;

	//效验参数
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//效验积分
	//金币房间
	if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
	{
		LONG lAllScore=m_lGameTaxScore+lTaxScore;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_dwPlayerID[i]!=0)
			{
				lAllScore+=m_ScoreInfo[i].lScore;
				lAllScore+=ScoreInfoArray[i].lScore;
			}
		}
		ASSERT(lAllScore==0L);
		if (lAllScore!=0L) return false;
	}//End if

	//游戏时间
	DWORD dwPlayTimeCount=(DWORD)time(NULL)-m_dwTimeStart;

	//写入分数
	m_bWriteScore=true;
	m_lGameTaxScore+=lTaxScore;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			m_ScoreInfo[i]=ScoreInfoArray[i];
			m_pIUserItem[i]->WriteScore(ScoreInfoArray[i],dwPlayTimeCount);
			m_pIGameServiceFrame->SendUserScore(m_pIUserItem[i]);
		}
	}

	return true;
}

//写入积分
bool __cdecl CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo)
{
	//效验状态
	ASSERT(m_bStarted==true);
	ASSERT(m_bWriteScore==false);
	if (m_bStarted==false) return false;
	if (m_bWriteScore==true) return false;

	//效验参数
	ASSERT(ScoreInfo.lScore<=0);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (ScoreInfo.lScore>0) return false;
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	//修改积分
	m_pIUserItem[wChairID]->WriteScore(ScoreInfo,(DWORD)time(NULL)-m_dwTimeStart);
	m_pIGameServiceFrame->SendUserScore(m_pIUserItem[wChairID]);

	//记录信息
	m_ScoreInfo[wChairID]=ScoreInfo;

	return true;
}

//开始游戏
void CTableFrame::StartGame()
{
	//效验状态
	ASSERT(m_bStarted==false);
	if (m_bStarted==true) return;

	//设置变量
	m_bStarted=true;
	m_dwTimeStart=(DWORD)time(NULL);
	memset(m_wOffLineCount,0,sizeof(m_wOffLineCount));

	//记录分数
	m_lGameTaxScore=0L;
	m_bWriteScore=false;
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	memset(m_dwPlayerID,0,sizeof(m_dwPlayerID));

	//设置玩家
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			pUserData=m_pIUserItem[i]->GetUserData();
			pUserData->cbUserStatus=US_PLAY;
			m_dwPlayerID[i]=pUserData->dwUserID;
		}
	}

	//发送状态
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);

	//通知事件
	m_pITableFrameSink->OnEventGameStart();

	return;
}

//判断开始
bool CTableFrame::StartVerdict()
{
	//比赛判断
	if (m_bStarted==true) return false;

	//时间模式
	enStartMode StartMode=m_pITableFrameSink->GetGameStartMode();
	if (StartMode==enStartMode_TimeControl) return false;

	//准备人数
	WORD wReadyUserCount=0;
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++) 
	{
		if (m_pIUserItem[i]!=NULL)
		{
			wReadyUserCount++;
			pUserData=m_pIUserItem[i]->GetUserData();
			if (pUserData->cbUserStatus!=US_READY) return false;
		}
	}//End for (WORD i=0;i<m_wChairCount;i++)

	//条件判断
//Doctor 	if (wReadyUserCount>1L)
	if (wReadyUserCount>0L)//支持一个
	{
		if (StartMode==enStartMode_Symmetry)
		{
			if ((wReadyUserCount%2)!=0) return false;
			if (wReadyUserCount==m_wChairCount) return true;
			WORD wHalfCount=m_wChairCount/2;
			for (WORD i=0;i<wHalfCount;i++)
			{
				if ((m_pIUserItem[i]==NULL)&&(m_pIUserItem[i+wHalfCount]!=NULL)) return false;
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i+wHalfCount]==NULL)) return false;
			}
			return true;
		}
		else 
		{
			if (StartMode==enStartMode_FullReady) return (wReadyUserCount==m_wChairCount);
			if (StartMode==enStartMode_AllReady && wReadyUserCount >= 2) 
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}//End if (wReadyUserCount>1L)

	return false;
}

//发送坐下失败
void CTableFrame::SendSitFailedPacket(IServerUserItem * pIServerUserItem, LPCTSTR pszFailedDescribe)
{
	//构造数据
	CMD_GR_SitFailed SitFailed;
	memset(&SitFailed,0,sizeof(SitFailed));
	lstrcpyn(SitFailed.szFailedDescribe,pszFailedDescribe,CountArray(SitFailed.szFailedDescribe));

	//发送数据
	WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountString(SitFailed.szFailedDescribe);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

	return;
}

//////////////////////////////////////////////////////////////////////////
