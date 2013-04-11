#include "StdAfx.h"
#include "TableFrameSink.h"

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

//静态变量

//麻将数据
BYTE CTableFrameSink::m_byMjs[ORG_MJ_MAX] = 
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
	0x31, 0x32, 0x33, 0x34,												//风
	0x41, 0x42, 0x43,													//字
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
	0x31, 0x32, 0x33, 0x34,												//风
	0x41, 0x42, 0x43,													//字
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
	0x31, 0x32, 0x33, 0x34,												//风
	0x41, 0x42, 0x43,													//字
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
	0x31, 0x32, 0x33, 0x34,												//风
	0x41, 0x42, 0x43													//字
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wBankerUser = 0;
	m_wCurrentUser = INVALID_CHAIR;
	m_wLastUser    = INVALID_CHAIR;
	m_strFang= NULL;
	m_wLostUser= INVALID_CHAIR;
	m_wQFeng = 0;
	m_bZiM= false;
	//m_stEnd.clear();
	for( int i=0; i<m_wPlayerCount; i++ )
	{
		m_nFang[i]= 0;
		m_ActionRight[i] = 0;
		m_GameLogic.CleanHu( &m_hu[i] );
	}
	
	//运行信息
	m_wTurnWiner  = INVALID_CHAIR;
	m_byOutMj	  = 255;

	//麻将信息
	memset( m_byHandMjs, 0, sizeof( m_byHandMjs ) );

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//没掷骰子
	m_nTouchMjPos  = 0;
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_strFang)
	{
		delete []m_strFang;
	}

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
	//m_wBankerUser = 0;
	//m_wQFeng= 0;
	m_wCurrentUser = INVALID_CHAIR;
	m_wLastUser    = INVALID_CHAIR;

	m_wLostUser= INVALID_CHAIR;
	m_bZiM= false;
	m_strFang= NULL;
	//m_stEnd.clear();
	for( int i=0; i<m_wPlayerCount; i++ )
	{
 		m_nFang[i]= 0;
		m_ActionRight[i] = 0;
		m_GameLogic.CleanHu( &m_hu[i] );
	}

	//运行信息
	m_wTurnWiner  = INVALID_CHAIR;
	m_byOutMj = 255;

	//麻将信息
	memset( m_byHandMjs, 0, sizeof( m_byHandMjs ) );

	//没掷骰子
	m_nTouchMjPos  = 0;
	
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
	CMD_S_GameStart GameStart;
	//随机骰子数
	GameStart.nSartNumber1 = m_GameLogic.ActionDice();
	GameStart.nSartNumber2 = m_GameLogic.ActionDice();
	GameStart.wBankerUser= m_wBankerUser;
	GameStart.wQuanFeng= m_wQFeng;
	m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof( GameStart));


	//Sleep( 3000 );
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//初始化变量
	m_byTouchMjCount = ORG_MJ_MAX;
	//混乱麻将
	m_GameLogic.RandMj( m_byMjs, ORG_MJ_MAX );
	//log
	for( int i=0; i<ORG_MJ_MAX; i++ )
	{
		REPORT_V( log, "i", i, 1 );
		REPORT_V( log, "混乱后mj", m_byMjs[i], 1 );
	}
	log.DrawLine( 3, 1 );
	


	//test
	//BYTE mjs[14] = { 0x01,0x09,0x11,0x19,0x21,0x29,0x31,0x32,0x33,0x34,0x41,0x42,0x43,0x01 };
	//分发麻将
	CMD_S_SendMj	SendMj;
	SendMj.wBankerUser = m_wBankerUser;
	SendMj.byStartMjPos = m_nTouchMjPos;
	for( WORD i=0; i<m_wPlayerCount; i++ )
	{
		if( i== m_wBankerUser )
		{
			m_byHandMjCount[i] = HAND_MJ_MAX;
			//CopyMemory( &m_byHandMjs[i], &m_byMjs[0], sizeof(BYTE)*m_byHandMjCount[i] );	
			CopyMemory( &m_byHandMjs[i], &m_byMjs[i*(HAND_MJ_MAX-1)],  HAND_MJ_MAX-1 );	
			m_byHandMjs[i][HAND_MJ_MAX-1]=m_byMjs[52];
			//test
			//CopyMemory( &m_byHandMjs[i], &mjs, sizeof( BYTE)*m_byHandMjCount[i]);
			m_GameLogic.SortMj( m_byHandMjs[i], m_byHandMjCount[i], m_byHandMjCount[i] );
		}
		else
		{
			m_byHandMjCount[i] = HAND_MJ_MAX-1;
			CopyMemory( &m_byHandMjs[i], &m_byMjs[i*(HAND_MJ_MAX-1)],  HAND_MJ_MAX-1 );
			/*CopyMemory( &m_byHandMjs[i], &m_byMjs[i*m_byHandMjCount[i]+1], sizeof(BYTE)*m_byHandMjCount[i] );*/
			//test
		//	CopyMemory( &m_byHandMjs[i], &mjs, sizeof( BYTE)*m_byHandMjCount[i]);
			m_byHandMjs[i][HAND_MJ_MAX-1] = 0;
			m_GameLogic.SortMj( m_byHandMjs[i], m_byHandMjCount[i], m_byHandMjCount[i]  );
		//	m_GameLogic.LMove( m_byHandMjs[i], HAND_MJ_MAX-1 );	
		}	
	}	

	//设置变量
	m_nTouchMjPos = 53;
	m_byTouchMjCount -= 53;

	//设置用户
	m_wCurrentUser = m_wBankerUser;
	//设置天、地、人胡初始化
	m_bTianHu=true;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(i==m_wBankerUser)
		{
			m_bDiHu[i]=false;
			m_bRenHu[i]=false;
		}
		else
		{
			m_bDiHu[i]=true;
			m_bRenHu[i]=true;
		}
	}
	
	//发送麻将牌
	for( WORD i=0; i<m_wPlayerCount; i++ )
	{
		SendMj.byMjCount = m_byHandMjCount[i];
		CopyMemory( &SendMj.byMjs, &m_byHandMjs[i], sizeof(BYTE)*m_byHandMjCount[i] );
		if( i == m_wBankerUser )
		{
			SendMj.nHu = m_GameLogic.IsHu( m_byHandMjs[i], m_byHandMjCount[i]-1, m_byHandMjs[i][HAND_MJ_MAX-1], m_hu[i],&m_tempHu[i] );
			SendMj.nGang = m_GameLogic.IsAnGang( m_byHandMjs[i], m_byHandMjCount[i] ).GangStyle;
			if( SendMj.nGang)
				m_ActionRight[i] = 4;
			if( SendMj.nHu)
				m_ActionRight[i] = 6;
		}
		else
		{
			SendMj.nHu = false;
			SendMj.nGang = false;
		}

		m_pITableFrame->SendTableData( i, SUB_S_SEND_MJ, &SendMj, sizeof( SendMj ) );
		m_pITableFrame->SendLookonData( i, SUB_S_SEND_MJ, &SendMj, sizeof( SendMj ) );
	}
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
	switch( cbReason )
	{
		//常规结束
	case GER_NORMAL:
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset( &GameEnd, 0, sizeof(CMD_S_GameEnd));

			//变量定义
			LONG lCeelScore=m_pGameServiceOption->lCellScore/512;
			bool bBankerWin=(m_wBankerUser==wChairID)?true:false;

			//统计分数
			//strcpy( &GameEnd.strEnd, m_strFang );
			//int a = strlen(m_strFang);
			///int b = strlen(GameEnd.strEnd);
			//番数统计
			CopyMemory( &GameEnd.strEnd, m_strFang, strlen(m_strFang));
			//亮牌
			for( int k=0; k<4; k++ )
			{
				CopyMemory( &GameEnd.byMjs[k], m_byHandMjs[k], sizeof(BYTE)*14);
			}
			for( WORD i=0; i<m_wPlayerCount;i++)
			{
				if( i==m_wTurnWiner )
				{
					//自摸
					if( m_bZiM)
					{
						//鸡胡处理
						if( m_nFang==0)
						{
							GameEnd.lGameScore[i] = lCeelScore*3;
						}
						else
						{
							GameEnd.lGameScore[i] = lCeelScore*m_GameLogic.Pow( 2, m_nFang[i] )*3;
						}
						
					}
					else
					{
						//鸡胡处理
						if( m_nFang==0)
						{
							GameEnd.lGameScore[i] = lCeelScore;
						}
						else
						{
							GameEnd.lGameScore[i] = m_GameLogic.Pow( 2, m_nFang[i] )*lCeelScore;
						}
						
					}
				}
				else 
				{
					//如果是自摸,三家输
					if( m_bZiM)
					{
						//鸡胡处理
						if( m_nFang==0)
						{
							GameEnd.lGameScore[i] = (-1)*lCeelScore;
						}
						else
						{
							GameEnd.lGameScore[i] = (-1)*m_GameLogic.Pow( 2, m_nFang[m_wTurnWiner] )*lCeelScore;
						}
						
					}
					else
					{
						if( i==m_wLastUser)
						{
							//鸡胡处理
							if( m_nFang==0)
							{
								GameEnd.lGameScore[i] = (-1)*lCeelScore;
							}
							else
							{
								GameEnd.lGameScore[i] = (-1)*m_GameLogic.Pow( 2, m_nFang[m_wTurnWiner] )*lCeelScore;
							}
							
						}
					}
				}
			}

			// 动态税收
			long myTax = m_pGameServiceOption->cbTaxRate;

			strTemp.Format(" 税率=%d", myTax);
			WriteLog(strFile, strTemp);
			//计算税收
			if( m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for( WORD i=0; i<m_wPlayerCount; i++ )
				{
					//Doctor if (GameEnd.lGameScore[i]>=100L)
					//个税起征收点 10
					if (GameEnd.lGameScore[i]>= TAX_MIN_NUM)
					{/*
						GameEnd.lGameTax+=GameEnd.lGameScore[i]/50L;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*49L/50L;
						*/

						GameEnd.lGameTax+=GameEnd.lGameScore[i] * myTax / 100L;

						strTemp.Format("GameEnd.lGameScore[%d]=%d,GameEnd.lGameTax=%d", i, GameEnd.lGameScore[i], GameEnd.lGameTax);
						WriteLog(strFile, strTemp);

						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*(100-myTax)/100L;


						strTemp.Format("GameEnd.lGameScore[%d]=%d", i, GameEnd.lGameScore[i]);
						WriteLog(strFile, strTemp);
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(CMD_S_GameEnd)+strlen(GameEnd.strEnd)+1);
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(CMD_S_GameEnd)+strlen(GameEnd.strEnd)+1);

			//庄位转换
			if( m_wTurnWiner != m_wBankerUser )
			{	
				if( m_wBankerUser==3)
				{
					m_wQFeng= (m_wQFeng+1)%4;
				}
				m_wBankerUser= (m_wBankerUser+3)%4;

			}
			//修改积分
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			memset(&ScoreInfo, 0, sizeof(ScoreInfo));
			for( WORD i=0;i<m_wPlayerCount;i++ )
			{
				ScoreInfo[i].lScore = GameEnd.lGameScore[i];
				ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
			}
			m_pITableFrame->WriteTableScore(ScoreInfo, m_wPlayerCount, GameEnd.lGameTax);

			//结束游戏
			m_pITableFrame->ConcludeGame();
			return true;
		}
	//	//用户离开
	//case GER_USER_LEFT:
	//	{
	//		//重置数据
	//		m_wBankerUser = 0;
	//		m_wQFeng= 0;
	//		return true;
	//	}
		//用户强退
	case GER_USER_LEFT:
		{
			//校验参数
			ASSERT( pIServerUserItem != NULL );
			ASSERT( wChairID< m_wPlayerCount );

			m_wBankerUser = 0;
			m_wQFeng= 0;
			//构造数据
			CMD_S_GameEnd GameEnd;
			memset( &GameEnd, 0, sizeof( GameEnd ) );
			//变量定义
			LONG lCeelScore=m_pGameServiceOption->lCellScore/512;

			//番数统计
			//GameEnd.strEnd=NULL;
			//CopyMemory( &GameEnd.strEnd, m_strFang, strlen(m_strFang));
			//亮牌
			for( int k=0; k<4; k++ )
			{
				CopyMemory( &GameEnd.byMjs[k], m_byHandMjs[k], sizeof(BYTE)*14);
			}
			for( WORD i=0; i<m_wPlayerCount;i++)
			{
				//强退,扣32倍底分
				if(i==wChairID)
				{
					GameEnd.lGameScore[i]=-32*lCeelScore;
				}
				else
				{
					GameEnd.lGameScore[i]=0;
				}
			}
			//发送信息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(CMD_S_GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(CMD_S_GameEnd)
				);
			//修改积分
			tagScoreInfo ScoreInfo;
			ScoreInfo.ScoreKind=enScoreKind_Draw;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);
			m_pITableFrame->ConcludeGame();
			return true;
		}
	}
	ASSERT( FALSE );
	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)
{
	switch( bGameStatus )
	{
	case GS_WK_FREE:
		{
			CMD_S_StatusFree StatusFree;
			memset( &StatusFree, 0, sizeof( StatusFree ) );

			//设置变量
			StatusFree.lBaseGold = m_pGameServiceOption->lCellScore;
			return m_pITableFrame->SendGameScene( pIServerUserItem, &StatusFree,sizeof( StatusFree ) );
		}
	case GS_WK_PLAYING:
		{
			CMD_S_StatusPlay StatusPlay;
			memset( &StatusPlay, 0, sizeof( StatusPlay ) );

			//设置变量
			StatusPlay.wLastOutUser = m_wTurnWiner;
			StatusPlay.lBaseScore = m_pGameServiceOption->lCellScore;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.byOutMj = m_byOutMj;
			CopyMemory( StatusPlay.byMjs, m_byHandMjs[wChairID], sizeof(BYTE)*m_byHandMjCount[wChairID] );

			//发送场景
			return m_pITableFrame->SendGameScene( pIServerUserItem, &StatusPlay, sizeof( StatusPlay ) );
		}
	}
	//效验结果
	ASSERT(FALSE);

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
	switch( wSubCmdID )
	{
	case SUB_C_TOUCH_MJ:

		break;
	case SUB_C_OUT_MJ:
		{
			//变量定义
			BYTE *pOutMj = ( BYTE* )pDataBuffer;
		
			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if( pUserData->cbUserStatus != US_PLAY )
			{
				return true;
			}
			
			//消息处理
			return OnUserOutMj( pUserData->wChairID, pOutMj );
		}
	case SUB_C_GANG_MJ:			//杠操作
		{
			//用户效验
			tagServerUserData *pUserData= pIServerUserItem->GetUserData();
			if( pUserData->cbUserStatus!= US_PLAY)
			{
				return true;
			}
			//消息处理
			return OnUserGang( pUserData->wChairID, pDataBuffer );
		}
	case SUB_C_SELECT_GANG:
		{
			//用户效验
			tagServerUserData *pUserData= pIServerUserItem->GetUserData();
			if( pUserData->cbUserStatus!= US_PLAY )
			{
				return true;
			}
			//消息处理
			return OnUserGangSelect( pUserData->wChairID, pDataBuffer);
		}
	case SUB_C_PENG_MJ:		//碰操作
		{
			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if( pUserData->cbUserStatus != US_PLAY )
			{
				MessageBox( 0, "UN_US_PLAY", 0, 0 );
				return true;
			}
			//消息处理
			return OnUserPeng(pUserData->wChairID, pDataBuffer);
		}
	case SUB_C_CHI_MJ:		//吃操作
		{
			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if( pUserData->cbUserStatus!= US_PLAY )
			{
				MessageBox( 0, "UN_US_PLAY", 0, 0 );
				return true;
			}
			//消息处理
			return OnUserChi(pUserData->wChairID, pDataBuffer);
		}
	case SUB_C_HU_MJ:
		{
			//用户效验
			tagServerUserData *pUserData=pIServerUserItem->GetUserData();
			{
				if( pUserData->cbUserStatus != US_PLAY )
				{
					MessageBox( 0, "UN_US_PLAY", 0, 0 );
					return true;
				}
			}
			//消息处理
			return OnUserHu(pUserData->wChairID, pDataBuffer);
		}
	case SUB_C_GIVEUP:			//放弃
		{
			//用户效验
			tagServerUserData *pUserData = pIServerUserItem->GetUserData();
			if( pUserData->cbUserStatus != US_PLAY )
			{
				return true;
			}
			return OnUserGiveUp( pUserData->wChairID, pDataBuffer);
		}
	}
	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户出牌
bool CTableFrameSink::OnUserOutMj( WORD wChairID, BYTE *byOutMj )
{
	//效验状态
	if( m_pITableFrame->GetGameStatus() != GS_WK_PLAYING )	return false;
	if( wChairID != m_wCurrentUser )
	{
		return false;
	}

	//天胡
	if(m_bTianHu)
	{
		m_bTianHu=false;
	}
	if(m_bRenHu[wChairID])
	{
		//庄家以外出牌，人和否定
		for(WORD i=0; i<m_wPlayerCount;i++)
		{
			m_bRenHu[i]=false;
		}
	}
	if(m_bDiHu[wChairID])
	{
		m_bDiHu[wChairID]=false;
	}
	//出牌
	m_byOutMj = *byOutMj;
	//取消操作权
	if( m_ActionRight[wChairID]>0)
	{
		m_ActionRight[wChairID] = 0;
	}

	//牌值校验
	for( BYTE i=0; i<HAND_MJ_MAX; i++ )
	{
		if( m_byHandMjs[wChairID][i] == m_byOutMj )
		{
			//有效删除麻将
			m_byHandMjs[wChairID][i] = 0;
			
			//左移,使摸牌区为O
			if( m_byHandMjs[wChairID][HAND_MJ_MAX-1] ==0 )
			{
				m_byHandMjCount[wChairID] -= 1;
			}
			else
			{
				m_GameLogic.LMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
				m_byHandMjCount[wChairID] -= 1;
				m_GameLogic.InsertMjList( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );	
			}

			//m_GameLogic.InsertMjList( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
			break;
		}
	}

	//发送数据结构
	m_wCurrentUser = wChairID;
	CMD_S_OutMj OutMj;
	OutMj.wOutMjUser = wChairID;
	OutMj.byOutMj = m_byOutMj;

	//发送数据
	WORD wSendSize = sizeof( CMD_S_OutMj );
	for( WORD i=0; i<m_wPlayerCount; i++ )
	{	
		//不发送自己的信息
		if( i != wChairID )
		{
			//下家,可以吃牌
			if( i == (wChairID+3)%m_wPlayerCount )
			{
				OutMj.nHu	= m_GameLogic.IsHu( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj, m_hu[i], &m_tempHu[i] );
				OutMj.nGang = m_GameLogic.IsGang( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj ).GangStyle;
				OutMj.bPeng = m_GameLogic.IsPeng( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj );
				OutMj.nChi  = m_GameLogic.IsChi( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj );

				//操作权限
				if( OutMj.nHu )
					m_ActionRight[i] = 6;
				else if( OutMj.nGang )
					m_ActionRight[i] = 4;
				else if( OutMj.bPeng )
					m_ActionRight[i] = 3;
				else if( OutMj.nChi )
					m_ActionRight[i] = 2;
			}
			else
			{
				OutMj.nHu	= m_GameLogic.IsHu( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj, m_hu[i], &m_tempHu[i] );
				OutMj.nGang = m_GameLogic.IsGang( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj ).GangStyle;
				OutMj.bPeng = m_GameLogic.IsPeng( m_byHandMjs[i], m_byHandMjCount[i], m_byOutMj );
				OutMj.nChi  = false;

				//操作权限
				if( OutMj.nHu )
					m_ActionRight[i] = 6;
				else if( OutMj.nGang )
					m_ActionRight[i] = 4;
				else if( OutMj.bPeng )
					m_ActionRight[i] = 3;
				else if( OutMj.nChi )
					m_ActionRight[i] = 2;
			}
			m_pITableFrame->SendTableData( i, SUB_S_OUT_MJ, &OutMj, wSendSize );
			m_pITableFrame->SendLookonData( i, SUB_S_OUT_MJ, &OutMj, wSendSize );
		}
	}

	//无人有操作,下家摸牌
	for( int j=0; j<m_wPlayerCount; j++ )
	{
		if( m_ActionRight[j] != 0 )
			break;
		else
		{
			//无人有吃碰杠胡
			if( j == m_wPlayerCount-1 )
			{	
				//切换用户
				if( m_byHandMjCount[wChairID] != 0 )
				{
					m_wCurrentUser = ( wChairID+3 )%m_wPlayerCount;
				}
				else
				{
					m_wCurrentUser = INVALID_CHAIR;
				}
				OnUserTouchMj( m_wCurrentUser );
			}
		}
	}

	//是否结束游戏
	//if( m_wCurrentUser == INVALID_CHAIR )
	//	OnEventGameEnd( wChairID, NULL, GER_NORMAL );

	return true;
}

//用户摸牌
bool CTableFrameSink::OnUserTouchMj( WORD wChairID )
{
	CMD_S_TouchMj TouchMj;
	TouchMj.wCurrentUser = wChairID;
	
	//设置成当前玩家
	m_wCurrentUser = wChairID;

	for( int i=0; i<m_wPlayerCount; i++ )
	{
		if( i == wChairID )
		{
			if( m_nTouchMjPos == ORG_MJ_MAX )
			{
				return OnGameEndEmpty();
			}
			TouchMj.byTouchMj = m_byMjs[m_nTouchMjPos];
			//加入牌面
			m_byHandMjs[i][HAND_MJ_MAX-1] = m_byMjs[m_nTouchMjPos];
			m_byHandMjCount[wChairID] += 1;
		
			//胡、杠牌判断
			//自摸
			TouchMj.nHu = m_GameLogic.IsHu(m_byHandMjs[i], m_byHandMjCount[i]-1, m_byHandMjs[i][HAND_MJ_MAX-1], m_hu[i], &m_tempHu[i] );
			TouchMj.nGang = m_GameLogic.IsAnGang(m_byHandMjs[i], m_byHandMjCount[i]).GangStyle+m_GameLogic.IsZMGang(m_byHandMjs[i], m_byHandMjCount[i], m_hu[i]).GangStyle;
			if( TouchMj.nGang>0)
				m_ActionRight[i] = 4;
			if( TouchMj.nHu)
				m_ActionRight[i] = 6;
		}
		else
		{
			TouchMj.nGang = false;
			TouchMj.nHu   = false;
			TouchMj.byTouchMj = 0xff;		//无效牌
		}
		m_pITableFrame->SendTableData( i, SUB_S_TOUCH_MJ, &TouchMj, sizeof( CMD_S_TouchMj ) );
		m_pITableFrame->SendLookonData( i, SUB_S_TOUCH_MJ, &TouchMj, sizeof( CMD_S_TouchMj ) );
	}
	//摸牌区-1;
	m_nTouchMjPos += 1;
	m_byTouchMjCount -= 1;
	
	return true;
}

bool CTableFrameSink::OnUserGiveUp( WORD wChairID, const void *pDataBuffer)
{
	//效验状态
	if( m_pITableFrame->GetGameStatus() != GS_WK_PLAYING )	
		return false;
	//变量定义
	CMD_C_Operation *Operation = ( CMD_C_Operation* )pDataBuffer;
	m_ActionRight[wChairID] = Operation->nRight;
	//非当前玩家
	if( m_byHandMjs[wChairID][HAND_MJ_MAX-1] == 0)
	{	
		int tempRight=0;
		WORD UserID=-1;
		for( int i=0; i<m_wPlayerCount; i++ )
		{
			if( m_ActionRight[i]>tempRight )
			{
				tempRight=m_ActionRight[i];
				UserID=i;
			}
		}
		if(tempRight>0)
		{
			//发送放弃消息
			m_pITableFrame->SendTableData(UserID,SUB_S_GIVEUP);
		}
		else
		{
			//全部人都放弃操作,下家摸牌
			WORD UserID = (m_wCurrentUser+3)%m_wPlayerCount;
			OnUserTouchMj( UserID );
		}
	}

	return true;
}

//用户碰牌
bool CTableFrameSink::OnUserPeng( WORD wChairID, const void *pDataBuffer )
{
	//效验状态
	if( m_pITableFrame->GetGameStatus() != GS_WK_PLAYING )	
	{
		return false;
	}


	CMD_C_Operation *Peng = ( CMD_C_Operation * )pDataBuffer;
	//权限效验
	if( m_ActionRight[wChairID] < Peng->nRight )
	{
		return false;
	}

	//无碰操作权限以上的操作
	int userCount = 0;		//操作者数
	for( int i=0; i<m_wPlayerCount; i++ )
	{
		//跳过自已,和出牌者
		if( i==wChairID || i==m_wCurrentUser )
			continue;

		if( m_ActionRight[i]>3 )
		{
			break;
		}

		//碰处理
		else 
		{
			userCount ++;
			m_ActionRight[i] = 0;
			if( userCount == 2 )
			{
				m_ActionRight[wChairID] = 0;
				//处理碰
				MJ_BLOCK block;
				block = m_GameLogic.Peng( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_byOutMj );
				m_GameLogic.AddBlock( &m_hu[wChairID], &block );
				m_GameLogic.RMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], 3 );
				m_byHandMjCount[wChairID] -= 2;				//牌面减2

				//发送消息
				CMD_S_Operation  Peng;
				Peng.mjFirst = block.first;
				Peng.blockStyle = block.style;
				Peng.wActionUser = wChairID;
				Peng.wOutMjUser= m_wCurrentUser;
				
				//当前用户变换
				m_wCurrentUser = wChairID;

				m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_PENG_MJ, &Peng, sizeof(CMD_S_Operation) );
				m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_PENG_MJ, &Peng, sizeof(CMD_S_Operation) );
				break;
			}
		}
	}
	return true;
}

//用户吃操作
bool CTableFrameSink::OnUserChi(WORD wChairID, const void *pDataBuffer)
{
	//效验状态
	if( m_pITableFrame->GetGameStatus() != GS_WK_PLAYING )	
	{
		MessageBox( NULL, "GS_WK_PLAYING", 0, 0 );
		return false;
	}

	//设置变量
 	CMD_C_Chi *Chi = ( CMD_C_Chi * )pDataBuffer;
	if( m_ActionRight[wChairID] < Chi->nRight )
	{
		return false;
	}

	//无碰操作权限以上的操作
	int userCount = 0;		//操作者数
	for( int i=0; i<m_wPlayerCount; i++ )
	{
		//跳过自已,和出牌者
		if( i==wChairID || i==m_wCurrentUser )
			continue;

		if( m_ActionRight[i]>2 )
		{
			break;
		}
		//吃处理
		else 
		{
			userCount ++;
			m_ActionRight[i] = 0;
			if( userCount == 2 )
			{
				//是否有吃选择
				if( Chi->nChiType>2 && Chi->nChiType!= 4)
				{
					OnUserChiSelect( wChairID, Chi->nChiType);
				}
				else
				{
					m_ActionRight[wChairID] = 0;
					//处理吃

					MJ_BLOCK block;
					block = m_GameLogic.Chi( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_byOutMj,Chi->nChiType );
					m_GameLogic.AddBlock( &m_hu[wChairID], &block );
					m_GameLogic.RMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], 3 );
					m_byHandMjCount[wChairID] -= 2;				//牌面减2
					//发送消息
					CMD_S_Chi  ChiMj;
					ChiMj.mjFirst = block.first;
					ChiMj.blockStyle = block.style;
					ChiMj.wActionUser = wChairID;
					ChiMj.nChiType= Chi->nChiType;
					//上次操作玩家-出牌者
					ChiMj.wOutMjUser= m_wCurrentUser;

					//当前用户变换
					m_wCurrentUser = wChairID;

					m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_CHI_MJ, &ChiMj, sizeof(CMD_S_Chi) );
					m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_CHI_MJ, &ChiMj, sizeof(CMD_S_Chi) );
				}

			}
		}
	}
	return true;
}

//用户吃牌选择
bool CTableFrameSink::OnUserChiSelect( WORD wChairID, int nChiType)
{
	CMD_S_ChiMode ChiMode;
	ChiMode.nChiType= nChiType;

	m_pITableFrame->SendTableData( wChairID, SUB_S_SELECT_CHI, &ChiMode, sizeof(CMD_S_ChiMode));
	return true;
}
//用户胡牌
bool CTableFrameSink::OnUserHu( WORD wChairID, const void *pDataBuffer)
{
	//效验状态
	if( m_pITableFrame->GetGameStatus() != GS_WK_PLAYING )	
	{
		MessageBox( NULL, "GS_WK_PLAYING", 0, 0 );
		return false;
	}

	CMD_C_Hu *hu = ( CMD_C_Hu * )pDataBuffer;
	//权限效验
	if( m_ActionRight[wChairID] < hu->nRight )
	{
		return false;
	}
	m_strFang= new char[300];
	int userCount = 0;		//操作者数
	for( int i=0; i<m_wPlayerCount; i++ )
	{
		//跳过自已,和出牌者
		if( i==wChairID || i==m_wCurrentUser )
			continue;

		//if( m_ActionRight[i]>6 )
		//{
		//	MessageBox( 0,"有一炮多响", 0, 0 );
		//	//break;
		//	//等待,可能有多人胡
		//}
		//胡处理
		else 
		{
			userCount ++;
			m_ActionRight[i] = 0;
			if( userCount == 2 )
			{
				//玩家结算
				m_wTurnWiner= wChairID;				//赢家
				//自摸
				if( m_byHandMjs[wChairID][HAND_MJ_MAX-1] != 0 )
				{
					//天胡
					if(m_bTianHu)
					{
						strcpy(m_strFang, "天胡 +9番");
						m_nFang[wChairID] = 9;	
						return  OnEventGameEnd( wChairID, NULL, GER_NORMAL );
					}
					//地胡
					if(m_bDiHu[wChairID])
					{
						strcpy(m_strFang, "地胡 +9番");
						m_nFang[wChairID] = 9;
						return  OnEventGameEnd( wChairID, NULL, GER_NORMAL );
					}
					m_bZiM= true;
					int huType = m_GameLogic.IsHu( m_byHandMjs[wChairID], m_byHandMjCount[wChairID]-1, m_byHandMjs[wChairID][HAND_MJ_MAX-1], m_hu[wChairID], &m_tempHu[wChairID]);
					if(  huType== 1)
					{
						m_nFang[wChairID] = m_GameLogic.CheckGDHu( m_tempHu[wChairID], m_GameLogic.TransIDToFeng( wChairID, m_wBankerUser), m_wQFeng, m_strFang );
						strcat( m_strFang, "自摸 +1番" );
						//自摸 +1番
						if( m_nFang[wChairID]<5)
						{
							m_nFang[wChairID]+= 1;
						}
					}
					else
					{
						//九莲灯
						if( huType-20 == 9 )
						{
							strcpy(m_strFang, "九莲宝灯 +9番,");
							m_nFang[wChairID] = 9;							
						}
						//十三幺
						else if( huType-20== 13)
						{
							strcpy( m_strFang, "十三幺 +9番,");
							m_nFang[wChairID] = 9;
						}
					}
				}
				//有人放炮
				else
				{
					if(m_bRenHu[wChairID])
					{
						strcpy(m_strFang, "人胡 +9番");
						m_nFang[wChairID] = 9;	
						return  OnEventGameEnd( wChairID, NULL, GER_NORMAL );
					}
					m_wLastUser= m_wCurrentUser;		//输家
					int huType = m_GameLogic.IsHu( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_byOutMj, m_hu[wChairID], &m_tempHu[wChairID]);
					if(  huType== 1)
					{
						m_nFang[wChairID] = m_GameLogic.CheckGDHu( m_tempHu[wChairID], m_GameLogic.TransIDToFeng( wChairID, m_wBankerUser), m_wQFeng, m_strFang );
					}
					else
					{
						//九莲灯
						if( huType-20 == 9 )
						{
							strcpy(m_strFang, "九莲宝灯 +9番,");
							m_nFang[wChairID] = 9;							
						}
						//十三幺
						else if( huType-20== 13)
						{
							strcpy( m_strFang, "十三幺 +9番,");
							m_nFang[wChairID] = 9;
						}
					}
				}
				////自摸
				//if( m_byHandMjs[wChairID][HAND_MJ_MAX-1] != 0 )
				//{
				//	
				//	//for( int i=0; i<m_wPlayerCount; i++)
				//	//{
				//	//	if( i==wChairID)
				//	//		continue;
				//	//	else
				//	//		m_nFang[i] -= m_nFang[wChairID];
				//	//}
				//}
				////有人放炮
				//else
				//{
				//
				//	//m_nFang[m_wCurrentUser] -= m_nFang[wChairID];
				//}
				//胡牌处理
				 return  OnEventGameEnd( wChairID, NULL, GER_NORMAL );
			}
		}
	}
	return true;
}

//杠牌操作
bool CTableFrameSink::OnUserGang( WORD wChairID, const void *pDataBuffer )
{
	//效验状态
	if( m_pITableFrame->GetGameStatus() != GS_WK_PLAYING )	
	{
		MessageBox( NULL, "GS_WK_PLAYING", 0, 0 );
		return false;
	}

	CMD_C_Gang *Gang = ( CMD_C_Gang * )pDataBuffer;
	//权限效验
	if( m_ActionRight[wChairID] < Gang->nRight )
	{
		return false;
	}

	//无杠操作权限以上的操作
	int userCount = 0;		//操作者数
	for( int i=0; i<m_wPlayerCount; i++ )
	{
		//跳过自已,和出牌者
		if( i==wChairID || i==m_wCurrentUser )
			continue;

		if( m_ActionRight[i]>Gang->nRight )
		{
			break;
		}

		//杠处理
		else 
		{
			userCount ++;
			m_ActionRight[i] = 0;
			if( userCount == 2 )
			{
				switch( Gang->nGangType)
				{
				case 1:					//普通杠
					{
						m_ActionRight[wChairID] = 0;
						MJ_BLOCK block;
						block = m_GameLogic.MGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_byOutMj );
						m_GameLogic.AddBlock( &m_hu[wChairID], &block);

						//待改
						m_GameLogic.RMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], 4 );
						m_byHandMjCount[wChairID] -= 2;				//牌面减2
						m_GameLogic.LMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
						m_byHandMjCount[wChairID] -= 1;


						//发送杠牌消息
						CMD_S_Gang GangS;
						GangS.blockStyle= block.style;
						GangS.mjFirst= block.first;
						GangS.wActionUser= wChairID;
						GangS.gangType= 1;
						GangS.wOutMjUser= m_wCurrentUser;

						//当前用户变换
						m_wCurrentUser = wChairID;

						m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof(CMD_S_Gang) );
						m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof(CMD_S_Gang) );
						//杠牌者摸牌
						OnUserTouchMj(wChairID);
					}
					break;
				case 2:					//补杠
					{
						//如果有多个补杠,进入杠牌选择模式
						BYTE GangMj[3] = { 0xff, 0xff, 0xff };
						MJ_GANGINFO gangInfo;
						gangInfo = m_GameLogic.IsZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_hu[wChairID] );
						GangMj[0] = gangInfo.GangMj;

						gangInfo =m_GameLogic.IsZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_hu[wChairID], GangMj[0]);
						GangMj[1] = gangInfo.GangMj;

						//有多个补杠,进入杠牌选择模式
						if( GangMj[1] != 0xff)
						{
							gangInfo = m_GameLogic.IsZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_hu[wChairID],GangMj[0], GangMj[1] );
							GangMj[2] = gangInfo.GangMj;
							
							//创建结构
							CMD_S_GangMode GangMode;
							CopyMemory( &GangMode.byMjGang, &GangMj, 3*sizeof(BYTE));
							GangMode.nGangType= 2;

							m_pITableFrame->SendTableData( wChairID, SUB_S_SELECT_GANG, &GangMode, sizeof( CMD_S_GangMode ));
						}
						//没有选择,杠处理
						else
						{
							m_ActionRight[wChairID] = 0;
							//MJ_BLOCK block;	
							//m_GameLogic.AddBlock( &m_hu[wChairID], &block);
						
							//block= m_GameLogic.ZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID],&m_hu[wChairID],  m_byOutMj );
							BYTE GangMj;
							GangMj = m_GameLogic.IsZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_hu[wChairID]).GangMj;
							m_GameLogic.ZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID],&m_hu[wChairID], GangMj );
							//待改
							m_GameLogic.LMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
							//m_GameLogic.RMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], 1 );
							m_byHandMjCount[wChairID] -= 1;				//牌面减1

							//发送杠牌消息
							CMD_S_Gang GangS;
							GangS.blockStyle= BLOCK_GANG;
							GangS.mjFirst= GangMj;
							GangS.wActionUser= wChairID;
							GangS.gangType= 2;
							GangS.wOutMjUser= m_wCurrentUser;

							//当前用户变换
							m_wCurrentUser = wChairID;

							m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof(CMD_S_Gang) );
							m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof(CMD_S_Gang) );
							//杠牌者摸牌
							OnUserTouchMj(wChairID);
						}
					}
					break;
				case 4:					//暗杠
					{
						//如果有多个暗杠,进先杠牌选择模式
						BYTE GangMj[3] = { 0xff, 0xff, 0xff };
						MJ_GANGINFO gangInfo;
						gangInfo = m_GameLogic.IsAnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
						GangMj[0] = gangInfo.GangMj;

						gangInfo =m_GameLogic.IsAnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], GangMj[0]);
						GangMj[1] = gangInfo.GangMj;

						//有多个补杠,进先杠牌选择模式
						if( GangMj[1] != 0xff)
						{
							gangInfo = m_GameLogic.IsAnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], GangMj[0], GangMj[1] );
							GangMj[2] = gangInfo.GangMj;

							//创建结构
							CMD_S_GangMode GangMode;
							CopyMemory( &GangMode.byMjGang, &GangMj, 3*sizeof(BYTE));
							GangMode.nGangType= 4;

							m_pITableFrame->SendTableData( wChairID, SUB_S_SELECT_GANG, &GangMode, sizeof( CMD_S_GangMode ));
						}
						else
						{
							m_ActionRight[wChairID] = 0;
							BYTE GangMj = m_GameLogic.IsAnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] ).GangMj;
							MJ_BLOCK block;

							block  = m_GameLogic.AnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], GangMj );
							m_GameLogic.AddBlock( &m_hu[wChairID], &block);

							//消牌处理
							m_GameLogic.SortMj(m_byHandMjs[wChairID],14,14);
							m_byHandMjCount[wChairID]-=3;
							m_GameLogic.LMove(m_byHandMjs[wChairID],m_byHandMjCount[wChairID]);
							m_byHandMjCount[wChairID] -= 1;
							//m_GameLogic.RMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], 4 );
							//m_byHandMjCount[wChairID] -= 3;				//牌面减1
							//m_GameLogic.LMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
							//m_byHandMjCount[wChairID] -= 1;
							//消息处理
							CMD_S_Gang GangS;
							GangS.blockStyle= block.style;
							GangS.mjFirst= block.first;
							GangS.wActionUser= wChairID;
							GangS.gangType= 4;

							//发送消息
							m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof( CMD_S_Gang));
							m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof( CMD_S_Gang));

							//杠牌者摸牌
							OnUserTouchMj(wChairID);
						}
					}
					break;
				case 6:					//有补杠也有暗杠,,,进行杠牌选择
					{
						BYTE GangMj[3] = { 0xff, 0xff, 0xff };
						MJ_GANGINFO gangInfo;
						gangInfo = m_GameLogic.IsAnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
						GangMj[0] = gangInfo.GangMj;

						gangInfo =m_GameLogic.IsZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_hu[wChairID], GangMj[0]);
						GangMj[1] = gangInfo.GangMj;

						//有多个补杠,进先杠牌选择模式
						if( GangMj[1] != 0xff)
						{
							gangInfo = m_GameLogic.IsAnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
							if( gangInfo.GangMj== 0xff)
							{
								gangInfo = m_GameLogic.IsZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], m_hu[wChairID],GangMj[0], GangMj[1] );
								GangMj[2] = gangInfo.GangMj;
							}
							else
							{
								GangMj[2] = gangInfo.GangMj;
							}
							//创建结构
							CMD_S_GangMode GangMode;
							CopyMemory( &GangMode.byMjGang, &GangMj, 3*sizeof(BYTE));
							GangMode.nGangType= 2;

							m_pITableFrame->SendTableData( wChairID, SUB_S_SELECT_GANG, &GangMode, sizeof( CMD_S_GangMode ));
					}
					break;
					}
				}
			}
		}
	}
	return true;
}

//用户选择杠
bool CTableFrameSink::OnUserGangSelect( WORD wChairID, const void *pDataBuffer )
{
	CMD_C_SelectGang *SelectGang = (CMD_C_SelectGang* )pDataBuffer;
	
	switch( SelectGang->nGangType)
	{
	case 2:		//补杠
		{
			m_ActionRight[wChairID] = 0;

			m_GameLogic.ZMGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID],&m_hu[wChairID], SelectGang->byGangMj );

			m_GameLogic.LMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
			m_byHandMjCount[wChairID] -= 1;				//牌面减1

			//发送杠牌消息
			CMD_S_Gang GangS;
			GangS.blockStyle= BLOCK_GANG;
			GangS.mjFirst= SelectGang->byGangMj;
			GangS.wActionUser= wChairID;
			GangS.gangType= 2;

			GangS.wOutMjUser= m_wCurrentUser;
			//当前用户变换
			m_wCurrentUser = wChairID;

			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof(CMD_S_Gang) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof(CMD_S_Gang) );
			//杠牌者摸牌
			OnUserTouchMj(wChairID);
		}
		break;
	case 4:		//暗杠
		{
			m_ActionRight[wChairID] = 0;
			MJ_BLOCK block;

			block  = m_GameLogic.AnGang( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], SelectGang->byGangMj );
			m_GameLogic.AddBlock( &m_hu[wChairID], &block);

			//消牌处理
			m_GameLogic.SortMj(m_byHandMjs[wChairID],14,14);
			m_byHandMjCount[wChairID]-=3;
			m_GameLogic.LMove(m_byHandMjs[wChairID],m_byHandMjCount[wChairID]);
			m_byHandMjCount[wChairID] -= 1;
			//m_GameLogic.RMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID], 4 );
			//m_byHandMjCount[wChairID] -= 3;				//牌面减1
			//m_GameLogic.LMove( m_byHandMjs[wChairID], m_byHandMjCount[wChairID] );
			//m_byHandMjCount[wChairID] -= 1;
			
			//消息处理
			CMD_S_Gang GangS;
			GangS.blockStyle= block.style;
			GangS.mjFirst= block.first;
			GangS.wActionUser= wChairID;
			GangS.gangType= 4;
			//发送消息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof( CMD_S_Gang));
			m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GANG_MJ, &GangS, sizeof( CMD_S_Gang));

			//杠牌者摸牌
			OnUserTouchMj(wChairID);
		}
		break;		
	}
	return true;
}
//流局
bool CTableFrameSink::OnGameEndEmpty()
{
	CMD_S_GameEndEmpty EndEmpty;
	memset( &EndEmpty, 0, sizeof(CMD_S_GameEndEmpty));
	//亮牌
	for( int k=0; k<4; k++ )
	{
		CopyMemory( &EndEmpty.byMjs[k], m_byHandMjs[k], sizeof(BYTE)*14);
	}
	m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_END_EMPTY, &EndEmpty, sizeof( CMD_S_GameEndEmpty));
	m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_END_EMPTY, &EndEmpty, sizeof( CMD_S_GameEndEmpty));
	m_pITableFrame->ConcludeGame();
	return true;
}
//////////////////////////////////////////////////////////////////////////
