#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wFirstUser=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bKouPaiPlayerCount=0;
	for(BYTE i=0;i<GAME_PLAYER;i++)
		m_bXianPaiCompetency[i]=true;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_bZhanPaiData[i].RemoveAll();
		m_bZhanPaiData[i].FreeExtra();
		ZeroMemory(&m_bUserOutCardInfo[i],sizeof(m_bUserOutCardInfo[i]));
	}

	//运行信息
	m_bTurnCardCount=0;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	

	//扑克信息
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bKouPaiPlayerCount=0;
	for(BYTE i=0;i<GAME_PLAYER;i++)
		m_bXianPaiCompetency[i]=true;

	//运行信息
	m_bTurnCardCount=0;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//扑克信息
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_bZhanPaiData[i].RemoveAll();
		m_bZhanPaiData[i].FreeExtra();
		ZeroMemory(&m_bUserOutCardInfo[i],sizeof(m_bUserOutCardInfo[i]));
	}


	return;
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

	//混乱扑克
	BYTE bRandCard[48];
	m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

	//分发扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_bCardCount[i]=16;
		CopyMemory(&m_bHandCardData[i],&bRandCard[i*m_bCardCount[i]],sizeof(BYTE)*m_bCardCount[i]);
		m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i]);
	}

	////设置用户
	//m_wFirstUser=m_wCurrentUser;
	//m_wCurrentUser=m_wFirstUser;
	srand((unsigned)time(NULL));
	m_wCurrentUser=(BYTE)(rand()%m_wPlayerCount);
	m_wBankerUser=m_wCurrentUser;
	m_wFirstUser=m_wCurrentUser;

	//发送扑克
	CMD_S_GameStart pGameStart;
	ZeroMemory(&pGameStart,sizeof(pGameStart));
	pGameStart.wBankUser=m_wCurrentUser;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		CopyMemory(pGameStart.bCardData,m_bHandCardData[i],sizeof(BYTE)*m_bCardCount[i]);
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&pGameStart,sizeof(pGameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&pGameStart,sizeof(pGameStart));
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}
			//计算输赢
			BYTE bTotalZhanPai=0;
			BYTE bLostPlayerCount=0;
			bool bHaveCiPai=false;
			//计算总占牌数
			for(BYTE k=0;k<m_wPlayerCount;k++)
			{
				bTotalZhanPai+=m_bZhanPaiData[k].GetSize();
				if(m_bZhanPaiData[k].GetSize()>=11)
					bHaveCiPai=true;
			}
			for(BYTE i=0;i<m_wPlayerCount;i++)
			{
				if(m_bZhanPaiData[i].GetSize()>=6)
					m_bUserOutCardInfo[i].IsWiner=1;
				else
				{
					m_bUserOutCardInfo[i].IsWiner=-1;
					bLostPlayerCount++;
				}
				if(bHaveCiPai)
					m_bUserOutCardInfo[i].IsCiPai=bTotalZhanPai-7;
			}
			//计算赢家分数
			long bTotalScore=0;
			for(BYTE j=0;j<GAME_PLAYER;j++)
			{
				int IsWiner=bLostPlayerCount==m_wPlayerCount?0:m_bUserOutCardInfo[j].IsWiner;
				if(IsWiner==-1)continue;
				BYTE CiPaiScore=m_bUserOutCardInfo[j].IsCiPai;
				BYTE LiangScore=m_bUserOutCardInfo[j].bLiangPai;
				BYTE OxScore=m_bUserOutCardInfo[j].bOxCard;
				BYTE XiScore=m_bUserOutCardInfo[j].bXiCard;
				BYTE XianScore=IsWiner?m_bUserOutCardInfo[j].bBeiXian:m_bUserOutCardInfo[j].bXianPai;
				if(bHaveCiPai)
					GameEnd.lGameScore[j]=IsWiner*((CiPaiScore+OxScore+XiScore)*bLostPlayerCount+XianScore*2);
				else
					GameEnd.lGameScore[j]=IsWiner*((LiangScore+OxScore+XiScore)*bLostPlayerCount+XianScore*2);


				bTotalScore+=GameEnd.lGameScore[j];

			}
			//计算输家分数
			for(BYTE m=0;m<GAME_PLAYER;m++)
			{
				if(bLostPlayerCount==1)
					if(m_bUserOutCardInfo[m].IsWiner==-1)
						GameEnd.lGameScore[m]=-bTotalScore;
				if(bLostPlayerCount==2)
					if(m_bUserOutCardInfo[m].IsWiner==-1)
						GameEnd.lGameScore[m]=-bTotalScore/2;
				if(bLostPlayerCount==3)
					GameEnd.lGameScore[m]=0;

			}

			//计算税收
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						GameEnd.lGameTax+=GameEnd.lGameScore[i];
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*45L/50L;
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			memset(&ScoreInfo,0,sizeof(ScoreInfo));
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,GameEnd.lGameTax);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.lGameScore[wChairID]=-20*m_pGameServiceOption->lCellScore;

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo;
			ScoreInfo.ScoreKind=enScoreKind_Draw;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;
	       
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_WK_SCORE:		//叫分状态
		{
			//构造数据
		 MessageBox(NULL,"GS_WK_SCORE",NULL,NULL);
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//设置变量
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID],m_bCardCount[wChiarID]);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));
			 MessageBox(NULL,"GS_WK_PLAYING",NULL,NULL);

			//设置变量
			StatusPlay.wLandUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_COUPAI:	//用户叫分
		{
			////效验数据
			//ASSERT(wDataSize==sizeof(CMD_C_CouPai));
			//if (wDataSize!=sizeof(CMD_C_CouPai)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			////消息处理
			//CMD_C_CouPai * pLandScore=(CMD_C_CouPai *)pDataBuffer;
			return OnUserKouPai(pUserData->wChairID);
		}
	case SUB_C_OUT_CART:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->bCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);
		}
	case SUB_C_XIANPAI:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			//消息处理
			return OnUserXianPai(pUserData->wChairID);

		}
	case SUB_C_QIANGCI:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			//消息处理
			return OnUserQiangCi(pUserData->wChairID);

		}
	case SUB_C_GIVEUP_QIANGCI:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			//消息处理
			return OnUserGiveUpQiangCi(pUserData->wChairID);

		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserPassCard(pUserData->wChairID);
		}
	}

	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//扣牌事件
bool CTableFrameSink::OnUserKouPai(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;


	//设置用户
	for(BYTE i=1;i<GAME_PLAYER;i++)
	{
		m_wCurrentUser=(wChairID+i)%m_wPlayerCount;				
		if(m_bXianPaiCompetency[m_wCurrentUser])
			break;

	}
	
	ASSERT(i!=GAME_PLAYER);
	if(i>=GAME_PLAYER) m_wCurrentUser=m_wBankerUser;
	ASSERT(m_bKouPaiPlayerCount<=3);
	
	//设置掀牌资格
	if(wChairID!=m_wBankerUser)
		m_bXianPaiCompetency[wChairID]=false;
	//发送消息
	CMD_S_User_CouPai CouPai;
	CouPai.wCouPaiUserID=wChairID;
	CouPai.wCurrentUser=m_wCurrentUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_COUPAI,&CouPai,sizeof(CouPai));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_COUPAI,&CouPai,sizeof(CouPai));
	//结束判断
	if(m_wCurrentUser==m_wBankerUser)
		OnEventGameEnd(wChairID,NULL,GER_NORMAL);	

	return true;
}
//用户掀牌
bool CTableFrameSink::OnUserXianPai(WORD wChairID)
{
	//变量设置
	m_bKouPaiPlayerCount=0;
	m_bUserOutCardInfo[wChairID].bXianPai++;
	m_bUserOutCardInfo[m_wBankerUser].bBeiXian++;
	//发送消息
	CMD_S_User_XianPai	XianPai;
	XianPai.wXianPaiUserID=wChairID;
	XianPai.wCurrentUser=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_XIANPAI,&XianPai,sizeof(XianPai));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_XIANPAI,&XianPai,sizeof(XianPai));

	return true;
}
//用户掀牌
bool CTableFrameSink::OnUserQiangCi(WORD wChairID)
{
	//设置变量
	m_wCurrentUser=wChairID;
	//发送消息
	CMD_S_User_QiangCi	QiangCi;
	QiangCi.wQiangCiUserID=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_QIANGCI,&QiangCi,sizeof(QiangCi));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_QIANGCI,&QiangCi,sizeof(QiangCi));

	return true;
}
//用户掀牌
bool CTableFrameSink::OnUserGiveUpQiangCi(WORD wChairID)
{
	//发送消息
	CMD_S_GiveUp_QiangCi	GiveUp;
	GiveUp.wGiveUpQiangCiUserID=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_GIVEUP_QIANGCI,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_GIVEUP_QIANGCI,&GiveUp,sizeof(GiveUp));

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_WK_PLAYING);
	ASSERT(wChairID==m_wCurrentUser);
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);
	if (bCardType==CT_INVALID) return false;
	//庄家设置
	m_wBankerUser=wChairID;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;
	else if (m_GameLogic.CompareCard(bCardData,m_bTurnCardData,bCardCount,m_bTurnCardCount)==false) return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;

	//出牌记录
	m_bTurnCardCount=bCardCount;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);
	//计分牌型统计
	if(m_GameLogic.IsXi(m_bTurnCardData,m_bTurnCardCount))
		m_bUserOutCardInfo[wChairID].bXiCard+=m_bTurnCardCount;
	if(m_GameLogic.IsOx(m_bTurnCardData,m_bTurnCardCount))
		m_bUserOutCardInfo[wChairID].bOxCard+=m_bTurnCardCount;
	if(m_GameLogic.IsXiBaoLiang(m_bTurnCardData,m_bTurnCardCount))
		m_bUserOutCardInfo[wChairID].bXiCard+=4;
	if(m_GameLogic.GetCardType(m_bTurnCardData,m_bTurnCardCount)==CT_LIANGZI)
		m_bUserOutCardInfo[wChairID].bLiangPai+=2;

////////////////////////////////////////////////////////////////////

	//切换用户
	bool bNewTurn=false;
	if (m_bCardCount[wChairID]!=0)
	{
		m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		if(m_wCurrentUser==m_wFirstUser)
		{
			m_wCurrentUser=m_wBankerUser;
			m_wFirstUser=m_wBankerUser;
			bNewTurn=true;
			for(BYTE i=0;i<m_bTurnCardCount;i++)
				m_bZhanPaiData[m_wBankerUser].Add(m_bTurnCardData[i]);		
		}
	}
	//构造数据
	CMD_S_OutCard OutCard;
	ZeroMemory(&OutCard,sizeof(CMD_S_OutCard));
	if(bNewTurn)
	{
		CopyMemory(&OutCard.bZhanPaiData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
		CopyMemory(&OutCard.bCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
		ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
		m_bTurnCardCount=0;
	}
	else
	{
		CopyMemory(&OutCard.bCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);

	}
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	OutCard.bNewTurn=bNewTurn;
	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	//判断结束
	BYTE bZhanPaiCount=0;
	for(BYTE j=0;j<GAME_PLAYER;j++)
	{
		bZhanPaiCount+=m_bZhanPaiData[j].GetSize();
	}
	if(bZhanPaiCount>=16)
		OnEventGameEnd(wChairID,NULL,GER_NORMAL);


	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_bTurnCardCount==0)) return false;
	ASSERT((wChairID==m_wCurrentUser)&&(m_bTurnCardCount));

	//设置变量
	bool bNewTurn=false;
	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	if(m_wCurrentUser==m_wFirstUser)
	{
		m_wCurrentUser=m_wBankerUser;
		m_wFirstUser=m_wBankerUser;	
		for(BYTE i=0;i<m_bTurnCardCount;i++)
			m_bZhanPaiData[m_wBankerUser].Add(m_bTurnCardData[i]);
		bNewTurn=true;
	}		

	//发送数据
	CMD_S_PassCard PassCard;
	ZeroMemory(&PassCard,sizeof(CMD_S_PassCard));
	if(bNewTurn)
	{
		CopyMemory(&PassCard.bZhanPaiData,m_bTurnCardData,sizeof(m_bTurnCardData));
		PassCard.bZhanpaiUser=m_wBankerUser;
		ZeroMemory(m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
		m_bTurnCardCount=0;
	}
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=bNewTurn;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	//判断结束
	BYTE bZhanPaiCount=0;
	for(BYTE j=0;j<GAME_PLAYER;j++)
	{
		bZhanPaiCount+=m_bZhanPaiData[j].GetSize();
	}		

	if(bZhanPaiCount>=16)
					OnEventGameEnd(wChairID,NULL,GER_NORMAL);
	return true;
}

//////////////////////////////////////////////////////////////////////////
