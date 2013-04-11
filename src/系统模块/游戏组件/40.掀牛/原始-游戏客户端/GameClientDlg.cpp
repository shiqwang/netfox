#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_MOST_CARD					201								//最大定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_LAND_SCORE					203								//叫分定时器
#define IDI_KOU_CARD					204								//扣牌定时器
#define IDI_XIAN_CARD					205								//掀牌定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_COUPAI,OnKouPai)
	ON_MESSAGE(IDM_XIANPAI,OnXianPai)
	ON_MESSAGE(IDM_QIANGCI,OnQiangCi)
	ON_MESSAGE(IDM_GIVEUP_QIANGCI,OnGiveUpQiangCi)
	ON_MESSAGE(IDM_AUTO_OUTCARD,OnAutoOutCard)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_FORETURN_CARD,OnForeTurn)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_bHandCardCount=0;
	m_wBanker=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		memset(m_bZhanPaiData[i],0,sizeof(m_bZhanPaiData[i]));
		ZeroMemory(m_bForeCard[i],sizeof(BYTE)*16);
		ZeroMemory(m_bForCardItem[i],sizeof(tagCardItem)*16);
	}

	//配置变量
	m_bDeasilOrder=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	ZeroMemory(m_lForeScore,sizeof(m_lForeScore));
	ZeroMemory(m_lAllScore,sizeof(m_lAllScore));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;

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
	SetWindowText(TEXT("掀牛游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	m_GameClientView.SetUserOrder(m_bDeasilOrder);
	m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,20);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_bHandCardCount=0;
	m_wBanker=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		memset(m_bZhanPaiData[i],0,sizeof(m_bZhanPaiData[i]));
		ZeroMemory(m_bForeCard[i],sizeof(BYTE)*16);
		ZeroMemory(m_bForCardItem[i],sizeof(tagCardItem)*16);
	}

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;

	//删除定时
	KillGameTimer(0);
	KillTimer(IDI_MOST_CARD);

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//获取参数
		m_bDeasilOrder=GameOption.m_bDeasilOrder;
		m_dwCardHSpace=GameOption.m_dwCardHSpace;

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);
		m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,16);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_OUT_CARD:			//用户出牌
		{
			//超时判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
				{
					OnOutCard(0,0);
					AutomatismOutCard();
				}
				return false;
			}
			m_GameClientView.SetTimeFlag(30,nElapse);

			//播放声音
			if (m_bHandCardCount<m_bTurnCardCount) return true;
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				//if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnStart(0,0);
				return false;
			}
			m_GameClientView.SetTimeFlag(30,nElapse);
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_LAND_SCORE:		//挖坑叫分
		{
			if (nElapse==0)
			{
				//if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnKouPai(255,255);
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	
	case IDI_KOU_CARD:
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnKouPai(255,255);
				return false;
			}
			m_GameClientView.SetTimeFlag(10,nElapse);
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

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
	switch (wSubCmdID)
	{
	//case SUB_S_SEND_CARD:		//发送扑克
	//	{
	//		return OnSubSendCard(pBuffer,wDataSize);
	//	}
	case SUB_S_USER_COUPAI:	//用户扣牌
		{
			return OnSubKouPai(pBuffer,wDataSize);
		}
	case SUB_S_USER_XIANPAI:	//用户掀牌
		{
			return OnSubXianPai(pBuffer,wDataSize);
		}
	case SUB_S_USER_QIANGCI:	//用户抢刺
		{
			return OnSubQiangCi(pBuffer,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_PASS_CARD:		//放弃出牌
		{
			return OnSubPassCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_WK_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置界面
			m_GameClientView.SetBaseScore(pStatusFree->lBaseScore);

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();
			}

			//设置扑克
			if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bHandCardCount=pStatusPlay->bCardCount[GetMeChairID()];
			m_bTurnOutType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);
			CopyMemory(m_bHandCardData,pStatusPlay->bCardData,m_bHandCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
			}
			m_GameClientView.SetBaseScore(pStatusPlay->lBaseScore);
			m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
				m_GameClientView.m_btAutoOutCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
			}

			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
				m_GameClientView.m_UserCardControl[wViewChairID].SetDisplayFlag(true);
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,30);

			//设置扑克
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
			}

			return true;
		}
	}

	return false;
}

////发送扑克
//bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
//{
//	//变量定义
//	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
//	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);
//
//	//效验数据
//	ASSERT(wDataSize>=wHeadSize);
//	if (wDataSize<wHeadSize) return false;
//	ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
//	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;
//
//	//删除定时器
//	KillTimer(IDI_MOST_CARD);
//	KillGameTimer(IDI_OUT_CARD);
//
//	//界面设置
//	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
//	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
//	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
//
//	//出牌设置
//	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
//	{
//		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->bCardData,pOutCard->bCardCount);
//	}
//
//	//清理桌面
//	if (m_bTurnCardCount==0)
//	{
//		for (WORD i=0;i<GAME_PLAYER;i++) 
//		{
//			if (i==pOutCard->wOutCardUser) continue;
//			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
//		}
//		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
//	}
//
//	//记录出牌
//	m_bTurnCardCount=pOutCard->bCardCount;
//	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
//	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);
//
//	////炸弹判断
//	//if ((m_bTurnOutType==CT_BOMB_CARD)||(m_bTurnOutType==CT_MISSILE_CARD))
//	//{
//	//	m_wBombTime*=2;
//	//	m_GameClientView.SetBombTime(m_wBombTime);
//	//}
//
//	//自己扑克
//	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
//	{
//		//删除扑克 
//		BYTE bSourceCount=m_bHandCardCount;
//		m_bHandCardCount-=pOutCard->bCardCount;
//		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);
//
//		//设置界面
//		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
//	}
//
//	//最大判断
//	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
//	{
//		//设置变量
//		m_bTurnCardCount=0;
//		m_bTurnOutType=CT_INVALID;
//		m_wMostUser=pOutCard->wCurrentUser;
//		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
//
//		//设置界面
//		for (WORD i=0;i<GAME_PLAYER;i++)
//		{
//			if (i!=pOutCard->wOutCardUser)
//			{
//				WORD wViewChairID=SwitchViewChairID(i);
//				m_GameClientView.SetPassFlag(wViewChairID,true);
//				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
//			}
//		}
//
//		//播放声音
//		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
//
//		//设置定时器
//		SetTimer(IDI_MOST_CARD,3000,NULL);
//
//		return true;
//	}
//
//	//播放声音
//	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser)) PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
//
//	////玩家设置
//	//if (pOutCard->wCurrentUser!=INVALID_CHAIR)
//	//{
//	//	WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
//	//	m_GameClientView.SetPassFlag(wViewChairID,false);
//	//	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
//	//}
//
//	//玩家设置
//	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
//	{
//		ActiveGameFrame();
//		m_GameClientView.m_btPassCard.EnableWindow(TRUE);
//		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
//		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
//		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
//		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
//		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
//	}
//
//	//设置时间
//	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
//	{
//		BYTE bCardCount=m_bCardCount[pOutCard->wCurrentUser];
//		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
//	}
//
//	return true;
//}

////用户叫分
//bool CGameClientDlg::OnSubLandScore(const void * pBuffer, WORD wDataSize)
//{
//	//效验数据
//	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
//	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;
//
//	//消息处理
//	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pBuffer;
//
//	//设置界面
//	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
//	m_GameClientView.SetLandScore(wViewChairID,pLandScore->bLandScore);
//
//	//玩家设置
//	if ((IsLookonMode()==false)&&(pLandScore->wCurrentUser==GetMeChairID()))
//	{
//		ActiveGameFrame();
//		m_GameClientView.m_btGiveUpCiangCi.ShowWindow(SW_SHOW);
//		m_GameClientView.m_btKouPai.ShowWindow(pLandScore->bCurrentScore<=0?SW_SHOW:SW_HIDE);
//		m_GameClientView.m_btXianPai.ShowWindow(pLandScore->bCurrentScore<=1?SW_SHOW:SW_HIDE);
//		m_GameClientView.m_btQiangCi.ShowWindow(pLandScore->bCurrentScore<=2?SW_SHOW:SW_HIDE);
//	}
//
//	//播放声音
//	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
//
//	//设置时间
//	SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,30);
//
//	return true;
//}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	/*m_wBanker=pGameStart->wLandUser;*/
	m_bCardCount[pGameStart->wBankUser]=16;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
	for(BYTE i=0;i<GAME_PLAYER;i++)
		memset(m_bZhanPaiData[i],0,sizeof(m_bZhanPaiData[i]));

	

	//	//效验数据
	//ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	//if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	//CMD_S_GameStart * pGameStart=(CMD_S_GameStart*)pBuffer;

	//设置数据
	m_bHandCardCount=CountArray(pGameStart->bCardData);
	CopyMemory(m_bHandCardData,pGameStart->bCardData,sizeof(pGameStart->bCardData));
	for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=CountArray(pGameStart->bCardData);
	//设置手中扑克
	m_GameClientView.m_HandCardControl.SetCardData(pGameStart->bCardData,16);
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(false);


	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.SetCardCount(i,m_bCardCount[i]);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.m_LeaveCardControl[0].SetCardData(NULL,0);
		m_GameClientView.m_LeaveCardControl[1].SetCardData(NULL,0);
	}

		//设置按钮
	if (IsLookonMode()==false)
	{
		ActiveGameFrame();
		m_GameClientView.m_btKouPai.ShowWindow(SW_SHOW);
		m_GameClientView.m_btKouPai.EnableWindow(false);
		m_GameClientView.m_btXianPai.ShowWindow(SW_SHOW);
		m_GameClientView.m_btXianPai.EnableWindow(false);
		m_GameClientView.m_btQiangCi.ShowWindow(SW_SHOW);
		m_GameClientView.m_btQiangCi.EnableWindow(false);
		m_GameClientView.m_btGiveUpCiangCi.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUpCiangCi.EnableWindow(false);
		m_GameClientView.m_btForeTurn.ShowWindow(SW_SHOW);
		m_GameClientView.m_btForeTurn.EnableWindow(false);
	}
	//抢刺设置
	if(IsLookonMode()==false&&GetMeChairID()!=pGameStart->wBankUser)
	{
		if(m_GameLogic.GetCardType(m_bHandCardData,m_bHandCardCount)==CT_DROGONANDPHONIX
			||m_GameLogic.GetCardType(m_bHandCardData,m_bHandCardCount)==CT_EIGHTDOUBLE)
		{
			m_GameClientView.m_btQiangCi.EnableWindow(true);
			m_GameClientView.m_btGiveUpCiangCi.EnableWindow(true);
			ASSERT(FALSE);

		}
	}



	//设置控件
	//m_GameClientView.ShowLandTitle(false);
	//m_GameClientView.m_BackCardControl.SetCardData(pGameStart->bBackCard,CountArray(pGameStart->bBackCard));

	//设置界面
	//m_GameClientView.SetLandScore(INVALID_CHAIR,0);
	///m_GameClientView.SetCardCount(SwitchViewChairID(pGameStart->wLandUser),m_bCardCount[pGameStart->wLandUser]);

	////地主设置
	//if (pGameStart->wLandUser==GetMeChairID())
	//{
	//	BYTE bCardCound=m_bHandCardCount;
	//	m_bHandCardCount+=CountArray(pGameStart->bBackCard);
	//	CopyMemory(&m_bHandCardData[bCardCound],pGameStart->bBackCard,sizeof(pGameStart->bBackCard));
	//	m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount);
	//	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	//}
	//m_GameClientView.SetLandUser(SwitchViewChairID(pGameStart->wLandUser),pGameStart->bLandScore);

	//玩家设置
	if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetPositively(true);

	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wBankUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btKouPai.EnableWindow(true);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		//m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);
	}


	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//设置时间
	SetGameTimer(pGameStart->wBankUser,IDI_OUT_CARD,30);

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	///变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);

	//效验数据
	ASSERT(wDataSize>=wHeadSize);
	if (wDataSize<wHeadSize) return false;
	ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//界面设置
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
	for(BYTE i=0;i<GAME_PLAYER;i++)
	m_GameClientView.m_ForeTurn[SwitchViewChairID(i)].SetCardData(NULL,0);

	//出牌设置
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->bCardData,pOutCard->bCardCount);
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetDisplayFlag(true);
	}

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//一轮判断
	if (pOutCard->bNewTurn)
	{
		
		for(BYTE i=0,j=0;i<20;i++)
		{
			if(!m_bZhanPaiData[pOutCard->wOutCardUser][i]&&m_bTurnCardData[j]&&j<m_bTurnCardCount)
			{
				m_bZhanPaiData[pOutCard->wOutCardUser][i]=m_bTurnCardData[j];
				j++;
			}

		}
		for(BYTE k=0;k<20;k++)
			if(!m_bZhanPaiData[pOutCard->wOutCardUser][k])
			{
				m_bZhanPaiData[pOutCard->wOutCardUser][k]=0x41;
				break;
			}
		//设置占牌数目
		m_GameClientView.IncreaseZhanPaiCount(wOutViewChairID,m_bTurnCardCount);
		ASSERT(k<=16);
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_INVALID;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	}


	//清理桌面
	if (pOutCard->bNewTurn)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			ZeroMemory(m_bForCardItem[i],sizeof(tagCardItem)*16);
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].GetCardData(m_bForCardItem[i],m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].GetCardCount());
			//if (i==pOutCard->wOutCardUser&&(!pOutCard->bNewTurn)) continue;
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
			//m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetDisplayFlag(false);
			m_GameClientView.m_btForeTurn.EnableWindow(TRUE);
		}
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}
	//设置占牌
	if(pOutCard->bNewTurn)
	{
		for(BYTE i=0;i<20;i++)
			if(!m_bZhanPaiData[pOutCard->wOutCardUser][i])
				break;
				
		WORD wViewUser=SwitchViewChairID(pOutCard->wOutCardUser);
		m_GameClientView.m_ZhanCardControl[wViewUser].SetCardData(m_bZhanPaiData[pOutCard->wOutCardUser],i);
	}
	//设置按钮
	if(pOutCard->bNewTurn)
		if(pOutCard->wCurrentUser==GetMeChairID())
				m_GameClientView.m_btKouPai.EnableWindow(true);



	//最大判断
	if (pOutCard->bMostCard)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_INVALID;
		m_wMostUser=pOutCard->wCurrentUser;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetPassFlag(wViewChairID,true);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
				//m_GameClientView.m_UserCardControl[wViewChairID].SetDisplayFlag(false);
			}
		}
		//设置按钮
			m_GameClientView.m_btKouPai.EnableWindow(true);

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	//播放声音
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser)) PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//玩家设置
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		//m_GameClientView.m_UserCardControl[wViewChairID].SetDisplayFlag(false);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btAutoOutCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		AutoOutCard(0);
	}

	//设置时间
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		BYTE bCardCount=m_bCardCount[pOutCard->wCurrentUser];
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
	}
	

	return true;
}

//放弃出牌
bool CGameClientDlg::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);

	//玩家设置
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))
	{
		WORD wViewChairID=SwitchViewChairID(pPassCard->wPassUser);
		m_GameClientView.SetPassFlag(wViewChairID,true);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		//m_GameClientView.m_UserCardControl[wViewChairID].SetDisplayFlag(false);
	}

	//一轮判断
	if (pPassCard->bNewTurn)
	{
		
		for(BYTE i=0,j=0;i<20;i++)
		{
			if(!m_bZhanPaiData[pPassCard->bZhanpaiUser][i]&&m_bTurnCardData[j]&&j<m_bTurnCardCount)
			{
				m_bZhanPaiData[pPassCard->bZhanpaiUser][i]=m_bTurnCardData[j];
				j++;
			}

		}
		for(BYTE k=0;k<20;k++)
			if(!m_bZhanPaiData[pPassCard->bZhanpaiUser][k])
			{
				m_bZhanPaiData[pPassCard->bZhanpaiUser][k]=0x41;
				break;
			}
		//设置占牌数目
			m_GameClientView.IncreaseZhanPaiCount(SwitchViewChairID(pPassCard->bZhanpaiUser),m_bTurnCardCount);
		ASSERT(k<=16);
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_INVALID;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	}
	if (pPassCard->bNewTurn)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			ZeroMemory(m_bForCardItem[i],sizeof(tagCardItem)*16);
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].GetCardData(m_bForCardItem[i],m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].GetCardCount());
			//if (i==pPassCard->wOutCardUser) continue;
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
			//m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetDisplayFlag(false);
		}
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
		m_GameClientView.m_btForeTurn.EnableWindow(TRUE);
	}
	//设置按钮
	if(pPassCard->bNewTurn&&pPassCard->bZhanpaiUser==GetMeChairID())
		m_GameClientView.m_btKouPai.EnableWindow(true);

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pPassCard->wCurrentUser);
	m_GameClientView.SetPassFlag(wViewChairID,false);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	//m_GameClientView.m_UserCardControl[wViewChairID].SetDisplayFlag(false);

	//玩家设置
	if ((IsLookonMode()==false)&&(pPassCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btAutoOutCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		AutoOutCard(0);


	}

	//设置占牌
	if(pPassCard->bNewTurn)
	{
		for(BYTE i=0;i<20;i++)
			if(!m_bZhanPaiData[pPassCard->bZhanpaiUser][i])
				break;
				
		WORD wViewUser=SwitchViewChairID(pPassCard->bZhanpaiUser);
		m_GameClientView.m_ZhanCardControl[wViewUser].SetCardData(m_bZhanPaiData[pPassCard->bZhanpaiUser],i);
	}


	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//设置时间
	if (m_bTurnCardCount)
	{
		BYTE bCardCount=m_bCardCount[pPassCard->wCurrentUser];
		SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
	}
	else SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,30);

	return true;
}
//用户扣牌
bool CGameClientDlg::OnSubKouPai(const void * pBuffer, WORD wDataSize)
{
	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_User_CouPai));
	if (wDataSize!=sizeof(CMD_S_User_CouPai)) return false;
	//消息处理
	CMD_S_User_CouPai * pCouPai=(CMD_S_User_CouPai *)pBuffer;
	m_GameClientView.ShowKouPaiTitle(SW_NORMAL,SwitchViewChairID(pCouPai->wCouPaiUserID));
	if(pCouPai->wCurrentUser==GetMeChairID())
	{
		ActiveGameFrame();
		m_GameClientView.m_btKouPai.EnableWindow(true);
		m_GameClientView.m_btXianPai.EnableWindow(true);
	}
	//设置定时器
	KillGameTimer(IDI_OUT_CARD);
	SetGameTimer(pCouPai->wCurrentUser,IDI_KOU_CARD,10);


	return true;
}
//用户掀牌
bool CGameClientDlg::OnSubXianPai(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_User_XianPai));
	if (wDataSize!=sizeof(CMD_S_User_XianPai)) return false;
	//消息处理
	CMD_S_User_XianPai * pXianPai=(CMD_S_User_XianPai *)pBuffer;
	m_GameClientView.ShowXianPaiTitle(SW_NORMAL,SwitchViewChairID(pXianPai->wXianPaiUserID));
	m_GameClientView.HideAllTitle(false,INVALID_CHAIR);
	//设置界面
	m_GameClientView.SetIncreaseXian(SwitchViewChairID(pXianPai->wXianPaiUserID));

	if(pXianPai->wCurrentUser==GetMeChairID())
	{
		ActiveGameFrame();
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_NORMAL);
		m_GameClientView.m_btOutCard.ShowWindow(SW_NORMAL);
		//设置被掀次数
		m_GameClientView.SetIncreaseBeiXian(SwitchViewChairID(GetMeChairID()));
	}
	//设置定时器
	KillGameTimer(IDI_OUT_CARD);
	SetGameTimer(pXianPai->wCurrentUser,IDI_OUT_CARD,30);

	return true;
}
//用户抢刺
bool CGameClientDlg::OnSubQiangCi(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_User_QiangCi));
	if (wDataSize!=sizeof(CMD_S_User_QiangCi)) return false;
	//消息处理
	CMD_S_User_QiangCi * pQiangCi=(CMD_S_User_QiangCi *)pBuffer;
	m_GameClientView.ShowQiangCiTitle(SW_NORMAL,SwitchViewChairID(pQiangCi->wQiangCiUserID));
	//设置界面
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	if(pQiangCi->wQiangCiUserID==GetMeChairID())
	{
		m_GameClientView.m_btOutCard.ShowWindow(SW_NORMAL);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_NORMAL);

	}

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

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);
	KillGameTimer(IDI_LAND_SCORE);

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btKouPai.ShowWindow(SW_HIDE);
	m_GameClientView.m_btXianPai.ShowWindow(SW_HIDE);
	m_GameClientView.m_btQiangCi.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpCiangCi.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
		m_lForeScore[i]=pGameEnd->lGameScore[i];
		m_lAllScore[i]+=pGameEnd->lGameScore[i];
		//设置界面
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.SetForeScore(wViewChairID,m_lForeScore[i]);
		m_GameClientView.SetAllScore(wViewChairID,m_lAllScore[i]);
	}
	m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置扑克
	BYTE bCardPos=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if (wViewChairID==0) m_GameClientView.m_LeaveCardControl[0].SetCardData(&pGameEnd->bCardData[bCardPos],pGameEnd->bCardCount[i]);
		else if (wViewChairID==2) m_GameClientView.m_LeaveCardControl[1].SetCardData(&pGameEnd->bCardData[bCardPos],pGameEnd->bCardCount[i]);
		bCardPos+=pGameEnd->bCardCount[i];
		if (pGameEnd->bCardCount[i]!=0)
		{
			m_GameClientView.SetPassFlag(wViewChairID,false);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}
	}

	//显示扑克
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		//SetGameTimer(GetMeChairID(),IDI_START_GAME,90);
	}
	

	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[16];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(bCardData,CountArray(bCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		

		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);	
			//类型判断
		if (bCardType==CT_INVALID) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(bCardData,m_bTurnCardData,bShootCount,m_bTurnCardCount);
	}

	return false;
}

//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	//先出牌者
	if (m_bTurnCardCount==0)
	{
		//控制界面
		KillGameTimer(IDI_OUT_CARD);
		m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=m_bHandCardData[m_bHandCardCount-1];
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);
		m_GameClientView.m_UserCardControl[1].SetDisplayFlag(true);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	}
	else OnPassCard(0,0);

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	if ((nIDEvent==IDI_MOST_CARD)&&(m_wMostUser!=INVALID_CHAIR))
	{
		//变量定义
		WORD wCurrentUser=m_wMostUser;
		m_wMostUser=INVALID_CHAIR;

		//删除定时器
		KillTimer(IDI_MOST_CARD);

		//设置界面
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
		for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

		//玩家设置
		if ((IsLookonMode()==false)&&(wCurrentUser==GetMeChairID()))
		{
			ActiveGameFrame();
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			//m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.EnableWindow(FALSE);
			m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		}

		//设置时间
		SetGameTimer(wCurrentUser,IDI_OUT_CARD,30);

		return;
	}

//Doctor	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;
	m_bHandCardCount=0;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.SetBaseScore(0L);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	//清理界面
	m_GameClientView.ClearPlayerOneGame();
	//清理数据
	ClearOneTime();

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置扑克
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	m_GameClientView.m_LeaveCardControl[0].SetCardData(NULL,0);
	m_GameClientView.m_LeaveCardControl[1].SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btKouPai.EnableWindow(false);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

	//发送数据
	CMD_C_OutCard OutCard;
	ZeroMemory(&OutCard,sizeof(CMD_C_OutCard));
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));
	//SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);
	m_GameClientView.m_UserCardControl[1].SetDisplayFlag(true);
	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//放弃出牌
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.SetPassFlag(1,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//扣牌消息
LRESULT CGameClientDlg::OnKouPai(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btKouPai.EnableWindow(false);
	m_GameClientView.m_btXianPai.EnableWindow(SW_HIDE);
	m_GameClientView.m_btQiangCi.EnableWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpCiangCi.EnableWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);

	////发送数据
	//CMD_C_CouPai CouPai;
	/*LandScore.bLandScore=(BYTE)wParam;*/
	//发送消息
	SendData(SUB_C_COUPAI);

	return 0;
}
	//掀牌消息
LRESULT CGameClientDlg::OnXianPai(WPARAM wParam,LPARAM lParam)
{
	//设置界面
	//KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btKouPai.EnableWindow(false);
	m_GameClientView.m_btXianPai.EnableWindow(SW_HIDE);
	m_GameClientView.m_btQiangCi.EnableWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpCiangCi.EnableWindow(SW_HIDE);
	//发送消息
	SendData(SUB_C_XIANPAI);
	return 0;
}
	//抢刺消息
LRESULT CGameClientDlg::OnQiangCi(WPARAM wParam,LPARAM lParam)
{
	//设置界面
	//KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btKouPai.EnableWindow(false);
	m_GameClientView.m_btXianPai.EnableWindow(SW_HIDE);
	m_GameClientView.m_btQiangCi.EnableWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpCiangCi.EnableWindow(SW_HIDE);
	//发送消息
	SendData(SUB_C_QIANGCI);
	return 0;
}
	//放弃抢刺
LRESULT CGameClientDlg::OnGiveUpQiangCi(WPARAM wParam,LPARAM lParam)
{
	//设置界面
	//KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btKouPai.EnableWindow(false);
	m_GameClientView.m_btXianPai.EnableWindow(SW_HIDE);
	m_GameClientView.m_btQiangCi.EnableWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpCiangCi.EnableWindow(SW_HIDE);
	//发送消息
	SendData(SUB_C_GIVEUP_QIANGCI);
	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
{
	AutoOutCard(0);
	return 0;
}

//右键扑克
LRESULT CGameClientDlg::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);

	return 0;
}

//左键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);

	return 0;
}
//上一轮提示
LRESULT CGameClientDlg::OnForeTurn(WPARAM wParam,LPARAM lParam)
{
	
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		for(BYTE j=0;j<16&&m_bForCardItem[i][j].bCardData;j++);
		m_GameClientView.m_ForeTurn[SwitchViewChairID(i)].SetCardItem(m_bForCardItem[i],j);
		m_GameClientView.m_ForeTurn[SwitchViewChairID(i)].SetDisplayFlag(TRUE);
	}
	return 0;
}

void CGameClientDlg::AutoOutCard(BYTE m_WhichOnsKindCard)
{
	int i=0;
	BYTE m_bWhichKindSel=0;
	BYTE							m_bTempSCardCount=0;				//扑克数目
	BYTE							m_bTempSCardData[16];				//手上扑克
	BYTE							m_bTempDCardCount=0;				//扑克数目
	BYTE							m_bTempDCardData[16];				//手上扑克
	BYTE							m_bTempTCardCount=0;				//扑克数目
	BYTE							m_bTempTCardData[16];				//手上扑克
	BYTE							m_bTempFCardCount=0;				//扑克数目
	BYTE							m_bTempFCardData[16];				//手上扑克
	BYTE							m_bTempGetCardCount=0;				//扑克数目
	BYTE							m_bTempBaiCardData[16];				//手上扑克
	BYTE							m_bTempBaiCardCount=0;				//扑克数目
	BYTE							m_bTempGetCardData[16];				//手上扑克
	BYTE							m_bTempGetCardIndex[16];			//手上扑克
	BYTE m_TempCard=0;
	//如果没有人出牌，不提示
	if(m_bTurnCardCount==0)
		return;
	m_GameClientView.m_HandCardControl.ShootAllCard(false);
	//获取单牌列表
	for(i=0;i<m_bHandCardCount;i++)
	{	
		BYTE m_GetCard=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		//if(m_TempCard!=m_GetCard)
		//{
			m_bTempSCardData[m_bTempSCardCount++]=m_bHandCardData[i];
			m_TempCard=m_GetCard;
		//}
	}
	//获取对牌列表
	m_TempCard=0;
	for(i=0;i<m_bHandCardCount-1;i++)
	{	
		BYTE m_GetCard1=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE m_GetCard2=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+1]);
		if(m_TempCard!=m_GetCard1&&m_GetCard1==m_GetCard2&&m_GetCard1<16)
		{
			m_bTempDCardData[m_bTempDCardCount++]=m_bHandCardData[i];
			m_bTempDCardData[m_bTempDCardCount++]=m_bHandCardData[i+1];
			m_TempCard=m_GetCard1;
		}
	}
	//获取三张牌列表
	m_TempCard=0;
	for(i=0;i<m_bHandCardCount-2;i++)
	{	
		BYTE m_GetCard1=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE m_GetCard2=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+1]);
		BYTE m_GetCard3=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+2]);
		if(m_TempCard!=m_GetCard1&&m_GetCard1==m_GetCard2&&m_GetCard1==m_GetCard3)
		{
			m_bTempTCardData[m_bTempTCardCount++]=m_bHandCardData[i];
			m_bTempTCardData[m_bTempTCardCount++]=m_bHandCardData[i+1];
			m_bTempTCardData[m_bTempTCardCount++]=m_bHandCardData[i+2];
			m_TempCard=m_GetCard1;
		}
	}
	//获取四张牌列表
	m_TempCard=0;
	for(i=0;i<m_bHandCardCount-3;i++)
	{	
		BYTE m_GetCard1=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE m_GetCard2=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+1]);
		BYTE m_GetCard3=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+2]);
		BYTE m_GetCard4=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+3]);
		if(m_TempCard!=m_GetCard1&&m_GetCard1==m_GetCard2&&m_GetCard1==m_GetCard3&&m_GetCard1==m_GetCard4)
		{
			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i];
			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i+1];
			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i+2];
			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i+3];
			m_TempCard=m_GetCard1;
		}
	}
	//获取摆牌列表
	BYTE bBaiTemp[16];
	ZeroMemory(bBaiTemp,sizeof(bBaiTemp));
	CopyMemory(bBaiTemp,m_bHandCardData,sizeof(bBaiTemp));
	for(BYTE m=0;m<m_bHandCardCount-2;m++)
	{
		if(m_GameLogic.GetCardValue(bBaiTemp[m])==0x0D)
			for(BYTE j=m;j<m_bHandCardCount;j++)
				if(m_GameLogic.GetCardValue(bBaiTemp[j])==0x0A)
					for(BYTE k=j;k<m_bHandCardCount;k++)
						if(m_GameLogic.GetCardValue(bBaiTemp[k])==0x08)
						{
							m_bTempBaiCardData[m_bTempBaiCardCount++]=m;
							m_bTempBaiCardData[m_bTempBaiCardCount++]=j;
							m_bTempBaiCardData[m_bTempBaiCardCount++]=k;
							bBaiTemp[m]=0;
							bBaiTemp[j]=0;
							bBaiTemp[k]=0;
							//m_GameLogic.SortCardList(bBaiTemp,m_bHandCardCount);
							//m=0;
							//ASSERT(0);
							break;
						}
		
	}
	
	//根据所出牌类型判断
	i=0;
	switch(m_bTurnOutType)
	{
	case CT_SINGLE:
	/*case CT_ONE_LINE:*/
		if(m_WhichOnsKindCard==1)   //判断是不是具有唯一性
		{
			for(i=m_bTempSCardCount;i>0;i--)
			{
				if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempSCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
				{
					if((m_bWhichKindSel++)>1)
						i=0;
				}
			}
		}
		for(i=m_bTempSCardCount;i>0;i--)
		{
			if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempSCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
			{
				//判断是不是最合理的
				bool m_bIsHaveCard=false;
				for(int j=0;j<m_bTempDCardCount;j++)
				{
					for(int n=0;n<m_bTurnCardCount;n++)
					{
						if(m_GameLogic.GetCardLogicValue(m_bTempSCardData[i-m_bTurnCardCount+n])==m_GameLogic.GetCardLogicValue(m_bTempDCardData[j]))
							m_bIsHaveCard=true;
					}
				}
				//把最合理的情况保存起来
				if(m_bTempGetCardCount==0||!m_bIsHaveCard)
				{
					CopyMemory(m_bTempGetCardData,&m_bTempSCardData[i-m_bTurnCardCount],m_bTurnCardCount);
					m_bTempGetCardCount=m_bTurnCardCount;
				}
				if(!m_bIsHaveCard)
					break;
			}
		}
		break;
	case CT_DOUBLE:
	/*case CT_DOUBLE_LINE:*/
		if(m_WhichOnsKindCard==1)     //判断是不是具有唯一性
		{
			for(i=m_bTempDCardCount;i>0;i--)
			{
				if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempDCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
				{
					if((m_bWhichKindSel++)>1)
						i=0;
				}
			}
		}
		for(i=m_bTempDCardCount;i>0;i--)
		{
			if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempDCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
			{
				//判断是不是最合理的
				bool m_bIsHaveCard=false;
				for(int j=0;j<m_bTempTCardCount;j++)
				{
					for(int n=0;n<m_bTurnCardCount;n++)
					{
						if(m_GameLogic.GetCardLogicValue(m_bTempDCardData[i-m_bTurnCardCount+n])==m_GameLogic.GetCardLogicValue(m_bTempTCardData[j]))
							m_bIsHaveCard=true;
					}
				}
				if(m_bTempGetCardCount==0||!m_bIsHaveCard)
				{
					CopyMemory(m_bTempGetCardData,&m_bTempDCardData[i-m_bTurnCardCount],m_bTurnCardCount);
					m_bTempGetCardCount=m_bTurnCardCount;
				}
				if(!m_bIsHaveCard)
					break;
			}
		}
		break;
	case CT_FISH:
	/*case CT_THREE_LINE:*/
		if(m_bTempBaiCardCount)
	{
		m_GameClientView.m_HandCardControl.SetShootCard(m_bTempBaiCardData,3);
		m_GameClientView.m_btOutCard.EnableWindow(TRUE);
		return ;
	}

		break;
		case CT_THREE:
	/*case CT_THREE_LINE:*/
		if(m_WhichOnsKindCard==1)           //判断是不是具有唯一性
		{
			for(i=m_bTempTCardCount;i>0;i--)
			{
				if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempTCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
				{
					if((m_bWhichKindSel++)>1)
						i=0;
				}
			}
		}
		for(i=m_bTempTCardCount;i>0;i--)
		{
			if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempTCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
			{
				//判断是不是最合理的
				bool m_bIsHaveCard=false;
				for(int j=0;j<m_bTempFCardCount;j++)
				{
					for(int n=0;n<m_bTurnCardCount;n++)
					{
						if(m_GameLogic.GetCardLogicValue(m_bTempTCardData[i-m_bTurnCardCount+n])==m_GameLogic.GetCardLogicValue(m_bTempFCardData[j]))
							m_bIsHaveCard=true;
					}
				}
				if(m_bTempGetCardCount==0||!m_bIsHaveCard)
				{
					CopyMemory(m_bTempGetCardData,&m_bTempTCardData[i-m_bTurnCardCount],m_bTurnCardCount);
					m_bTempGetCardCount=m_bTurnCardCount;
				}
				if(!m_bIsHaveCard&&m_bTempGetCardCount!=0)
					break;
			}
		}
		break;
	
	
	if(m_bTempGetCardCount==0)
	{
		m_bWhichKindSel=0;
		//判断炸弹的可能性
		if(m_bTempFCardCount>3)
		{
			for(i=m_bTempFCardCount-4;i>=0;i--)
			{
				if(m_GameLogic.CompareCard(&m_bTempFCardData[i],m_bTurnCardData,4,m_bTurnCardCount))
				{
					if((m_bWhichKindSel++)==0)
					{
						CopyMemory(m_bTempGetCardData,&m_bTempFCardData[i],4);
						m_bTempGetCardCount=4;
					}
				}
			}
		}
		if(m_bTempGetCardCount==0)
		{
			if(m_bHandCardCount>1)
			{
				if(m_GameLogic.GetCardLogicValue(m_bHandCardData[0])>15&&m_GameLogic.GetCardLogicValue(m_bHandCardData[1])>15)
				{
					CopyMemory(m_bTempGetCardData,m_bHandCardData,2);
					m_bTempGetCardCount=2;
					if(m_WhichOnsKindCard==1)
						m_bWhichKindSel=1;
				}
			}
		}
	}
	}
	BYTE m_GetIndex=0;
	if(m_bTempGetCardCount==0)
	{
		if(m_WhichOnsKindCard!=1)
			OnPassCard(0,0);
	}
	else
	{
		for(int j=0;j<m_bTempGetCardCount;j++)
		{
			for(i=0;i<m_bHandCardCount;i++)
			{
				if(m_bHandCardData[i]==m_bTempGetCardData[j])
				{
					m_bTempGetCardIndex[m_GetIndex++]=i;
				}
			}
		}
		
	}
	if(m_GameLogic.CompareCard(m_bTempGetCardData,m_bTurnCardData,m_bTempGetCardCount,m_bTurnCardCount))
	{
		if(m_WhichOnsKindCard==1&&m_bWhichKindSel==1||m_WhichOnsKindCard!=1)
		{
			m_GameClientView.m_HandCardControl.SetShootCard(m_bTempGetCardIndex,m_GetIndex);
			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
		}
	}
	else
	{
		if(m_WhichOnsKindCard!=1)
			OnPassCard(0,0);
	}
}
//一局要清的东东
void CGameClientDlg::ClearOneTime()
{
	for(BYTE i=0;i<GAME_PLAYER;i++)
		ZeroMemory(m_bForCardItem[i],sizeof(tagCardItem)*16);
}

//////////////////////////////////////////////////////////////////////////

