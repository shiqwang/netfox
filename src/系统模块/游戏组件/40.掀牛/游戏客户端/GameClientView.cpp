#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "..\游戏服务器\GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_COUPAI						101								//1 分按钮
#define IDC_XIANPAI						102								//2 分按钮
#define IDC_QIANGCI						103								//3 分按钮
#define IDC_GIVE_UP_SCORE				104								//放弃按钮
#define IDC_OUT_CARD					105								//出牌按钮
#define IDC_PASS_CARD					106								//PASS按钮
#define IDC_AUTO_OUTCARD                107                             //提示按纽
#define IDC_FORETURN_CARD				108								//上一轮扑克

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_COUPAI, OnKouPai)
	ON_BN_CLICKED(IDC_XIANPAI, OnXianPai)
	ON_BN_CLICKED(IDC_QIANGCI, OnQiangCi)
	ON_BN_CLICKED(IDC_AUTO_OUTCARD, OnAutoOutCard)
	ON_BN_CLICKED(IDC_GIVE_UP_SCORE, OnGiveUpQingCi)
	ON_BN_CLICKED(IDC_FORETURN_CARD,OnForTurn)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
long                      CGameClientView::m_lTimeCount=112	;						//时间
//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//游戏变量
	m_lBaseScore=0;

	m_wBanker=INVALID_CHAIR;

	//配置变量
	m_bDeasilOrder=true;

	//状态变量
	ZeroMemory(m_bCouPaiTitle,sizeof(m_bCouPaiTitle));
	ZeroMemory(m_bXianPaiTitle,sizeof(m_bXianPaiTitle));
	ZeroMemory(m_bQiangCiTitle,sizeof(m_bQiangCiTitle));
	ZeroMemory(m_bZhanPaiCount,sizeof(m_bZhanPaiCount));
	ZeroMemory(m_lForeScore,sizeof(m_lForeScore));
	ZeroMemory(m_lAllScore,sizeof(m_lAllScore));
	m_lTimeCount=0;

	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	ZeroMemory(m_bXianCount,sizeof(m_bXianCount));
	ZeroMemory(m_bBeiXianCount,sizeof(m_bBeiXianCount));

	//位置信息
	memset(m_ptScore,0,sizeof(m_ptScore));
	memset(m_ptLand,0,sizeof(m_ptLand));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBomb.SetLoadInfo(IDB_BOMB,hInstance);
	m_ImageScore.SetLoadInfo(IDB_SCORE,hInstance);
	m_ImageLand.SetLoadInfo(IDB_BANKER,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageCenter.SetLoadInfo(IDB_VIEW_CENTER,hInstance);
	m_ImageLc.SetLoadInfo(IDB_VIEW_LC,hInstance);
	m_ImageHeadBg.SetLoadInfo(IDB_HEAD_BG,hInstance);
	m_ImageUserPass.SetLoadInfo(IDB_PASS_FLAG,hInstance);
	m_ImageTime.SetLoadInfo(IDB_TIME,hInstance);
	m_ImageTime2.SetLoadInfo(IDB_TIME2,hInstance);
	//设置扑克
	m_HandCardControl.SetCardImage(IDB_CARD);
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_ZhanCardControl[i].SetCardImage(IDB_ZHAN_CARD);
		m_UserCardControl[i].SetCardImage(IDB_CARD);
		m_ZhanCardControl[i].SetCardData(NULL,0);
		m_ForeTurn[i].SetCardImage(IDB_CARD);
	}
	//计算位置
	CImageHandle ImageHandleLand(&m_ImageLand);
	m_LandSize.cx=m_ImageLand.GetWidth();
	m_LandSize.cy=m_ImageLand.GetHeight();



	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect CreateRect(0,0,0,0);
	m_ScoreView.Create(IDD_GAME_SCORE,this);

	//创建扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].SetDirection(true);
		m_UserCardControl[i].SetDisplayFlag(true);
		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,20+i);
		m_ForeTurn[i].SetDirection(true);
		m_ForeTurn[i].SetDisplayFlag(false);
		m_ForeTurn[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,23+i);
	}
	for (WORD i=0;i<2;i++) 
	{
		m_LeaveCardControl[i].SetDirection(false);
		m_LeaveCardControl[i].SetDisplayFlag(true);
		m_LeaveCardControl[i].SetCardSpace(0,18,0);
		m_LeaveCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
	}

	m_HandCardControl.SetSinkWindow(AfxGetMainWnd());
	m_HandCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,40);
	for(BYTE i=0;i<GAME_PLAYER;i++)m_ZhanCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,42+i);
	

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
	m_btKouPai.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_COUPAI);
	m_btXianPai.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_XIANPAI);
	m_btQiangCi.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_QIANGCI);
	m_btGiveUpCiangCi.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_GIVE_UP_SCORE);
	m_btAutoOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_AUTO_OUTCARD);
	m_btForeTurn.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_FORETURN_CARD);


	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false);
	m_btKouPai.SetButtonImage(IDB_VIEW_BUTTON,hInstance,false);
	m_btXianPai.SetButtonImage(IDB_VIEW_BUTTON,hInstance,false);
	m_btQiangCi.SetButtonImage(IDB_VIEW_BUTTON,hInstance,false);
	m_btGiveUpCiangCi.SetButtonImage(IDB_VIEW_BUTTON,hInstance,false);
	m_btAutoOutCard.SetButtonImage(IDB_AUTO_OUT_CARD,hInstance,false);
	m_btForeTurn.SetButtonImage(IDB_VIEW_BUTTON,hInstance,false);
	m_btKouPai.SetButtonImage(IDB_KOU_PAI,hInstance,false);
	m_btXianPai.SetButtonImage(IDB_XIAN_PAI,hInstance,false);
	m_btForeTurn.SetButtonImage(IDB_FORE_TURN,hInstance,false);
	m_btQiangCi.SetButtonImage(IDB_QIANG_CI,hInstance,false);
	m_btGiveUpCiangCi.SetButtonImage(IDB_GIVE_UP,hInstance,false);
	
	//设置占牌
	for(BYTE j=0;j<GAME_PLAYER;j++)
	{
		m_ZhanCardControl[j].SetCardSpace(14,0,0);
		m_ZhanCardControl[j].SetDisplayFlag(true);
	}
	//读取配置
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

		

	return 0;
}

//鼠标消息
void CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,1,1);

	return;
}

//鼠标双击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_HandCardControl.ShootAllCard(false);

	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//游戏变量
	m_lBaseScore=0;
	m_wBanker=INVALID_CHAIR;

	//状态变量
	ZeroMemory(m_bCouPaiTitle,sizeof(m_bCouPaiTitle));
	ZeroMemory(m_bXianPaiTitle,sizeof(m_bXianPaiTitle));
	ZeroMemory(m_bQiangCiTitle,sizeof(m_bQiangCiTitle));
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	ZeroMemory(m_bZhanPaiCount,sizeof(m_bZhanPaiCount));
	ZeroMemory(m_lForeScore,sizeof(m_lForeScore));
	ZeroMemory(m_lAllScore,sizeof(m_lAllScore));
	m_lTimeCount=0;
	ZeroMemory(m_bXianCount,sizeof(m_bXianCount));
	ZeroMemory(m_bBeiXianCount,sizeof(m_bBeiXianCount));

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	m_btKouPai.ShowWindow(SW_HIDE);
	m_btXianPai.ShowWindow(SW_HIDE);
	m_btQiangCi.ShowWindow(SW_HIDE);
	m_btGiveUpCiangCi.ShowWindow(SW_HIDE);
	m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_btForeTurn.ShowWindow(SW_HIDE);

	//禁用控件
	m_btOutCard.EnableWindow(FALSE);
	m_btPassCard.EnableWindow(FALSE);
	m_btAutoOutCard.EnableWindow(FALSE);
	m_btForeTurn.EnableWindow(FALSE);

	//扑克控件
	m_BackCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayFlag(false);
	m_LeaveCardControl[0].SetCardData(NULL,0);
	m_LeaveCardControl[1].SetCardData(NULL,0);

	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_UserCardControl[i].SetCardData(NULL,0);
		m_ForeTurn[i].SetCardData(NULL,0);
	}
	for(BYTE j=0;j<GAME_PLAYER;j++)m_ZhanCardControl[j].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标


	m_ptFace[0].x=nWidth/2-362+m_nXFace/2+m_nXBorder;//左家
	m_ptFace[0].y=nHeight/3-8;
	m_ptName[0].x=m_ptFace[0].x+m_nXFace/2-13;
	m_ptName[0].y=m_ptFace[0].y+m_nYFace+5;
	m_ptTimer[0].x=m_ptFace[0].x;
	m_ptTimer[0].y=m_ptFace[0].y-m_nYFace;
	m_ptReady[0].x=m_ptFace[0].x+3*m_nXFace/2;
	m_ptReady[0].y=m_ptFace[0].y+m_nYFace/2;
	m_pPass[0].x=m_ptFace[0].x+2*m_nXFace;
	m_pPass[0].y=m_ptFace[0].y;

	m_ptFace[1].x=nWidth/2-362+m_nXFace/2+m_nXBorder;//自己
	m_ptFace[1].y=nHeight-6*m_nYFace-m_nYBorder-8;
	m_ptName[1].x=m_ptFace[1].x+m_nXFace/2-13;
	m_ptName[1].y=m_ptFace[1].y+m_nYFace+5;
	m_ptTimer[1].x=m_ptFace[1].x;
	m_ptTimer[1].y=m_ptFace[1].y-m_nYFace;
	m_ptReady[1].x=m_ptFace[1].x+3*m_nXFace/2;
	m_ptReady[1].y=m_ptFace[1].y+m_nYFace/2;
	m_pPass[1].x=m_ptFace[1].x+2*m_nXFace;
	m_pPass[1].y=m_ptFace[1].y;

	m_ptFace[2].x=nWidth/2+375-m_nXFace-3*m_nXBorder;//右家
	m_ptFace[2].y=nHeight/3;
	m_ptName[2].x=m_ptFace[2].x+10;
	m_ptName[2].y=m_ptFace[2].y+m_nYFace+5;
	m_ptTimer[2].x=m_ptFace[2].x;
	m_ptTimer[2].y=m_ptFace[2].y-m_nYFace-3;
	m_ptReady[2].x=m_ptFace[2].x-2*m_nXBorder;
	m_ptReady[2].y=m_ptFace[2].y+m_nYFace/2;
	m_pPass[2].x=m_ptFace[2].x-2*m_nXFace;
	m_pPass[2].y=m_ptFace[2].y;

	//占牌用户名
	m_pZhanPaiName[0].x=nWidth/2-9*m_nXFace-5;
	m_pZhanPaiName[0].y=nHeight-3*m_nYFace-m_nYBorder+20-8;
	m_pZhanPaiName[1].x=nWidth/2-9*m_nXFace-5;
	m_pZhanPaiName[1].y=nHeight-2*m_nYFace-m_nYBorder+20-8;
	m_pZhanPaiName[2].x=nWidth/2-9*m_nXFace-5;
	m_pZhanPaiName[2].y=nHeight-m_nYFace-m_nYBorder+20-8;

	//时间标志
	m_pTimeFlag.x=m_ptFace[0].x;
	m_pTimeFlag.y=2*m_nYFace-10-8;


	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(10);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-250-2*m_nXFace,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()*3/2-10,nHeight-9*m_nYFace-3*m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAutoOutCard,NULL,nWidth/2+rcButton.Width()/2+10,nHeight-9*m_nYFace-3*m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+rcButton.Width()/2+10,nHeight-200-m_nYBorder,0,0,uFlags);

	DeferWindowPos(hDwp,m_btKouPai,NULL,nWidth/2+372-rcButton.Width(),nHeight-190-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btXianPai,NULL,nWidth/2+352-2*rcButton.Width(),nHeight-190-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btQiangCi,NULL,nWidth/2+352-2*rcButton.Width(),nHeight-225-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUpCiangCi,NULL,nWidth/2+372-rcButton.Width(),nHeight-225-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btForeTurn,NULL,nWidth/2+362-rcButton.Width()-rcButton.Width()/2,nHeight-225-2*m_nYBorder-rcButton.Height(),0,0,uFlags);


	EndDeferWindowPos(hDwp);

	//移动扑克
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nHeight-5*m_nYFace-m_nYBorder,enXCenter,enYBottom);


	m_ZhanCardControl[0].SetBenchmarkPos(nWidth/2-7*m_nXFace+2*m_nXBorder,nHeight-3*m_nYFace-m_nYBorder+30-8,enXLeft,enYBottom);
	m_ZhanCardControl[1].SetBenchmarkPos(nWidth/2-7*m_nXFace+2*m_nXBorder,nHeight-2*m_nYFace-m_nYBorder+30-8,enXLeft,enYBottom);
	m_ZhanCardControl[2].SetBenchmarkPos(nWidth/2-7*m_nXFace+2*m_nXBorder,nHeight-m_nYFace-m_nYBorder+30-8,enXLeft,enYBottom);
	m_ForeTurn[0].SetBenchmarkPos(nWidth/2-326+m_nXBorder+m_nXFace+90,nHeight/2-4*m_nYFace-8,enXLeft,enYCenter);
	m_ForeTurn[2].SetBenchmarkPos(nWidth/2+326-m_nXFace-m_nXBorder-90,nHeight/2-4*m_nYFace-8,enXRight,enYCenter);
	m_ForeTurn[1].SetBenchmarkPos(nWidth/2,nHeight-7*m_nYFace-2*m_nYBorder-8,enXCenter,enYBottom);

	//用户扑克
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2+326-m_nXFace-m_nXBorder-90,nHeight/2-4*m_nYFace-8,enXRight,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-9*m_nYFace-2*m_nYBorder-8,enXCenter,enYBottom);
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2-326+m_nXBorder+m_nXFace+90,nHeight/2-4*m_nYFace-8,enXLeft,enYCenter);

	m_LeaveCardControl[0].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-50,nHeight/2-80,enXRight,enYCenter);
	m_LeaveCardControl[1].SetBenchmarkPos(m_nXBorder+m_nXFace+50,nHeight/2-80,enXLeft,enYCenter);
	
	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,(nHeight-rcControl.Height()-100)/2,0,0,SWP_NOZORDER|SWP_NOSIZE);

	return;
}
//绘画时间标志
void CGameClientView::DrawTime(CDC * pDC,int nWidth,int nHeight)
{
	CImageHandle HandleTime(&m_ImageTime);
	m_ImageTime.AlphaDrawImage(pDC,m_pTimeFlag.x,m_pTimeFlag.y,RGB(255,0,255));
	CImageHandle HandleTime2(&m_ImageTime2);
	m_lTimeCount=m_lTimeCount<112?m_lTimeCount:112;
	for(BYTE i=0;i<m_lTimeCount;i++)
		m_ImageTime2.AlphaDrawImage(pDC,m_pTimeFlag.x+8*m_nXBorder-1+i,m_pTimeFlag.y,RGB(255,0,255));
	
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);
	DrawViewImage(pDC,m_ImageCenter,enMode_Centent);
	CImageHandle HandleHeadBg(&m_ImageHeadBg);
	
	//放弃标志
	CImageHandle HnadleUserPass(&m_ImageUserPass);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bPass[i]) 
		{
			m_ImageUserPass.AlphaDrawImage(pDC,m_pPass[i].x,m_pPass[i].y,RGB(255,0,255));
		}
	}

	//绘画用户
	TCHAR szBuffer[32];
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);
		pDC->SetTextColor(RGB(0,255,255));

		//绘画用户
		if (pUserData!=NULL)
		{
			//m_ImageHeadBg.BitBlt(pDC->m_hDC,m_ptFace[i].x-2,m_ptFace[i].y-2);
			//用户名字
			//pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
			//pDC->SetTextAlign(i==0||i==2?TA_RIGHT:TA_LEFT);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));
			pDC->TextOut(m_pZhanPaiName[i].x,m_pZhanPaiName[i].y-30,pUserData->szName,lstrlen(pUserData->szName));
			pDC->TextOut(nWidth/2-2*m_nXFace-2*m_nXBorder,2*m_nYFace+m_nYBorder-2*16+i*16+26,pUserData->szName,lstrlen(pUserData->szName));
			//设置上一局成绩
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lForeScore[i]);
			pDC->TextOut(nWidth/2+7,2*m_nYFace+m_nYBorder-2*16+i*16+26,szBuffer,lstrlen(szBuffer));
			//设置总成绩
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lAllScore[i]);
			pDC->TextOut(nWidth/2+2*m_nXFace+2*m_nXBorder,2*m_nYFace+m_nYBorder-2*16+i*16+26,szBuffer,lstrlen(szBuffer));
		
			
			

			//剩余扑克
			if (pUserData->cbUserStatus>=US_PLAY)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("（%ld 张）"),m_bCardCount[i]);
				pDC->TextOut(m_pZhanPaiName[i].x+17*m_nXFace+6*m_nXBorder,m_pZhanPaiName[i].y-30,szBuffer,lstrlen(szBuffer));

				//设置掀人次数
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("掀牌：%d次"),m_bXianCount[i]);
				pDC->TextOut(m_pZhanPaiName[i].x+12*m_nXFace+2*m_nXBorder,m_pZhanPaiName[i].y-30,szBuffer,lstrlen(szBuffer));
				//设置被掀次数
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("被掀：%d次"),m_bBeiXianCount[i]);
				pDC->TextOut(m_pZhanPaiName[i].x+12*m_nXFace+2*m_nXBorder,m_pZhanPaiName[i].y-43,szBuffer,lstrlen(szBuffer));
				//设置占牌数目
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_bZhanPaiCount[i]);
				pDC->TextOut(m_pZhanPaiName[i].x+500+4*m_nXBorder,m_pZhanPaiName[i].y-25,szBuffer);

					//绘画时间标志
			   DrawTime(pDC,nWidth,nHeight);

			}

			//其他信息
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData->wFaceID,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
			
		}
	}
	
	

	//描述信息
	for(BYTE i=0;i<GAME_PLAYER;i++)
		if (m_bCouPaiTitle[i]==true)
		{
			//创建资源
			CFont InfoFont;
			InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

			//设置 DC
			pDC->SetTextColor(RGB(201,229,133));
			pDC->SetTextAlign(TA_CENTER|TA_TOP);
			CFont * pOldFont=pDC->SelectObject(&InfoFont);

			//绘画信息
			if(i%2)
				pDC->TextOut(nWidth/2,m_ptFace[i].y-6*m_nYFace-8,"扣牌");
			else if(m_ptFace[i].x<nWidth/2)
				pDC->TextOut(m_ptFace[i].x+5*m_nXFace,m_ptFace[i].y-8,"扣牌");
			else 
				pDC->TextOut(m_ptFace[i].x-4*m_nXFace,m_ptFace[i].y-8,"扣牌");


			//清理资源
			pDC->SelectObject(pOldFont);
			InfoFont.DeleteObject();
		}
	//描述信息
	for(BYTE i=0;i<GAME_PLAYER;i++)
		if (m_bXianPaiTitle[i]==true)
		{
			//创建资源
			CFont InfoFont;
			InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

			//设置 DC
			pDC->SetTextColor(RGB(201,229,133));
			pDC->SetTextAlign(TA_CENTER|TA_TOP);
			CFont * pOldFont=pDC->SelectObject(&InfoFont);

			//绘画信息
			if(i%2)
				pDC->TextOut(nWidth/2,m_ptFace[i].y-6*m_nYFace-8,"掀牌");
			else if(m_ptFace[i].x<nWidth/2)
				pDC->TextOut(m_ptFace[i].x+5*m_nXFace,m_ptFace[i].y-8,"掀牌");
			else 
				pDC->TextOut(m_ptFace[i].x-4*m_nXFace,m_ptFace[i].y-8,"掀牌");


			//清理资源
			pDC->SelectObject(pOldFont);
			InfoFont.DeleteObject();
		}
	//描述信息
	for(BYTE i=0;i<GAME_PLAYER;i++)
		if (m_bQiangCiTitle[i]==true)
		{
			//创建资源
			CFont InfoFont;
			InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

			//设置 DC
			pDC->SetTextColor(RGB(201,229,133));
			pDC->SetTextAlign(TA_CENTER|TA_TOP);
			CFont * pOldFont=pDC->SelectObject(&InfoFont);

			//绘画信息
			if(i%2)
				pDC->TextOut(nWidth/2,m_ptFace[i].y-6*m_nYFace-8,"抢刺");
			else if(m_ptFace[i].x<nWidth/2)
				pDC->TextOut(m_ptFace[i].x+5*m_nXFace,m_ptFace[i].y-8,"抢刺");
			else 
				pDC->TextOut(m_ptFace[i].x-4*m_nXFace,m_ptFace[i].y-8,"抢刺");


			//清理资源
			pDC->SelectObject(pOldFont);
			InfoFont.DeleteObject();
		}

	return;
}
//用户玩一局要清理的
void CGameClientView::ClearPlayerOneGame()
{
	//游戏变量
	m_wBanker=INVALID_CHAIR;

	//状态变量

	ZeroMemory(m_bCouPaiTitle,sizeof(m_bCouPaiTitle));
	ZeroMemory(m_bXianPaiTitle,sizeof(m_bXianPaiTitle));
	ZeroMemory(m_bQiangCiTitle,sizeof(m_bQiangCiTitle));
	ZeroMemory(m_bZhanPaiCount,sizeof(m_bZhanPaiCount));
	m_lTimeCount=0;
	ZeroMemory(m_bXianCount,sizeof(m_bXianCount));
	ZeroMemory(m_bBeiXianCount,sizeof(m_bBeiXianCount));

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	m_btKouPai.ShowWindow(SW_HIDE);
	m_btXianPai.ShowWindow(SW_HIDE);
	m_btQiangCi.ShowWindow(SW_HIDE);
	m_btGiveUpCiangCi.ShowWindow(SW_HIDE);
	m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_btForeTurn.ShowWindow(SW_HIDE);

	for (WORD i=0;i<GAME_PLAYER;i++) m_UserCardControl[i].SetCardData(NULL,0);
	for(BYTE j=0;j<GAME_PLAYER;j++)m_ZhanCardControl[j].SetCardData(NULL,0);

	return;
}

//用户顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder)
{
	//状态判断
	if (m_bDeasilOrder==bDeasilOrder) return;

	//设置变量
	m_bDeasilOrder=bDeasilOrder;
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);

	//设置界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyGameView(rcClient.Width(),rcClient.Height());

	//刷新界面
	UpdateGameView(NULL);

	return;
}


//基础分数
void CGameClientView::SetBaseScore(LONG lBaseScore)
{
	//设置变量
	m_lBaseScore=lBaseScore;

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//显示提示
void CGameClientView::ShowKouPaiTitle(bool bCouPaiTitle,WORD wChairID)
{
	//设置变量
	m_bCouPaiTitle[wChairID]=bCouPaiTitle;

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//显示提示
void CGameClientView::ShowXianPaiTitle(bool bXianPaiTitle,WORD wChairID)
{
	//设置变量
	m_bXianPaiTitle[wChairID]=bXianPaiTitle;	

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//显示提示
void CGameClientView::ShowQiangCiTitle(bool bQiangCiTitle,WORD wChairID)
{
	//设置变量
	m_bQiangCiTitle[wChairID]=bQiangCiTitle;

	//刷新界面
	UpdateGameView(NULL);

	return;
}
//清理显示
void CGameClientView::HideAllTitle(bool bHide,WORD wChairID)
{
	if(wChairID==INVALID_CHAIR)
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			ShowQiangCiTitle(bHide,i);
			ShowKouPaiTitle(bHide,i);
			ShowXianPaiTitle(bHide,i);
		}
	else
		{
			ShowQiangCiTitle(bHide,wChairID);
			ShowKouPaiTitle(bHide,wChairID);
			ShowXianPaiTitle(bHide,wChairID);
			
		}
		return;
}


//设置放弃
void CGameClientView::SetPassFlag(WORD wChairID, bool bPass)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bPass[i]=bPass;
	}
	else m_bPass[wChairID]=bPass;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//扑克数目
void CGameClientView::SetCardCount(WORD wChairID, BYTE bCardCount)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=bCardCount;
	}
	else m_bCardCount[wChairID]=bCardCount;

	//更新界面
	UpdateGameView(NULL);

	return;
}
//设置占牌数目
void CGameClientView::IncreaseZhanPaiCount(WORD wChairID,BYTE bCardCount)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bZhanPaiCount[i]=bCardCount;
	}
	else m_bZhanPaiCount[wChairID]+=bCardCount;
	if(!bCardCount)
		m_bZhanPaiCount[wChairID]=bCardCount;

	//更新界面
	UpdateGameView(NULL);

}
//设置上一局成绩
void CGameClientView::SetForeScore(WORD wChairID,LONG lScore)
{
	//设置变量
	m_lForeScore[wChairID]=lScore;

	//更新界面
	UpdateGameView(NULL);


}
	//设置总成绩
void CGameClientView::SetAllScore(WORD wChairID,LONG lScore)
{
	//设置变量
	m_lAllScore[wChairID]=lScore;

	//更新界面
	UpdateGameView(NULL);

}
//设置时间
void CGameClientView::SetTimeFlag(LONG lTimeCount,DWORD dCurrentTime)
{
	//设置变量
	int ITemp=112/lTimeCount+1;
	m_lTimeCount=ITemp*dCurrentTime;
	//更新界面
	UpdateGameView(NULL);
}
//设置掀牌增加
void CGameClientView::SetIncreaseXian(WORD wChairID)
{
		//设置变量
	m_bXianCount[wChairID]++;

	//更新界面
	UpdateGameView(NULL);


}
//设置被掀增加
void CGameClientView::SetIncreaseBeiXian(WORD wChairID)
{
	//设置变量
	m_bBeiXianCount[wChairID]++;

	//更新界面
	UpdateGameView(NULL);

}


//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

//出牌按钮
void CGameClientView::OnOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,1,1);
	return;
}

//放弃按钮
void CGameClientView::OnPassCard()
{
	AfxGetMainWnd()->SendMessage(IDM_PASS_CARD,1,1);
	return;
}

//叫分按钮
void CGameClientView::OnKouPai()
{
	AfxGetMainWnd()->SendMessage(IDM_COUPAI,1,1);
	return;
}

//叫分按钮
void CGameClientView::OnXianPai()
{
	AfxGetMainWnd()->SendMessage(IDM_XIANPAI,2,2);
	return;
}

//叫分按钮
void CGameClientView::OnQiangCi()
{
	AfxGetMainWnd()->SendMessage(IDM_QIANGCI,3,3);
	return;
}

//放弃按钮
void CGameClientView::OnGiveUpQingCi()
{
	AfxGetMainWnd()->SendMessage(IDM_GIVEUP_QIANGCI,255,255);
	return;
}
//上一轮提示
void CGameClientView::OnForTurn()
{
	AfxGetMainWnd()->SendMessage(IDM_FORETURN_CARD,0,0);
	return;
}

//出牌提示
void CGameClientView::OnAutoOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_AUTO_OUTCARD,0,0);
	return;
}

//////////////////////////////////////////////////////////////////////////

