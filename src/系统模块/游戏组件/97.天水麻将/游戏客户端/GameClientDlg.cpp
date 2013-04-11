/**********
1.定时器问题；
2.保存数据；
3.
4.

***********/

#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientDlg.h"

#include "Struct.h"

int isStartInt = 0;


//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_START_GAME				200									//开始定时器
#define IDI_OPERATE_CARD			202									//操作定时器

//游戏定时器
#define TIME_START_GAME				40									//开始定时器
#define TIME_HEAR_STATUS			3									//出牌定时器
#define TIME_OPERATE_CARD			30									//操作定时器

///////////////////////////计时器线程//////////////////////////////////////////////////////
extern int sz1,sz2;
int showNext = 0;

UINT MyThreadProc( LPVOID pParam )
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sMyThreadProc.log",strTime);

	strTemp.Format("MyThreadProc");
	theApp.WriteLog(strFile, strTemp);

	CGameClientDlg *GameView = (CGameClientDlg*)pParam;

	strTemp.Format("GameView->m_GameClientView.m_tDice=%d", GameView->m_GameClientView.m_tDice);
	theApp.WriteLog(strFile, strTemp);

	while( GameView->m_GameClientView.m_tDice > 0 )
	{
		Sleep( 50 );
		if( GameView->m_GameClientView.m_tDice<12)
		{
			GameView->m_GameClientView.m_tDice ++;
			GameView->m_GameClientView.UpdateGameView( NULL );

			strTemp.Format("GameView->m_GameClientView.m_tDice=%d", GameView->m_GameClientView.m_tDice);
			theApp.WriteLog(strFile, strTemp);

		}
		else
		{
			Sleep( 3000 );
			if (0 == showNext)
			showNext = 1;
		//	GameView->m_GameClientView.m_Timer.ShowWindow( SW_SHOW );
		//	GameView->m_GameClientView.m_tDice= 0;
	//		GameView->m_GameClientView.SetTimer(ID_OUTFLAG_TIME, 150,(TIMERPROC) NULL);
			GameView->ShowUserMJ();
			GameView->m_GameClientView.UpdateGameView( NULL );
			strTemp.Format("GameView->m_GameClientView.m_tDice=%d", GameView->m_GameClientView.m_tDice);
			theApp.WriteLog(strFile, strTemp);

			return 0;
		}
	}
	/*Del by doctor 20071214
	while( GameView->m_GameClientView.m_bActionOut== true )
	{
		Sleep(50);
		//int m_playTime = timeGetTime()*1000;
		//if( (m_playTime-GameView->m_GameClientView.m_tOutStartTime)%2==1)
		{
			GameView->m_GameClientView.m_tOut += 1;
			GameView->m_GameClientView.m_bPlay=false;
			if( GameView->m_GameClientView.m_tOut==10)
			{
				GameView->m_GameClientView.m_tOut = 0;
				GameView->m_GameClientView.m_bActionOut= false;
				GameView->m_GameClientView.m_iUpMj = -1;
			}
			GameView->m_GameClientView.UpdateGameView(NULL);
		}
	}*/
	strTemp.Format("end ");
	theApp.WriteLog(strFile, strTemp);

	return 0;
}

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

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_TING_CARD,OnTingCard)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
	ON_MESSAGE(IDM_SHOW_SZ,OnShowSZ)
	ON_MESSAGE(IDM_RESET_UI,OnResetUI )							
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	uiShowInt = 0;

	//状态变量
	m_bHearStatus=false;

	//堆立变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合麻将
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//麻将变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置标题
	SetWindowText(TEXT("天水麻将"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载资源
	g_CardResource.LoadResource();

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	m_bHearStatus=false;

	//堆立变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合麻将
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//麻将变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		EnableSound(GameOption.m_bEnableSound);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnTimerMessage.log",strTime);

	switch (nTimerID)
	{
		case ID_OUTFLAG_TIME:
			{
				ShowUserMJ();
	strTemp.Format("ShowUserMJ");
	theApp.WriteLog(strFile, strTemp);

			//AfxMessageBox("ddd");
			KillGameTimer(ID_OUTFLAG_TIME);
				return true;
			}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				OnStart(0,0);
				return false;
			}
			if (nElapse<=5) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
			//超时判断
			if ((nElapse==0)&&(IsLookonMode()==false))
			{
				//获取位置
				WORD wMeChairID=GetMeChairID();

				//动作处理
				if (wChairID==wMeChairID)
				{
					if (m_wCurrentUser==wMeChairID)
					{
						BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
						OnOutCard(cbCardData,cbCardData);
					}
					else OnCardOperate(WIK_NULL,0);
				}

				return false;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnGameMessage.log",strTime);

			strTemp.Format("OnGameMessage wSubCmdID=%d", wSubCmdID);
			theApp.WriteLog(strFile, strTemp);

	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
				strTemp.Format("游戏开始");
				theApp.WriteLog(strFile, strTemp);

				showNext = 0;
				isStartInt = 0;

				CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
				cbSiceFirst=HIBYTE(pGameStart->wSiceCount) -1;
				cbSiceSecond=LOBYTE(pGameStart->wSiceCount) -1;


				cbSice2First=HIBYTE(pGameStart->wSiceCount2) -1;
				cbSice2Second=LOBYTE(pGameStart->wSiceCount2) -1;

				memcpy( (char *)&myGameStart , (char *) pBuffer, wDataSize);
				PostMessage(IDM_SHOW_SZ,0,0);

		//听牌按钮
		m_GameClientView.m_btTingCard.ShowWindow(SW_SHOW);
				return true;
			//return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			strTemp.Format("用户出牌");
			theApp.WriteLog(strFile, strTemp);
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			strTemp.Format("发牌消息");
			theApp.WriteLog(strFile, strTemp);
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			strTemp.Format("操作提示");
			theApp.WriteLog(strFile, strTemp);
			return OnSubOperateNotify(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			strTemp.Format("操作结果");
			theApp.WriteLog(strFile, strTemp);
			return OnSubOperateResult(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			strTemp.Format("游戏结束");
			theApp.WriteLog(strFile, strTemp);

		//听牌按钮
		m_GameClientView.m_btTingCard.ShowWindow(SW_HIDE);
			return OnSubGameEnd(pBuffer,wDataSize);
		}
//听牌
	case SUB_S_TING_CARD:
		{
			strTemp.Format("听牌");
			theApp.WriteLog(strFile, strTemp);
			return OnSubTingCard(pBuffer,wDataSize);			
		};

	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnGameSceneMessage.log",strTime);

	strTemp.Format("OnGameSceneMessage  cbGameStation=%d", cbGameStation);
	theApp.WriteLog(strFile, strTemp);

	switch (cbGameStation)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置数据
			m_wBankerUser=pStatusFree->wBankerUser;
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
				m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;

			//历史变量
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));

			//麻将变量
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			//辅助变量
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++) wViewChairID[i]=SwitchViewChairID(i);

			//界面设置
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);

			//组合麻将
			BYTE cbWeaveCard[4]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				for (BYTE j=0;j<m_cbWeaveCount[i];j++)
				{
					BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbCenterCard,cbWeaveCard);

	strTemp.Format("cbWeaveCardCount  =%d", cbWeaveCardCount);
	theApp.WriteLog(strFile, strTemp);

					m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetCardData(cbWeaveCard,cbWeaveCardCount);
					if (cbWeaveKind&(WIK_GANG|WIK_FILL))
					{
						if (m_WeaveItemArray[i][j].wProvideUser==i) m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetDisplayItem(false);
					}
				}
			}

			//用户麻将
			if (m_wCurrentUser==GetMeChairID())
			{
				BYTE cbCardCount=pStatusPlay->cbCardCount;
				m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[cbCardCount-cbCardCount]);
			}
			else m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 

			//麻将设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//用户麻将
				if (i!=GetMeChairID())
				{
					BYTE cbCardCount=13-m_cbWeaveCount[i]*3;
					WORD wUserCardIndex=(wViewChairID[i]<2)?wViewChairID[i]:2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount,(m_wCurrentUser==i));
				}

				//丢弃麻将
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(m_cbDiscardCard[i],m_cbDiscardCount[i]);
			}

			//控制设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
			}

			//堆立麻将
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
			}

			//分发麻将
			BYTE cbTakeCount=MAX_REPERTORY-m_cbLeftCardCount;
			WORD wTakeChairID=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
/*
	//Del by doctor 		BYTE cbTakeCount=108-m_cbLeftCardCount;			
			BYTE cbTakeCount=MAX_REPERTORY-m_cbLeftCardCount;
			BYTE cbSiceFirst=HIBYTE(pStatusPlay->wSiceCount);
			BYTE cbSiceSecond=LOBYTE(pStatusPlay->wSiceCount);
			WORD wTakeChairID=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;


//色子
	//选取大点的做
	if( cbSiceFirst> cbSiceSecond)
	{
		m_GameClientView.m_nDiceCount1 = cbSiceFirst;
		m_GameClientView.m_nDiceCount2 = cbSiceSecond;
	}
	else
	{
		m_GameClientView.m_nDiceCount1 = cbSiceSecond;
		m_GameClientView.m_nDiceCount2 = cbSiceFirst;
	}

	strTemp.Format("m_nDiceCount1  =%d, m_GameClientView.m_nDiceCount2", m_GameClientView.m_nDiceCount1,m_GameClientView.m_nDiceCount2);
	theApp.WriteLog(strFile, strTemp);

			m_GameClientView.m_tDice = 1;
		//	AfxBeginThread( MyThreadProc, this);

		//	Sleep(3000);
*/
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//计算数目
				BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceSecond-1)*2:0);
				BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

				//提取麻将
				cbTakeCount-=cbRemoveCount;
				m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

				//完成判断
				if (cbTakeCount==0)
				{
					m_wHeapHand=wTakeChairID;
					m_wHeapTail=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
					break;
				}

				//切换索引
				wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
			}

			//堆立界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//历史麻将
			if (m_wOutCardUser!=INVALID_CHAIR)
			{
				WORD wOutChairID=SwitchViewChairID(m_wOutCardUser);
				m_GameClientView.SetOutCardInfo(wOutChairID,m_cbOutCardData);
			}

			//出牌设置
			if (cbGameStation==GS_MJ_PLAY)
			{
				//操作界面
				if ((IsLookonMode()==false)&&(pStatusPlay->cbActionMask!=WIK_NULL))
				{
					//获取变量
					BYTE cbActionMask=pStatusPlay->cbActionMask;
					BYTE cbActionCard=pStatusPlay->cbActionCard;

					//变量定义
					tagGangCardResult GangCardResult;
					ZeroMemory(&GangCardResult,sizeof(GangCardResult));

					//杠牌判断
					if ((cbActionMask&(WIK_GANG|WIK_FILL))!=0)
					{
						//桌面杆牌
						if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
						{
							GangCardResult.cbCardCount=1;
							GangCardResult.cbCardData[0]=cbActionCard;
						}

						//自己杆牌
						if ((m_wCurrentUser==GetMeChairID())||(cbActionCard==0))
						{
							WORD wMeChairID=GetMeChairID();
							m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
						}
					}

					//设置界面
					m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
					if (m_wCurrentUser==INVALID_CHAIR) SetGameTimer(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
				}

				//设置时间
				if (m_wCurrentUser!=INVALID_CHAIR)
				{
					//计算时间
					WORD wTimeCount=TIME_OPERATE_CARD;
					//启动定时器
					if ((m_bHearStatus==true)&&(m_wCurrentUser==GetMeChairID())) 
					{
						wTimeCount=TIME_HEAR_STATUS;
					}

					//设置时间
					SetGameTimer(m_wCurrentUser,TIME_OPERATE_CARD,wTimeCount);
				}
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置状态
//	SetGameStatus(GS_MJ_PLAY);

	//设置变量
	m_bHearStatus=false;
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合麻将
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置麻将
	BYTE cbCardCount=(GetMeChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//设置界面
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//旁观界面
	if (bPlayerMode==false)
	{
		m_GameClientView.SetHuangZhuang(false);
		m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
		m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	}

	//堆立麻将
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
	}

	//分发麻将
	BYTE cbTakeCount=(MAX_COUNT-1)*GAME_PLAYER+1;
	/*
	BYTE cbSiceFirst=HIBYTE(pGameStart->wSiceCount);
	BYTE cbSiceSecond=LOBYTE(pGameStart->wSiceCount);
	*/
	//计算拿的DESK
//Del by 20080101	WORD wTakeChairID=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;

	WORD wTakeChairID=(m_wBankerUser+ 13 -(cbSiceFirst+cbSiceSecond))%GAME_PLAYER;

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//计算数目
//Del by 20080101		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceSecond-1)*2:0);
		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSice2First+cbSice2Second-1)*2:0);
		BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

		//提取麻将
		cbTakeCount-=cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

		//完成判断
		if (cbTakeCount==0)
		{
			m_wHeapHand=wTakeChairID;
//Del by 20080101			m_wHeapTail=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
			m_wHeapTail=(m_wBankerUser+ 13 -(cbSiceFirst+cbSiceSecond) )%GAME_PLAYER;
			break;
		}//End if

		//切换索引
		wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
	}//End for (WORD i=0;i<GAME_PLAYER;i++)

	//麻将设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//组合界面
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);

		//用户麻将
		if (wViewChairID!=2)
		{
			WORD wIndex=(wViewChairID>=3)?2:wViewChairID;
			m_GameClientView.m_UserCard[wIndex].SetCardData(CountArray(pGameStart->cbCardData),(i==m_wBankerUser));
		}
		else
		{
			BYTE cbBankerCard=(i==m_wBankerUser)?pGameStart->cbCardData[MAX_COUNT-1]:0;
			m_GameClientView.m_HandCardControl.SetCardData(pGameStart->cbCardData,MAX_COUNT-1,cbBankerCard);
		}

		//旁观界面
		if (bPlayerMode==false)
		{
			m_GameClientView.m_TableCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][3].SetCardData(NULL,0);
		}
	}

	//出牌提示
	if ((bPlayerMode==true)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		WORD wMeChairID=GetMeChairID();
		if (m_wCurrentUser==wMeChairID) m_GameClientView.SetStatusFlag(true,false);
	}

	//动作处理
	if ((bPlayerMode==true)&&(pGameStart->cbUserAction!=WIK_NULL))
	{
		ShowOperateControl(pGameStart->cbUserAction,0);
		SetGameTimer(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	}//End 

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//激活框架
	if (bPlayerMode==true) ActiveGameFrame();

	//环境处理
//	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_OPERATE_CARD);

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSubOutCard.log",strTime);

	strTemp.Format("OnSubOutCard ");
	theApp.WriteLog(strFile, strTemp);

	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;

	//其他用户
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID))
	{
		//环境设置
		KillGameTimer(IDI_OPERATE_CARD); 

		const tagUserData * pUserData=GetUserData(pOutCard->wOutCardUser);

			strTemp.Format("pUserData->cbGender=%d ", pUserData->cbGender);
			theApp.WriteLog(strFile, strTemp);

		PlayCardSound(pOutCard->cbOutCardData, pUserData->cbGender);

		//出牌界面
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(wOutViewChairID,pOutCard->cbOutCardData);

		//设置麻将
		if (wOutViewChairID==2)
		{
			//删除麻将
			BYTE cbCardData[MAX_COUNT];
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);

			//设置麻将
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex=(wOutViewChairID>2)?2:wOutViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}//End if
	}//End if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID))

	return true;
}

//发牌消息
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//丢弃麻将
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃麻将
		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);

		//设置变量
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//发牌处理
	if (pSendCard->cbCardData!=0)
	{
		//取牌界面
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		if (wViewChairID!=2)
		{
			WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(true);
		}
		else
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;
			m_GameClientView.m_HandCardControl.SetCurrentCard(pSendCard->cbCardData);
		}
		
		//扣除麻将
		DeductionTableCard(true);
	}

	//当前用户
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		//激活框架
		ActiveGameFrame();

		//动作处理
		if (pSendCard->cbActionMask!=WIK_NULL)
		{
			//获取变量
			BYTE cbActionCard=pSendCard->cbCardData;
			BYTE cbActionMask=pSendCard->cbActionMask;

			//变量定义
			tagGangCardResult GangCardResult;
			ZeroMemory(&GangCardResult,sizeof(GangCardResult));

			//杠牌判断
			if ((cbActionMask&(WIK_GANG|WIK_FILL))!=0)
			{
				WORD wMeChairID=GetMeChairID();
				m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			}

			//设置界面
			m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
			/*Doctor 
WORD wMeChairID=GetMeChairID();
	//分析是否可以听牌
	if (m_GameLogic.IsCanTingCard(m_cbCardIndex, m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID]) )
	{

		if (!m_GameClientView.m_bTingFlag[wMeChairID])
		{
			m_GameClientView.m_ControlWnd.ShowHideTingCardButton(1);
		}
		else
		{
			m_GameClientView.m_ControlWnd.ShowHideTingCardButton(0);
		}
	}
	else
	{
		m_GameClientView.m_ControlWnd.ShowHideTingCardButton(0);
	}
	*/
		}
	}

	//出牌提示
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID),false);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//计算时间
	WORD wTimeCount=TIME_OPERATE_CARD;
	//启动定时器
	if ((m_bHearStatus==true)&&(m_wCurrentUser==wMeChairID)) 
	{
		wTimeCount=TIME_HEAR_STATUS;
	}

	//设置时间
	SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);

	return true;
}

//操作提示
bool CGameClientDlg::OnSubOperateNotify(const void * pBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSubOperateNotify.log",strTime);

	strTemp.Format("OnSubOperateNotify");
	theApp.WriteLog(strFile, strTemp);

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//用户界面
	if ((IsLookonMode()==false)&&(pOperateNotify->cbActionMask!=WIK_NULL))
	{
		//获取变量
		BYTE cbActionMask=pOperateNotify->cbActionMask;
		BYTE cbActionCard=pOperateNotify->cbActionCard;

		//变量定义
		tagGangCardResult GangCardResult;
		ZeroMemory(&GangCardResult,sizeof(GangCardResult));

		strTemp.Format("cbActionMask=%d,cbActionCard=%d ", cbActionMask, cbActionCard);
		theApp.WriteLog(strFile, strTemp);

		//杠牌判断
		if ((cbActionMask&(WIK_GANG|WIK_FILL))!=0)
		{
			//桌面杆牌
			if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
			{
				GangCardResult.cbCardCount=1;
				GangCardResult.cbCardData[0]=cbActionCard;
			}//End if

			//自己杆牌
			if ((m_wCurrentUser==GetMeChairID())||(cbActionCard==0))
			{
				WORD wMeChairID=GetMeChairID();
				m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			}//End if
		}//End if

		//设置界面
		ActiveGameFrame();
		SetGameTimer(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
		m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
/*
WORD wMeChairID=GetMeChairID();
	//分析是否可以听牌
	if (m_GameLogic.IsCanTingCard(m_cbCardIndex, m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID]) )
	{

		if (!m_GameClientView.m_bTingFlag[wMeChairID])
		{
			m_GameClientView.m_ControlWnd.ShowHideTingCardButton(1);
		}
		else
		{
			m_GameClientView.m_ControlWnd.ShowHideTingCardButton(0);
		}
	}
	else
	{
		m_GameClientView.m_ControlWnd.ShowHideTingCardButton(0);
	}
	*/

	}//End if

	return true;
}

//操作结果
bool CGameClientDlg::OnSubOperateResult(const void * pBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSubOperateResult.log",strTime);

			strTemp.Format("OnSubOperateResult");
			theApp.WriteLog(strFile, strTemp);

	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	//变量定义
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;
	WORD wOperateViewID=SwitchViewChairID(wOperateUser);

			strTemp.Format("wOperateUser=%d,pOperateResult->cbOperateCode=%d", wOperateUser,pOperateResult->cbOperateCode );
			theApp.WriteLog(strFile, strTemp);

	//出牌变量
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置组合
	if ((pOperateResult->cbOperateCode&(WIK_GANG|WIK_FILL))!=0)
	{
		//设置变量
		m_wCurrentUser=INVALID_CHAIR;

		//组合麻将
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
				break;
			}
		}//End for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)

		//组合麻将
		if (cbWeaveIndex==0xFF)
		{
			//暗杠判断
			cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

			//设置麻将
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}//End if (cbWeaveIndex==0xFF)

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);

			strTemp.Format("cbWeaveCardCount=%d",  cbWeaveCardCount);
			theApp.WriteLog(strFile, strTemp);

		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDisplayItem((cbPublicCard==TRUE)?true:false);

		//麻将设置
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;
		}//End if (GetMeChairID()==wOperateUser)

		//设置麻将
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,false);
		}//End if
	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		//设置变量
		m_wCurrentUser=pOperateResult->wOperateUser;

		//设置组合
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);

			strTemp.Format("cbWeaveCardCount2=%d",  cbWeaveCardCount);
			theApp.WriteLog(strFile, strTemp);

		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);

		//删除麻将
		if (GetMeChairID()==wOperateUser)
		{
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		}

		//设置麻将
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,true);
		}//End if
	}//End if

	//设置界面
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserAction(wOperateViewID,pOperateResult->cbOperateCode);
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	const tagUserData * pUserData=GetUserData(wOperateUser);

	//环境设置
	PlayActionSound(pOperateResult->cbOperateCode, pUserData->cbGender);

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//计算时间
		WORD wTimeCount=TIME_OPERATE_CARD;
		//
		if ((m_bHearStatus==true)&&(m_wCurrentUser==GetMeChairID()))
		{
			wTimeCount=TIME_HEAR_STATUS;
		}

		//设置时间
		SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);
	}//End if (m_wCurrentUser!=INVALID_CHAIR)

	return true;
}
	//听牌
bool CGameClientDlg::OnSubTingCard(const void * pBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSubTingCard.log",strTime);

	strTemp.Format("OnSubTingCard ");
	theApp.WriteLog(strFile, strTemp);

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameTingCard));
	if (wDataSize!=sizeof(CMD_S_GameTingCard)) return false;

	//消息处理
	CMD_S_GameTingCard * pGameEnd=(CMD_S_GameTingCard *)pBuffer;


	WORD wOutViewChairID=SwitchViewChairID(pGameEnd->wTingCardUser);

	m_GameClientView.m_bTingFlag[wOutViewChairID] = true;
	return true;

}
//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//设置状态
	SetGameStatus(GS_MJ_FREE);
	m_GameClientView.SetStatusFlag(false,false);

	//删除定时器
	KillGameTimer(IDI_OPERATE_CARD);

	//设置控件
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);

	//结束设置
	if (pGameEnd->cbChiHuCard!=0)
	{
		tagGameScoreInfo GameScoreInfo;
		GameScoreInfo.wBankerUser=m_wBankerUser;
		GameScoreInfo.cbChiHuCard=pGameEnd->cbChiHuCard;
		GameScoreInfo.wProvideUser=pGameEnd->wProvideUser;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			const tagUserData * pUserData=GetUserData(i);
			GameScoreInfo.lScore[i]=pGameEnd->lGameScore[i];
			GameScoreInfo.wCardType[i]=pGameEnd->wChiHuKind[i];
			lstrcpyn(GameScoreInfo.szUserName[i],pUserData->szName,CountArray(GameScoreInfo.szUserName[i]));
		}
		m_GameClientView.m_GameScoreWnd.SetScoreInfo(GameScoreInfo);
	}
	else 
	{
		DeductionTableCard(true);
		m_GameClientView.SetHuangZhuang(true);
	}

	//用户麻将
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if (pGameEnd->wChiHuKind[i]!=CHK_NULL) m_GameClientView.SetUserAction(wViewChairID,WIK_CHI_HU);
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}

	//设置麻将
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//播放声音
	LONG lScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//播放声音
void CGameClientDlg::PlayCardSound(BYTE cbCardData, int sexInt)
{
	//变量定义
	TCHAR szSoundName[16]=TEXT("");
//Del by doctor 20071202	TCHAR szKind[]={TEXT('W'),TEXT('S'),TEXT('T'),TEXT('F')};
	TCHAR szKind[]={TEXT('W'),TEXT('S'),TEXT('T'),TEXT('Z')};


	//构造名字
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;

	//是男声
	if (sexInt == 1)
	{
		_snprintf(szSoundName,CountArray(szSoundName),TEXT("%c_%d"),szKind[cbColor],cbValue);
	}
	else
	{
		_snprintf(szSoundName,CountArray(szSoundName),TEXT("W_%c_%d"),szKind[cbColor],cbValue);	
	}

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),szSoundName);

	return;
}

//播放声音
void CGameClientDlg::PlayActionSound(BYTE cbAction , int sexInt )
{
	switch (cbAction)
	{
	case WIK_NULL:		//取消
		{
				//是男声
	if (sexInt == 1)
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	}
	else
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("W_OUT_CARD"));
	}

			break;
		}
/*
	case WIK_LEFT:
	case WIK_CENTER:
	case WIK_RIGHT:		//上牌
		{
				//是男声
	if (sexInt == 1)
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("A_SHANG"));
	}
	else
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("W_A_SHANG"));
	}

			break;
		}
		*/
	case WIK_PENG:		//碰牌
		{
				//是男声
	if (sexInt == 1)
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("A_PENG"));
	}
	else
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("W_A_PENG"));
	}

			break;
		}
	case WIK_FILL:		//补牌
		{
				//是男声
	if (sexInt == 1)
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("A_FILL"));
	}
	else
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("W_A_FILL"));
	}

			break;
		}
	case WIK_GANG:		//杠牌
		{
				//是男声
	if (sexInt == 1)
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("A_GANG"));
	}
	else
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("W_A_GANG"));
	}

			break;
		}
	case WIK_CHI_HU:	//吃胡
		{
				//是男声
	if (sexInt == 1)
	{
					PlayGameSound(AfxGetInstanceHandle(),TEXT("A_CHI_HU"));
	}
	else
	{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("W_A_CHI_HU"));
	}

			break;
		}
	}

	return;
}

//扣除麻将
void CGameClientDlg::DeductionTableCard(bool bHeadCard)
{
	if (bHeadCard==true)
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHand][0]+m_cbHeapCardInfo[m_wHeapHand][1];
		if (cbHeapCount==HEAP_FULL_COUNT) m_wHeapHand=(m_wHeapHand+1)%CountArray(m_cbHeapCardInfo);

		//减少麻将
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapHand][0]++;

		//堆立麻将
		WORD wHeapViewID=SwitchViewChairID(m_wHeapHand);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapHand][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapHand][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}
	else
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapTail][0]+m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT) m_wHeapTail=(m_wHeapTail+3)%CountArray(m_cbHeapCardInfo);

		//减少麻将
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapTail][1]++;

		//堆立麻将
		WORD wHeapViewID=SwitchViewChairID(m_wHeapTail);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapTail][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapTail][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}

	return;
}

//显示控制
bool CGameClientDlg::ShowOperateControl(BYTE cbUserAction, BYTE cbActionCard)
{
	//变量定义
	tagGangCardResult GangCardResult;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//杠牌判断
	if ((cbUserAction&(WIK_GANG|WIK_FILL))!=0)
	{
		//桌面杆牌
		if (cbActionCard!=0)
		{
			GangCardResult.cbCardCount=1;
			GangCardResult.cbCardData[0]=cbActionCard;
		}

		//自己杆牌
		if (cbActionCard==0)
		{
			WORD wMeChairID=GetMeChairID();
			m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
		}
	}//End if ((cbUserAction&(WIK_GANG|WIK_FILL))!=0)

	//显示界面
	m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbUserAction,GangCardResult);
/*
	WORD wMeChairID=GetMeChairID();
	//分析是否可以听牌
	if (m_GameLogic.IsCanTingCard(m_cbCardIndex, m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID]) )
	{

		if (!m_GameClientView.m_bTingFlag[wMeChairID])
		{
			m_GameClientView.m_ControlWnd.ShowHideTingCardButton(1);
		}
		else
		{
			m_GameClientView.m_ControlWnd.ShowHideTingCardButton(0);
		}
	}
	else
	{
		m_GameClientView.m_ControlWnd.ShowHideTingCardButton(0);
	}
*/
	return true;
}
LRESULT CGameClientDlg::OnShowSZ(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_nDiceCount1 = cbSiceFirst;
	m_GameClientView.m_nDiceCount2 = cbSiceSecond;

	//strTemp.Format("m_nDiceCount1  =%d, m_GameClientView.m_nDiceCount2", m_GameClientView.m_nDiceCount1,m_GameClientView.m_nDiceCount2);
	//theApp.WriteLog(strFile, strTemp);
	//环境处理
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	m_GameClientView.m_tDice = 1;
	AfxBeginThread( MyThreadProc, this);

	return 0;
}
//听牌按钮
LRESULT CGameClientDlg::OnTingCard(WPARAM wParam, LPARAM lParam)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnTingCard.log",strTime);

	strTemp.Format("into OnTingCard");
	theApp.WriteLog(strFile, strTemp);

	WORD wMeChairID=GetMeChairID();

	strTemp.Format("wMeChairID=%d", wMeChairID);
	theApp.WriteLog(strFile, strTemp);
	//分析是否可以听牌
	if (m_GameLogic.IsCanTingCard(m_cbCardIndex, m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID]) )
	{

		m_GameClientView.m_btTingCard.ShowWindow(SW_HIDE);
		//发送听牌网络消息

	}
	else
	{
		AfxMessageBox("你还不能听牌！");
	}
	return 0;
}
//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//环境设置
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);

	//设置界面
	m_GameClientView.SetHuangZhuang(false);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);

	//麻将设置
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//麻将设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	//堆立麻将
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
		m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	}

	//状态变量
	m_bHearStatus=false;

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合麻将
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//堆立麻将
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//麻将变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//出牌操作
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnOutCard.log",strTime);

			strTemp.Format("OnOutCard ");
			theApp.WriteLog(strFile, strTemp);

	//出牌判断
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return 0;

	const tagUserData * pUserData=GetUserData(m_wCurrentUser);

	//听牌判断
	//如果已经听牌了,不能换张
	if ((m_bHearStatus==true)&&(m_GameClientView.m_HandCardControl.GetCurrentCard()!=(BYTE)wParam)) return 0;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	BYTE cbOutCardData=(BYTE)wParam;
	m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData);

	//设置麻将
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
	m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);

	//设置界面
	KillGameTimer(IDI_OPERATE_CARD);
	m_GameClientView.UpdateGameView(NULL);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(2,cbOutCardData);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

			strTemp.Format("m_wCurrentUser=%d ",m_wCurrentUser);
			theApp.WriteLog(strFile, strTemp);



			strTemp.Format("pUserData->cbGender=%d ", pUserData->cbGender);
			theApp.WriteLog(strFile, strTemp);

	//播放声音
	PlayCardSound(cbOutCardData, pUserData->cbGender);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCardData;
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

	return 0;
}

//麻将操作
LRESULT CGameClientDlg::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbOperateCode=(BYTE)(wParam);
	BYTE cbOperateCard=(BYTE)(LOWORD(lParam));

	//状态判断
	if ((m_wCurrentUser==GetMeChairID())&&(cbOperateCode==WIK_NULL))
	{
		m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
		return 0;
	};

	//环境设置
	KillGameTimer(IDI_OPERATE_CARD);
	m_GameClientView.SetStatusFlag(false,true);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=cbOperateCard;
	SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

	return 0;
}



//显示用户MJ
void CGameClientDlg::ShowUserMJ()
{
	m_GameClientView.m_tDice ++;
	m_GameClientView.UpdateGameView(NULL);
	if (1 == showNext)
	{
		showNext++;

		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

		isStartInt = 1;
		m_GameClientView.m_tDice = 1;
		m_GameClientView.m_nDiceCount1 = cbSice2First;
		m_GameClientView.m_nDiceCount2 = cbSice2Second;
		AfxBeginThread( MyThreadProc, this);

		return ;
	}
	if( isStartInt == 1)
	{
		OnSubGameStart(&myGameStart ,sizeof(myGameStart));
		isStartInt = 0;
	}

	
}
//////////////////////////////////////////////////////////////////////////
//调整视频窗口
void CGameClientDlg::ResetVideoWindowPostion(int wInt, int hInt)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sResetVideoWindowPostion.log",strTime);

	strTemp.Format("%s", "ResetVideoWindowPostion()");
	WriteLog(strFile, strTemp);

	uiShowInt++;
//设置自己的视频显示窗口
	int myX,myY,myW,myH;
	myW= MYSLEF_VIDEO_W;
	myH= MYSLEF_VIDEO_H;
	myX= wInt-myW-10;//
	myY= hInt-myH-10; 
	m_showSelfVideo.MoveWindow( myX, myY, myW, myH);
	if ( 1== uiShowInt)
	m_showSelfVideo.ShowWindow(true);

	//自己工具栏
	if ( 1== uiShowInt)
	m_MyselfOpKuang.ShowWindow(true);
	m_MyselfOpKuang.MoveWindow(myX, myY+myH+15, 80, 20);
	//动态计算X,Y
	int x,y,w,h;
	int xx,yy,ww,hh;

	int xOff = wInt;
	int yOff = 25;//m_GameClientView.yOffInt;


	strTemp.Format("xOff = %d", xOff);
	WriteLog(strFile, strTemp);

	CString showMsg;
	showMsg.Format("xOff=%d,yOff=%d", xOff, yOff);

	int screenInt = GetSystemMetrics(SM_CXSCREEN);
	ww= AV_TOOLS_ALL_BUTTON_W  ;
	hh= AV_TOOLS_BUTTON_H  ;

	int opW,opH;
	opW = AV_TOOLS_W  ;
	opH = AV_TOOLS_H + 8 ;


	w= VIDEO_W;
	h= VIDEO_H;

	int topX = 8;
	int topY = 34+ yOff;

	x = topX ;//+ xOff;
	y = topY+VIDEO_PIC_H/3;



	int rightX= xOff - (VIDEO_PIC_W +OP_BACK_PIC_W ) ;//577+xOff;

	if ( 800 == screenInt)
	{
		y = 70;
	}
	else if (1152 == screenInt)
	{
		y = 210;
	}


	int myDeskInt = GetMeChairID();

	if ( myDeskInt > GAME_PLAYER)
	{
		strTemp.Format("myDeskInt=%d",myDeskInt);
		WriteLog(strFile, strTemp);
		m_GameClientView.xOffInt = -1;
		m_GameClientView.yOffInt = -1;
		uiShowInt=0;
		return ;
	}
	else
	{
		strTemp.Format("myDeskInt=%d",myDeskInt);
		WriteLog(strFile, strTemp);
	}

	CStatic * myVideoList[GAME_PLAYER] = {&m_showRemoteVideo, &m_showRemoteVideo1, &m_showRemoteVideo2, &m_showRemoteVideo3};
	CLabelEx * myToolsList[GAME_PLAYER]={&m_OpKuang, &m_OpKuang1, &m_OpKuang2, &m_OpKuang3};
	for (int i=0; i < GAME_PLAYER; i++)
	{
		if ( i == myDeskInt)
		{
			m_GameClientView.m_uVideoInt[i] = 0;
			myVideoList[i]->ShowWindow(false);
			myToolsList[i]->ShowWindow(false);
		}
		else
		{
			m_GameClientView.m_uVideoInt[i] = 1;
			if ( 1== uiShowInt)
			{
			myVideoList[i]->ShowWindow(true);
			myToolsList[i]->ShowWindow(true);
			}

		}//End if

	}	//End for

//top right left
CStatic * myVideo[GAME_PLAYER-1];
CLabelEx * myTools[GAME_PLAYER-1];
CPoint * myPt[GAME_PLAYER-1];
int listInt [3]={1, 2, 3};

if ( myDeskInt > (GAME_PLAYER-1) || myDeskInt < 0)
return ;
switch(myDeskInt)
{
case 0:
	{
//		int listInt [3] = {1, 2, 3};

	}
	break;
case 1:
	{
//0,2,3


listInt[0]=0;
listInt[1]=2;
listInt[2]=3;

	}
	break;
case 2:
	{
//0,1,3

	listInt[0]=0;
listInt[1]=1;
listInt[2]=3;

	}
	break;
case 3:
	{
		//0,1,2
	listInt[0]=0;
listInt[1]=1;
listInt[2]=2;

	}
	break;
}

//top right left
for (int i=0; i < GAME_PLAYER-1; i++)
{
myVideo[i]=myVideoList[listInt[i]];
myTools[i]=myToolsList[listInt[i]];
myPt[i]=&m_GameClientView.m_PtVideo[listInt[i]];

	if ( i==1 )
	{	
		x = rightX;
		y = topY;
	}
	else if(i == 2)
	{
		x = topX;
		y = hInt/2;
	}

	myVideo[i]->MoveWindow( x, y, w, h);
	myPt[i]->x =x-VIDEO_X_OFF;
	myPt[i]->y =y-VIDEO_Y_OFF;

	xx = x+opW;
	yy = y+opH;
	myTools[i]->MoveWindow(xx, yy, ww, hh);
}//End for


m_GameClientView.UpdateGameView(NULL);

	strTemp.Format("UpdateGameView");
	WriteLog(strFile, strTemp);
}


	//RESET UI
LRESULT CGameClientDlg::OnResetUI(WPARAM wParam, LPARAM lParam)
{
	int wInt=(int)wParam;
	int hInt=(int)lParam;

	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnResetUI.log",strTime);

	strTemp.Format("into OnResetUI(%d, %d", wInt, hInt);
	WriteLog(strFile, strTemp);

	if ( wInt >0 && wInt < 1500)
	{
		ResetVideoWindowPostion( wInt,  hInt);
	}
	return 0;
}
