#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"

#include "Struct.h"

#include "GameClient.h"

//应用程序对象
extern CGameClientApp theApp;
extern UINT MyThreadProc( LPVOID pParam );

//#define	DEBUG_GAME
//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_TING_CARD					102									//听牌按钮

#define IDC_TEST						999
#define IDC_TEST2						998	
#define IDC_TEST3						997
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
//	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_TING_CARD,OnTingCard)
	ON_BN_CLICKED( IDC_TEST,	OnTest	 )
	ON_BN_CLICKED( IDC_TEST2,   OnTest2  )
	ON_BN_CLICKED( IDC_TEST3,   OnTest3  )
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	xOffInt=0;
	yOffInt=0;
	//标志变量
	m_bOutCard=false;
	m_bWaitOther=false;
	m_bHuangZhuang=false;

	//游戏属性
	m_lCellScore=0l;
	m_wBankerUser=INVALID_CHAIR;

	m_nDiceCount2= -1;
	m_nDiceCount1= -1;
	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageWait.SetLoadInfo(IDB_WAIT_TIP,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageUserFlag.SetLoadInfo(IDB_USER_FLAG,hInstance);
	m_ImageOutCard.SetLoadInfo(IDB_OUT_CARD_TIP,hInstance);
	m_ImageUserAction.SetLoadInfo(IDB_USER_ACTION,hInstance);
	m_ImageActionBack.SetLoadInfo(IDB_ACTION_BACK,hInstance);
	m_ImageHuangZhuang.SetLoadInfo(IDB_HUANG_ZHUANG,hInstance);

	m_ImageTingBack.SetLoadInfo(IDB_TING_BACK,hInstance);
	

	//动画-骰子
	m_ActionDice1.SetLoadInfo( IDB_ACTION_DICE, hInstance);
	CImageHandle HandleDice( &m_ActionDice1);
	m_csActionDice1.cx=m_ActionDice1.GetWidth()/13;
	m_csActionDice1.cy=m_ActionDice1.GetHeight();

	m_ActionDice2.SetLoadInfo( IDB_ACTION_DICE2, hInstance);
	CImageHandle HandleDice2( &m_ActionDice2);
	m_csActionDice2.cx=m_ActionDice2.GetWidth()/6;
	m_csActionDice2.cy=m_ActionDice2.GetHeight();
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

	//创建控件区域
	CRect CreateRect( 0, 0, 40, 30 );

	//变量定义
	enDirection Direction[]={Direction_North,Direction_East,Direction_South,Direction_West};

	//设置控件
	for (WORD i=0;i<4;i++)
	{
		//用户麻将
		m_HeapCard[i].SetDirection(Direction[i]);
		m_TableCard[i].SetDirection(Direction[i]);
		m_DiscardCard[i].SetDirection(Direction[i]);

		//组合麻将
		m_WeaveCard[i][0].SetDisplayItem(true);
		m_WeaveCard[i][1].SetDisplayItem(true);
		m_WeaveCard[i][2].SetDisplayItem(true);
		m_WeaveCard[i][3].SetDisplayItem(true);
		m_WeaveCard[i][0].SetDirection(Direction[i]);
		m_WeaveCard[i][1].SetDirection(Direction[i]);
		m_WeaveCard[i][2].SetDirection(Direction[i]);
		m_WeaveCard[i][3].SetDirection(Direction[i]);
	}

	//设置控件
	m_UserCard[0].SetDirection(Direction_North);
	m_UserCard[1].SetDirection(Direction_East);
	m_UserCard[2].SetDirection(Direction_West);

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_GameScoreWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,100);
	m_ControlWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,10,NULL);

	//创建控件
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);
	m_btStart.SetButtonImage(IDB_BT_START,AfxGetInstanceHandle(),false);

	//听按钮
	m_btTingCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_TING_CARD);
	m_btTingCard.SetButtonImage(IDB_BT_TING_CARD,AfxGetInstanceHandle(),false);

	//时钟控件
	m_Timer.SetBitmap( IDB_TIMERBACK, IDB_TIMERARROW );
	m_Timer.SetSinkWindow( AfxGetMainWnd() );
	m_Timer.SetDesOrder( false );
	m_Timer.Create( NULL, NULL, WS_VISIBLE|WS_CHILD, CreateRect, this, 20 );
	m_Timer.ShowWindow( SW_HIDE);

#ifdef DEBUG_GAME	
	CRect rect( 550, 480, 580,500 ),rect2(580,480,610,500);
	btTest.Create( "测试", WS_CHILD, rect2, this, IDC_TEST );
	btTest2.Create( "出牌", WS_CHILD, rect, this, IDC_TEST2 );
	btTest3.Create( "测试2", WS_CHILD, CRect( 610, 480, 640, 500 ),this,IDC_TEST3 ); 
	btTest2.ShowWindow( SW_SHOW );
	btTest.ShowWindow( SW_SHOW );
	btTest3.ShowWindow( SW_SHOW );
#endif
	return 0;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//标志变量
	m_bOutCard=false;
	m_bWaitOther=false;
	m_bHuangZhuang=false;

	//游戏属性
	m_lCellScore=0l;
	m_wBankerUser=INVALID_CHAIR;

	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));

	//界面设置
	m_btStart.ShowWindow(SW_HIDE);
	m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameScoreWnd.ShowWindow(SW_HIDE);

	//麻将设置
	m_UserCard[0].SetCardData(0,false);
	m_UserCard[1].SetCardData(0,false);
	m_UserCard[2].SetCardData(0,false);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayItem(false);
	m_HandCardControl.SetCardData(NULL,0,0);

	//麻将设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HeapCard[i].SetCardData(0,0,0);
		m_TableCard[i].SetCardData(NULL,0);
		m_DiscardCard[i].SetCardData(NULL,0);
		m_WeaveCard[i][0].SetCardData(NULL,0);
		m_WeaveCard[i][1].SetCardData(NULL,0);
		m_WeaveCard[i][2].SetCardData(NULL,0);
		m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	int xInt;
	int yInt;
	if (nWidth > 0 && nWidth < 1500)
	{
		if (xOffInt != nWidth)
		{
			xOffInt=nWidth;
			yOffInt=nHeight;
			AfxGetMainWnd()->SendMessage(IDM_RESET_UI, nWidth, nHeight);
		}
	}//End if
	//设置坐标
	m_ptFace[0].x=nWidth/2-m_nXFace;
	m_ptFace[0].y=5+m_nYBorder;
	m_ptName[0].x=nWidth/2+5;
	m_ptName[0].y=5+m_nYBorder;
	m_ptTimer[0].x=nWidth/2-m_nXFace-m_nXTimer*2-5;
	m_ptTimer[0].y=5+m_nYBorder;
	m_ptReady[0].x=nWidth/2-m_nXFace-m_nXTimer-5;
	m_ptReady[0].y=5+m_nYBorder+m_nYTimer/2;
	m_UserFlagPos[0].x=nWidth/2-m_nXFace-m_nXTimer*2-32;
	m_UserFlagPos[0].y=5+m_nYBorder;

	xInt = -20;//整体移位
	m_ptFace[1].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptFace[1].x+=xInt;
	m_ptFace[1].y=nHeight/2-m_nYFace-50;
	m_ptName[1].x=nWidth-m_nXBorder-5;
	m_ptName[1].x+=xInt;
	m_ptName[1].y=nHeight/2-45;
	m_ptTimer[1].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptTimer[1].x+=xInt;
	m_ptTimer[1].y=nHeight/2-m_nYFace-55-m_nYTimer;
	m_ptReady[1].x=nWidth-m_nXBorder-m_nXTimer-5;
	m_ptReady[1].x+=xInt;
	m_ptReady[1].y=nHeight/2-m_nYFace-55-m_nYTimer/2;
	m_UserFlagPos[1].x=nWidth-m_nXBorder-28;
	m_UserFlagPos[1].x+=xInt;
	m_UserFlagPos[1].y=nHeight/2-m_nYFace-83-m_nYTimer;

	m_ptFace[2].x=nWidth/2-m_nXFace/2;
	m_ptFace[2].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[2].x=nWidth/2+5+m_nXFace/2;
	m_ptName[2].y=nHeight-m_nYBorder-m_nYFace-2;
	m_ptTimer[2].x=nWidth/2-m_nXFace/2-m_nXTimer*2-5;
	m_ptTimer[2].y=nHeight-m_nYBorder-m_nYTimer-10;
	m_ptReady[2].x=nWidth/2-m_nXTimer-m_nXFace/2-5;
	m_ptReady[2].y=nHeight-m_nYBorder-m_nYTimer/2-10;
	m_UserFlagPos[2].x=nWidth/2-m_nXFace/2-m_nXTimer*2-32;
	m_UserFlagPos[2].y=nHeight-m_nYBorder-m_nYTimer-10;

	m_ptFace[3].x=m_nXBorder+5;
	m_ptFace[3].y=nHeight/2-m_nYFace-50;
	m_ptName[3].x=m_nXBorder+5;
	m_ptName[3].y=nHeight/2-45;
	m_ptTimer[3].x=m_nXBorder+5;
	m_ptTimer[3].y=nHeight/2-m_nYFace-55-m_nYTimer;
	m_ptReady[3].x=m_nXBorder+5+m_nXTimer;
	m_ptReady[3].y=nHeight/2-m_nYFace-55-m_nYTimer/2;
	m_UserFlagPos[3].x=m_nXBorder+8;
	m_UserFlagPos[3].y=nHeight/2-m_nYFace-83-m_nYTimer;

	//用户麻将
	m_UserCard[0].SetControlPoint(nWidth/2-200,m_nYBorder+m_nYFace+10);
	m_UserCard[1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-49,nHeight/2-255);
	m_UserCard[2].SetControlPoint(m_nXBorder+m_nXFace+30,nHeight/2+177);
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-m_nYBorder-15,enXCenter,enYBottom);

	//桌面麻将
	m_TableCard[0].SetControlPoint(nWidth/2-179,m_nYBorder+m_nYFace+10);
	m_TableCard[1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-30,nHeight/2-197);
	m_TableCard[2].SetControlPoint(nWidth/2+185,nHeight-m_nYFace-m_nYBorder-15);
	m_TableCard[3].SetControlPoint(m_nXBorder+m_nXFace+30,nHeight/2+97);

	//堆积麻将
	int nXCenter=nWidth/2;
	int nYCenter=nHeight/2-40;
	m_HeapCard[0].SetControlPoint(nXCenter-98,nYCenter-108);
	m_HeapCard[1].SetControlPoint(nXCenter+101,nYCenter-68);
	m_HeapCard[2].SetControlPoint(nXCenter-192,nYCenter+73);
	m_HeapCard[3].SetControlPoint(nXCenter-123,nYCenter-172);

	//丢弃麻将
	m_DiscardCard[0].SetControlPoint(nXCenter-98,nYCenter-110);
	m_DiscardCard[1].SetControlPoint(nXCenter+125,nYCenter-68);
	m_DiscardCard[2].SetControlPoint(nXCenter+71,nYCenter+112);
	m_DiscardCard[3].SetControlPoint(nXCenter-126,nYCenter+59);

	//组合麻将
	m_WeaveCard[0][0].SetControlPoint(nWidth/2+145,m_nYBorder+m_nYFace+52);
	m_WeaveCard[0][1].SetControlPoint(nWidth/2+55,m_nYBorder+m_nYFace+52);
	m_WeaveCard[0][2].SetControlPoint(nWidth/2-35,m_nYBorder+m_nYFace+52);
	m_WeaveCard[0][3].SetControlPoint(nWidth/2-125,m_nYBorder+m_nYFace+52);

	//组合麻将
	m_WeaveCard[1][0].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2+100);
	m_WeaveCard[1][1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2+20);
	m_WeaveCard[1][2].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2-60);
	m_WeaveCard[1][3].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2-140);

	//组合麻将
	m_WeaveCard[2][0].SetControlPoint(nWidth/2-190,nHeight-m_nYFace-m_nYBorder-68);
	m_WeaveCard[2][1].SetControlPoint(nWidth/2-100,nHeight-m_nYFace-m_nYBorder-68);
	m_WeaveCard[2][2].SetControlPoint(nWidth/2-10,nHeight-m_nYFace-m_nYBorder-68);
	m_WeaveCard[2][3].SetControlPoint(nWidth/2+80,nHeight-m_nYFace-m_nYBorder-68);

	//组合麻将
	m_WeaveCard[3][0].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2-200);
	m_WeaveCard[3][1].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2-120);
	m_WeaveCard[3][2].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2-40);
	m_WeaveCard[3][3].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2+40);

	//控制窗口
	m_ControlWnd.SetBenchmarkPos(nWidth/2+298,nHeight-m_nYBorder-120);

	//调整成绩
	CRect rcScore;
	m_GameScoreWnd.GetWindowRect(&rcScore);
	m_GameScoreWnd.MoveWindow((nWidth-rcScore.Width())/2,(nHeight-rcScore.Height())/2-30,rcScore.Width(),rcScore.Height());

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//移动调整
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-155-m_nYBorder,0,0,uFlags);

	//移动调整
	m_btTingCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btTingCard,NULL,(nWidth-rcButton.Width() - m_nXBorder),nHeight-105-m_nYBorder,0,0,uFlags);


	//结束移动
	EndDeferWindowPos(hDwp);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sDrawGameView.log",strTime);
	if (nWidth > 0 && nWidth < 1500)
	{
		if (xOffInt != nWidth)
		{
			xOffInt=nWidth;
			yOffInt=nHeight;

			AfxGetMainWnd()->SendMessage(IDM_RESET_UI, nWidth, nHeight);
		}

	}

	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);

				strTemp.Format("DrawViewImage  m_tDice=%d", m_tDice);
			theApp.WriteLog(strFile, strTemp);
#ifdef DEBUG_GAME
			if( m_tDice>0 && m_tDice<12)
			{
				ActionDice1( pDC,m_tDice);

			strTemp.Format("ActionDice1 ");
			theApp.WriteLog(strFile, strTemp);

			}
			else
			{
				if( m_tDice== 12)
				{
					ActionDice2( pDC,m_nDiceCount1,m_nDiceCount2);

			strTemp.Format("ActionDice2");
			theApp.WriteLog(strFile, strTemp);

				}//End if
			}//End if
#endif
	int iXPos;
	int iYPos;
	for (int i=0; i < GAME_PLAYER; i++)
	{
		iXPos=	m_PtVideo[i].x;
		iYPos=m_PtVideo[i].y;

		if ( 1 == m_uVideoInt[i])
		DrawOtherVideoPic(  pDC,  iXPos,  iYPos);
	}//End for

	//自己
	int myX,myY,myW,myH;
	myW= MYSLEF_VIDEO_W;
	myH= MYSLEF_VIDEO_H;
	myX=  nWidth-myW-10-3;//
	myY=  nHeight-myH-10-35; 
	DrawMyselfVideoPic( pDC,  myX,  myY);
	//绘画用户
	pDC->SetTextColor(RGB(255,255,0));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign((i==1)?(TA_RIGHT|TA_TOP):(TA_LEFT|TA_TOP));
			DrawTextString(pDC,pUserData->szName,RGB(240,240,240),RGB(50,50,50),m_ptName[i].x,m_ptName[i].y);

			if( m_tDice>0 && m_tDice<12)
			{
				ActionDice1( pDC,m_tDice);

				strTemp.Format("ActionDice1 ");
				theApp.WriteLog(strFile, strTemp);

			}
			else
			{
				if( m_tDice== 12)
				{
					ActionDice2( pDC,m_nDiceCount1,m_nDiceCount2);
					strTemp.Format("ActionDice2  m_nDiceCount1=%d,m_nDiceCount2=%d", m_nDiceCount1,m_nDiceCount2);
					theApp.WriteLog(strFile, strTemp);
		//	m_tDice++;
				}//End if
			}//End if

			//其他信息
			WORD wUserTimer=GetUserTimer(i);
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData->wFaceID,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}//End for (WORD i=0;i<GAME_PLAYER;i++)

	//用户标志
	if (m_wBankerUser!=INVALID_CHAIR)
	{
		//加载位图
		CImageHandle ImageHandle(&m_ImageUserFlag);
		int nImageWidth=m_ImageUserFlag.GetWidth()/4;
		int nImageHeight=m_ImageUserFlag.GetHeight();

		//绘画标志
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wIndex=((i+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
			m_ImageUserFlag.BitBlt(pDC->m_hDC,m_UserFlagPos[i].x,m_UserFlagPos[i].y,nImageWidth,nImageHeight,nImageWidth*wIndex,0);
		}
	}

	//桌面麻将
	for (WORD i=0;i<4;i++)
	{
		m_TableCard[i].DrawCardControl(pDC);
		m_DiscardCard[i].DrawCardControl(pDC);
		m_WeaveCard[i][0].DrawCardControl(pDC);
		m_WeaveCard[i][1].DrawCardControl(pDC);
		m_WeaveCard[i][2].DrawCardControl(pDC);
		m_WeaveCard[i][3].DrawCardControl(pDC);
	}

	//堆积麻将
	m_HeapCard[3].DrawCardControl(pDC);
	m_HeapCard[0].DrawCardControl(pDC);
	m_HeapCard[1].DrawCardControl(pDC);
	m_HeapCard[2].DrawCardControl(pDC);

	//用户麻将
	m_UserCard[0].DrawCardControl(pDC);
	m_UserCard[1].DrawCardControl(pDC);
	m_UserCard[2].DrawCardControl(pDC);
	m_HandCardControl.DrawCardControl(pDC);

	//出牌提示
	if (m_bOutCard==true)
	{
		CImageHandle HandleOutCard(&m_ImageOutCard);
		m_ImageOutCard.AlphaDrawImage(pDC,(nWidth-m_ImageOutCard.GetWidth())/2,nHeight-150,RGB(255,0,255));
	}

	//等待提示
	if (m_bWaitOther==true)
	{
		CImageHandle HandleWait(&m_ImageWait);
		m_ImageWait.AlphaDrawImage(pDC,(nWidth-m_ImageWait.GetWidth())/2,nHeight-150,RGB(255,0,255));
	}

	//荒庄标志
	if (m_bHuangZhuang==true)
	{
		CImageHandle HandleHuangZhuang(&m_ImageHuangZhuang);
		m_ImageHuangZhuang.AlphaDrawImage(pDC,(nWidth-m_ImageHuangZhuang.GetWidth())/2,nHeight/2-103,RGB(255,0,255));
	}

	//用户状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_wOutCardUser==i)||(m_cbUserAction[i]!=0))
		{
			//计算位置
			int nXPos=0,nYPos=0;
			switch (i)
			{
			case 0:	//北向
				{
					nXPos=nWidth/2-32;
					nYPos=m_nYBorder+95;
					break;
				}
			case 1:	//东向
				{
					nXPos=nWidth-m_nXBorder-170;
					nYPos=nHeight/2-71;
					break;
				}
			case 2:	//南向
				{
					nXPos=nWidth/2-32;
					nYPos=nHeight-m_nYBorder-220;
					break;
				}
			case 3:	//西向
				{
					nXPos=m_nXBorder+115;
					nYPos=nHeight/2-71;
					break;
				}
			}

			//动作背景
			CImageHandle ImageHandle(&m_ImageActionBack);
			m_ImageActionBack.AlphaDrawImage(pDC,nXPos,nYPos,m_ImageActionBack.GetWidth()/4,m_ImageActionBack.GetHeight(),
				i*m_ImageActionBack.GetWidth()/4,0,RGB(255,0,255));

			//画听标记
															
			int x= m_UserFlagPos[i].x + 60 ;
			int y= m_UserFlagPos[i].y ;
			if( m_bTingFlag[i] )
			{
				ShowTingPic( pDC, x,  y );
			}//End if

			//绘画动作
			if (m_cbUserAction[i]!=WIK_NULL)
			{
				//变量定义
				int nXImagePos=0;
				CImageHandle ImageHandle(&m_ImageUserAction);
				int nItemWidth=m_ImageUserAction.GetWidth()/7;

				//计算位置
				if (m_cbUserAction[i]&WIK_PENG) nXImagePos=nItemWidth;
				else if (m_cbUserAction[i]&WIK_FILL) nXImagePos=nItemWidth*2;
				else if (m_cbUserAction[i]&WIK_GANG) nXImagePos=nItemWidth*3;
				else if (m_cbUserAction[i]&WIK_CHI_HU) nXImagePos=nItemWidth*4;

				//绘画动作
				int nItemHeight=m_ImageUserAction.GetHeight();
				m_ImageUserAction.BitBlt(pDC->m_hDC,nXPos+13,nYPos+15,nItemWidth,nItemHeight,nXImagePos,0);
			}
			else
			{
				//绘画麻将
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData,nXPos+18,nYPos+15);
			}
		}
	}

	return;
}

//基础积分
void CGameClientView::SetCellScore(LONG lCellScore)
{
	//设置麻将
	if (lCellScore!=m_lCellScore)
	{
		//设置变量
		m_lCellScore=lCellScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//海底麻将
void CGameClientView::SetHuangZhuang(bool bHuangZhuang)
{
	//设置麻将
	if (bHuangZhuang!=m_bHuangZhuang)
	{
		//设置变量
		m_bHuangZhuang=bHuangZhuang;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//庄家用户
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	//设置用户
	if (wBankerUser!=m_wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//状态标志
void CGameClientView::SetStatusFlag(bool bOutCard, bool bWaitOther)
{
	//设置变量
	m_bOutCard=bOutCard;
	m_bWaitOther=bWaitOther;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//出牌信息
void CGameClientView::SetOutCardInfo(WORD wViewChairID, BYTE cbCardData)
{
	//设置变量
	m_cbCardData=cbCardData;
	m_wOutCardUser=wViewChairID;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//动作信息
void CGameClientView::SetUserAction(WORD wViewChairID, BYTE bUserAction)
{
	//设置变量
	if (wViewChairID<GAME_PLAYER) m_cbUserAction[wViewChairID]=bUserAction;
	else ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));

	//更新界面
	UpdateGameView(NULL);

	return;
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}

//光标消息
//鼠标划过变动
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//点击测试
	bool bRePaint=false;
	bool bHandle=m_HandCardControl.OnEventSetCursor(MousePoint,bRePaint);

	//重画控制
	if (bRePaint==true) UpdateGameView(NULL);

	//光标控制
	if (bHandle==false) __super::OnSetCursor(pWnd,nHitTest,uMessage);

	return TRUE;
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//获取麻将
	BYTE cbHoverCard=m_HandCardControl.GetHoverCard();
	if (cbHoverCard!=0) AfxGetMainWnd()->PostMessage(IDM_OUT_CARD,cbHoverCard,cbHoverCard);

	return;
}

//开始按钮
void CGameClientView::OnStart()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);

	return;
}

//听牌按钮
void CGameClientView::OnTingCard()
{
	AfxGetMainWnd()->SendMessage(IDM_TING_CARD,0,0);
	return;
}

///////////////////////////////////////////////动画/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//骰子动画
void CGameClientView::ActionDice1( CDC *pDC, int nFrame )
{
	CImageHandle ImageHandle( &m_ActionDice1);
	LONG lWidth, lHeight;
	lWidth = m_csActionDice1.cx;
	lHeight = m_csActionDice1.cy;

	m_ActionDice1.AlphaDrawImage( pDC, PS_BX_DICE, PS_BY_DICE, lWidth, lHeight, lWidth*nFrame, 0, COLORTRANS);
}

//骰子动画2
void CGameClientView::ActionDice2( CDC *pDC,int pos1, int pos2 )
{
	CImageHandle ImageHandle( &m_ActionDice2);
	LONG lWidth, lHeight;
	lWidth = m_csActionDice2.cx;
	lHeight = m_csActionDice2.cy;

	m_ActionDice2.AlphaDrawImage( pDC, PS_BX_DICE1, PS_BY_DICE1, lWidth, lHeight, lWidth*pos1, 0, COLORTRANS);
	m_ActionDice2.AlphaDrawImage( pDC, PS_BX_DICE2, PS_BY_DICE2, lWidth, lHeight, lWidth*pos2, 0, COLORTRANS);
}

//画听标记
void CGameClientView::ShowTingPic( CDC *pDC,int x, int y )
{
	CImageHandle ImageHandle( &m_ImageTingBack);
	LONG lWidth, lHeight;
	lWidth = 44;
	lHeight = 44;

	m_ImageTingBack.AlphaDrawImage( pDC, x, y, lWidth, lHeight, 0, 0, COLORTRANS);

}

void CGameClientView::OnTest()
{
	/*Del by doctor 20071214
#ifdef DEBUG_GAME
	AfxGetMainWnd()->SendMessage( IDM_TING_MJ, 4, 4 );
	//for(int i=0;i<4;i++)
	//{
	//	m_vcOutMjs[i].push_back(2);
	//}
	//MessageBox("结束");
	SetTimer(ID_OUTFLAG_TIME, 150,(TIMERPROC) NULL);
	//m_bEnd= true;
	UpdateGameView(NULL);
	//m_nChiMode=9;
	//if( m_byTouchMjCount[1] == 0 )return;
	////m_byTouchMjCount[1] -=1;
	////m_byTouchMjCount[0] -=1;
	//for( BYTE i=0; i<4; i++ )
	//{
	//	m_byTouchMjCount[i] -= 1;
	//}
	////m_nChiMode = 7;
	////m_byOutMj = 2;
	//m_Timer.SetTimerUp( 0, 10, false );

	//UpdateGameView( NULL );
#endif 
	//AfxGetMainWnd()->SendMessage( IDM_OUT_MJ, 2,2 );
	*/
}

void CGameClientView::OnTest2()
{

//	AfxGetMainWnd()->PostMessage(IDM_SHOW_SZ,0,0);
	AfxGetMainWnd()->SendMessage(IDM_SHOW_SZ,0,0);
	return;
		m_nDiceCount1 = 3;
		m_nDiceCount2 = 5;


//	strTemp.Format("m_nDiceCount1  =%d, m_GameClientView.m_nDiceCount2", m_GameClientView.m_nDiceCount1,m_GameClientView.m_nDiceCount2);
//	theApp.WriteLog(strFile, strTemp);

			m_tDice = 1;
			AfxBeginThread( MyThreadProc, this);

			/*
	//AfxMessageBox("OnTest2");
m_nDiceCount1 = 4;
m_nDiceCount2 = 3;
	m_tDice=0;

				//	m_tDice ++;
			m_tDice = 12;
		UpdateGameView( NULL );
//SetTimer( ID_DICE_TIME, 3000,0 );
*/
	/*Del by doctor 20071214
#ifdef DEBUG_GAME
	m_nActionOutFlag= 0;
	m_bPlay= true;
	//MessageBox("开始");
	//m_tDice=0;
	//SetTimer( ID_DICE_TIME, 1,0 );
	//m_bPlay=true;
	//m_btGang.EnableWindow( true );
	for( int i=0; i<4; i++ )
	{
		m_vcOutMjs[i].push_back( 34 );
	}
	UpdateGameView( NULL );

	//for( WORD i=0; i<GAME_PLAYER; i++)
	//{
	//	//const tagUserData *pUserData = GetUserData(i);
	//	m_scoreView.SetGameScore(i,"aa", 24);
	//}
	char *kk = new char[255];
	kk = "鸡胡 +0番";
	m_scoreView.SetFangText( kk );
		m_scoreView.ShowWindow( SW_SHOW);
#endif 
		*/
}

void CGameClientView::OnTest3()
{
	/*Del by doctor 20071214
#ifdef DEBUG_GAME
	m_nActionOutFlag= (m_nActionOutFlag+1)%4;
	for( BYTE i=0; i<GAME_PLAYER; i++ )
	{
		m_byHandMjCount[i] -= 3;
		//m_vcOutMjs[i].clear();
		//风位初始化
		//m_byFengWei[i] = 255;
	}
	//m_bPlay= true;
	//for( int i=0; i<4; i++ )
	//{
	//	if( i != 2 )
	//		m_byHandMjCount[i] -= 1;
	//}
	//m_scoreView.ShowWindow(SW_HIDE);
	//m_nChiMode = 0;
	UpdateGameView( NULL );
#endif 
	*/
}

//无法变化鼠标
void CGameClientView::OnMouseMove(UINT nFlags, CPoint MousePoint)
{
//	m_Logic.OnMouseMove(Point);
//	SendMessage(IDM_OUT_CARD,0,0);
//	Invalidate();
	//获取光标
//	CPoint MousePoint;
//	GetCursorPos(&MousePoint);
//	ScreenToClient(&MousePoint);

	//点击测试
	bool bRePaint=false;
	bool bHandle=m_HandCardControl.OnEventSetCursor(MousePoint,bRePaint);

	//重画控制
	if (bRePaint==true) UpdateGameView(NULL);

}