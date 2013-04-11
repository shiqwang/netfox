#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"


//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME					200								//开始定时器
#define IDI_GIVE_UP						201								//放弃定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)//开始
	ON_MESSAGE(IDM_FOLLOW,OnFollow)//跟
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)//放弃
	ON_MESSAGE(IDM_ADD_GOLD,OnAddGold)//增加
	ON_MESSAGE(IDM_SHOWHAND,OnShowHand)//
	ON_MESSAGE(IDM_KAIPAI,KaiPai)        //开牌
	ON_MESSAGE(IDM_ONFOLLOW,ShowFollow)  //ADD
	ON_MESSAGE(IDM_KANPAI,SetKanPai)     //看牌
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	AddGold=0;  //add
	MyCurrentUser=0;
	KanPai=false;	//add
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	memset(m_GameClientView.mArrayKanPai,0,sizeof(m_GameClientView.mArrayKanPai));                  //add

	memset(m_GameClientView.myKanPaiBool,0,sizeof(m_GameClientView.myKanPaiBool));                 //add
	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	memset(m_szName,0,sizeof(m_szName));

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
	SetWindowText(TEXT("诈金花游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	memset(m_GameClientView.mArrayKanPai,0,sizeof(m_GameClientView.mArrayKanPai));                 //add

	memset(m_GameClientView.myKanPaiBool,0,sizeof(m_GameClientView.myKanPaiBool));                 //add

	
	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	memset(m_szName,0,sizeof(m_szName));
	for (int i=0;i<GAME_PLAYER;i++)m_GameClientView.m_CardControl[i].SetCardData(NULL,0);

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
	case IDI_START_GAME:		//继续游戏定时器
		{
			if (nElapse==0)
			{
				OnStart(0,0);
				return false;
			}
			if (nElapse<=10) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_GIVE_UP:			//放弃定时器
		{
			WORD wViewChairID=SwitchViewChairID(wChairID);
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wViewChairID==3)) OnGiveUp(1,1);            //been wViewChairID==2 
				return false;
			}
			if ((nElapse<=10)&&(wViewChairID==2)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

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
		//	theApp.WriteLog(strFile, strTemp);
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_ADD_GOLD:	//用户下注
		{
			return OnSubAddGold(pBuffer,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			return OnSubGiveUp(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_KANPAI:     //看牌
		{
			strTemp.Format("SUB_S_KANPAI 看牌");
			theApp.WriteLog(strFile, strTemp);

			return OnSubKanPai(pBuffer, wDataSize);
		}
	case SUB_S_OVER:
		{
			return OnSubOver(pBuffer, wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//辅助变量
			m_bShowHand=false;
			m_bMeAddGold=false;

			//设置界面
			m_lGoldShow=0L;
			m_lTurnBasicGold=0L;
			m_GameClientView.SetGoldTitleInfo(0,pStatusFree->dwBasicGold);

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
			}

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_lTurnMaxGold=pStatusPlay->lTurnMaxGold;
			m_lTurnBasicGold=pStatusPlay->lTurnBasicGold;
			CopyMemory(m_bPlayStatus,pStatusPlay->bPlayStatus,sizeof(m_bPlayStatus));
			//CopyMemory(m_GameClientView.mArrayKanPai,pStatusPlay->bKanPai,sizeof(m_GameClientView.mArrayKanPai));							//add

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewStation=SwitchViewChairID(i);
				const tagUserData * pUserData=GetUserData(i);
				if (pUserData!=NULL)
				{
					lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
					//test
					//if (m_bPlayStatus[i]==TRUE) m_GameClientView.m_CardControl[wViewStation].SetCardData(pStatusPlay->bTableCardArray[i],pStatusPlay->bTableCardCount[i]);
				}
				m_GameClientView.SetUserGoldInfo(wViewStation,false,pStatusPlay->lTableGold[2*i]);
				m_GameClientView.SetUserGoldInfo(wViewStation,true,pStatusPlay->lTableGold[2*i+1]);

				//判断是否梭哈
				if (i==GetMeChairID())
				{
					m_bShowHand=((pStatusPlay->lMaxGold-pStatusPlay->lTableGold[2*i+1]-pStatusPlay->lTableGold[2*i])<=0L);
				}
			}
			m_GameClientView.SetGoldTitleInfo(pStatusPlay->lMaxGold,pStatusPlay->lBasicGold);

			//判断是否观看
			if ((IsLookonMode()==false)||(IsAllowLookon()==true)) m_GameClientView.m_CardControl[2].AllowPositively(true);

			//判断是否自己下注
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				int j=0;														                   //add
				m_bMeAddGold=true;
				BYTE wCardCount=pStatusPlay->bTableCardCount[GetMeChairID()];
				m_GameClientView.m_btFollow.ShowWindow(SW_SHOW); 
				//m_GameClientView.m_btFollow.EnableWindow(false);                                   //add
				m_GameClientView.m_btKanPai.ShowWindow(SW_SHOW);                                   //add
				
///////////////////////////////////////////////////////////////////////////////// begin add
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if (m_bPlayStatus[i]==TRUE) j++;
		}
		if (j==2) 
		{
            m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW); 
			m_GameClientView.m_btKaiPai.EnableWindow(true);
		}
		else
		{
			m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW);
			m_GameClientView.m_btKaiPai.EnableWindow(FALSE);				
		}
/////////////////////////////////////////////////////////////////////////////////// end add 
				m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAddGold.ShowWindow((m_bShowHand==false)?SW_SHOW:SW_HIDE);
				m_GameClientView.m_btShowHand.ShowWindow(((wCardCount>=4)&&(m_bShowHand==false))?SW_SHOW:SW_HIDE);
			}
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_GIVE_UP,30);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//游戏变量
	m_lTurnMaxGold=pGameStart->lTurnMaxGold;
	m_lTurnBasicGold=pGameStart->lTurnBasicGold;
	memset(m_szName,0,sizeof(m_szName));
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	memset(m_GameClientView.mArrayKanPai,0,sizeof(m_GameClientView.mArrayKanPai));                     //add
	memset(m_GameClientView.myKanPaiBool,0,sizeof(m_GameClientView.myKanPaiBool));                 //add
	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	bool bLookonMode=IsLookonMode();
	LONG lBaseGold=m_GameClientView.m_lBasicGold;

	//设置状态
	SetGameStatus(GS_PLAYING);
	m_GameClientView.SetGoldTitleInfo(pGameStart->lMaxGold,lBaseGold);

	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);                                 //隐藏比赛结束成绩比分视图	
    for (WORD i=0;i<GAME_PLAYER;i++)
	{				  
		const tagUserData * pUserData=GetUserData(i);
		//设置界面
		if (pUserData!=NULL)
		{
			m_bPlayStatus[i]=TRUE;
			m_GameClientView.m_CardControl[SwitchViewChairID(i)].SetCardData(pGameStart->bCardData[i],3);              
			m_GameClientView.SetUserGoldInfo(SwitchViewChairID(i),false,0L);
			m_GameClientView.SetUserGoldInfo(SwitchViewChairID(i),true,lBaseGold);
			lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
		}
		else
		{
			m_bPlayStatus[i]=FALSE;
			m_GameClientView.SetUserGoldInfo(SwitchViewChairID(i),true,0L);
			m_GameClientView.SetUserGoldInfo(SwitchViewChairID(i),false,0L);
			m_GameClientView.m_CardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
		}		
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	WORD mIndexBanker;
	for(WORD i=GAME_PLAYER-1;i>=0;i--)                   
	{
		mIndexBanker=(pGameStart->wCurrentUser+i)%GAME_PLAYER;
		if (m_bPlayStatus[mIndexBanker]==TRUE) break;
	}
	m_GameClientView.GetCurrentUser(SwitchViewChairID(mIndexBanker)); 
	//for(int i=0;i<GAME_PLAYER;i++)
	//{
	//	mIndexBanker=(pGameStart->wCurrentUser+i+5)%GAME_PLAYER;					//找庄家
	//	if(m_bPlayStatus[mIndexBanker]==TRUE) i=1000;	                            //(i=1000的目的是退出for循环	 
	//}	             
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (bLookonMode==false) 
	{
		ActiveGameFrame();
		m_GameClientView.m_CardControl[2].AllowPositively(true);
	}

	//设置控制
	if ((bLookonMode==false)&&(pGameStart->wCurrentUser==wMeChairID))                              //判断当前玩家
	{
		int j=0;                                                                                   //add
		m_bMeAddGold=true;
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW); 
		m_GameClientView.m_btFollow.EnableWindow(false);										   //add
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btKanPai.ShowWindow(SW_SHOW);                                           //add
		
///////////////////////////////////////////////////////////////////////////////// begin add
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if (m_bPlayStatus[i]==TRUE) j++;
		}
		if (j==2) 
		{
            m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW); 
			m_GameClientView.m_btKaiPai.EnableWindow(true);
		}
		else
		{
			m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW);
			m_GameClientView.m_btKaiPai.EnableWindow(FALSE);				
		}
/////////////////////////////////////////////////////////////////////////////////// end add
		m_GameClientView.m_btAddGold.ShowWindow(SW_SHOW);
	}

	//设置定时器
	SetGameTimer(pGameStart->wCurrentUser,IDI_GIVE_UP,30);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	
	for(int i=0;i<GAME_PLAYER;i++)															       //add
	    m_GameClientView.m_CardControl[i].SetShowCardFlag(false);                                  //add
	 MyCurrentUser=pGameStart->wCurrentUser;          //add	
	 m_Begin=pGameStart->m_Begin;                     //add
	return true;
}

//用户加注
bool CGameClientDlg::OnSubAddGold(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_AddGold)) return false;
	CMD_S_AddGold * pAddGold=(CMD_S_AddGold *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pAddGold->wLastChairID);

	//处理数据
	if(pAddGold->lLastAddGold==0)  NULL;
	else
	{
		m_lTurnBasicGold=pAddGold->lLastAddGold;
	    m_GameClientView.SetUserGoldInfo(wViewChairID,false,m_GameClientView.m_GoldView[wViewChairID*2].GetGold()+m_lTurnBasicGold);
	}
	if ((IsLookonMode()==false)&&(pAddGold->wCurrentUser==wMeChairID))
	{
		//设置数据		
		m_bMeAddGold=true;
		m_lGoldShow=m_GameClientView.m_GoldView[4].GetGold();

		//自己下注
		ActiveGameFrame();
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW); 
		if(m_Begin)
			m_GameClientView.m_btFollow.EnableWindow(false);                                          //add
		else
			m_GameClientView.m_btFollow.EnableWindow(true);                                          //add
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btKanPai.ShowWindow(SW_SHOW);                                            //add			
		m_GameClientView.m_btAddGold.ShowWindow((m_bShowHand==false)?SW_SHOW:SW_HIDE);
		///////////////////////////////////////////////////////////////////////// begin add
		int j=0;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if (m_bPlayStatus[i]==TRUE) j++;
		}
		if (j==2) 
		{
            m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW); 
			m_GameClientView.m_btKaiPai.EnableWindow(true);
		}
		else
		{
			m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW);
			m_GameClientView.m_btKaiPai.EnableWindow(FALSE);				
		}
		/////////////////////////////////////////////////////////////////////////// end add
	}	

	//其他处理
	SetGameTimer(pAddGold->wCurrentUser,IDI_GIVE_UP,30);
	if (m_bPlayStatus[pAddGold->wCurrentUser]==TRUE) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
	MyCurrentUser=pAddGold->wCurrentUser;     //add


	return true;
}

//用户放弃
bool CGameClientDlg::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_bPlayStatus[pGiveUp->wUserChairID]=false;
	if (pGiveUp->wUserChairID==GetMeChairID()) SetGameStatus(GS_FREE);

	//设置界面
	BYTE bCard[5]={0,0,0,0,0};
	WORD wViewStation=SwitchViewChairID(pGiveUp->wUserChairID);
	WORD bCount=m_GameClientView.m_CardControl[wViewStation].GetCardCount();
	m_GameClientView.m_CardControl[wViewStation].AllowPositively(false);
	m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(false);                             
	m_GameClientView.m_CardControl[wViewStation].SetCardData(NULL,0);							   //add
	
	//其他处理 
	if (pGiveUp->wUserChairID==GetTimeChairID()) KillGameTimer(IDI_GIVE_UP);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//发牌消息
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_R_SendCard)) return false;
	CMD_R_SendCard * pSendCard=(CMD_R_SendCard *)pBuffer;

	//设置变量
	m_lGoldShow=0L;
	m_lTurnBasicGold=0L;
	m_lTurnMaxGold=pSendCard->lMaxGold;

	//更新界面
	//BYTE bCardData[5];               
	LONG lUserTableGold=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);
		//设置金币
		lUserTableGold=m_GameClientView.m_GoldView[wViewChairID*2].GetGold();
		lUserTableGold+=m_GameClientView.m_GoldView[wViewChairID*2+1].GetGold();
		m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
		m_GameClientView.SetUserGoldInfo(wViewChairID,true,lUserTableGold);
	}

	//下注判断
	if ((IsLookonMode()==false)&&(pSendCard->wCurrentUser==GetMeChairID()))
	{
		int j=0;															                       //add
		ActiveGameFrame();
		m_bMeAddGold=true;
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btKanPai.ShowWindow(SW_SHOW);                                           //add
		
///////////////////////////////////////////////////////////////////////////////// begin add
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if (m_bPlayStatus[i]==TRUE) j++;
		}
		if (j==2) 
		{
            m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW); 
			m_GameClientView.m_btKaiPai.EnableWindow(true);
		}
		else
		{
			m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW);
			m_GameClientView.m_btKaiPai.EnableWindow(FALSE);				
		}
/////////////////////////////////////////////////////////////////////////////////// end add
		m_GameClientView.m_btAddGold.ShowWindow((m_bShowHand==false)?SW_SHOW:SW_HIDE);
		m_GameClientView.m_btShowHand.ShowWindow(((m_GameClientView.m_CardControl[2].GetCardData(NULL,0)>=4)&&(m_bShowHand==false))?SW_SHOW:SW_HIDE);
	}

	//设置定时器
	SetGameTimer(pSendCard->wCurrentUser,IDI_GIVE_UP,30);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameGold[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//游戏正常结束
	BYTE bCardData[3];
	m_GameClientView.m_ScoreView.ResetScore();
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameGold[i]!=0L)
		{
			if (m_szName[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameGold[i]);
			else m_GameClientView.m_ScoreView.SetGameScore(i,m_szName[i],pGameEnd->lGameGold[i]);
			if (pGameEnd->bUserCard[i]!=0)
			{
				WORD wViewStation=SwitchViewChairID(i);
				WORD wCardCount=m_GameClientView.m_CardControl[wViewStation].GetCardData(bCardData,CountArray(bCardData));
				bCardData[0]=pGameEnd->bUserCard[i];
				m_GameClientView.m_CardControl[wViewStation].SortCardData();
				m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(true);    
			}
		}
	}
	m_GameClientView.m_ScoreView.SetTax(pGameEnd->lTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置控件
	KillGameTimer(IDI_GIVE_UP);
	m_GameClientView.m_btKanPai.ShowWindow(SW_HIDE);                                               //add
	m_GameClientView.m_btKaiPai.ShowWindow(SW_HIDE);                                               //add
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddGold.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);

	//设置状态
	SetGameStatus(GS_FREE);

	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	memset(m_GameClientView.mArrayKanPai,0,sizeof(m_GameClientView.mArrayKanPai));             //add
	memset(m_GameClientView.myKanPaiBool,0,sizeof(m_GameClientView.myKanPaiBool));                 //add
	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	memset(m_szName,0,sizeof(m_szName));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btStart.SetFocus();
		SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
	}

//////////////////////////////////////////////////////////////////////////                         //begin add
	
	for(int i=0;i<GAME_PLAYER;i++)
	{
	    m_GameClientView.m_CardControl[i].SetShowCardFlag(true);
		m_GameClientView.mArrayKanPai[i]=false;
	
		m_GameClientView.myKanPaiBool[i]=false;              //add
	}
	m_GameClientView.m_btKanPai.EnableWindow(true);
	m_GameClientView.mArrayKanPai[GAME_PLAYER]=true;
	AddGold=0;
//////////////////////////////////////////////////////////////////////////						   //end add	
    return true;
}

//开始按钮
LRESULT	CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//加注按钮 
LRESULT	CGameClientDlg::OnAddGold(WPARAM wParam, LPARAM lParam)
{
	if ((m_bMeAddGold==true)&&(m_GameClientView.m_GoldControl.IsWindowVisible()==FALSE))
	{
		LONG lGold=m_lTurnMaxGold;
		m_GameClientView.m_GoldControl.SetMaxGold(lGold);
		m_GameClientView.m_GoldControl.ShowWindow(SW_SHOW);      //显示加注多少的视图		       
	}
	
	return 0;
}

//放弃按钮
LRESULT CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	
		//放弃游戏
		m_bMeAddGold=false;
		KillGameTimer(IDI_GIVE_UP);
		m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAddGold.ShowWindow(SW_HIDE);
		m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
		m_GameClientView.m_btKanPai.ShowWindow(SW_HIDE);                                           //add
		m_GameClientView.m_btKaiPai.ShowWindow(SW_HIDE);                                           //add
		m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
		m_GameClientView.SetUserGoldInfo(2,false,m_lGoldShow);
		
		//发送消息
		SendData(SUB_C_GIVE_UP);

	return 0;
}

//跟注按钮
LRESULT CGameClientDlg::OnFollow(WPARAM wParam, LPARAM lParam)
{
	
	//获取金币
	/*LONG lGold=m_GameClientView.m_GoldView[4].GetGold()+m_GameClientView.m_GoldControl.GetGold();
	lGold=__max(lGold,m_lTurnBasicGold);*/
	LONG lGold=m_lTurnBasicGold+AddGold;     //add
	//lGold=__max(lGold,m_lTurnBasicGold);	 //add
	if(!m_Begin)
	{
		if( m_GameClientView.mArrayKanPai[MyCurrentUser])
		{
			if(m_GameClientView.mArrayKanPai[GAME_PLAYER])                       
				lGold=__max(lGold,m_lTurnBasicGold);			   
			else
			{
				lGold=2*__max(lGold,m_lTurnBasicGold)-AddGold;				
			}//End if
		}
		else
		{
			if(m_GameClientView.mArrayKanPai[GAME_PLAYER])
			{
				if(m_lTurnBasicGold>1)
					lGold=m_lTurnBasicGold/2+AddGold;
				else
					lGold=m_lTurnBasicGold+AddGold;
			}
			else
			{
				lGold=__max(lGold,m_lTurnBasicGold);
			}//End if
		}//End if( m_GameClientView.mArrayKanPai[MyCurrentUser])

		//算算金额
		if ( IsOverSelfMoney(lGold ) )
		{
			AfxMessageBox("金额不足！！");
			return 0;
		}
	}//End if(!m_Begin)
	
	//设置变量
	m_bMeAddGold=false;  
	//m_bMeAddGold=true;      //add
	//if ((m_GameClientView.m_lMaxGold-m_GameClientView.m_GoldView[5].GetGold()-lGold)==0) m_bShowHand=true;

	//设置界面
	//m_GameClientView.m_GoldControl.SetMaxGold(0L);                                               //delete 不显示跟注的增加
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btKanPai.ShowWindow(SW_HIDE);		          	                           //add
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);                                               //add
	m_GameClientView.m_btKaiPai.ShowWindow(SW_HIDE);                               
	m_GameClientView.m_btAddGold.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);	       


	//删除定时器
	KillGameTimer(IDI_GIVE_UP);
	
	//发送数据
	CMD_C_AddGold AddPoint;
	AddPoint.lGold=lGold;
	AddPoint.m_Begin=false;                                    //add
	if(m_GameClientView.mArrayKanPai[MyCurrentUser])
		SendData(SUB_C_KANPAI);
	else 
		SendData(SUB_C_NOKANPAI);

	SendData(SUB_C_ADD_GOLD,&AddPoint,sizeof(AddPoint));
	AddGold=0;												   //add
	m_GameClientView.m_GoldControl.SetGold(0);                 //add
	
	return 0;
}

//梭哈按钮
LRESULT CGameClientDlg::OnShowHand(WPARAM wParam, LPARAM lParam)
{
	LONG lGold=m_lTurnMaxGold-m_GameClientView.m_GoldView[5].GetGold();
	LONG dwMoney4=m_GameClientView.m_GoldView[4].GetGold();
	if ((lGold-dwMoney4)>0L)
	{
		CString strMessage;
		strMessage.Format(TEXT("梭哈的下注是 %ld 筹码，确实要梭哈吗？"),lGold-dwMoney4);
		if (IDYES!=AfxMessageBox(strMessage,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION)) return 0;
	}
	if (m_bMeAddGold==true)
	{
		m_GameClientView.SetUserGoldInfo(2,false,lGold);
		OnFollow(0,0);
	}

	return 0;
}

LRESULT CGameClientDlg::KaiPai(WPARAM wParam, LPARAM lParam)                     //add 开牌按钮
{
	if(m_Begin)
		AddGold=BOTTOMGOLD;              //add底注为1
	OnFollow(wParam,lParam);
	//发送消息
	SendData(SUB_C_KAIPAI);

	return 0;
}

LRESULT CGameClientDlg::ShowFollow(WPARAM wParam, LPARAM lParam)                 //add
{
	m_GameClientView.m_btFollow.EnableWindow(true);
	int j=0;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if (m_bPlayStatus[i]==TRUE) j++;
	}
	if (j==2) 
	{
        m_GameClientView.m_btKaiPai.ShowWindow(SW_SHOW); 
		m_GameClientView.m_btKaiPai.EnableWindow(true);
	}
	AddGold=m_GameClientView.m_GoldControl.GetGold();                           //add
	return 0;
}

LRESULT CGameClientDlg::SetKanPai(WPARAM wParam, LPARAM lParam)					//add
{
	m_GameClientView.mArrayKanPai[MyCurrentUser]=true;
	m_GameClientView.m_btKanPai.EnableWindow(false);
	return 0;
}
//看牌
bool  CGameClientDlg::OnSubKanPai(const void * pBuffer, WORD wDataSize)         //add
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSubKanPai.log",strTime);

			strTemp.Format("OnSubKanPai ");
			theApp.WriteLog(strFile, strTemp);

	if (wDataSize!=sizeof(m_GameClientView.mArrayKanPai)) return false;
	WORD *pArray;
	pArray=(WORD *)pBuffer;
	CopyMemory(m_GameClientView.mArrayKanPai,pArray,sizeof(m_GameClientView.mArrayKanPai));

	//bool                            mArrayKanPai[GAME_PLAYER+1];
//	memset(m_GameClientView.myKanPaiBool,0,sizeof(m_GameClientView.myKanPaiBool));                 //add
			//看牌标记
	WORD wViewStation;//=SwitchViewChairID(i);
	//	memset(m_GameClientView.myKanPaiBool,0,sizeof(m_GameClientView.myKanPaiBool));                 //add
	for (int i=0; i<GAME_PLAYER; i++)
	{
		wViewStation=SwitchViewChairID(i);
		m_GameClientView.myKanPaiBool[wViewStation] = m_GameClientView.mArrayKanPai[i];

		if (m_GameClientView.mArrayKanPai[i])
		{
			strTemp.Format("ok i=%d wViewStation=%d ", i, wViewStation);
		}
		else
		{
			strTemp.Format("no i=%d wViewStation=%d", i, wViewStation);
		}
		theApp.WriteLog(strFile, strTemp);

	}//End for
	return true;
}

bool CGameClientDlg::OnSubOver(const void * pBuffer, WORD wDataSize)           //add
{	
	if (wDataSize!=sizeof(WORD)) return false;
	m_Begin=false;
	return true;
}
//判断是否超过帐户金额
//输入：增加钱数
bool CGameClientDlg::IsOverSelfMoney(int addInt )
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsOverSelfMoney.log",strTime);

	strTemp.Format("IsOverSelfMoney  addInt=%d", addInt);
	theApp.WriteLog(strFile, strTemp);

	bool resultBool = true;
	//帐户金额
	int userDeskInt = GetMeChairID();
	int userAllMoney;
	int allInputInt;
	const tagUserData * pUserData=GetUserData(userDeskInt);
	if (pUserData!=NULL)
	{
		userAllMoney = pUserData->lScore;
	}

	strTemp.Format("userAllMoney=%d", userAllMoney);
	theApp.WriteLog(strFile, strTemp);

	//已经押的金额
	//allInputInt = m_GameClientView.m_GoldView[4].GetGold();//m_lGoldShow ;//m_GameClientView.GetOneUserInputAllMoney(userDeskInt);

		//获取位置
		WORD wViewChairID=SwitchViewChairID(userDeskInt);
		//设置金币
		allInputInt=m_GameClientView.m_GoldView[wViewChairID*2].GetGold();
		allInputInt+=m_GameClientView.m_GoldView[wViewChairID*2+1].GetGold();

	strTemp.Format("allInputInt=%d", allInputInt);
	theApp.WriteLog(strFile, strTemp);

	//想要压的
	if (addInt < (userAllMoney - allInputInt))
	{
		resultBool = false;
	}

	strTemp.Format("addInt(%d) < (userAllMoney - allInputInt)(%d)", addInt , (userAllMoney - allInputInt));
	theApp.WriteLog(strFile, strTemp);

	return resultBool;
}