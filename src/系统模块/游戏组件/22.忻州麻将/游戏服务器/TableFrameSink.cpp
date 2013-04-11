#include "StdAfx.h"
#include "TableFrameSink.h"

extern void WriteLog(CString strFileName, CString strText);
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);

	m_wSiceCount2 =MAKEWORD(1,1);

	m_wBankerUser=INVALID_CHAIR;//没有确定庄家
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//运行变量
	m_cbProvideCard=0;
	m_wTingUser = INVALID_CHAIR;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus=false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

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
	//听变量	
	memset( (char *)m_bTingFlag, 0, sizeof(m_bTingFlag));

	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);
	m_wSiceCount2 =MAKEWORD(1,1);

	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//运行变量
	m_cbProvideCard=0;
	m_wTingUser = INVALID_CHAIR;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus=false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

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
	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

	//混乱麻将
	m_wSiceCount=MAKEWORD(rand()%6+1,rand()%6+1);

	m_wSiceCount2=MAKEWORD(rand()%6+1,rand()%6+1);

	m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
	m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

	//分发麻将
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_cbLeftCardCount-=(MAX_COUNT-1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
	}

	//发送麻将
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//设置变量
	m_cbProvideCard=0;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;//当前出牌者

	//动作分析
	bool bAroseAction=false;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//特殊胡牌
		//m_cbUserAction[i]|=m_GameLogic.EstimateChiHu(m_cbCardIndex[i]);

		//庄家判断
		if (i==m_wBankerUser)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[i]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[i],NULL,0,GangCardResult);

			//胡牌判断
			tagChiHuResult ChiHuResult;
			m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],NULL,0,0,0,ChiHuResult);
		}

		//状态设置
		if ((bAroseAction==false)&&(i!=m_wBankerUser)&&(m_cbUserAction[i]!=WIK_NULL))
		{
			bAroseAction=true;

			m_wResumeUser=m_wCurrentUser;
			m_wCurrentUser=INVALID_CHAIR;
		}//End if
	}

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.wSiceCount=m_wSiceCount;
	GameStart.wSiceCount2=m_wSiceCount2;

	GameStart.wBankerUser=m_wBankerUser;//设置庄家
	GameStart.wCurrentUser=m_wCurrentUser;

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//设置变量
		GameStart.cbUserAction=m_cbUserAction[i];
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;
}
/*
和牌点数计算

和  牌：1点

砍  和：1点（吊牌也属于砍和）

清一色：5点

一条龙：5点

七小对：1点（七小对也属砍胡，不累加）

缺一门：1点

庄家：(连N庄) N点。初始1点

注意：

1．  放炮者没有听牌多加1点（听牌则同其他玩家支付同样点数）

2．  庄家输牌要加扣连庄点

3．  自摸所有点数翻倍。

4．  明杠1点，暗杠2点，流局无效（自摸不加倍）

 

得分举例：

1 非庄家 和牌，砍和，缺一门，自摸。

庄家（连庄2次）支付6+2*2=10点，其余玩家支付3*2=6点 （和牌1点，砍和1点，缺一门1点）。

2 有人放炮庄家和牌，连庄2次，一条龙。（有个暗杠）

放炮者（未听牌）支付9点，其余玩家支付8点（和牌1点，庄家2点，一条龙5点）

因为有个暗杠，都再多付2点（如果是自摸也是2点）。

*/
//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnEventGameEnd.log",strTime);

	strTemp.Format("OnEventGameEnd") ;
	WriteLog(strFile, strTemp);

	int baseInt = 1;//基本分

	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			strTemp.Format("常规结束") ;
			WriteLog(strFile, strTemp);

			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			LONG lCellScore=m_pGameServiceOption->lCellScore;
			BYTE cbBirdValue=(m_cbRepertoryCard[0]&MASK_VALUE);

			//结束信息
			GameEnd.wProvideUser=wChairID;
			GameEnd.cbChiHuCard=m_cbChiHuCard;
			for (WORD i=0;i<m_wPlayerCount;i++) GameEnd.wChiHuKind[i]=m_ChiHuResult[i].wChiHuKind;


			strTemp.Format("wProvideUser=%d,cbChiHuCard=%d,",
				//GameEnd.cbCardCount,
				//GameEnd.cbCardData,
				GameEnd.wProvideUser ,
				GameEnd.cbChiHuCard
				//GameEnd.lGameScore,
				//GameEnd.wChiHuKind,
				);
				
			WriteLog(strFile, strTemp);

			//拷贝麻将
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}//End for


			//变量定义
			WORD wChiHuUserCount=0;

			//统计积分
			if (m_wProvideUser!=INVALID_CHAIR)
			{
				//自摸类型
				if (m_ChiHuResult[m_wProvideUser].wChiHuKind!=CHK_NULL)
				{
					strTemp.Format("自摸类型  自摸所有点数翻倍") ;
					WriteLog(strFile, strTemp);

					//翻数计算
					BYTE cbChiHuOrder=m_GameLogic.GetChiHuActionRank(m_ChiHuResult[m_wProvideUser]);

					strTemp.Format("cbChiHuOrder=%d", cbChiHuOrder) ;
					WriteLog(strFile, strTemp);

					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//赢家过滤
						if (i==m_wProvideUser) 
						{
							GameEnd.lGameScore[i] = cbChiHuOrder;
							continue;
						}
						else
						{
							//积分统计
							GameEnd.lGameScore[i] = -cbChiHuOrder;
						}
					}

					//胡牌数目
					wChiHuUserCount=1;

					//庄家设置
					//
					//Del by 20080101 m_wBankerUser=m_wProvideUser;//前一家

//变量定义
					WORD wNextBankerUser=INVALID_CHAIR;

					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//输家过滤
						if (m_ChiHuResult[i].wChiHuKind==CHK_NULL) continue;

						//设置变量
						wNextBankerUser=i;

					}//End for
					//如果是庄家自摸
					if ( m_wProvideUser == m_wBankerUser)
					{
						//庄家不变动
					}
					else
					{
						m_wBankerUser = wNextBankerUser;//下一家
					}
				}//End if (m_ChiHuResult[m_wProvideUser].wChiHuKind!=CHK_NULL)

				//捉炮类型
				if (m_ChiHuResult[m_wProvideUser].wChiHuKind==CHK_NULL)
				{
					strTemp.Format("捉炮类型") ;
					WriteLog(strFile, strTemp);

					//变量定义
					WORD wNextBankerUser=INVALID_CHAIR;

					//听牌状态
					for (int k=0; k < m_wPlayerCount; k++)
					{
						//GameEnd.cbTingFlag[k] = 0;
						//如果已经听了
						if (m_bTingFlag[k])
						{
						//	GameEnd.cbTingFlag[k]=1;
							strTemp.Format("听=%d", k) ;
							WriteLog(strFile, strTemp);

						}
					}//End for

					//计算得分
					int mjResultInt = 0;//得分
					for(int i=0; i < m_wPlayerCount; i++)
					{
					//翻数计算
						BYTE cbChiHuOrder=m_GameLogic.GetChiHuActionRank(m_ChiHuResult[i]);

						strTemp.Format("m_ChiHuResult[i]=0x%x", m_ChiHuResult[i]) ;
						WriteLog(strFile, strTemp);

						if (m_ChiHuResult[i].wChiHuKind != CHK_NULL)
						{
							//番数
							int fanInt = m_GameLogic.GetWinMJNum(  m_cbCardIndex[i], MAX_INDEX);	

							//是否为砍和 //CHK_JI_HU //CHK_PING_HU
							m_ChiHuResult[i].wChiHuKind &=0x0001;

							strTemp.Format("m_ChiHuResult[i]=0x%x", m_ChiHuResult[i]) ;
							WriteLog(strFile, strTemp);

							if ( (m_ChiHuResult[i].wChiHuKind == CHK_JI_HU) || (m_ChiHuResult[i].wChiHuKind == CHK_PING_HU) )
							{
								fanInt ++;
								strTemp.Format("为砍和") ;
								WriteLog(strFile, strTemp);

							}	

							strTemp.Format("m_ChiHuResult[i]=0x%x", m_ChiHuResult[i]) ;
							WriteLog(strFile, strTemp);

							mjResultInt = baseInt + fanInt;

							break;
						}//End if
					}//End for

					strTemp.Format("得分 mjResultInt=%d", mjResultInt) ;
					WriteLog(strFile, strTemp);

					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//翻数计算
						BYTE cbChiHuOrder=m_GameLogic.GetChiHuActionRank(m_ChiHuResult[i]);

						//strTemp.Format("cbChiHuOrder=%d, %d", cbChiHuOrder, m_ChiHuResult[i].wChiHuKind) ;
						//WriteLog(strFile, strTemp);


						//其余 3家
						if (m_ChiHuResult[i].wChiHuKind==CHK_NULL)
						{
							//积分统计
							//点炮的
							if ( m_wProvideUser == i)
							{
								GameEnd.lGameScore[i] = -mjResultInt;

								if (!m_bTingFlag[i])
								{
									GameEnd.lGameScore[i]-= 1;//没有听牌，要多出一点

									strTemp.Format("没有听牌，要多出一点 GameEnd.lGameScore[i]=%d", GameEnd.lGameScore[i]) ;
									WriteLog(strFile, strTemp);

								}
							
								//庄家点炮
								if (m_wBankerUser == m_wProvideUser)
								{
									GameEnd.lGameScore[i] -= 1;

									strTemp.Format("庄家点炮 GameEnd.lGameScore[i]=%d", GameEnd.lGameScore[i]) ;
									WriteLog(strFile, strTemp);

								}
							}
							else
							{
								GameEnd.lGameScore[i] = -mjResultInt;
							}
							continue;
						}
						//赢家
						else
						{
							GameEnd.lGameScore[i] = mjResultInt*3;

							//积分统计
							if (!m_bTingFlag[m_wProvideUser])
							{
								GameEnd.lGameScore[i] +=1;//
								strTemp.Format(" 赢家 点炮未听牌 GameEnd.lGameScore[i]=%d", GameEnd.lGameScore[i]) ;
								WriteLog(strFile, strTemp);
	
							}
		
						//庄家点炮
							if (m_wBankerUser == m_wProvideUser)
							{
								GameEnd.lGameScore[i] += 1;//

								strTemp.Format(" 赢家 庄家点炮 GameEnd.lGameScore[i]=%d", GameEnd.lGameScore[i]) ;
								WriteLog(strFile, strTemp);
							}

                         //庄家赢牌
							if (i == m_wBankerUser)
							{
								GameEnd.lGameScore[i] += 1;//

								strTemp.Format(" 庄家赢牌 GameEnd.lGameScore[i]=%d", GameEnd.lGameScore[i]) ;
								WriteLog(strFile, strTemp);	
							}
							strTemp.Format("赢家的分%d, i=%d", mjResultInt, i) ;
							WriteLog(strFile, strTemp);
						}//End if

						//设置变量
						wNextBankerUser=i;
						wChiHuUserCount++;
					}//End for

					//庄家设置
					m_wBankerUser=(wChiHuUserCount>=2)?m_wProvideUser:wNextBankerUser;

					m_wBankerUser = wNextBankerUser;
				}//End if
			}
			else if ((m_cbOutCardCount==0)&&(m_cbSendCardCount==1))
			{
				strTemp.Format("m_cbOutCardCount==0)&&(m_cbSendCardCount==1") ;
				WriteLog(strFile, strTemp);

				//变量定义
				WORD wNextBankerUser=INVALID_CHAIR;

				//特殊胡牌
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//输家过滤
					if (m_ChiHuResult[i].wChiHuKind==CHK_NULL) continue;

					//循环累计
					if ((m_ChiHuResult[i].wChiHuKind&0x00F0)!=0)
					{
						for (WORD j=0;j<m_wPlayerCount;j++)
						{
							if (m_ChiHuResult[j].wChiHuKind==CHK_NULL)
							{
							}
						}//End for (WORD j=0;j<m_wPlayerCount;j++)
					}
					else
					{
						for (WORD j=0;j<m_wPlayerCount;j++)
						{
							if (m_ChiHuResult[j].wChiHuKind==CHK_NULL)
							{
							}
						}//End for (WORD j=0;j<m_wPlayerCount;j++)
					}//End if

					//设置变量
					wChiHuUserCount++;
					wNextBankerUser=i;
				}//End for (WORD i=0;i<m_wPlayerCount;i++)

				//庄家设置
				if (wChiHuUserCount==1) m_wBankerUser=wNextBankerUser;
			}
			else
			{

				strTemp.Format("荒庄处理") ;
				WriteLog(strFile, strTemp);
				//荒庄处理
				GameEnd.cbChiHuCard=0;
				//庄家不变动

			}//End if

			//拷贝麻将
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}//End for

			//发送信息
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
			}//End for
			m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,0L);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
		strTemp.Format("用户强退") ;
			WriteLog(strFile, strTemp);

			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.wProvideUser=INVALID_CHAIR;
			GameEnd.lGameScore[wChairID]=-24*m_pGameServiceOption->lCellScore;

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
			StatusFree.wBankerUser=m_wBankerUser;//
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_PLAY:
		{
		}//游戏状态
	case GS_MJ_HAI_DI:	//海底状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wSiceCount=m_wSiceCount;
			StatusPlay.wSiceCount2=m_wSiceCount2;

			StatusPlay.wBankerUser=m_wBankerUser;//
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
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnGameMessage.log",strTime);

//			strTemp.Format("OnGameMessage ");
//			WriteLog(strFile, strTemp);

	switch (wSubCmdID)
	{
//听牌
	case SUB_C_TING_CARD:
		{
		//	OnUserTingCard(WORD wChairID, BYTE cbCardData);
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_TingCard));
			if (wDataSize!=sizeof(CMD_C_TingCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_TingCard * pOutCard=(CMD_C_TingCard *)pDataBuffer;
			return OnUserTingCard(pUserData->wChairID,pOutCard->cbCardData);

		}
		break;
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
		m_wBankerUser=pIServerUserItem->GetChairID();//第一个坐下的当庄
	}

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(wChairID==m_wBankerUser))
	{
		m_wBankerUser=INVALID_CHAIR;//如果走的是庄家

		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if ((i!=wChairID)&&(m_pITableFrame->GetServerUserItem(i)!=NULL))
			{
				m_wBankerUser=i;
				break;
			}//End if
		}//End for
	}//End if ((bLookonUser

	return true;
}
//用户TING牌
bool CTableFrameSink::OnUserTingCard(WORD wChairID, BYTE cbCardData)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnUserTingCard.log",strTime);

	strTemp.Format("into OnUserTingCard(%d,%d", wChairID,  cbCardData) ;
	WriteLog(strFile, strTemp);


	WORD i=0 ;
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//错误断言
//	ASSERT(wChairID==m_wCurrentUser);
//	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
//	if (wChairID!=m_wCurrentUser) return false;
//	if (m_GameLogic.IsValidCard(cbCardData)==false) return false;

	m_bTingFlag[wChairID] = true;
//用户听牌
	//构造数据
	CMD_S_GameTingCard OutCard;
	OutCard.wTingCardUser=wChairID;


	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TING_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TING_CARD,&OutCard,sizeof(OutCard));



	return true;	
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	WORD i=0 ;
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

	//设置变量
	m_bSendStatus=true;
	m_bEnjoinChiHu[wChairID]=false;
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

	//用户切换
	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;
	m_wCurrentUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//抢杆设置
	if (m_bGangStatus==true)
	{
		for (i=0;i<m_wPlayerCount;i++)
		{
			if ((m_cbUserAction[i]&WIK_CHI_HU)!=0) break;
		}
		if (i==m_wPlayerCount) m_bGangStatus=false;
	}

	//派发麻将
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnUserOperateCard.log",strTime);

	strTemp.Format("OnUserOperateCard") ;
	WriteLog(strFile, strTemp);

	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;
	//效验用户
	ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));

	BYTE cbTargetAction=cbOperateCode;
	//是否听牌操作
	if (cbTargetAction==WIK_TING_CARD)
	{
		//m_bTingFlag[wChairID] = true;
		//通知大家

		//构造数据
		CMD_S_GameTingCard SendCard;
		SendCard.wTingCardUser= m_wCurrentUser;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TING_CARD,&SendCard,sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TING_CARD,&SendCard,sizeof(SendCard));

		return true;
	}//End if


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
		}//End for (WORD i=0;i<m_wPlayerCount;i++)
		if (m_bResponse[wTargetUser]==false) return true;

		//吃胡等待
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU)) return true;
			}
		}//End if (cbTargetAction==WIK_CHI_HU)

		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

			//发送麻将
			DispatchCardData(m_wResumeUser);

			return true;
		}//End if (cbTargetAction==WIK_NULL)

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
			if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==1)) wChiHuRight|=CHR_DI;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==0)) wChiHuRight|=CHR_TIAN;

			//胡牌判断
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//过虑判断
				if ((i==m_wProvideUser)||((m_cbPerformAction[i]&WIK_CHI_HU)==0)) continue;

				//普通胡牌
				if (m_cbChiHuCard!=0)
				{
					//胡牌判断
					BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
					tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
					m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,wChiHuRight,m_ChiHuResult[i]);

					//插入麻将
					if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL) m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
				}
			}//End for (WORD i=0;i<m_wPlayerCount;i++)

			//结束游戏
			ASSERT(m_ChiHuResult[wTargetUser].wChiHuKind!=CHK_NULL);
			OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

			return true;
		}//End if (cbTargetAction==WIK_CHI_HU)

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//组合麻将
		ASSERT(m_cbWeaveItemCount[wTargetUser]<4);
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
		case WIK_FILL:		//补牌操作
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
		if ((cbTargetAction==WIK_GANG)||(cbTargetAction==WIK_FILL))
		{
			//效验动作
			bool bAroseAction=EstimateUserRespond(wTargetUser,cbTargetCard,EstimatKind_GangCard);

			//发送麻将
			if (bAroseAction==false) DispatchCardData(wTargetUser);

			return true;
		}

		//动作判断
		if (m_cbLeftCardCount>1)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
				m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);

			//结果处理
			if (GangCardResult.cbCardCount>0)
			{
				//设置变量
				m_cbUserAction[m_wCurrentUser]|=(WIK_GANG|WIK_FILL);

				//发送动作
				m_wTingUser = wChairID;
				SendOperateNotify();
			}
		}

		return true;
	}//End if (m_wCurrentUser==INVALID_CHAIR)

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
		case WIK_FILL:			//补牌操作
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

				//发送麻将
				if (bAroseAction==false) DispatchCardData(wChairID);

				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				//吃牌权位
				WORD wChiHuRight=0;
				if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;
				if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==1)) wChiHuRight|=CHR_DI;
				if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==0)) wChiHuRight|=CHR_TIAN;

				//普通胡牌
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,0,wChiHuRight,m_ChiHuResult[wChairID]);

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
			OperateNotify.wOperateUser = m_wTingUser;
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
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser)
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
    if (m_cbLeftCardCount==0)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}//End if

	//发牌处理
	if (m_bSendStatus==true)
	{
		//发送麻将
		m_cbSendCardCount++;
		m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_wProvideUser=wCurrentUser;
		m_cbProvideCard=m_cbSendCardData;

		//杠牌判断
		if (m_cbLeftCardCount>1)
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult);
		}

		//牌型权位
		WORD wChiHuRight=0;
		if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;

		//胡牌判断
		tagChiHuResult ChiHuResult;
		m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],
			m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],0,wChiHuRight,ChiHuResult);
	}

	//设置变量
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;

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
				/*Del by doctor 20071202
				WORD wEatUser=(wCenterUser+m_wPlayerCount-1)%m_wPlayerCount;
				if (wEatUser==i) m_cbUserAction[i]|=m_GameLogic.EstimateEatCard(m_cbCardIndex[i],cbCenterCard);
				*/
			}

			//杠牌判断
			if (m_cbLeftCardCount>1) 
			{
				m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
			}
		}

		//胡牌判断
		if (m_bEnjoinChiHu[i]==false)
		{
			//牌型权位
			WORD wChiHuRight=0;
			if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==1)) wChiHuRight|=CHR_DI;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==0)) wChiHuRight|=CHR_TIAN;

			//吃胡判断
			tagChiHuResult ChiHuResult;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];
			m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,wChiHuRight,ChiHuResult);

			//吃胡限制
			if ((m_cbUserAction[i]&WIK_CHI_HU)!=0) m_bEnjoinChiHu[i]=true;
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
		m_wTingUser = wCenterUser;
		SendOperateNotify();

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
