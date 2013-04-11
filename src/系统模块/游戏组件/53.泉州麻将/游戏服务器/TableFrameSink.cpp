#include "StdAfx.h"
#include "TableFrameSink.h"
#include ".\tableframesink.h"

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

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wDoorDiceCount = 0;
	m_wGoldDiceCount = 0;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	//ZeroMemory(m_cbDisPengIndex,sizeof(m_cbDisPengIndex));
	//ZeroMemory(m_cbDisHuIndex,sizeof(m_cbDisHuIndex));
	msg=NULL;
	m_GoldSwim = GoldSwim_None;
	m_bGameStart = false;
	memset(m_UserGoldSwim,GoldSwim_None,sizeof(m_UserGoldSwim));
	ZeroMemory(m_cbCurCardIndex,sizeof(m_cbCurCardIndex));
	 
	
	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbRepairFlowerPos = 0;
	m_cbMaxLeftCount = MAX_LEFT_COUNT;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbFlowerCount,sizeof(m_cbFlowerCount));
	ZeroMemory(m_cbGoldCount,sizeof(m_cbGoldCount));
	ZeroMemory(m_cbGangCount,sizeof(m_cbGangCount));
	m_cbDoorCard=0;


	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;


	//状态变量
	m_bSendStatus=false;
	m_bGangStatus=false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));	
	ZeroMemory(m_cbFlowerCard,sizeof(m_cbFlowerCard));
	m_bGoOnBankerTime = 1;
	
	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));
	ZeroMemory(m_lUserCapital,sizeof(m_lUserCapital));
	for(BYTE i=0;i<4;i++)m_bHuUser[i]=false;

	//组合麻将
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(&m_ChiHuResult,sizeof(m_ChiHuResult));


	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if(msg)
	{
	   delete[]msg;
	}	
}

//接口查询
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableFrameEvent,Guid,dwQueryVer);
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
	m_wDoorDiceCount = 0;
	m_wGoldDiceCount = 0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	//ZeroMemory(m_cbDisPengIndex,sizeof(m_cbDisPengIndex));
	//ZeroMemory(m_cbDisHuIndex,sizeof(m_cbDisHuIndex));
	m_GoldSwim = GoldSwim_None;
	m_bGameStart = false;
	memset(m_UserGoldSwim,GoldSwim_None,sizeof(m_UserGoldSwim));
	ZeroMemory(m_cbCurCardIndex,sizeof(m_cbCurCardIndex));


	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	msg=NULL;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbRepairFlowerPos = 0;
	m_cbMaxLeftCount = MAX_LEFT_COUNT;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbFlowerCount,sizeof(m_cbFlowerCount));
	ZeroMemory(m_cbGoldCount,sizeof(m_cbGoldCount));
	ZeroMemory(m_cbGangCount,sizeof(m_cbGangCount));
	m_cbDoorCard=0;


	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus=false;	
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));	
	ZeroMemory(m_cbFlowerCard,sizeof(m_cbFlowerCard));
	m_bGoOnBankerTime = 1;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));
	ZeroMemory(m_lUserCapital,sizeof(m_lUserCapital));

	//组合麻将
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(&m_ChiHuResult,sizeof(m_ChiHuResult));

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
	m_pITableFrame->SetGameStatus(GS_MJ_BEFOREPLAY);

	//设置变量
	m_cbRepairFlowerPos = 0;

	//中间变量
	IServerUserItem *pUserItem;

	//获取玩家资本
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		pUserItem=m_pITableFrame->GetServerUserItem(i);
		if(pUserItem!=NULL)
		{
			tagUserScore* pUserScore=(tagUserScore*)pUserItem->GetUserScore();
			m_lUserCapital[i]=pUserScore->lScore;
		}

	}

	//掷骰子确定庄家
	m_wBankerUser = (rand()%6+1)%GAME_PLAYER;

		for(BYTE i=0;i<4;i++)m_bHuUser[i]=false;		
		ZeroMemory(&m_ChiHuResult,sizeof(m_ChiHuResult));
		ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	
		//混乱麻将		
		m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
		m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

		//分发麻将
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			m_cbLeftCardCount-=(MAX_COUNT-1);
		
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);

		} 


		//设置变量
		m_cbProvideCard=0;
		m_wProvideUser=INVALID_CHAIR;
		m_wCurrentUser=m_wBankerUser;
		
		
		
		//发送消息
		CMD_S_GameStart GameStart;
		
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.wCurrentUser=m_wCurrentUser;			

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//设置变量			
			m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		
			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			

		}		
		
		return true;


	

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{	
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnEventGameEnd.log",strTime);

	strTemp.Format("into OnEventGameEnd");
	WriteLog(strFile, strTemp);

	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			msg=new char[100];			
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			//LONG lCellScore=m_pGameServiceOption->lCellScore;	
			GameEnd.lCellScore = m_pGameServiceOption->lCellScore;
			GameEnd.bGoOnBankerTime = m_bGoOnBankerTime;
			strcpy(msg,"");

			
			//东西南北
			bool bAllSidesCard[4];
			ZeroMemory(bAllSidesCard,sizeof(bAllSidesCard));
			//春夏秋冬
			bool bAllSeasonCard[4];
			ZeroMemory(bAllSeasonCard,sizeof(bAllSeasonCard));
			//梅兰竹菊
			bool bAllPlantCard[4];
			ZeroMemory(bAllPlantCard,sizeof(bAllPlantCard));


			//结束信息
			GameEnd.wProvideUser=wChairID;
			GameEnd.cbChiHuCard=m_cbChiHuCard;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.wChiHuKind[i]=m_ChiHuResult[i].wChiHuKind;					
				GameEnd.bFlowerCount[i]=m_cbFlowerCount[i];
				GameEnd.bGangCount[i]=m_cbGangCount[i];

				//统计金牌数目
				m_cbGoldCount[i]+=m_GameLogic.GetGoldCardCount(m_cbCardIndex[i],m_WeaveItemArray[i],m_cbWeaveItemCount[i],m_cbGoldCard,4);
				GameEnd.bGoldCount[i]=m_cbGoldCount[i];

				//梅兰竹菊
				if(m_GameLogic.IsExistAllPlantCard(m_cbCardIndex[i]))
				{
					bAllPlantCard[i]=true;
					GameEnd.bAllPlantCard[i]=true;
				}

				//春夏秋冬
				if(m_GameLogic.IsExistAllSeasonCard(m_cbCardIndex[i]))
				{
					bAllSeasonCard[i]=true;
					GameEnd.bAllSeasonCard[i]=true;
				}

				//东西南北
				if(m_GameLogic.IsExistAllSidesCard(m_cbCardIndex[i]))
				{
					bAllSidesCard[i]=true;
					GameEnd.bAllSidesCard[i]=true;
				}
			}			
			
			//统计玩家非胡牌翻数
			BYTE bUserTime[GAME_PLAYER];
			ZeroMemory(bUserTime,sizeof(bUserTime));

			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				bUserTime[i]=m_cbGoldCount[i]+m_cbFlowerCount[i]+m_cbGangCount[i]+
					(bAllPlantCard[i]?1:0)+(bAllSidesCard[i]?1:0)+(bAllSeasonCard[i]?1:0);
			}

			LONG lTmpScore=0L;
			LONG lBeiShu=1L;

			//统计积分
			if (m_wProvideUser!=INVALID_CHAIR)
			{
				
				//自摸类型
				if (m_ChiHuResult[m_wProvideUser].wChiHuKind!=CHK_NULL)
				{

					//基础分数
					LONG lCellScore=m_pGameServiceOption->lCellScore*2;
					
					//翻数计算					
					GameEnd.wWinner[m_wProvideUser]=true;

					//统计非胡牌玩家输赢翻数
					BYTE bTmpUserTime=0;		

					for(WORD i=0;i<GAME_PLAYER;i++)
					{
						//过滤玩家
						if(i==m_wProvideUser)continue;

						bTmpUserTime=0;

						//统计其余两家的翻数
						for(WORD j=0;j<GAME_PLAYER;j++)
						{
							//过滤玩家
							if((i==m_wProvideUser)||(i==j))continue;

							bTmpUserTime+=bUserTime[j];

						}

						//统计分数
						GameEnd.lGameScore[i]+=(2*bUserTime[i]-bTmpUserTime)*lCellScore*m_bGoOnBankerTime;	
				

					}

					

					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//赢家过滤
						if (i==m_wProvideUser) continue;						

						lBeiShu=1L;	
						lBeiShu*=m_bGoOnBankerTime;				

						//初始分数						
						LONG lInitScore=1;

						//翻牌分数
						LONG lTimeScore = bUserTime[m_wProvideUser]*lCellScore*m_bGoOnBankerTime;
						

						//积分统计
						switch(m_ChiHuResult[m_wProvideUser].wChiHuKind|m_ChiHuResult[m_wProvideUser].wChiHuRight)
						{

							//普通天游
						case CHK_SKY_SWIM:
							{
								lBeiShu*=8;

								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];

								GameEnd.lGameScore[i]-=lTmpScore;								
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"天游");

								break;
							}

							//杠上花天游
						case CHR_GANG_HU|CHK_SKY_SWIM:
							{
								lBeiShu*=16;

								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];

								GameEnd.lGameScore[i]-=lTmpScore;								
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"天游,杠上花");

								break;
							}

							//普通牌型					
						case CHK_PING_HU:
							{								
								lInitScore=2;
								
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];


								GameEnd.lGameScore[i]-=lTmpScore;								
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"自摸平胡");
								
								break;
							}		
					
						case CHR_GANG_HU|CHK_PING_HU:
							{								
								lInitScore=2;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"杠上花");
								
								break;
							}

							//游金牌型
						case CHR_SINGLE_SWIM|CHK_PING_HU:
						case CHK_SINGLE_SWIM:
						case CHK_SINGLE_SWIM|CHR_SINGLE_SWIM:
						case CHK_SINGLE_SWIM|CHR_SINGLE_SWIM|CHK_PING_HU:
							{
								lInitScore=3;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"游金");
								
								break;

							}
						case CHR_GANG_HU|CHR_SINGLE_SWIM|CHK_PING_HU:
						case CHR_GANG_HU|CHK_SINGLE_SWIM:
						case CHR_GANG_HU|CHK_SINGLE_SWIM|CHR_SINGLE_SWIM:
						case CHR_GANG_HU|CHK_SINGLE_SWIM|CHR_SINGLE_SWIM|CHK_PING_HU:
							{
								lInitScore=6;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"游金,杠上花");
								
								break;

							}

							//双游牌型
						case CHR_DOUBLE_SWIM|CHK_PING_HU:
						case CHK_DOUBLE_SWIM:
						case CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM:
						case CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM|CHK_PING_HU:
							{
								lInitScore=4;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"双游");
								
								break;

							}
						case CHR_GANG_HU|CHR_DOUBLE_SWIM|CHK_PING_HU:
						case CHR_GANG_HU|CHK_DOUBLE_SWIM:
						case CHR_GANG_HU|CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM:
						case CHR_GANG_HU|CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM|CHK_PING_HU:
							{
								lInitScore=8;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"双游,杠上花");
								
								break;

							}
							//三游牌型
						case CHR_THREE_SWIM|CHK_PING_HU:
						case CHK_THREE_SWIM:
						case CHK_THREE_SWIM|CHR_THREE_SWIM:
						case CHK_THREE_SWIM|CHR_THREE_SWIM|CHK_PING_HU:
							{
								lInitScore=16;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"三游");
								
								break;

							}
						case CHR_GANG_HU|CHK_THREE_SWIM|CHK_PING_HU:
						case CHR_GANG_HU|CHK_THREE_SWIM:
						case CHR_GANG_HU|CHK_THREE_SWIM|CHR_THREE_SWIM:
						case CHR_GANG_HU|CHK_THREE_SWIM|CHR_THREE_SWIM|CHK_PING_HU:
							{
								lInitScore=32;
								lTmpScore=lInitScore*lBeiShu*lCellScore+lTimeScore;	

								//资本不够
								if(m_lUserCapital[i]<0)lTmpScore=0;
								else if(m_lUserCapital[i]<lTmpScore)lTmpScore=m_lUserCapital[i];
								
								GameEnd.lGameScore[i]-=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]+=lTmpScore;								
								strcpy(msg,"三游,杠上花");
								
								break;

							}										
						
						default:
							break;

						}
					}		   

					//胡牌数目
					//wChiHuUserCount=1;

					//庄家设置
					if(m_wBankerUser==m_wProvideUser)//庄家连庄
					{
						m_wBankerUser=m_wProvideUser;

						//变量设置
						m_bGoOnBankerTime*=2;
					}
					else//下家当庄
					{
						m_wBankerUser = (m_wBankerUser+GAME_PLAYER-1)%GAME_PLAYER;

						//变量设置
						m_bGoOnBankerTime=1;
					}
				}

				//捉炮类型
				if (m_ChiHuResult[m_wProvideUser].wChiHuKind==CHK_NULL)
				{

					//基础分数
					LONG lCellScore = m_pGameServiceOption->lCellScore;

					//统计非胡牌玩家输赢翻数
					BYTE bTmpUserTime=0;
					for(WORD i=0;i<GAME_PLAYER;i++)
					{
						//过滤玩家
						if(m_ChiHuResult[i].wChiHuKind!=CHK_NULL)continue;

						bTmpUserTime=0;

						for(BYTE j=0;j<GAME_PLAYER;j++)
						{
							//过滤玩家
							if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;

							bTmpUserTime+=bUserTime[j];
						}

						//统计分数
						GameEnd.lGameScore[i]+=(2*bUserTime[i]-bTmpUserTime)*lCellScore*m_bGoOnBankerTime;	

					}

					

				
					


					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//输家过滤
						if (m_ChiHuResult[i].wChiHuKind==CHK_NULL) continue;	

						

						//赢家							
						GameEnd.wWinner[i]=true;						
						
						lBeiShu=1L;	

						//连庄次数
						lBeiShu*=m_bGoOnBankerTime;
						

						//花牌分数
						LONG lTimeScore = bUserTime[i]*lCellScore*m_bGoOnBankerTime;						


						//test 
						/*char szBuffer[20];
						sprintf(szBuffer,TEXT("%d"),m_ChiHuResult[i].wChiHuKind|m_ChiHuResult[i].wChiHuRight);
						AfxMessageBox(szBuffer);*/
						

						switch(m_ChiHuResult[i].wChiHuKind|m_ChiHuResult[i].wChiHuRight)						
						{
						//普通牌型
						case CHK_PING_HU:	
							{
								lBeiShu*=2;
								
								lTmpScore =lBeiShu*lCellScore+lTimeScore;
								/*

								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}
								strcpy(msg,"平胡");	
							
								break;
							}			
						
						case  CHR_GANG_HU|CHK_PING_HU:
							{
								
								lBeiShu*=2;

								lTmpScore =lBeiShu*lCellScore+lTimeScore;

								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}																
								
								strcpy(msg,"杠上泡");
								
								break;
							}

						//游金牌型
						case CHR_SINGLE_SWIM|CHK_PING_HU:
						case CHK_SINGLE_SWIM:
						case CHK_SINGLE_SWIM|CHR_SINGLE_SWIM:
						case CHK_SINGLE_SWIM|CHR_SINGLE_SWIM|CHK_PING_HU:
							{
								lBeiShu*=6;
								lTmpScore=lBeiShu*lCellScore+lTimeScore;	

								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}								
								strcpy(msg,"游金");
								
								break;

							}
						case CHR_GANG_HU|CHR_SINGLE_SWIM|CHK_PING_HU:
						case CHR_GANG_HU|CHK_SINGLE_SWIM:
						case CHR_GANG_HU|CHK_SINGLE_SWIM|CHR_SINGLE_SWIM:
						case CHR_GANG_HU|CHK_SINGLE_SWIM|CHR_SINGLE_SWIM|CHK_PING_HU:
							{
								lBeiShu*=12;
								lTmpScore=lBeiShu*lCellScore+lTimeScore;	

								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}							
								strcpy(msg,"游金,杠上花");
								
								break;

							}

						//双游牌型
						case CHR_DOUBLE_SWIM|CHK_PING_HU:
						case CHK_DOUBLE_SWIM:
						case CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM:
						case CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM|CHK_PING_HU:
							{
								lBeiShu*=8;
								lTmpScore=lBeiShu*lCellScore+lTimeScore;	

								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}								
								strcpy(msg,"双游");
								
								break;

							}
						case CHR_GANG_HU|CHR_DOUBLE_SWIM|CHK_PING_HU:
						case CHR_GANG_HU|CHK_DOUBLE_SWIM:
						case CHR_GANG_HU|CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM:
						case CHR_GANG_HU|CHK_DOUBLE_SWIM|CHR_DOUBLE_SWIM|CHK_PING_HU:
							{
								lBeiShu*=16;
								lTmpScore=lBeiShu*lCellScore+lTimeScore;	

								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}									
								strcpy(msg,"双游,杠上花");
								
								break;

							}

						//三游牌型
						case CHR_THREE_SWIM|CHK_PING_HU:
						case CHK_THREE_SWIM:
						case CHK_THREE_SWIM|CHR_THREE_SWIM:
						case CHK_THREE_SWIM|CHR_THREE_SWIM|CHK_PING_HU:
							{
								lBeiShu*=32;
								lTmpScore=lBeiShu*lCellScore+lTimeScore;	

								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}								
								strcpy(msg,"三游");
								
								break;

							}
						case CHR_GANG_HU|CHK_THREE_SWIM|CHK_PING_HU:
						case CHR_GANG_HU|CHK_THREE_SWIM:
						case CHR_GANG_HU|CHK_THREE_SWIM|CHR_THREE_SWIM:
						case CHR_GANG_HU|CHK_THREE_SWIM|CHR_THREE_SWIM|CHK_PING_HU:
							{
								lBeiShu*=64;
								lTmpScore=lBeiShu*lCellScore+lTimeScore;	
								
								/*
								//资本不够
								if(m_lUserCapital[m_wProvideUser]<0)lTmpScore=0;
								else if(m_lUserCapital[m_wProvideUser]<lTmpScore)lTmpScore=m_lUserCapital[m_wProvideUser];

								GameEnd.lGameScore[i]+=lTmpScore;
								GameEnd.lGameScore[m_wProvideUser]-=lTmpScore;	
								*/

								for(BYTE j=0;j<GAME_PLAYER;j++)
								{
									if((i==j)||(m_ChiHuResult[j].wChiHuKind!=CHK_NULL))continue;
									//资本不够
									if(m_lUserCapital[j]<0)lTmpScore=0;
									else if(m_lUserCapital[j]<lTmpScore)lTmpScore=m_lUserCapital[j];

									GameEnd.lGameScore[i]+=lTmpScore;
									GameEnd.lGameScore[j]-=lTmpScore;	

								}									
								strcpy(msg,"三游,杠上花");
								
								break;

							}
										
						
						default:							
							break;

						}

						if(i==m_wBankerUser)
						{							
							//变量设置
							m_bGoOnBankerTime*=2;
						}
						else//下家当庄
						{
							m_wBankerUser = (m_wBankerUser+GAME_PLAYER-1)%GAME_PLAYER;

							//变量设置
							m_bGoOnBankerTime=1;
						}
						
						//庄家设置
						//if(wNextBankerUser==INVALID_CHAIR)m_wBankerUser=i;

						

						//积分统计

						
						//设置变量
						//wNextBankerUser=i;						
						//wChiHuUserCount++;
						
						
					}
					//庄家设置
					//m_wBankerUser=(wChiHuUserCount>=2)?m_wProvideUser:wNextBankerUser;
				}				
			}			
			else
			{	
							
					GameEnd.cbChiHuCard=0;
				
			}
			
			//拷贝麻将
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(GameEnd.wWinner[i]==false)
					GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
				else 
					GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCurCardIndex[i],GameEnd.cbCardData[i]);
			}		

			if(GameEnd.cbChiHuCard==0)
			{			
				ZeroMemory(GameEnd.strEnd,sizeof(GameEnd.strEnd));
			}
			else
			    strcpy(GameEnd.strEnd,msg) ;

			//发送信息

			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						GameEnd.lGameTax+=GameEnd.lGameScore[i]/10L;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*45L/50L;
					}
				}
			}
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				if (GameEnd.lGameScore[i]==0L) ScoreInfo[i].ScoreKind=enScoreKind_Draw;
				else ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,0L);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.wProvideUser=INVALID_CHAIR;
			GameEnd.lGameScore[wChairID]=-50*m_pGameServiceOption->lCellScore;
			GameEnd.lCellScore = m_pGameServiceOption->lCellScore;
			GameEnd.bGoOnBankerTime = m_bGoOnBankerTime;
			//test
			for(WORD k=0;k<GAME_PLAYER;k++)
			{
				if(k==wChairID)continue;
					GameEnd.lGameScore[k]=8*m_pGameServiceOption->lCellScore;
					GameEnd.bFlowerCount[k]=m_cbFlowerCount[k];
					GameEnd.bGoldCount[k]=m_cbGoldCount[k];	
					GameEnd.bGangCount[k]=m_cbGangCount[k];
			}

			//通知消息
			TCHAR szMessage[128]=TEXT("");
			_snprintf(szMessage,CountArray(szMessage),TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetAccounts());
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pISendUserItem!=NULL) m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			}
			WORD wIndex=0;
			do
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
				if (pISendUserItem==NULL) break;
				m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			} while (true);

		
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						GameEnd.lGameTax+=GameEnd.lGameScore[i]/10L;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*45L/50L;
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo;
			ScoreInfo.ScoreKind=enScoreKind_Flee;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//变量定义
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wSiceCount=m_wDoorDiceCount;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;

			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;

			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard,m_cbDiscardCard,sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//组合麻将
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			//麻将数据
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID],StatusPlay.cbCardData);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GS_MJ_BEFOREPLAY:
		{
			return m_pITableFrame->SendGameScene(pIServerUserItem,NULL,0);
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
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pUserData->wChairID,pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_OPENDOOR://开门消息
		{	
			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if(pUserData->cbUserStatus!=US_PLAY)return true;

			return OnUserOpenDoor();
		}

	case SUB_C_OPENGOLD://开金消息
		{
			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if(pUserData->cbUserStatus!=US_PLAY)return true;

			return OnUserOpenGold();
		}
	case SUB_C_REPAIRFLOWER://补花消息
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_RepairFlower));
			if(wDataSize!=sizeof(CMD_C_RepairFlower))return false;

			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if(pUserData->cbUserStatus!=US_PLAY)return true;

			return OnUserRepairFlower(pDataBuffer);


		}

	}

	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool __cdecl CTableFrameSink::OnEventUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR))
	{
		m_wBankerUser=pIServerUserItem->GetChairID();
	}

	//变量设置
	m_bGoOnBankerTime = 1;

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(wChairID==m_wBankerUser))
	{
		m_wBankerUser=INVALID_CHAIR;

		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if ((i!=wChairID)&&(m_pITableFrame->GetServerUserItem(i)!=NULL))
			{
				m_wBankerUser=i;
				break;
			}
		}
	}

	//变量设置
	m_bGoOnBankerTime = 1;

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) return false;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return false;

	//删除麻将
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//游戏开始
	if(m_bGameStart&&(wChairID!=m_wBankerUser))m_bGameStart=false;


	//设置变量
	m_bSendStatus=true;
	m_bEnjoinChiHu[wChairID]=true;
	BYTE cbIndex=m_GameLogic.SwitchToCardIndex(cbCardData);
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(i==wChairID)
		{
			m_bEnjoinChiPeng[i]=true;
			continue;
		}

		//if(m_cbDisPengIndex[i][cbIndex]>0)//碰牌限制
			//m_bEnjoinChiPeng[i]=true;
		//else
			m_bEnjoinChiPeng[i]=false;
		//if(m_cbDisHuIndex[i][cbIndex]>0)//胡牌限制
			//m_bEnjoinChiHu[i]=true;
		//else
			m_bEnjoinChiHu[i]=false;
	}
	
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	//打出金牌
	if(m_GameLogic.IsGoldCard(cbCardData,m_cbGoldCard,4))m_cbGoldCount[wChairID]++;//设置变量

	if(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY)
	{
		//游牌状态
		bool bGoldSwim=false;
		tagChiHuResult ChiHuResult;
		WORD wChiHuRight=0;
		BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
		tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
		/*for(BYTE i=0;i<4;i++)
		{
			bGoldCardEye=false;
			m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,m_cbGoldCard[i],WIK_NULL,ChiHuResult,m_cbGoldCard,4,bGoldCardEye);
			if(bGoldCardEye)break;
		}*/
		//判断游牌
		//CopyMemory(m_cbCurCardIndex[wChairID],m_cbCardIndex[wChairID],sizeof(m_cbCardIndex[wChairID]));
		if(m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,cbCardData,wChiHuRight,ChiHuResult,m_cbGoldCard,4,m_cbCurCardIndex[wChairID]))
			bGoldSwim=true;

		if(bGoldSwim==true)
		{
			if(m_UserGoldSwim[wChairID]==GoldSwim_None)//无游牌状态
			{
				m_UserGoldSwim[wChairID]=GoldSwim_Single;
			}
			else 
			{
				if(m_GameLogic.IsGoldCard(cbCardData,m_cbGoldCard,4))//打出金牌
				{
					//单游变双游
					if(m_UserGoldSwim[wChairID]==GoldSwim_Single)m_UserGoldSwim[wChairID]=GoldSwim_Double;
					else m_UserGoldSwim[wChairID]=GoldSwim_Three;//双游变三游

					
				}
				else 
					m_UserGoldSwim[wChairID]=GoldSwim_Single;//单游状态
			}
		}
		else
		{
			m_UserGoldSwim[wChairID]=GoldSwim_None;//无游牌状态
		}

		//发送消息
		CMD_S_GoldSwim GoldSwim;

		for(WORD i=0;i<GAME_PLAYER;i++)
			GoldSwim.GoldSwim[i]=m_UserGoldSwim[i];

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GOLD_SWIM,&GoldSwim,sizeof(GoldSwim));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GOLD_SWIM,&GoldSwim,sizeof(GoldSwim));
		
	}



	//用户切换
	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;
	m_wCurrentUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;		
	

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//杠泡设置
	if (m_bGangStatus==true)
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if ((m_cbUserAction[i]&WIK_CHI_HU)!=0) break;
		}
		if (i==m_wPlayerCount)
		{
			m_bGangStatus=false;

		}
	}

	

	
	//派发麻将
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser,false);
	//else AfxMessageBox("110");

	

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//效验用户
	ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) return false;

	//被动动作
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		ASSERT(m_bResponse[wChairID]==false);
		ASSERT((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)!=0));

		//效验状态
		if (m_bResponse[wChairID]==true) return false;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0)) return false;

		//变量定义
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		//设置变量
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;

		if(m_cbPerformAction[wChairID]==WIK_GANG)
		{			
			m_bGangStatus=false;
		}

		m_cbOperateCard[wChairID]=(cbOperateCard==0)?m_cbProvideCard:cbOperateCard;
		
		
		//执行判断
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取动作
			BYTE cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];	

			

			//优先级别
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);				

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{				
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}

		}		

		if (m_bResponse[wTargetUser]==false)
		{
			return true;
		}

		//吃胡等待
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU)) return true;
			}
		}

		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));
			for(BYTE i=0;i<4;i++)m_bHuUser[i]=false;

			//臭牌记录
			BYTE cbIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

			//能碰未碰
			//if(m_GameLogic.EstimatePengCard(m_cbCardIndex[wChairID],cbOperateCard))
				//m_cbDisPengIndex[wChairID][cbIndex]++;

			//能胡未胡			
			/*WORD wChiHuRight=0;
			tagChiHuResult ChiHuResult;
			CopyMemory(m_cbCurCardIndex[wChairID],m_cbCardIndex[wChairID],sizeof(m_cbCardIndex[wChairID]));
			//if(m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],cbOperateCard,wChiHuRight,ChiHuResult,m_cbGoldCard,4,bGoldCardEye))
			m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],cbOperateCard,wChiHuRight,ChiHuResult,m_cbGoldCard,4,m_cbCurCardIndex[wChairID]);
			if(ChiHuResult.wChiHuKind!=WIK_NULL)	
				m_cbDisHuIndex[wChairID][cbIndex]++;*/

			DispatchCardData(m_wResumeUser,false);			
			return true;
		}

		//变量定义
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser];

		//出牌变量
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;

		//胡牌操作
		if (cbTargetAction==WIK_CHI_HU)
		{
			//结束信息
			m_cbChiHuCard=cbTargetCard;
			m_wProvideUser=m_wProvideUser;			

			//吃牌权位
			WORD wChiHuRight=0;			
			if (m_bGangStatus==true)  wChiHuRight|=CHR_GANG_HU;
			

			//胡牌判断
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//过虑判断
				if ((i==m_wProvideUser)||((m_cbPerformAction[i]&WIK_CHI_HU)==0)) 
				{
                    //游牌状态
					if(m_UserGoldSwim[i]!=GoldSwim_None)m_UserGoldSwim[i]=GoldSwim_Single;//双游，三游没有游过
					continue;
				}

				//普通胡牌
				if (m_cbChiHuCard!=0)
				{
					//胡牌判断					
					BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
					tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
					//CopyMemory(m_cbCurCardIndex[i],m_cbCardIndex[i],sizeof(m_cbCardIndex[i]));
					//m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,wChiHuRight,m_ChiHuResult[i],m_cbGoldCard,4,bGoldCardEye);	
					m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,wChiHuRight,m_ChiHuResult[i],m_cbGoldCard,4,m_cbCurCardIndex[i]);
					m_bHuUser[i]=true;

					//插入麻将
					if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL) 
					{
						m_cbCurCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
						m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					}
					
					
				}			
				
			}
			
			//结束游戏
			ASSERT(m_ChiHuResult[wTargetUser].wChiHuKind!=CHK_NULL);
			

			OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

			return true;

			
		}

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//组合麻将
		ASSERT(m_cbWeaveItemCount[wTargetUser]<5);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//删除麻将
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//上牌操作
			{
				//删除麻将
				BYTE cbRemoveCard[]={cbTargetCard+1,cbTargetCard+2};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case WIK_RIGHT:		//上牌操作
			{
				//删除麻将
				BYTE cbRemoveCard[]={cbTargetCard-2,cbTargetCard-1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case WIK_CENTER:	//上牌操作
			{
				//删除麻将
				BYTE cbRemoveCard[]={cbTargetCard-1,cbTargetCard+1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case WIK_PENG:		//碰牌操作
			{
				//删除麻将
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				
				break;
			}
		case WIK_GANG:		//杠牌操作
			{
				
				//删除麻将
				if ((m_cbSendCardCount==1)&&(m_cbOutCardData==0))
				{
					BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard,cbTargetCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}
				else
				{
					BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}
				break;
			}
		}

		//构造结果
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//设置状态
		if (cbTargetAction==WIK_GANG)
		{
					
			m_bGangStatus=true;
			m_bEnjoinChiPeng[wTargetUser]=true;
		}
		

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//设置用户
		m_wCurrentUser=wTargetUser;

		//杠牌处理		
		if (cbTargetAction==WIK_GANG)
		{
			//效验动作
			bool bAroseAction=EstimateUserRespond(wTargetUser,cbTargetCard,EstimatKind_GangCard);
			
			//杠牌计算
			m_cbGangCount[wChairID]++;

			//留牌张数+1
			if(m_cbMaxLeftCount==MAX_LEFT_COUNT)
			m_cbMaxLeftCount = MAX_LEFT_COUNT+1;
			//if(cbTargetAction==WIK_GANG)
			//{
				//m_GangScore[m_wCurrentUser]=m_GangScore[m_wCurrentUser]+1*m_pGameServiceOption->lCellScore;
				//m_GangScore[m_wProvideUser]=m_GangScore[m_wProvideUser]-1*m_pGameServiceOption->lCellScore;

				//外杠记分

			/*	LONG  lBeiShu=1,lTmpBeiShu=1;
				//if(m_bUserChui[wChairID])lBeiShu*=2;
				for(WORD k=0;k<GAME_PLAYER;k++)
				{
					if(k==wChairID)continue;
					lTmpBeiShu=1;
					//if(m_bUserChui[k]&&(k!=wChairID))
						//lTmpBeiShu*=2;					
					m_GangScore[wChairID]+=m_pGameServiceOption->lCellScore*10*lBeiShu*lTmpBeiShu;						
					if(k!=wChairID)
						m_GangScore[k]-=1*m_pGameServiceOption->lCellScore*10*lBeiShu*lTmpBeiShu;
				}*/
			//}
			//发送麻将
			if (bAroseAction==false) DispatchCardData(wTargetUser,false);

			return true;
		}

		//动作判断
		if ((MAX_REPERTORY-m_cbSendCardCount-64)>m_cbMaxLeftCount)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
				m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);

			//结果处理
			if (GangCardResult.cbCardCount>0)
			{
				//设置变量				
				m_cbUserAction[m_wCurrentUser]|=WIK_GANG;
				

				//发送动作
				SendOperateNotify();
			}
		}

		return true;
	}

	//主动动作
	if (m_wCurrentUser==wChairID)
	{
		//效验操作
		ASSERT((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)!=0));
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0)) return false;

		//麻将效验
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) return false;

		//设置变量
		m_bSendStatus=true;
		m_cbUserAction[wChairID]=WIK_NULL;
		m_cbPerformAction[wChairID]=WIK_NULL;

		//执行动作
		switch (cbOperateCode)
		{		
		case WIK_GANG:			//杠牌操作
		{
				//变量定义
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

				//杠牌处理
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//寻找组合
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
						{
							cbWeaveIndex=i;

							//杠牌计算
							m_cbGangCount[wChairID]+=2;

							//留牌张数+1
							if(m_cbMaxLeftCount==MAX_LEFT_COUNT)
								m_cbMaxLeftCount = MAX_LEFT_COUNT+1;						

							break;
						}
					}

					//效验动作
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//组合麻将
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}
				else
				{
					//麻将效验
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) return false;

					//设置变量
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;


					//杠牌计算
					m_cbGangCount[wChairID]+=2;

					//留牌张数+1
					if(m_cbMaxLeftCount==MAX_LEFT_COUNT)
						m_cbMaxLeftCount = MAX_LEFT_COUNT+1;					
				}

				//删除麻将
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				//设置状态				
				if (cbOperateCode==WIK_GANG)
				{
										
					m_bGangStatus=true;
					m_bEnjoinChiPeng[wChairID]=true;
					
				}

				

				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard=cbOperateCard;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				//效验动作
				bool bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				/*BYTE mbHu=WIK_NULL;					
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{

					if(i==wChairID)continue;

					//胡牌判断	
					bool bGoldCardEye=false;
					tagChiHuResult ChiHuResult;
					BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
					tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
					mbHu=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,cbOperateCard,WIK_NULL,ChiHuResult,m_cbGoldCard,4,bGoldCardEye);	
									
				}	*/			

				//发送麻将
				if (bAroseAction==false) DispatchCardData(wChairID,false);

				return true;
			}
		case WIK_CHI_HU:		//吃胡操作(自摸，算上游牌牌型)
			{
				//吃牌权位
				WORD wChiHuRight=0;
				if (m_bGangStatus==true) wChiHuRight|=CHR_GANG_HU;	

				//游牌权位
				switch(m_UserGoldSwim[wChairID])
				{
				case GoldSwim_Three:
					{
						wChiHuRight|=CHR_THREE_SWIM;
					
						break;
					}
				case GoldSwim_Double:
					{
						wChiHuRight|=CHR_DOUBLE_SWIM;
					
						break;
					}
				case GoldSwim_Single:
					{
						wChiHuRight|=CHR_SINGLE_SWIM;
					
						break;
					}
				}
			
				//普通胡牌				
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				//CopyMemory(m_cbCurCardIndex[wChairID],m_cbCardIndex[wChairID],sizeof(m_cbCardIndex[wChairID]));
				//m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,0,wChiHuRight,m_ChiHuResult[wChairID],m_cbGoldCard,4,bGoldCardEye);
				m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,0,wChiHuRight,m_ChiHuResult[wChairID],m_cbGoldCard,4,m_cbCurCardIndex[wChairID]);
				m_bHuUser[wChairID]=true;

				//游牌状态
				for(WORD i=0;i<GAME_PLAYER;i++)
				{
					if(i==wChairID)continue;

					//游牌贬值,双游，三游未游过
					if(m_UserGoldSwim[i]!=GoldSwim_None)m_UserGoldSwim[i]=GoldSwim_Single;
				}

				//结束信息
				m_cbChiHuCard=m_cbProvideCard;
				

				//结束游戏				
				OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

				return true;
			}
		}

		return true;
	}

	return false;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
	}

	return true;
}

//派发麻将
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bRepairFlower)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR) return false;

	//丢弃麻将
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}

	//荒庄结束
	if((MAX_REPERTORY-m_cbSendCardCount-64)<=m_cbMaxLeftCount)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}


	//发牌处理
	if (m_bSendStatus==true)
	{
		//发送麻将
		
		
		if(bRepairFlower)//进行补花
		{
			//从后面抽取扑克
			

			//跳过开金麻将
			if(m_cbRepairFlowerPos==(m_wGoldDiceCount-1))m_cbRepairFlowerPos++;

            //补花麻将
			m_cbSendCardData = m_cbRepertoryCard[m_cbRepairFlowerPos++];
			m_cbSendCardCount++;

			m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;			

			
		}
		else
		{			
			m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
			//while((m_cbSendCardData==0x00)&&m_cbLeftCardCount)
			//{
				//m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];				
			//}
			//if(m_cbSendCardData!=0x00)
			//{
				m_cbSendCardCount++;
				m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
			//}			
		}

		
		//设置变量
		m_wProvideUser=wCurrentUser;
		m_cbProvideCard=m_cbSendCardData;

		//杠牌判断
		if ((MAX_REPERTORY-m_cbSendCardCount-64)>m_cbMaxLeftCount)
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult);
		}

		//牌型权位
		WORD wChiHuRight=0;		
		if (m_bGangStatus==true) wChiHuRight|=CHR_GANG_HU;	

		//游牌状态
		m_GoldSwim =  GetCurrentGoldSwim();

		//胡牌判断
		if((m_GoldSwim!=GoldSwim_Three)||//无人做三游
			((m_GoldSwim==m_UserGoldSwim[m_wCurrentUser])||(m_bGangStatus==true)))//自己做三游
		{
			tagChiHuResult ChiHuResult;		
			//CopyMemory(m_cbCurCardIndex[wCurrentUser],m_cbCardIndex[wCurrentUser],sizeof(m_cbCardIndex[wCurrentUser]));
			//m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],
				//m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],0,wChiHuRight,ChiHuResult,m_cbGoldCard,4,bGoldCardEye);
			m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],0,wChiHuRight,ChiHuResult,m_cbGoldCard,4,m_cbCurCardIndex[wCurrentUser]);
		}
	}

	//设置变量
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.bRepairFlower = bRepairFlower;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;

	//状态判断
	if(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY)
		SendCard.bGameSatusPlay = true;
	else
		SendCard.bGameSatusPlay = false;


	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//用户过滤
		if (wCenterUser==i) continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard)
		{
			//吃碰判断
			if (m_bEnjoinChiPeng[i]==false)
			{
				//碰牌判断
				m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

				//吃牌判断
				//test
				WORD wEatUser=(wCenterUser+m_wPlayerCount-1)%m_wPlayerCount;
				if (wEatUser==i) m_cbUserAction[i]|=m_GameLogic.EstimateEatCard(m_cbCardIndex[i],cbCenterCard);
			}

			//杠牌判断
			if (((MAX_REPERTORY-m_cbSendCardCount-52)>m_cbMaxLeftCount)&&(m_bGameStart==true)) 
			{
				m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
			}
		}

		//胡牌判断
		if (m_bEnjoinChiHu[i]==false)
		{
			//牌型权位
			WORD wChiHuRight=0;			
			 if(m_bGangStatus==true) wChiHuRight|=CHR_GANG_HU;			

			 //游牌状态
			 m_GoldSwim=GetCurrentGoldSwim();

			 if((m_GoldSwim!=GoldSwim_Double)&&//无人做双游
				 (m_GoldSwim!=GoldSwim_Three))//无人做三游
			 {

				 //吃胡判断				 
				tagChiHuResult ChiHuResult;
				BYTE cbWeaveCount=m_cbWeaveItemCount[i];
				//CopyMemory(m_cbCurCardIndex[i],m_cbCardIndex[i],sizeof(m_cbCardIndex[i]));
				//m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,wChiHuRight,ChiHuResult,m_cbGoldCard,4,bGoldCardEye);
				m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,wChiHuRight,ChiHuResult,m_cbGoldCard,4,m_cbCurCardIndex[i]);
				
				//吃胡限制			
				if ((m_cbUserAction[i]&WIK_CHI_HU)!=0) 
				{
					//判断是不是天游
					if((m_cbUserAction[i]&CHK_SKY_SWIM)!=0)
					{									
						m_cbUserAction[i]^=CHK_SKY_SWIM;
					}

					m_bEnjoinChiHu[i]=true;

				}

				
			 }		

		
		}

		//结果判断
		if (m_cbUserAction[i]!=WIK_NULL) bAroseAction=true;
	}
	

	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();

		return true;
	}

	return false;
}

// 开门消息
bool CTableFrameSink::OnUserOpenDoor()
{
	//设置变量
	m_wDoorDiceCount  = rand()%6+1+rand()%6+1;

	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];			
	m_cbSendCardCount++;
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//开金玩家
	WORD wOpenGoldUser = (m_wBankerUser+m_wDoorDiceCount-2+GAME_PLAYER)%GAME_PLAYER;

	m_cbProvideCard=m_cbSendCardData;
	m_cbDoorCard=m_cbSendCardData;

	//发送消息
	CMD_S_OpenDoor OpenDoor;
	OpenDoor.wDiceCount = m_wDoorDiceCount;	
	OpenDoor.cbDoorCard=m_cbSendCardData;
	OpenDoor.wOpenGoldUser = wOpenGoldUser;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPENDOOR,&OpenDoor,sizeof(OpenDoor));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPENDOOR,&OpenDoor,sizeof(OpenDoor));

	return true;
}

// 开金消息
bool CTableFrameSink::OnUserOpenGold()
{
	m_wGoldDiceCount = rand()%6+1+rand()%6+1;

	//获取金牌
	BYTE cbGoldCard = m_cbRepertoryCard[m_wGoldDiceCount-1];

	//分析金牌
	BYTE cbGoldIndex = m_GameLogic.SwitchToCardIndex(cbGoldCard);
	if(cbGoldIndex<34)//金牌为筒，万，索，东，南，西，北，中，发，白中的一种
	{
		for(BYTE i=0;i<4;i++)
			m_cbGoldCard[i]=cbGoldCard;

		//花牌麻将
		ZeroMemory(m_cbFlowerCard,sizeof(m_cbFlowerCard));
		for(BYTE i=0;i<8;i++)
			m_cbFlowerCard[i]=0x41+i;
	}
	else
	{
		if(cbGoldIndex<38)//春夏秋冬为金牌
		{
			for(BYTE i=0;i<4;i++)
				m_cbGoldCard[i]=m_GameLogic.SwitchToCardData(34)+i;

			//花牌麻将
			ZeroMemory(m_cbFlowerCard,sizeof(m_cbFlowerCard));
			for(BYTE i=0;i<4;i++)
				m_cbFlowerCard[i]=0x45+i;
		}
		else//梅兰菊竹为金牌
		{
			for(BYTE i=0;i<4;i++)
				m_cbGoldCard[i]=m_GameLogic.SwitchToCardData(38)+i;

			//花牌麻将
			ZeroMemory(m_cbFlowerCard,sizeof(m_cbFlowerCard));
			for(BYTE i=0;i<4;i++)
				m_cbFlowerCard[i]=0x41+i;			
		}
	}

	//设置变量
	m_cbSendCardCount++;
	


	//发送消息
	CMD_S_OpenGold OpenGold;
	OpenGold.wDiceCount = m_wGoldDiceCount;	
	CopyMemory(OpenGold.cbGoldCard,m_cbGoldCard,sizeof(m_cbGoldCard));
	OpenGold.cbKeyGoldCard = cbGoldCard;
	CopyMemory(OpenGold.cbFlowerCard,m_cbFlowerCard,sizeof(m_cbFlowerCard));

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPENGOLD,&OpenGold,sizeof(OpenGold));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPENGOLD,&OpenGold,sizeof(OpenGold));



	//补花开始	
	bool bNeedRepairFlower = false;
	

	//补花玩家
	WORD wChairID=m_wBankerUser;	

	//花牌数组
	//BYTE cbFlowerCard[8]={0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};
	

	//判断花牌
	if(!m_GameLogic.IsHaveFlowerCard(m_cbCardIndex[wChairID],m_cbFlowerCard,8))	
	{
		
		//下一补花玩家
		do
		{
			wChairID = (wChairID+GAME_PLAYER-1)%GAME_PLAYER;
			if(wChairID==m_wBankerUser)break;
			if(m_GameLogic.IsHaveFlowerCard(m_cbCardIndex[wChairID],m_cbFlowerCard,8))bNeedRepairFlower = true;
		}while(bNeedRepairFlower==false);
	
	}
	else
	{
		bNeedRepairFlower = true;
	}

	

	//是否补花
	if(bNeedRepairFlower==false)//不需要补花
	{
		//设置变量
		m_wCurrentUser = m_wBankerUser;
		m_bGameStart = true;

        //设置状态
		m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

		//设置变量
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
		//m_wProvideUser=m_wBankerUser;
		m_cbProvideCard=m_cbDoorCard;

		//杠牌判断		
		tagGangCardResult GangCardResult;
		m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
		m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);		

		//胡牌判断		
		tagChiHuResult ChiHuResult;	
		WORD wChiHuRight=0;
		//CopyMemory(m_cbCurCardIndex[m_wCurrentUser],m_cbCardIndex[m_wCurrentUser],sizeof(m_cbCardIndex[m_wCurrentUser]));
		//m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wCurrentUser],
		//m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],0,WIK_NULL,ChiHuResult,m_cbGoldCard,4,bGoldCardEye);
		m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[m_wCurrentUser],
		m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],0,wChiHuRight,ChiHuResult,m_cbGoldCard,4,m_cbCurCardIndex[m_wCurrentUser]);
	
		//构造数据
		CMD_S_SendCard SendCard;
		SendCard.wCurrentUser=m_wCurrentUser;
		SendCard.bRepairFlower = false;
		SendCard.cbActionMask=m_cbUserAction[m_wCurrentUser];
		SendCard.cbCardData=0;			
		SendCard.bGameSatusPlay = true;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_END_FLOWER,&SendCard,sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_END_FLOWER,&SendCard,sizeof(SendCard));



	

	}
	else//需要补花
	{
		
		//设置变量
		m_wCurrentUser = wChairID;
		m_bSendStatus = true;

		//发送扑克
		DispatchCardData(wChairID,true);
		
	}
	
	
	return true;
}



// 补花消息
bool CTableFrameSink::OnUserRepairFlower(const void *pBuffer)
{

	//转换数据
	CMD_C_RepairFlower *pRepairFlower = (CMD_C_RepairFlower *)pBuffer;

	//设置变量
	m_bSendStatus = true;
	m_wCurrentUser = pRepairFlower->wChairID;
	m_cbOutCardData = pRepairFlower->cbCurrentCard;

	//删除扑克
	m_GameLogic.RemoveCard(m_cbCardIndex[m_wCurrentUser],pRepairFlower->cbCurrentCard);

	//花朵计数
	if(m_GameLogic.IsFlowerCard(pRepairFlower->cbCurrentCard,m_cbFlowerCard,8))
		m_cbFlowerCount[pRepairFlower->wChairID]++;	
	

	//判断状态
	if(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY)//游戏过程中，玩家补花
	{	
		//发送扑克
		DispatchCardData(m_wCurrentUser,true);

	}
	else if(m_pITableFrame->GetGameStatus()==GS_MJ_BEFOREPLAY)//游戏状态前,所有玩家进行补花
	{

		//构造数据
		CMD_S_OutCard OutCard;
		OutCard.wOutCardUser=pRepairFlower->wChairID;
		OutCard.cbOutCardData=pRepairFlower->cbCurrentCard;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

		
		//补花开始	
		bool bNeedRepairFlower = false;	

		//补花玩家
		WORD wChairID=m_wCurrentUser;	

		//花牌数组
		//BYTE cbFlowerCard[8]={0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};

		//需要补花玩家个数
		BYTE bNeedCount=GAME_PLAYER;
	
		//下一补花玩家
		do
		{
			wChairID = (wChairID+GAME_PLAYER-1)%GAME_PLAYER;								
			if(m_GameLogic.IsHaveFlowerCard(m_cbCardIndex[wChairID],m_cbFlowerCard,8))
				bNeedRepairFlower = true;
			else 
				bNeedCount--;

			if(bNeedCount==0)break;	
			
		}while(bNeedRepairFlower==false);
	
	

		//是否补花
		if(bNeedRepairFlower==false)//不需要补花
		{
			//设置变量
			m_wCurrentUser = m_wBankerUser;
			m_bGameStart = true;

			//设置状态
			m_pITableFrame->SetGameStatus(GS_MJ_PLAY);


			//设置变量
			m_cbOutCardData=0;
			m_wOutCardUser=INVALID_CHAIR;
			m_wProvideUser=m_wBankerUser;
			m_cbProvideCard=m_cbDoorCard;

			//杠牌判断		
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
			m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);		

			//胡牌判断		
			tagChiHuResult ChiHuResult;	
			//CopyMemory(m_cbCurCardIndex[m_wCurrentUser],m_cbCardIndex[m_wCurrentUser],sizeof(m_cbCardIndex[m_wCurrentUser]));

			//m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wCurrentUser],
			//m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],0,WIK_NULL,ChiHuResult,m_cbGoldCard,4,bGoldCardEye);
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseChiHuLevity(m_cbCardIndex[m_wCurrentUser],
			m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],0,WIK_NULL,ChiHuResult,m_cbGoldCard,4,m_cbCurCardIndex[m_wCurrentUser]);
		
			//构造数据
			CMD_S_SendCard SendCard;
			SendCard.wCurrentUser=m_wCurrentUser;
			SendCard.bRepairFlower = false;
			SendCard.cbActionMask=m_cbUserAction[m_wCurrentUser];
			SendCard.cbCardData=0;			
			SendCard.bGameSatusPlay = true;

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_END_FLOWER,&SendCard,sizeof(SendCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_END_FLOWER,&SendCard,sizeof(SendCard));


		}
		else//需要补花
		{
			//设置变量
			m_wCurrentUser = wChairID;
			m_bSendStatus = true;

			//发送扑克
			DispatchCardData(m_wCurrentUser,true);		
		}	

		

	}

	///////////////
	return true;
}

//游牌状态
enGoldSwim CTableFrameSink::GetCurrentGoldSwim()
{
	
	//中间变量
	bool bGoldSwim[4];
	for(BYTE i=0;i<4;i++)bGoldSwim[i]=false;

	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_UserGoldSwim[i]==GoldSwim_Three)bGoldSwim[3]=true;//三游
		else if(m_UserGoldSwim[i]==GoldSwim_Double)bGoldSwim[2]=true;//双游
		else if(m_UserGoldSwim[i]==GoldSwim_Single)bGoldSwim[1]=true;//单游
		
	}

	if(bGoldSwim[3]==true)return GoldSwim_Three;
	else if(bGoldSwim[2]==true)return GoldSwim_Double;
	else if(bGoldSwim[1]==true)return GoldSwim_Single;
	else return GoldSwim_None;
}

//游牌状态
enGoldSwim CTableFrameSink::GetUserGoldSwim(WORD wChairID)
{
	//中间变量
	BYTE cbGoldCount=0;//金牌数量	
	
	cbGoldCount = m_GameLogic.GetGoldCardCount(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],m_cbGoldCard,4);

	if(cbGoldCount==0)return GoldSwim_None;
	else if(cbGoldCount==1)return GoldSwim_Single;
	else if(cbGoldCount==2)return GoldSwim_Double;
	else return GoldSwim_Three;

}
