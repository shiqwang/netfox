#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "..\消息定义\CMD_QuickRun.h"
#include ".\gameclientdlg.h"
#include "GameOption.h"


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
	ON_MESSAGE(WM_OUTPISA,OnCmdOutPisa)
	ON_MESSAGE(WM_USERREADY,OnCmdUserReady)
	ON_MESSAGE(IDM_TIMER_OUT,OnCmdTimerOut)
	ON_MESSAGE(IDM_RESET_UI,OnResetUI )	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	uiShowInt = 0;
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
#ifdef _TWICE
	SetWindowText(TEXT("跑得快游戏（两副牌）  --  Ver：6.0.1.0"));
#else
	SetWindowText(TEXT("跑得快游戏  --  Ver：6.0.1.0"));
#endif

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置 配置控件
	m_GameClientView.SetUserOrder(AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),TRUE)?true:false);
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	TRACE("CGameClientDlg::ResetGameFrame");
	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),GameOption.m_bDeasilOrder?TRUE:FALSE);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	return true;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	TRACE("CGameClientDlg::OnLookonChanged");
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	TRACE("CGameClientDlg::OnGameMessage:SubCmdID:%d",wSubCmdID);
	switch(wSubCmdID)
	{
		case MSG_SET_INITPISA:
		{
			//初始化
			struct tagSetInitPisa *pInit=(struct tagSetInitPisa *)pBuffer;
			m_GameClientView.m_nOwnerIndex=(pInit->nOwnerIndex+2)%GAME_PLAYER;//在桌面上的相对偏移
			TRACE("收到初始化的索引：%d,计算后的索引：%d,我的椅子id：%d\n",pInit->nOwnerIndex,m_GameClientView.m_nOwnerIndex,GetMeChairID());
			//m_GameClientView.m_nFocusIndex=0;
			for(int i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_nPlayerLeftCount[i]=PISACOUNT;
				m_GameClientView.m_OutPisaArray[i].RemoveAll();
				m_GameClientView.m_bOutPass[i]=FALSE;
			}
			m_GameClientView.m_PisaArray.RemoveAll();
			for(int i=0;i<PISACOUNT;i++)
			{
				m_GameClientView.m_PisaArray.Add(pInit->tPisa[i]);
				m_GameClientView.m_bOutSelect[i]=FALSE;
			}
			m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
			m_GameClientView.m_bStart=TRUE;
		}
			break;
		case MSG_SET_FOCUS:
		{
			//设置当前谁出牌
			struct tagSetFocus *pFocus=(struct tagSetFocus *)pBuffer;
			m_GameClientView.m_nFocusIndex=(pFocus->nFoucsId+GAME_PLAYER-m_GameClientView.m_nOwnerIndex)%GAME_PLAYER;
			//如果设置焦点不是自己，那么就说明不是自己先出牌
			if (m_GameClientView.m_nFocusIndex!=2)
				m_GameClientView.m_bStart=FALSE;
			TRACE("收到设置焦点的索引：%d,计算后的索引：%d\n",pFocus->nFoucsId,m_GameClientView.m_nFocusIndex);
			if (pFocus->bClean)
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					m_GameClientView.m_OutPisaArray[i].RemoveAll();
					m_GameClientView.m_bOutPass[i]=FALSE;
				}
			}
			m_GameClientView.SetTimerUp(TRUE);
		}
			break;
		case MSG_SET_OTHERPISA:
		{
			//设置其它玩家出了什么牌
			struct tagSetOtherPisa *pOther=(struct tagSetOtherPisa *)pBuffer;
			int nLocalIndex=(pOther->nChairId+GAME_PLAYER-m_GameClientView.m_nOwnerIndex)%GAME_PLAYER;
			TRACE("收到设置玩家出牌的索引：%d,计算后的索引：%d\n",pOther->nChairId,nLocalIndex);
			m_GameClientView.m_nPlayerLeftCount[nLocalIndex]-=pOther->nCount;
			m_GameClientView.m_OutPisaArray[nLocalIndex].RemoveAll();
			if (pOther->nCount==0)
			{
				m_GameClientView.m_bOutPass[nLocalIndex]=TRUE;
			}
			else
			{
				m_GameClientView.m_bOutPass[nLocalIndex]=FALSE;
				for(int i=0;i<pOther->nCount;i++)
					m_GameClientView.m_OutPisaArray[nLocalIndex].Add(pOther->tOutPisa[i]);
			}
		}
			break;
		case MSG_OUTPISA_MSG:
		{
			//玩家出的牌经过服务器检查返回的结果
			struct tagOutPisaMsg *pOutPisa=(struct tagOutPisaMsg *)pBuffer;
			if (pOutPisa->nResult==0)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
				//delete all select out pisa
				for(int i=PISACOUNT-1;i>=0;i--)
				{
					if (m_GameClientView.m_bOutSelect[i])
					{
						m_GameClientView.m_PisaArray.RemoveAt(i);
						m_GameClientView.m_bOutSelect[i]=FALSE;
					}
				}
			}
			else
			{
				m_GameClientView.SetTimerPause(false);
				MessageBox(pOutPisa->chErrMsg,"不能这样出牌");
			}
		}
			break;
		case MSG_GAME_END:
		{
			struct tagGameEnd *pGameEnd=(struct tagGameEnd *)pBuffer;
			//设置积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				const tagUserData * pUserData=GetUserData(i);
				m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
			}
			m_GameClientView.m_nFocusIndex=-1;
			m_GameClientView.m_bReady=FALSE;
			m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);
			m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

			//播放声音
			WORD wMeChairID=GetMeChairID();
			LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
			if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
			else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
			else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

		}
			break;
	}
	m_GameClientView.UpdateGameView(NULL);
	return true;
	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	TRACE("CGameClientDlg::OnGameMessage:GameStation:%d,Lookon:%d",cbGameStation,bLookonOther);
	return true;
	return false;
}

LRESULT CGameClientDlg::OnCmdOutPisa(WPARAM wParam,LPARAM lParam)
{
	struct tagSetOutPisa *pOut=(struct tagSetOutPisa *)wParam;
	TRACE("我的椅子id:%d，id:%d\n",this->GetMeChairID(),pOut->nChairId);
	m_GameClientView.SetTimerPause(true);
	SendData(MSG_SET_OUTPISA,pOut,sizeof(struct tagSetOutPisa));
	return 0;
}

LRESULT CGameClientDlg::OnCmdUserReady(WPARAM wParam,LPARAM lParam)
{
	SendUserReady(NULL,0);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("Ready"));
	return 0;
}

LRESULT CGameClientDlg::OnCmdTimerOut(WPARAM wParam,LPARAM lParam)
{
	struct tagSetOutPisa out;
	memset(&out,0,sizeof(tagSetOutPisa));
	if (m_GameClientView.m_bStart)
	{
		//如果是刚开始的出牌，需要把黑桃3发出去
		int index=m_GameClientView.SetHeiTao3Out();
		//获取黑桃3错误
		if (index==-1) return 0;
		out.tOutPisa[0].line=4;
		out.tOutPisa[0].row=3;
		out.nCount=1;
	}
	SendData(MSG_SET_OUTPISA,&out,sizeof(struct tagSetOutPisa));
	return 0;
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
	myX= 10;//
	myY=  hInt-myH-10; 

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

	x = 8 ;//+ xOff;
	y = 285+ yOff - 70;

	int rlTopY = 285+ yOff - 70;
	int topX = 280+xOff;
	int topY = 34+ yOff;

	int rightX= xOff - (VIDEO_PIC_W+OP_BACK_PIC_W)+15 ;//577+xOff;
	int leftX = 8;

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

		}

	}	
//left right top 
CStatic * myVideo[GAME_PLAYER-1];
CLabelEx * myTools[GAME_PLAYER-1];
CPoint * myPt[GAME_PLAYER-1];
int listInt [3]={1, 2, 3};

if ( myDeskInt > (GAME_PLAYER-1) || myDeskInt < 0)
return ;
	//顺时钟
//0,1,2,3,0,1,2,3
	if (m_GameClientView.IsDeasilOrder()!=true)
	{
	switch(myDeskInt)
	{
	case 0:
		{
			listInt[0]=1;
			listInt[1]=2;
			listInt[2]=3;

		}
		break;
	case 1:
		{
			//0,2,3


			listInt[0]=2;
			listInt[1]=3;
			listInt[2]=0;

		}
		break;
	case 2:
		{
			//0,1,3

			listInt[0]=3;
			listInt[1]=0;
			listInt[2]=1;

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
	}
//NI
//3,2,1,0,3,2,1,0
	else
	{
	
	switch(myDeskInt)
	{
	case 0:
		{

			listInt[0]=3;
			listInt[1]=2;
			listInt[2]=1;

		}
		break;
	case 1:
		{
			//0,2,3


			listInt[0]=0;
			listInt[1]=3;
			listInt[2]=2;

		}
		break;
	case 2:
		{
			//0,1,3

			listInt[0]=1;
			listInt[1]=0;
			listInt[2]=3;

		}
		break;
	case 3:
		{
			//0,1,2
			listInt[0]=2;
			listInt[1]=1;
			listInt[2]=0;

		}
		break;
	}
	}


for (int i=0; i < GAME_PLAYER-1; i++)
{
myVideo[i]=myVideoList[listInt[i]];
myTools[i]=myToolsList[listInt[i]];
myPt[i]=&m_GameClientView.m_PtVideo[listInt[i]];
//
		/*
		if ( i==1 )
		{	
			x = rightX;
		}
//
		else if(i == 2)
		{
			y = topY;
			x = (xOff-VIDEO_PIC_W)/2 +30;
		}
*/
		switch(i)
		{

//ok
		case 0:
			y = rlTopY;
			x = leftX;			
			break;
//ok
		case 1:
			y = topY;
			x = (xOff-VIDEO_PIC_W)/2 +30;
			break;
//ok
		case 2:
			y = rlTopY;
			x = rightX;
			break;

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
