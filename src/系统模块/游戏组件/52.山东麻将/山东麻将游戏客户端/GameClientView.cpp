#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include ".\gameclientview.h"
#include "ScoreView.h"
#include "Struct.h"


//#define	DEBUG_GAME
////////////////////////////////////////////////////////////////////////////////
//按扭标识
#define	IDC_START						100				//开始按扭
#define IDC_HU_MJ						101				//胡牌按扭
#define IDC_TING_MJ						102				//听牌按扭
#define IDC_GANG_MJ						103				//杠牌按扭
#define IDC_PENG_MJ						104				//碰牌按扭
#define IDC_CHI_MJ						105				//吃牌按扭
#define IDC_GIVEUP						106				//放弃按扭

#define IDC_CANCEL						107				//关闭按扭
#define IDC_SHOWMINI					108				//最小化
#define IDC_GAMESET						109				//设置

#define IDC_TEST						999
#define IDC_TEST2						998	
#define IDC_TEST3						997

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED( IDC_START,	OnStart  )
	ON_BN_CLICKED( IDC_HU_MJ,	OnHuMj	 )
	ON_BN_CLICKED( IDC_TING_MJ, OnTingMj )
	ON_BN_CLICKED( IDC_GANG_MJ, OnGangMj )
	ON_BN_CLICKED( IDC_PENG_MJ, OnPengMj )
	ON_BN_CLICKED( IDC_CHI_MJ,  OnChiMj  )
	ON_BN_CLICKED( IDC_GIVEUP,  OnGiveUp )

	ON_BN_CLICKED( IDC_CANCEL, OnCancel )
	ON_BN_CLICKED( IDC_SHOWMINI, OnShowMini)
	ON_BN_CLICKED( IDC_GAMESET, OnGameSet)

	ON_BN_CLICKED( IDC_TEST,	OnTest	 )
	ON_BN_CLICKED( IDC_TEST2,   OnTest2  )
	ON_BN_CLICKED( IDC_TEST3,   OnTest3  )
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{

	//配置变量
	m_bDeasilOrder=		true;
	m_bLookOn=			true;
	m_bPlay=			false;
	m_bAction=			false;
	m_bActionOut=		false;
	m_nActionOutFlag=	-1;

	m_nChiMode=		0;
	m_nGangMode=	0;
	m_nChiType=		0;
	m_nGangType=	0;

	m_iUpMj		   = -1;
	m_wLastUser    = -1;
	m_wCurrentUser = -1;

	m_nDiceCount2= -1;
	m_nDiceCount1= -1;

	m_tDice= -1;
	m_tOut= -1;
	m_tOutFlag=-1;

	m_bEndEmpty= false;
	m_bEnd= false;

	for( BYTE i=0; i<3; i++ )
	{
		m_byGangMj[i] = -1;
	}

	//test
	//初始化手中麻牌
	for( int i=0; i<4; i++)
	{
		for( BYTE j=0; j<14; j++ )
		{
			m_byHandMjs[i][j] = 0;
		}
	}

#ifdef DEBUG_GAME
	for( int i=0; i<4; i++)
	{
		for( BYTE j=0; j<7; j++ )
		{
			m_byHandMjs[i][j] = 15;
		}
	}
	m_byOutMj = 4;
#endif
//#ifdef DEBUG_GAME
	for( int i=0; i<4; i++ )
	{
		m_byHandMjCount[i] =14;
	}
//#endif
	for( BYTE i=0; i<GAME_PLAYER; i++ )
	{
		m_byTouchMjCount[i] = COUNT_MJ_TOUCH;
		m_vcOutMjs[i].clear();
		//风位初始化
		m_byFengWei[i] = 255;
	}
	m_wQuanWei= 0;
	m_wQuanWeiBanker= 255;
	
	memset( m_hu, 0, sizeof(MJ_HU)*4 );
	//加载资源
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo( IDB_VIEW_BACK, hInstance );
	m_ImageBack1.SetLoadInfo(IDB_BACK,hInstance);

	//摸牌区
	m_ImageTouchMj[0].SetLoadInfo( IDB_MJ_TOUCH02, hInstance );
	m_ImageTouchMj[1].SetLoadInfo( IDB_MJ_TOUCH13, hInstance );
	m_ImageTouchMj[2].SetLoadInfo( IDB_MJ_TOUCH02, hInstance );
	m_ImageTouchMj[3].SetLoadInfo( IDB_MJ_TOUCH13, hInstance );

	//出牌区
	m_ImageOutMj[0].SetLoadInfo( IDB_MJ_OUT0, hInstance );
	m_ImageOutMj[1].SetLoadInfo( IDB_MJ_OUT1, hInstance );
	m_ImageOutMj[2].SetLoadInfo( IDB_MJ_OUT2, hInstance );
	m_ImageOutMj[3].SetLoadInfo( IDB_MJ_OUT3, hInstance );

	//牌面区
	m_ImageFaceMj[0].SetLoadInfo( IDB_MJ_FACE0, hInstance );
	m_ImageFaceMj[1].SetLoadInfo( IDB_MJ_FACE1, hInstance );
	m_ImageFaceMj[2].SetLoadInfo( IDB_MJ_FACE2, hInstance );
	m_ImageBackMj.SetLoadInfo( IDB_MJ_BACK, hInstance );
	m_ImageFaceMj[3].SetLoadInfo( IDB_MJ_FACE3, hInstance );

	//遮罩图
	m_ImageMaskMj.SetLoadInfo( IDB_MJ_MASK, hInstance );

	//胡,吃碰牌区
	m_ImageHuMj[0].SetLoadInfo( IDB_MJ_OUT0, hInstance );
	m_ImageHuMj[1].SetLoadInfo( IDB_MJ_OUT1, hInstance );
	m_ImageHuMj[2].SetLoadInfo( IDB_MJ_HU2, hInstance );
	m_ImageHuMj[3].SetLoadInfo( IDB_MJ_OUT3, hInstance );
	
	//流局图
	m_ImageEndEmpty.SetLoadInfo( IDB_EMPTY,hInstance);

	//风位图
	m_ImageFengWei.SetLoadInfo( IDB_MJ_FENGWEI, hInstance );
	CImageHandle HandleMjFengWei( &m_ImageFengWei );
	m_csFengWei.cx = m_ImageFengWei.GetWidth()/5;
	m_csFengWei.cy = m_ImageFengWei.GetHeight();

	//m_ImageButtonBk.SetLoadInfo( IDB_BT_BACK, hInstance );

	//圈风图
	//m_ImageQuanFengBk.SetLoadInfo( IDB_FENG_QUAN_BK, hInstance);
	//CImageHandle HandleQuanFengBk( &m_ImageQuanFengBk);

	//m_ImageQuanFeng.SetLoadInfo( IDB_FENG_QUAN, hInstance);
	//CImageHandle HandleQuanFeng( &m_ImageQuanFeng);
	//m_csQuanFeng.cx=m_ImageQuanFeng.GetWidth()/5;
	//m_csQuanFeng.cy=m_ImageQuanFeng.GetHeight();

	//动画-骰子
	m_ActionDice1.SetLoadInfo( IDB_ACTION_DICE, hInstance);
	CImageHandle HandleDice( &m_ActionDice1);
	m_csActionDice1.cx=m_ActionDice1.GetWidth()/13;
	m_csActionDice1.cy=m_ActionDice1.GetHeight();

	m_ActionDice2.SetLoadInfo( IDB_ACTION_DICE2, hInstance);
	CImageHandle HandleDice2( &m_ActionDice2);
	m_csActionDice2.cx=m_ActionDice2.GetWidth()/6;
	m_csActionDice2.cy=m_ActionDice2.GetHeight();

	//动画-出牌动画
	m_ActionOut.SetLoadInfo( IDB_ACTION_OUT, hInstance);
	CImageHandle HandleOut( &m_ActionOut);
	m_csActionOut.cx=m_ActionOut.GetWidth()/10;
	m_csActionOut.cy=m_ActionOut.GetHeight();

	//动画-出牌标志
	m_ActionOutFlag.SetLoadInfo( IDB_ACTION_OUTFLAG, hInstance);
	CImageHandle HandleOutFlag( &m_ActionOutFlag);
	m_csActionOutFlag.cx=m_ActionOutFlag.GetWidth()/16;
	m_csActionOutFlag.cy=m_ActionOutFlag.GetHeight();

	//操作图
	m_ActionOption.SetLoadInfo( IDB_ACTION_OPTION, hInstance);
	CImageHandle HandleOutOption( &m_ActionOption );
	m_csActionOption.cx=m_ActionOption.GetWidth()/4;
	m_csActionOption.cy=m_ActionOption.GetHeight();

	//设置麻将大小
	
	for( int i=0; i<GAME_PLAYER; i++ )
	{
		//出牌区
		CImageHandle HandleOMj( &m_ImageOutMj[i] );
		m_csOutMj[i].cx = m_ImageOutMj[i].GetWidth()/35;
		m_csOutMj[i].cy = m_ImageOutMj[i].GetHeight();

		//牌面区
		CImageHandle HandleFMj( &m_ImageFaceMj[i] );
		m_csFaceMj[i].cx  = m_ImageFaceMj[i].GetWidth();
		m_csFaceMj[i].cy  = m_ImageFaceMj[i].GetHeight();

		//摸牌区
		CImageHandle HandleTMj( &m_ImageTouchMj[i] );
		m_csTouchMj[i].cx = m_ImageTouchMj[i].GetWidth();
		m_csTouchMj[i].cy = m_ImageTouchMj[i].GetHeight();
		//胡牌区
		CImageHandle HandleHMj( &m_ImageHuMj[i] );
		m_csHuMj[i].cx = m_ImageHuMj[i].GetWidth()/35;
		m_csHuMj[i].cy = m_ImageHuMj[i].GetHeight();

	}
	CImageHandle HandleFMj( &m_ImageFaceMj[2] );
	m_csFaceMj[2].cx = m_ImageFaceMj[2].GetWidth()/43;

	
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
	m_scoreView.Create( IDD_GAME_SCORE, this );

	//创建按扭
	m_btStart.Create( TEXT(""),WS_CHILD, CreateRect, this, IDC_START );
	m_btChi.Create( TEXT(""), WS_CHILD, CreateRect, this, IDC_CHI_MJ );
	m_btHu.Create( TEXT(""), WS_CHILD, CreateRect, this, IDC_HU_MJ );
	m_btPeng.Create( TEXT(""), WS_CHILD, CreateRect, this, IDC_PENG_MJ );
	//m_btTing.Create( TEXT(""), WS_CHILD, CreateRect, this, IDC_TING_MJ );
	m_btGang.Create( TEXT(""), WS_CHILD, CreateRect, this, IDC_GANG_MJ );
	m_btGiveUp.Create( TEXT(""), WS_CHILD, CreateRect,this, IDC_GIVEUP );

	m_btCancel.Create( TEXT(""), WS_CHILD, CreateRect,this, IDC_CANCEL );
	m_btShowMini.Create( TEXT(""), WS_CHILD, CreateRect,this, IDC_SHOWMINI );
	m_btGameSet.Create(  TEXT(""), WS_CHILD, CreateRect,this, IDC_GAMESET );

	//设置按扭( 载图 )
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_btStart.SetButtonImage( IDB_START, hInstance, 0 );
	m_btChi.SetButtonImage( IDB_CHI, hInstance, 0 );
	m_btPeng.SetButtonImage( IDB_PENG, hInstance, 0 );
	//m_btTing.SetButtonImage( IDB_TING, hInstance, 0 );
	m_btGang.SetButtonImage( IDB_GANG, hInstance, 0 );
	m_btHu.SetButtonImage( IDB_HU, hInstance, 0 );
	m_btGiveUp.SetButtonImage( IDB_GIVE_UP, hInstance, 0 );
	m_btCancel.SetButtonImage( IDB_BT_CANCEL, hInstance, 0 );
	m_btShowMini.SetButtonImage( IDB_BT_SHOWMINI, hInstance, 0 );
	m_btGameSet.SetButtonImage(  IDB_BT_GAMESET, hInstance, 0 );

	m_btStart.ShowWindow( SW_HIDE );
	m_btHu.ShowWindow( SW_HIDE );
	//m_btTing.ShowWindow( SW_HIDE );
	m_btGang.ShowWindow( SW_HIDE );
	m_btPeng.ShowWindow( SW_HIDE );
	m_btChi.ShowWindow( SW_HIDE );
	m_btGiveUp.ShowWindow( SW_HIDE );

	//时钟控件
	m_Timer.SetBitmap( IDB_TIMERBACK, IDB_TIMERARROW );
	m_Timer.SetSinkWindow( AfxGetMainWnd() );
	m_Timer.SetDesOrder( false );
	m_Timer.Create( NULL, NULL, WS_VISIBLE|WS_CHILD, CreateRect, this, 20 );
	m_Timer.ShowWindow( SW_HIDE);
	//test
	//m_btChi.Create( TEXT("吃牌"), WS_CHILD, CreateRect, this, IDC_CHI_MJ );
	//m_btHu.Create( TEXT("胡牌"), WS_CHILD, CreateRect, this, IDC_HU_MJ );
	//m_btPeng.Create( TEXT("碰牌"), WS_CHILD, CreateRect, this, IDC_PENG_MJ );
	//m_btTing.Create( TEXT("听牌"), WS_CHILD, CreateRect, this, IDC_TING_MJ );
	//m_btGang.Create( TEXT("杠牌"), WS_CHILD, CreateRect, this, IDC_GANG_MJ );
	//m_btGiveUp.Create( TEXT("放弃"), WS_CHILD, CreateRect,this, IDC_GIVEUP );
	//m_btStart.ShowWindow( SW_SHOW );
	//m_btChi.ShowWindow( SW_SHOW );
	//m_btPeng.ShowWindow( SW_SHOW );
	//m_btGang.ShowWindow( SW_SHOW );
	//m_btHu.ShowWindow( SW_SHOW );
	////m_btTing.ShowWindow( SW_SHOW );
	//m_btGiveUp.ShowWindow( SW_SHOW );

	m_scoreView.ShowWindow( SW_HIDE );

	//m_btCancel.ShowWindow( SW_SHOW );
	//m_btShowMini.ShowWindow( SW_SHOW );
	//m_btGameSet.ShowWindow( SW_SHOW );

	//btTest.SetButtonImage( IDB_START, hInstance, 0 );
#ifdef DEBUG_GAME	
	CRect rect( 550, 480, 580,500 ),rect2(580,480,610,500);
	btTest.Create( "测试", WS_CHILD, rect2, this, IDC_TEST );
	btTest2.Create( "出牌", WS_CHILD, rect, this, IDC_TEST2 );
	btTest3.Create( "测试2", WS_CHILD, CRect( 610, 480, 640, 500 ),this,IDC_TEST3 ); 
	btTest2.ShowWindow( SW_SHOW );
	btTest.ShowWindow( SW_SHOW );
	btTest3.ShowWindow( SW_SHOW );
#endif
	m_btHu.EnableWindow( false );
	//m_btTing.EnableWindow( false );
	m_btGang.EnableWindow( false );
	m_btPeng.EnableWindow( false );
	m_btChi.EnableWindow( false );
	m_btGiveUp.EnableWindow( false );

	//读取变量
	//m_bDeasilOrder = AfxGetApp()->GetProfileInt( TEXT("GameOption"), TEXT("DeasilOrder"), FALSE )?true:false;

	//SetTimer(ID_OUTFLAG_TIME, 50,(TIMERPROC) NULL);
	return 0;
}

//重置界面
void CGameClientView::ResetGameView()
{
	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	m_ptFace[0].x	=	nWidth/2-m_nXFace;
	m_ptFace[0].y	=	m_nYBorder+REVISE_PS;
	m_ptName[0].x	=	m_ptFace[0].x+m_nXFace+REVISE_PS;
	m_ptName[0].y	=	m_ptFace[0].y+m_nYFace/2;
	m_ptTimer[0].x	=	m_ptFace[0].x;
	m_ptTimer[0].y	=	m_ptFace[0].y+m_nYFace+REVISE_PS;
	m_ptReady[0].x	=	m_ptFace[0].x+m_nXTimer;
	m_ptReady[0].y	=	m_ptFace[0].y+m_nYFace+m_nYTimer/2+REVISE_PS;
	//对家麻将区
	m_ptFaceMj[0].x  = nWidth/3+50;
	m_ptFaceMj[0].y  = m_ptFace[0].y+m_nYFace+REVISE_MJ_FACE-15;
	m_ptTouchMjs[0].x = nWidth/3-10;
	m_ptTouchMjs[0].y = m_ptFaceMj[0].y+m_csFaceMj[0].cy+REVISE_MJ_FACE-15;
	m_ptOutMjs[0].x   = nWidth/3;
	m_ptOutMjs[0].y   = m_ptTouchMjs[0].y+REVISE_MJ_FACE+5;
	m_ptHuMjs[0].x	  = m_ptFaceMj[0].x-30;
	m_ptHuMjs[0].y    = m_ptFaceMj[0].y;
	//风位
	m_ptFengWei[0].x  = m_ptFace[0].x-REVISE_FENGWEI;
	m_ptFengWei[0].y  = m_ptFace[0].y+m_ptFace[0].y;

	m_ptQuanFengBk.x=nWidth/30*25/*+REVISE_FENG_QUAN*/;
	m_ptQuanFengBk.y=m_ptFaceMj[0].y-20/*+REVISE_FENG_QUAN*/;
	
	m_ptFace[1].x=nWidth-m_nXBorder-m_nXFace;
	m_ptFace[1].y=nHeight/2-m_nYFace-50;
	m_ptName[1].x=m_ptFace[1].x;
	m_ptName[1].y=m_ptFace[1].y+m_nYFace+REVISE_PS;
	m_ptTimer[1].x=m_ptFace[1].x;
	m_ptTimer[1].y=m_ptFace[1].y-m_nYTimer-REVISE_PS;
	m_ptReady[1].x=m_ptFace[1].x+m_nXTimer;
	m_ptReady[1].y=m_ptFace[1].y-m_nYTimer/2-REVISE_PS;

	//下家麻将区
	m_ptFaceMj[1].x=m_ptFace[1].x-REVISE_PS*2;
	m_ptFaceMj[1].y=m_ptFace[1].y-90;
	m_ptTouchMjs[1].x=m_ptFace[1].x-REVISE_PS*4;
	m_ptTouchMjs[1].y=m_ptFaceMj[1].y;
	m_ptOutMjs[1].x=m_ptFace[1].x-REVISE_PS*6-10;
	m_ptOutMjs[1].y=200;
	m_ptHuMjs[1].x=m_ptFaceMj[1].x-18;
	m_ptHuMjs[1].y=m_ptFaceMj[1].y+322;
	
	//风位
	m_ptFengWei[1].x=m_ptFace[1].x;
	m_ptFengWei[1].y=m_ptFace[1].y-REVISE_FENGWEI;

	m_ptFace[2].x=nWidth/2-m_nXFace;
	m_ptFace[2].y=nHeight*4/5-m_nYFace+80;
	m_ptName[2].x=m_ptFace[2].x+m_nXFace;
	m_ptName[2].y=m_ptFace[2].y+m_nYFace/2;
	m_ptTimer[2].x=m_ptFace[2].x;
	m_ptTimer[2].y=m_ptFace[2].y-m_nYTimer-REVISE_PS;
	m_ptReady[2].x=m_ptFace[2].x+m_nXTimer;
	m_ptReady[2].y=m_ptFace[2].y-m_nYTimer/2-REVISE_PS;

	//自已麻将区
	m_ptFaceMj[2].x=nWidth/4-120;
	m_ptFaceMj[2].y=m_ptFace[2].y-50-m_nYFace;
	m_ptTouchMjs[2].x=nWidth*2/3;
	m_ptTouchMjs[2].y=m_ptFaceMj[2].y-REVISE_MJ_FACE*2+20;
	m_ptOutMjs[2].x=m_ptFaceMj[2].x+155;
	m_ptOutMjs[2].y=m_ptTouchMjs[2].y-REVISE_MJ_FACE*2;
	m_ptHuMjs[2].x=m_ptFaceMj[2].x-REVISE_MJ_HU-10;
	m_ptHuMjs[2].y=m_ptFaceMj[2].y;
	//风位
	m_ptFengWei[2].x  = m_ptFace[2].x-REVISE_FENGWEI;
	m_ptFengWei[2].y  = m_ptFace[2].y;

	m_ptFace[3].x=m_nXBorder+REVISE_PS+20;
	m_ptFace[3].y=nHeight/2-m_nYFace-50;
	m_ptName[3].x=m_ptFace[3].x;
	m_ptName[3].y=m_ptFace[3].y+m_nYFace+REVISE_PS;
	m_ptTimer[3].x=m_ptFace[3].x;
	m_ptTimer[3].y=m_ptFace[3].y-m_nYTimer-REVISE_PS;
	m_ptReady[3].x=m_ptFace[3].x+m_nXTimer;
	m_ptReady[3].y=m_ptFace[3].y-m_nYTimer/2-REVISE_PS;
	//上家麻将区
	m_ptFaceMj[3].x=m_ptFace[3].x+m_nXFace+REVISE_PS*2-10;
	m_ptFaceMj[3].y=m_ptFace[3].y-130;
	m_ptTouchMjs[3].x=m_ptFace[3].x+m_nXFace+REVISE_PS*4-10;
	m_ptTouchMjs[3].y=m_ptFace[3].y+160;
	m_ptOutMjs[3].x=m_ptFace[3].x+m_nXFace+REVISE_PS*6-10;
	m_ptOutMjs[3].y=m_ptFaceMj[3].y+60;
	m_ptHuMjs[3].x=m_ptFaceMj[3].x+10;
	m_ptHuMjs[3].y=m_ptFaceMj[3].y-35;//nHeight-m_ptTouchMjs[3].y-REVISE_MJ_HU-90;

	//风位
	m_ptFengWei[3].x  = m_ptFace[3].x;
	m_ptFengWei[3].y  = m_ptFace[3].y-30;
	

	//移动按扭
	CRect rcButton,rc;
	HDWP hDwp=BeginDeferWindowPos(9);
	m_btChi.GetWindowRect(&rcButton);
	m_btGang.GetWindowRect(&rc);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	int w = 120+rc.Width(), h = 43;
	DeferWindowPos( hDwp, m_btStart, NULL, nWidth/2-20,nHeight/2+80, 0,0, uFlags );	
	DeferWindowPos(hDwp,m_btChi,NULL,nWidth/2+w,nHeight-rcButton.Height()-h,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPeng,NULL,nWidth/2+w+rcButton.Width(),nHeight-rcButton.Height()-h,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGang,NULL,nWidth/2+w+rcButton.Width()+rc.Width(),nHeight-rcButton.Height()-h,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btTing,NULL,nWidth/2+w+rcButton.Width()+rc.Width()*2,nHeight-rcButton.Height()-h,0,0,uFlags);
	DeferWindowPos(hDwp,m_btHu,NULL,nWidth/2+w+rcButton.Width()+rc.Width()*2,nHeight-rcButton.Height()-h,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth/2+w+rcButton.Width()+rc.Width()*3,nHeight-rcButton.Height()-h,0,0,uFlags);

//	DeferWindowPos( hDwp, m_btCancel, NULL, PS_BTX_CANCEL, PS_BTY_CANCEL, 0, 0, uFlags );
//	DeferWindowPos( hDwp, m_btShowMini, NULL, PS_BTX_SHOWMINI, PS_BTY_SHOWMINI, 0,0, uFlags );
//	DeferWindowPos( hDwp, m_btGameSet, NULL, PS_BTX_GAMESET, PS_BTY_GAMESET, 0, 0, uFlags );

	//test
	/*DeferWindowPos( hDwp, m_btStart, NULL, nWidth-PS_BTX_START,nHeight-PS_BTY_START-m_nYBorder, 0,0, uFlags );	*/
	//DeferWindowPos( hDwp, m_btHu, NULL, nWidth-PS_BTX_HU, nHeight-PS_BTY_HU-m_nYBorder, 0, 0, uFlags );
	////DeferWindowPos( hDwp, m_btTing, NULL, nWidth-PS_BTX_TING, nHeight-PS_BTY_TING-m_nYBorder, 0, 0, uFlags );
	//DeferWindowPos( hDwp, m_btGang, NULL, nWidth-PS_BTX_GANG, nHeight-PS_BTY_GANG-m_nYBorder, 0, 0, uFlags );
	//DeferWindowPos( hDwp, m_btPeng, NULL, nWidth-PS_BTX_PENG, nHeight-PS_BTY_PENG-m_nYBorder, 0, 0, uFlags );
	//DeferWindowPos( hDwp, m_btChi, NULL, nWidth-PS_BTX_CHI, nHeight-PS_BTY_CHI-m_nYBorder, 0, 0, uFlags );
	//DeferWindowPos( hDwp, m_btGiveUp, NULL, nWidth-PS_BTX_GIVEUP, nHeight-PS_BTY_GIVEUP-m_nYBorder, 0, 0, uFlags );
	EndDeferWindowPos(hDwp);

	//时钟控件
	m_Timer.SetBenchmarkPos( nWidth/2+10, nHeight/2-40);
	//积分视图
	CRect rcControl;
	m_scoreView.GetWindowRect(&rcControl);
	m_scoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,(nHeight-rcControl.Height()-100)/2,0,0,SWP_NOZORDER|SWP_NOSIZE);
	return;
}

//摸牌区域
void CGameClientView::SetImageTMj( CDC *pDC, BYTE byPlayer )
{	
	if( byPlayer>3 || byPlayer<0 )
		return;
	//设置变量
	LONG lWidth, lHeight;
	lWidth = m_csTouchMj[byPlayer].cx;
	lHeight = m_csTouchMj[byPlayer].cy;
	int index;

	//加载资源
	CImageHandle HandleMj(&m_ImageTouchMj[byPlayer]);

	//0,2位摸牌区图
	WORD  wStartUser = (m_wBankerUser + 4-(m_nDiceCount1-1))%4; //1在庄家，2庄家逆时针1个。
	switch( byPlayer )
	{
	case 0:
		{
			if(wStartUser==0) // if( (m_wBankerUser+m_bySaiDianShu)%4==0)
			{
				for(int i=0;i<(m_nDiceCount1+m_nDiceCount2)*2;i++)
				{
					index = i/2;
					if( i%2 ==0 )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					else
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 );
				}
				for( int i=COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]+(m_nDiceCount1+m_nDiceCount2)*2; i<COUNT_MJ_TOUCH; i++ )
				{		
					index = i/2;
					if( (i == COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]-(m_nDiceCount1+m_nDiceCount2)*2) && (i%2 !=0))
					{
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					}
					else
					{
						if( i%2 ==0 )
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
						else
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 );
					}
				}
			}
			else
			{
				for( int i=COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]; i<COUNT_MJ_TOUCH; i++ )
				{		
					index = i/2;
					if( i == COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]&&(i%2 !=0))
					{
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					}
					else
					{
						if( i%2 ==0 )
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
						else
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x+lWidth*(index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 );
					}
				}
			}

		}
		break;
	case 1:
		{
			//char buf[20];
			//memset(buf,0,sizeof(buf));
			//sprintf(buf,"play=%d,m_byTouchMjCount[play]=%d,wStartUser=%d,(m_nDiceCount1+m_nDiceCount2)=%d.",byPlayer,m_byTouchMjCount[byPlayer],wStartUser,(m_nDiceCount1+m_nDiceCount2));
			//MessageBox(0,buf,0,0);

			if(wStartUser==1) // if( (m_wBankerUser+m_bySaiDianShu)%4==0)
			{
				for(int i=0;i<(m_nDiceCount1+m_nDiceCount2)*2;i++)
				{
					index = i/2;	
					if( i%2==0  )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index), lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index), lWidth, lHeight,0, 0 );
					else 
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 );
				}
				for( int i=COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]+(m_nDiceCount1+m_nDiceCount2)*2; i<COUNT_MJ_TOUCH; i++)
				{
					index = i/2;	
					if( i%2==0  )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index), lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index), lWidth, lHeight,0, 0 );
					else 
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 );
				}
			}
			else
			{
				for( int i=COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]; i<COUNT_MJ_TOUCH; i++)
				{
					index = i/2;	
					if( i%2==0  )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index), lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index), lWidth, lHeight,0, 0 );
					else 
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y+(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 );
				}
			}
		}
		break;
	case 2:
		{
			if(wStartUser==2) // if( (m_wBankerUser+m_bySaiDianShu)%4==0)
			{
				for(int i=COUNT_MJ_TOUCH-1;i>=COUNT_MJ_TOUCH-(m_nDiceCount1+m_nDiceCount2)*2;i--)
				{
					index = i/2;	
					if( i%2 ==0 )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 );
					else
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
				}

				for( int i= m_byTouchMjCount[byPlayer]-1-(m_nDiceCount1+m_nDiceCount2)*2; i>=0 ; i--  )
				{		
					index = i/2;
					if(  m_byTouchMjCount[byPlayer]%2!=0 && i==m_byTouchMjCount[byPlayer]-1)
					{
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					}
					else
					{
						if( i%2 ==0 )
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 );
						else
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					}

				}
			}
			else
			{
				for( int i= m_byTouchMjCount[byPlayer]-1; i>=0 ; i--  )
				{		
					index = i/2;
					if(  m_byTouchMjCount[byPlayer]%2!=0 && i==m_byTouchMjCount[byPlayer]-1)
					{
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					}
					else
					{
						if( i%2 ==0 )
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y-5, lWidth, lHeight,0, 0 );
						else
							//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 ,COLORTRANS );
							m_ImageTouchMj[byPlayer].BitBlt( pDC->m_hDC,m_ptTouchMjs[byPlayer].x-lWidth*(COUNT_MJ_TOUCH/2-index), m_ptTouchMjs[byPlayer].y, lWidth, lHeight,0, 0 );
					}

				}

			}

		}
		break;
	case 3:
		{
			if(wStartUser==3) // if( (m_wBankerUser+m_bySaiDianShu)%4==0)
			{
				for( int i=COUNT_MJ_TOUCH-1; i>COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]+(m_nDiceCount1+m_nDiceCount2)*2; i-- )
				{
					index = i/2;	
					if( i%2==0  )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt(pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 );
					else 
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index), lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt(pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index), lWidth, lHeight,0, 0 );
				}

				for(int i=(m_nDiceCount1+m_nDiceCount2)*2;i>=0;i--)
				{
					index = i/2;	
					if( i%2==0  )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt(pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 );
					else 
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index), lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt(pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index), lWidth, lHeight,0, 0 );
				}

			}
			else
			{
				for( int i=COUNT_MJ_TOUCH-1; i>=COUNT_MJ_TOUCH-m_byTouchMjCount[byPlayer]; i-- )
				{
					index = i/2;	
					if( i%2==0  )
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt(pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index)-5, lWidth, lHeight,0, 0 );
					else 
						//m_ImageTouchMj[byPlayer].AlphaDrawImage( pDC, m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index), lWidth, lHeight,0, 0 ,COLORTRANS );
						m_ImageTouchMj[byPlayer].BitBlt(pDC->m_hDC,m_ptTouchMjs[byPlayer].x, m_ptTouchMjs[byPlayer].y-(lHeight-5)*(index), lWidth, lHeight,0, 0 );
				}
			}

		}
		break;
	}

	//清除资源
	//BufferDC.DeleteDC();
	//BitmapImage.DeleteObject();
}

//减少牌面
void CGameClientView::SubTouchMJ()
{
	WORD  wStartUser = (m_wBankerUser + 4-(m_nDiceCount1-1))%4;
	if(m_byTouchMjCount[wStartUser]-(m_nDiceCount1+m_nDiceCount2)*2 >0)
		m_byTouchMjCount[wStartUser] -= 1;
	else if(m_byTouchMjCount[(wStartUser+1)%4]>0)
		m_byTouchMjCount[(wStartUser+1)%4] -=1;
	else if(m_byTouchMjCount[(wStartUser+2)%4]>0)
		m_byTouchMjCount[(wStartUser+2)%4] -=1;
	else if(m_byTouchMjCount[(wStartUser+3)%4]>0)
		m_byTouchMjCount[(wStartUser+3)%4] -=1;
}
//绘制对手牌面
void CGameClientView::SetImageFMj( CDC *pDC, BYTE byPlayer )
{
	if( byPlayer!=0 && byPlayer!=1 && byPlayer!=3 )
		return;
	//if( m_byHandMjCount[byPlayer] <= 0 )
	//	return;

	//加载资源
	CImageHandle ImageHandle( &m_ImageFaceMj[byPlayer] );
	
	//设置变量
	LONG lWidth, lHeight;
	lWidth = m_csFaceMj[byPlayer].cx;
	lHeight = m_csFaceMj[byPlayer].cy;

	int y=0;
	//绘图
	switch( byPlayer )
	{
	case 0:
		{	
			for( int i=0; i<m_byHandMjCount[byPlayer]; i++ )
			{
				m_ImageFaceMj[byPlayer].BitBlt( pDC->m_hDC,m_ptFaceMj[byPlayer].x+lWidth*i, m_ptFaceMj[byPlayer].y, lWidth, lHeight,0, 0 );
			}
		}
		break;
	case 1:
		{	
			y=m_ptFaceMj[byPlayer].y;
			for( int j=3; j>=0; j--)
			{
				if( m_hu[byPlayer].block[j].style!= BLOCK_NULL)
				{
					switch( j )
					{
					case 3:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j+16;
						break;
					case 2:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j;
						break;
					case 1:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j-5;
						break;
					case 0:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j-10;
						break;
					}
					//y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j+16+j;
					break;
				}
			}
			for( int i=0; i<m_byHandMjCount[byPlayer]; i++ )
			{
				m_ImageFaceMj[byPlayer].BitBlt( pDC->m_hDC,m_ptFaceMj[byPlayer].x, y+(lHeight-20)*i, lWidth, lHeight,0, 0 );
			}
		}
		break;
	case 3:
		{
			y=m_ptFaceMj[byPlayer].y;
			for( int j=3; j>=0; j--)
			{
				if( m_hu[byPlayer].block[j].style!= BLOCK_NULL)
				{
					switch( j )
					{
					case 3:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-38;
						break;
					case 2:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-25;
						break;
					case 1:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-18;
						break;
					case 0:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-10;
						break;
					}
					break;
				}
			}
			for( int i=m_byHandMjCount[byPlayer]-1; i>=0; i-- )
			{
				m_ImageFaceMj[byPlayer].BitBlt( pDC->m_hDC,m_ptFaceMj[byPlayer].x, y-(lHeight-20)*(i-13), lWidth, lHeight,0, 0 );
			}
		}
		break;
	}
}


//绘制自已牌面
void CGameClientView::SetImageSelfMj( CDC *pDC, BYTE byData[], BYTE byMjCount )
{	

	//设置变量
	LONG lWidth, lHeight;
	lWidth = m_csFaceMj[2].cx;
	lHeight = m_csFaceMj[2].cy;
	
	//加载资源
	CImageHandle ImageHandle( &m_ImageFaceMj[2] );
	//绘制面
	for( BYTE i=0; i<HAND_MJ_MAX; i++ )
	{
		//跳过无效值
		if( byData[i] == 0 )
			continue;
		if( i == m_iUpMj )
		{
			if( i==HAND_MJ_MAX-1 )
			{
						//MessageBox("ww ");
				m_ImageFaceMj[2].BitBlt( pDC->m_hDC, m_ptFaceMj[2].x+REVISE_MJ_SELF+lWidth*i, m_ptFaceMj[2].y-REVISE_MJ_SELF, lWidth, lHeight, lWidth*(byData[i]-1), 0 );

				//出牌动画
				if( m_bActionOut)
				{
					if( m_tOut>0 )
					{
						ActionOut( pDC,m_ptFaceMj[2].x+REVISE_MJ_SELF+lWidth*i-lWidth/2, m_ptFaceMj[2].y-REVISE_MJ_SELF-10,m_tOut );
					}
				}
			}
			else
			{
				m_ImageFaceMj[2].BitBlt( pDC->m_hDC, m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y-REVISE_MJ_SELF, lWidth, lHeight, lWidth*(byData[i]-1), 0 );
				//出牌动画
				if( m_bActionOut)
				{
					//SetTimer( ID_OUT_TIME, 10, 0);
					if( m_tOut>0 )
					{
						ActionOut( pDC,m_ptFaceMj[2].x+lWidth*i-lWidth/2, m_ptFaceMj[2].y-REVISE_MJ_SELF-10,m_tOut );
					}
				}
			}
			
		}
		else
		{
			if(  i==HAND_MJ_MAX-1 )
			{
				m_ImageFaceMj[2].BitBlt( pDC->m_hDC, m_ptFaceMj[2].x+REVISE_MJ_SELF+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0 );
			}
			else
			{
				m_ImageFaceMj[2].BitBlt(pDC->m_hDC,m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0);

			}	
		}
	}
}

//绘制出牌
void CGameClientView::SetImageOMj( CDC *pDC,BYTE byPlayer )
{
	//设置变量
	LONG lWidth, lHeight, x, y;
	lWidth = m_csOutMj[byPlayer].cx;
	lHeight = m_csOutMj[byPlayer].cy;

	//获取资源,
	CImageHandle HandleOImage( &m_ImageOutMj[byPlayer] );
	switch( byPlayer )
	{
	case 0:
		for( int i= 0; i<(int)m_vcOutMjs[byPlayer].size(); i++ )
		{
			if( i< COUNT_MJ_OUT_LINE0 )
			{
				x = m_ptOutMjs[byPlayer].x-lWidth*i;
				y = m_ptOutMjs[byPlayer].y;
			}
			else if( i < COUNT_MJ_OUT_LINE0*2-2 )
			{
				x = m_ptOutMjs[byPlayer].x-lWidth*(i-COUNT_MJ_OUT_LINE0+1);
				y = m_ptOutMjs[byPlayer].y+REVISE_MJ_OUT_H0;
			}
			else
			{
				x = m_ptOutMjs[byPlayer].x-lWidth*(i-COUNT_MJ_OUT_LINE0*2+2+2);
				y = m_ptOutMjs[byPlayer].y+REVISE_MJ_OUT_H0*2;
			}	
			m_ImageOutMj[byPlayer].BitBlt( pDC->m_hDC,x+(COUNT_MJ_OUT_LINE0-1)*lWidth, y, lWidth, lHeight, lWidth*(m_vcOutMjs[byPlayer][i]-1), 0 );
			if( byPlayer==m_nActionOutFlag && i==(int)m_vcOutMjs[byPlayer].size()-1 )
			{
				ActionOutFlag( pDC,x+7+(COUNT_MJ_OUT_LINE0-1)*lWidth,y-lHeight/2+5,m_tOutFlag );
			}
		}

		break;

	case 1:
		for( int i=(int)m_vcOutMjs[byPlayer].size(); i>0; i--  )
		{
			if( i<= COUNT_MJ_OUT_LINE1 )
			{
				x = m_ptOutMjs[byPlayer].x;
				y = m_ptOutMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*(i-1);
			}
			else if( i <= COUNT_MJ_OUT_LINE1*2-2 )
			{
				x = m_ptOutMjs[byPlayer].x-m_csOutMj[byPlayer].cx;
				y = m_ptOutMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*(i-COUNT_MJ_OUT_LINE1);
			}
			else
			{
				x = m_ptOutMjs[byPlayer].x-m_csOutMj[byPlayer].cx*2;
				y = m_ptOutMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*(i-COUNT_MJ_OUT_LINE1*2+1+2);
			}
			m_ImageOutMj[byPlayer].BitBlt(pDC->m_hDC,x, y+(lHeight-REVISE_MJ_OUT_H1)*(COUNT_MJ_OUT_LINE1-1), lWidth, lHeight, lWidth*(m_vcOutMjs[byPlayer][i-1]-1), 0 ); 

			if( byPlayer==m_nActionOutFlag && i==(int)m_vcOutMjs[byPlayer].size() )
			{
				ActionOutFlag( pDC,x+lWidth/2-6,y-14+(lHeight-REVISE_MJ_OUT_H1)*(COUNT_MJ_OUT_LINE1-1),m_tOutFlag );
			}
		}
		break;

	case 2:
		for( int i=(int)m_vcOutMjs[byPlayer].size(); i>0/*.capacity()*/; i-- )
		{
			if( i<=COUNT_MJ_OUT_LINE2 )
			{
				x = m_ptOutMjs[byPlayer].x+lWidth*(i-1);
				y = m_ptOutMjs[byPlayer].y;
			}
			else if( i <= COUNT_MJ_OUT_LINE2*2-2 )
			{
				x = m_ptOutMjs[byPlayer].x+lWidth*(i-COUNT_MJ_OUT_LINE2);
				y = m_ptOutMjs[byPlayer].y-REVISE_MJ_OUT_H2;
			}
			else
			{
				x = m_ptOutMjs[byPlayer].x+lWidth*(i-COUNT_MJ_OUT_LINE2*2+1+2 );
				y = m_ptOutMjs[byPlayer].y-REVISE_MJ_OUT_H2*2;
			}
			m_ImageOutMj[byPlayer].BitBlt( pDC->m_hDC,x, y, lWidth, lHeight, lWidth*(m_vcOutMjs[byPlayer][i-1]-1), 0 );
			if( byPlayer==m_nActionOutFlag && i==(int)m_vcOutMjs[byPlayer].size() )
			{
				ActionOutFlag( pDC,x+lWidth/2-6,y-10,m_tOutFlag );
			}
		}
		break;

	case 3:
		for( int i=0; i<(int)m_vcOutMjs[byPlayer].size(); i++ )
		{
			if( i< COUNT_MJ_OUT_LINE3 )
			{
				x = m_ptOutMjs[byPlayer].x;
				y = m_ptOutMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H12)*i;
			}
			else if( i < COUNT_MJ_OUT_LINE3*2-2 )
			{
				x = m_ptOutMjs[byPlayer].x+m_csOutMj[byPlayer].cx;
				y = m_ptOutMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H12)*(i-COUNT_MJ_OUT_LINE3+1 );
			}
			else
			{
				x = m_ptOutMjs[byPlayer].x+m_csOutMj[byPlayer].cx*2;
				y = m_ptOutMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H1)*(i-COUNT_MJ_OUT_LINE3*2+2+2);
			}
			m_ImageOutMj[byPlayer].BitBlt( pDC->m_hDC,x, y, lWidth, lHeight, lWidth*(m_vcOutMjs[byPlayer][i]-1), 0 );
			if( byPlayer==m_nActionOutFlag && i==(int)m_vcOutMjs[byPlayer].size()-1 )
			{
				ActionOutFlag( pDC,x+11,y-lHeight/2,m_tOutFlag );
			}
		}
		break;
	}
}

//结束亮牌
void CGameClientView::SetImageEnd( CDC *pDC, BYTE byPlayer )
{

	LONG lWidth, lHeight;
	lWidth = m_csOutMj[byPlayer].cx;
	lHeight = m_csOutMj[byPlayer].cy;

	int y=0;
	//获取资源,
	CImageHandle HandleOImage( &m_ImageOutMj[byPlayer] );
	switch( byPlayer )
	{
	case 0:
		for( int i= HAND_MJ_MAX-1; i>=0; i-- )
		{
			//跳过空值
			if( m_byHandMjs[byPlayer][i]==0 )
				continue;
			m_ImageOutMj[byPlayer].BitBlt( pDC->m_hDC, m_ptFaceMj[byPlayer].x-lWidth*(i-13),m_ptFaceMj[byPlayer].y, lWidth, lHeight, lWidth*(m_byHandMjs[byPlayer][i]-1), 0 );
		}
		break;
	case 1:
		{
			y=m_ptFaceMj[byPlayer].y;
			for( int j=3; j>=0; j--)
			{
				if( m_hu[byPlayer].block[j].style!= BLOCK_NULL)
				{
					switch( j )
					{
					case 3:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j+16;
						break;
					case 2:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j;
						break;
					case 1:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j-5;
						break;
					case 0:
						y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j-10;
						break;
					}
					//y=m_ptFaceMj[byPlayer].y-(lHeight-20)*j+16+j;
					break;
				}
			}
			for( int i=HAND_MJ_MAX-1; i>=0; i--  )
			{
				if( m_byHandMjs[byPlayer][i]==0 )
					continue;
				m_ImageOutMj[byPlayer].BitBlt(pDC->m_hDC,m_ptFaceMj[byPlayer].x,m_ptFaceMj[byPlayer].y-(i-13)*(lHeight-REVISE_MJ_OUT_H1), lWidth, lHeight, lWidth*(m_byHandMjs[byPlayer][i]-1), 0 ); 
			}
		}
		break;
	case 2:
		break;
	case 3:
		{
			y=m_ptFaceMj[byPlayer].y;
			for( int j=3; j>=0; j--)
			{
				if( m_hu[byPlayer].block[j].style!= BLOCK_NULL)
				{
					switch( j )
					{
					case 3:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-38;
						break;
					case 2:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-25;
						break;
					case 1:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-18;
						break;
					case 0:
						y=m_ptFaceMj[byPlayer].y+(lHeight-20)*j-10;
						break;
					}
					break;
				}
			}
			for( int i=0; i<HAND_MJ_MAX; i++ )
			{
				if( m_byHandMjs[byPlayer][i]==0 )
					continue;
				m_ImageOutMj[byPlayer].BitBlt( pDC->m_hDC,m_ptFaceMj[byPlayer].x, m_ptFaceMj[byPlayer].y+(lHeight-REVISE_MJ_OUT_H12)*i, lWidth, lHeight, lWidth*(m_byHandMjs[byPlayer][i]-1), 0 );
			}
		}
		break;
	}
}

//胡牌区
void CGameClientView::SetImageHMj( CDC *pDC, BYTE byPlayer )
{
	//设置变量x,y,坐标变换, index图索引
	LONG lWidth, lHeight, x, y, index;
	lWidth = m_csHuMj[byPlayer].cx;
	lHeight = m_csHuMj[byPlayer].cy;

	//获取资源
	CImageHandle HandleHImage( &m_ImageHuMj[byPlayer] );
	switch( byPlayer )
	{
	case 0:
		{
			for( int i=0; i<4; i++ )
			{
				if( m_hu[byPlayer].block[i].style == BLOCK_NULL )
				{
					break;
				}
				switch( m_hu[byPlayer].block[i].style )
				{
				case BLOCK_NULL:
					break;
				case BLOCK_CHI:
					{
						for( int j=0; j<3; j++ )
						{
							x = m_ptHuMjs[byPlayer].x-lWidth*j-i*3*lWidth-REVISE_MJ_HU_FACE*i;
							y = m_ptHuMjs[byPlayer].y;
							index = (m_hu[byPlayer].block[i].first+j-1)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
						break;
					}
				case BLOCK_PENG:
					{
						for( int j=0; j<3; j++ )
						{
							x = m_ptHuMjs[byPlayer].x-lWidth*j-i*3*lWidth-REVISE_MJ_HU_FACE*i;
							y = m_ptHuMjs[byPlayer].y;
							index = (m_hu[byPlayer].block[i].first-1)*lWidth;
							//m_ImageHuMj[byPlayer].AlphaDrawImage( pDC, x, y, lWidth, lHeight, index, 0, COLORTRANS );
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
						break;
					}
				case BLOCK_GANG:
					{
						for( int j=0; j<3; j++ )
						{
							x = m_ptHuMjs[byPlayer].x-lWidth*j-i*3*lWidth-REVISE_MJ_HU_FACE*i;
							y = m_ptHuMjs[byPlayer].y;
							index = (m_hu[byPlayer].block[i].first-1)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 1 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-10, lWidth, lHeight, index, 0 );
							}
						}
						break;
					}
				case BLOCK_AN_GANG:
					{
						for( int j=0; j<3; j++ )
						{
							x = m_ptHuMjs[byPlayer].x-lWidth*j-i*3*lWidth-REVISE_MJ_HU_FACE*i;
							y = m_ptHuMjs[byPlayer].y;
							index = 34*lWidth;//(m_hu[byPlayer].block[i].first-1)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 1 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-10, lWidth, lHeight, index, 0 );
							}
						}
						break;
					}
				}
			}
		}
		break;
	case 1:
		{
			for( int i=0; i<4; i++ )
			{
				if( m_hu[byPlayer].block[i].style == BLOCK_NULL )
				{
					break;
				}
				switch( m_hu[byPlayer].block[i].style )
				{
				case BLOCK_CHI:
					{
						for( int j=3; j>0; j-- )
						{
							x = m_ptHuMjs[byPlayer].x+REVISE_MJ_HU1_X;
							y = m_ptHuMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*j-3*i*(lHeight-REVISE_MJ_OUT_H1)-(REVISE_MJ_HU_FACE+5)*i;
							index = (m_hu[byPlayer].block[i].first+j-2)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
					}
					break;
				case BLOCK_PENG:
					{
						for( int j=3; j>0; j-- )
						{
							x = m_ptHuMjs[byPlayer].x+REVISE_MJ_HU1_X;
							y = m_ptHuMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*j-3*i*(lHeight-REVISE_MJ_OUT_H1)-(REVISE_MJ_HU_FACE+5)*i;
							index =(m_hu[byPlayer].block[i].first-1)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
					}
					break;
				case BLOCK_GANG:
					{
						for( int j=3; j>0; j-- )
						{
							x = m_ptHuMjs[byPlayer].x+REVISE_MJ_HU1_X;
							y = m_ptHuMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*j-3*i*(lHeight-REVISE_MJ_OUT_H1)-(REVISE_MJ_HU_FACE+5)*i;
							index =(m_hu[byPlayer].block[i].first-1)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 2 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-10, lWidth, lHeight, index, 0 );
							}
						}
					}
					break;
				case BLOCK_AN_GANG :
					{
						for( int j=3; j>0; j-- )
						{
							x = m_ptHuMjs[byPlayer].x+REVISE_MJ_HU1_X;
							y = m_ptHuMjs[byPlayer].y-(lHeight-REVISE_MJ_OUT_H1)*j-3*i*(lHeight-REVISE_MJ_OUT_H1)-(REVISE_MJ_HU_FACE+5)*i;
							index =34*lWidth; //(m_hu[byPlayer].block[i].first-1)*lWidth;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 2 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-10, lWidth, lHeight, index, 0 );
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 2:
		{
			
			for( int i=0; i<4; i++ )
			{
				if( m_hu[byPlayer].block[i].style == BLOCK_NULL )
				{
					break;
				}
				switch( m_hu[byPlayer].block[i].style )
				{
				case BLOCK_CHI:
					{
						for( int j=0; j<3; j++ )
						{
							index = ( m_hu[byPlayer].block[i].first+j-1 )*lWidth;
							x     = m_ptHuMjs[byPlayer].x+3*i*lWidth+j*lWidth+REVISE_MJ_HU_FACE*i;
							y	  = m_ptHuMjs[byPlayer].y;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
					}
					break;
				case BLOCK_PENG:
					{
						for( int j=0; j<3; j++ )
						{
							index = ( m_hu[byPlayer].block[i].first-1 )*lWidth;
							x     = m_ptHuMjs[byPlayer].x+3*i*lWidth+j*lWidth+REVISE_MJ_HU_FACE*i;
							y	  = m_ptHuMjs[byPlayer].y;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
					}
					break;
				case BLOCK_GANG:
					{
						for( int j=0; j<3; j++ )
						{
							index = ( m_hu[byPlayer].block[i].first-1 )*lWidth;
							x     = m_ptHuMjs[byPlayer].x+3*i*lWidth+j*lWidth+REVISE_MJ_HU_FACE*i;
							y	  = m_ptHuMjs[byPlayer].y;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 1 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-20, lWidth, lHeight, index, 0 );
							}
						}
					}
					break;
				case BLOCK_AN_GANG:
					{
						for( int j=0; j<3; j++ )
						{
							index = 34*lWidth; //( m_hu[byPlayer].block[i].first-1 )*lWidth;
							x     = m_ptHuMjs[byPlayer].x+3*i*lWidth+j*lWidth+REVISE_MJ_HU_FACE*i;
							y	  = m_ptHuMjs[byPlayer].y;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 1 )
							{
								index = ( m_hu[byPlayer].block[i].first-1 )*lWidth;
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-20, lWidth, lHeight, index, 0 );
							}
						}
					}
					break;
				}
			}
		}
		break;
	case 3:
		{
			for( int i=0; i<4; i++ )
			{
				if( m_hu[byPlayer].block[i].style == BLOCK_NULL )
				{
					break;
				}
				switch( m_hu[byPlayer].block[i].style )
				{
				case BLOCK_CHI:
					{
						for( int j=0; j<3; j++ )
						{
							index = ( m_hu[byPlayer].block[i].first+j-1 )*lWidth;
							x	  = m_ptHuMjs[byPlayer].x-REVISE_MJ_HU3_X;
							y     = m_ptHuMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H1)*j+3*i*(lHeight-REVISE_MJ_OUT_H1)+(REVISE_MJ_HU_FACE+5)*i;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
					}
					break;
				case BLOCK_PENG:
					{
						for( int j=0; j<3; j++ )
						{
							index = ( m_hu[byPlayer].block[i].first-1 )*lWidth;
							x	  = m_ptHuMjs[byPlayer].x-REVISE_MJ_HU3_X;
							y     = m_ptHuMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H1)*j+3*i*(lHeight-REVISE_MJ_OUT_H1)+(REVISE_MJ_HU_FACE+5)*i;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
						}
					}
					break;
				case BLOCK_GANG:
					{
						for( int j=0; j<3; j++ )
						{
							index = ( m_hu[byPlayer].block[i].first-1 )*lWidth;
							x	  = m_ptHuMjs[byPlayer].x-REVISE_MJ_HU3_X;
							y     = m_ptHuMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H1)*j+3*i*(lHeight-REVISE_MJ_OUT_H1)+(REVISE_MJ_HU_FACE+5)*i;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 1 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-10, lWidth, lHeight, index, 0 );
							}
						}
					}
					break;
				case BLOCK_AN_GANG:
					{
						for( int j=0; j<3; j++ )
						{
							index =	34*lWidth;	//( m_hu[byPlayer].block[i].first-1 )*lWidth;
							x	  = m_ptHuMjs[byPlayer].x-REVISE_MJ_HU3_X;
							y     = m_ptHuMjs[byPlayer].y+(lHeight-REVISE_MJ_OUT_H1)*j+3*i*(lHeight-REVISE_MJ_OUT_H1)+(REVISE_MJ_HU_FACE+5)*i;
							m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y, lWidth, lHeight, index, 0 );
							if( j == 1 )
							{
								m_ImageHuMj[byPlayer].BitBlt(pDC->m_hDC,  x, y-10, lWidth, lHeight, index, 0 );
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
}

//流局
void CGameClientView::SetImageEmpty( CDC *pDC )
{
	CImageHandle HandleImage( &m_ImageEndEmpty );
	m_ImageEndEmpty.BitBlt( pDC->m_hDC, 280, 240);

}
//绘制风位
void CGameClientView::SetImageFengWei( CDC *pDC, BYTE byPlayer )
{
	//设置变量
//	CImageHandle HandleFengImage( &m_ImageFengWei );
//	LONG lWidth , lHeight;
//	lWidth = m_csFengWei.cx;
	//lHeight = m_csFengWei.cy;

	//m_ImageFengWei.AlphaDrawImage( pDC,m_ptFengWei[byPlayer].x, m_ptFengWei[byPlayer].y, lWidth, lHeight, m_byFengWei[byPlayer]*lWidth, 0, COLORTRANS );
}

//绘制圈风
void CGameClientView::SetImageQuanFeng( CDC *pDC )
{
	//设置变量
	//CImageHandle HandleImage( &m_ImageQuanFengBk);
	//CImageHandle HandleImage2( &m_ImageQuanFeng);

	//LONG lWidth, lHeight;
	//lWidth = m_csQuanFeng.cx;
//	lHeight  = m_csQuanFeng.cy;

	//m_ImageQuanFengBk.BitBlt( pDC->m_hDC, m_ptQuanFengBk.x, m_ptQuanFengBk.y);
	//圈风
	//m_ImageQuanFeng.AlphaDrawImage( pDC, m_ptQuanFengBk.x+PS_BTX_QUANFENG, m_ptQuanFengBk.y+PS_BTY_QUANFENG, lWidth, lHeight,m_wQuanWei*lWidth, 0, COLORTRANS);
	//风
	//m_ImageQuanFeng.AlphaDrawImage( pDC, m_ptQuanFengBk.x+PS_BTX_QUANFENG-4+lWidth, m_ptQuanFengBk.y+PS_BTY_QUANFENG, lWidth, lHeight, 4*lWidth, 0, COLORTRANS);
	//庄位
	//m_ImageQuanFeng.AlphaDrawImage( pDC, m_ptQuanFengBk.x+PS_BTX_QUANFENG-4+lWidth*2, m_ptQuanFengBk.y+PS_BTY_QUANFENG, lWidth, lHeight, m_wQuanWeiBanker*lWidth, 0, COLORTRANS);

}
//选择吃牌
void CGameClientView::SetImageSelectChi( CDC *pDC, BYTE byData[], BYTE byMjCount, int nChiMode, BYTE byMjOut )
{
	//设置变量
	LONG lWidth, lHeight;
	lWidth = m_csFaceMj[2].cx;
	lHeight = m_csFaceMj[2].cy;
	int m_iUpMj2= -1;			//第二个弹起标志
	CImageHandle ImageHandle( &m_ImageMaskMj );
	CImageHandle ImageHandle2( &m_ImageFaceMj[2] );
	if( m_iUpMj != -1)
	{	
		switch( nChiMode)
		{
		case 3:
			if( byData[m_iUpMj] == m_byOutMj-2 )
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == m_byOutMj-1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 1;
						break;
					}
				}
			}
			else if( byData[m_iUpMj] == m_byOutMj-1 )
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == m_byOutMj+1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 2;
						break;
					}
				}
			}
			else if( byData[m_iUpMj] == m_byOutMj+11 )
			{
				for( int j=m_iUpMj; j>HAND_MJ_MAX-byMjCount-1; j--)
				{
					if( byData[j] == m_byOutMj-1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 2;
						break;
					}
				}
			}
			break;
		case 6:
			if( byData[m_iUpMj] == m_byOutMj+1 )
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == m_byOutMj+2 )
					{
						m_iUpMj2 = j;
						m_nChiType= 4;
						break;
					}
				}
			}
			else if( byData[m_iUpMj] == m_byOutMj-1 )
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == m_byOutMj+1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 2;
						break;
					}
				}
			}
			else if( byData[m_iUpMj] == m_byOutMj+1 )
			{
				for( int j=m_iUpMj; j>HAND_MJ_MAX-byMjCount-1; j--)
				{
					if( byData[j] == m_byOutMj+1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 4;
						break;
					}
				}
			}
			break;

		case 7:
			if( byData[m_iUpMj] == m_byOutMj-2 )
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == m_byOutMj-1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 1;
						break;
					}
				}
			}
			else if( byData[m_iUpMj] == m_byOutMj-1 )
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == m_byOutMj+1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 2;
						break;
					}
				}
			}
			else if( byData[m_iUpMj]== m_byOutMj+1)
			{
				for( int j=m_iUpMj; j<HAND_MJ_MAX; j++)
				{
					if( byData[j] == byMjOut+2)
					{
						m_iUpMj2 = j;
						m_nChiType= 4;
						break;
					}
				}
			}
			else if( byData[m_iUpMj]== m_byOutMj+2)
			{
				for( int j=m_iUpMj; j>HAND_MJ_MAX-byMjCount-1; j--)
				{
					if( byData[j] == m_byOutMj+1 )
					{
						m_iUpMj2 = j;
						m_nChiType= 4;
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}

	//绘制面
	for( BYTE i=0; i<byMjCount; i++ )
	{
		//跳过无效值
		if( byData[i] == 0 )
			continue;
		//遮罩区
		if( byData[i]<byMjOut-2 || byData[i]>byMjOut+2 || byData[i]== byMjOut)
		{
			m_ImageMaskMj.AlphaDrawImage( pDC,m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
		}
		//亮区
		else
		{		
			if( i==m_iUpMj || i==m_iUpMj2 )
			{
				m_ImageFaceMj[2].AlphaDrawImage(pDC, m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y-REVISE_MJ_SELF, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS);	
			}
			else
			{
				m_ImageFaceMj[2].AlphaDrawImage(pDC, m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS);
			}
		}
	}
}
//选择杠牌
void CGameClientView::SetImageSeletGang( CDC *pDC, BYTE byData[], BYTE byMjCount, int nGangMode, BYTE byGangMj[] )
{
	//设置变量
	LONG lWidth, lHeight;
	int nSameGangMj=0;		//相同牌
	bool bMask = true;;
	lWidth = m_csFaceMj[2].cx;
	lHeight = m_csFaceMj[2].cy;
	//int m_iUpMj2= -1;			//第二个弹起标志
	CImageHandle ImageHandle( &m_ImageMaskMj );
	CImageHandle ImageHandle2( &m_ImageFaceMj[2] );

	//绘制面
	for( BYTE i=0; i<byMjCount; i++ )
	{
		//跳过无效值
		if( byData[i] == 0 )
			continue;
		for( int j=0; j<(HAND_MJ_MAX-2)/3-1; j++)
		{
			if( byData[i]==byGangMj[j])
			{
				bMask = false;
				break;
			}
			bMask = true;
		}
		//暗区
		if( bMask)
		{
			if(  i==byMjCount-1 )
			{
				m_ImageMaskMj.AlphaDrawImage( pDC,m_ptFaceMj[2].x+REVISE_MJ_SELF+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
			}
			else
			{
				m_ImageMaskMj.AlphaDrawImage( pDC,m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
			}	
		}
		//明区-可选的杠牌,
		else
		{
			if( byData[i] != byData[m_iUpMj])
			{
				if(  i==byMjCount-1 )
				{
					m_ImageFaceMj[2].AlphaDrawImage( pDC,m_ptFaceMj[2].x+REVISE_MJ_SELF+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
				}
				else
				{
					m_ImageFaceMj[2].AlphaDrawImage( pDC,m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
				}	
			}
			//明区-可选的杠牌,,弹起
			else if( byData[i]==byData[m_iUpMj])
			{
				nSameGangMj ++;
				if( i==byMjCount-1)
				{
					m_ImageFaceMj[2].AlphaDrawImage( pDC,m_ptFaceMj[2].x+REVISE_MJ_SELF+lWidth*i, m_ptFaceMj[2].y-REVISE_MJ_SELF, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
				}
				else
				{
					m_ImageFaceMj[2].AlphaDrawImage( pDC,m_ptFaceMj[2].x+lWidth*i, m_ptFaceMj[2].y-REVISE_MJ_SELF, lWidth, lHeight, lWidth*(byData[i]-1), 0, COLORTRANS );
				}	
			}
		}

		//决定杠牌类型
		if( nSameGangMj>1)
		{
			m_nGangType= 4;		//暗杠
		}
		else
		{
			m_nGangType= 2;		//补杠
		}
	}
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

//出牌动画
void CGameClientView::ActionOut( CDC *pDC, int x, int y, int nFrame )
{
	CImageHandle ImageHandle( &m_ActionOut);
	LONG lWidth, lHeight;
	lWidth=m_csActionOut.cx;
	lHeight=m_csActionOut.cy;

	m_ActionOut.AlphaDrawImage( pDC, x, y, lWidth, lHeight, lWidth*nFrame, 0, COLORTRANS);
}

//出牌标志动画
void CGameClientView::ActionOutFlag( CDC *pDC, int x, int y, int nFrame )
{
	CImageHandle ImageHandle( &m_ActionOutFlag);
	LONG lWidth, lHeight;
	lWidth=m_csActionOutFlag.cx;
	lHeight=m_csActionOutFlag.cy;

	m_ActionOutFlag.AlphaDrawImage( pDC, x, y, lWidth, lHeight, lWidth*nFrame, 0, COLORTRANS);
}

//出牌绘制
void CGameClientView::AddOutMj( WORD wChairID,BYTE data )
{
	m_vcOutMjs[wChairID].push_back( data );
}

//操作图
void CGameClientView::ActionOption( CDC *pDC, BYTE byPlayer,int OptionType )
{
	CImageHandle ImageHandle( &m_ActionOption );
	LONG lWidth, lHeight;
	lWidth=m_csActionOption.cx;
	lHeight=m_csActionOption.cy;
	switch(byPlayer)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
	}
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//设置背景
	//WORD a = timeGetTime();
	CRect rc;
	GetClientRect(&rc);
	//pDC->FillSolidRect(10,10,rc.Width(),rc.Height(),RGB(0,72,81));
	DrawViewImage( pDC, m_ImageBack1, enMode_Spread);
	DrawViewImage( pDC, m_ImageBack, enMode_Centent );
	
	//CImageHandle HandleImage( &m_ImageButtonBk);
	
	//绘制用户
	//TCHAR szBuffer[32];
	if( m_wQuanWeiBanker != 255)
	{
		SetImageQuanFeng( pDC);
		
	}
	for( BYTE i=0; i<GAME_PLAYER; i++ )
	{
		WORD wUserTimer = GetUserTimer( i );
		const tagUserData* pUserData;
		pUserData = GetUserInfo(i);
		
		if( pUserData != NULL )
		{
			//m_ImageButtonBk.AlphaDrawImage( pDC, PS_BTX_BTBACK, PS_BTY_BTBACK,COLORTRANS);
			pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
			//pDC->SetTextAlign(((i==0)&&(m_bDeasilOrder==true))||((i==2)&&(m_bDeasilOrder==false))?TA_RIGHT:	TA_LEFT);

			SetArtText( pDC, m_ptName[i].x, m_ptName[i].y, RGB(0,0,120),RGB(222, 233,2),  pUserData->szName );
			//char str[100];	
			//sprintf( str, "金%ld币",  pUserData->lScore);
			//pDC->TextOut( i*100,200,str );
			//其他信息
			//if (wUserTimer!=0) 
			//{
			//	DrawUserTimer( pDC, m_ptTimer[i].x, m_ptTimer[i].y, wUserTimer );
			//}
			if (pUserData->cbUserStatus==US_READY) 
			{
				//DrawUserReady( pDC, m_ptReady[i].x, m_ptReady[i].y );	
				SetImageTMj( pDC,i ); 
				//if( pUserData!=NULL)
				//{		
				//	//MessageBox("金币");
				//	char str[100];
				//	sprintf( str, "%l金币",  pUserData->lGold);
				//	pDC->TextOut( i*100,200,str );
				//}
				
			}	
			if( m_tDice>0 && m_tDice<12)
			{
				ActionDice1( pDC,m_tDice);
			}
			else
			{
				if( m_tDice== 12)
				{
					ActionDice2( pDC,m_nDiceCount1,m_nDiceCount2);
				}
			}	
			if( pUserData->cbUserStatus == US_PLAY )
			{	
				//设置圈风位
				if( m_byFengWei[i]!= 255 )
				{ 
					SetImageFengWei( pDC, i );
				}
				
				if( m_wQuanWeiBanker != 255)
				{
					SetImageQuanFeng( pDC);
				}
				if( m_tDice== 0)
				{
					SetImageTMj( pDC,i ); 
					SetImageFMj( pDC, i );	
					SetImageOMj( pDC, i );
					SetImageHMj( pDC, i );
				}
			}	
			if( m_bEnd )
			{
				SetImageTMj( pDC,i ); 
				//SetImageFMj( pDC, i );	
				//显牌
				SetImageEnd( pDC, i );
				SetImageOMj( pDC, i );
				SetImageHMj( pDC, i );
				//设置圈风位
				if( m_byFengWei[i]!= 255 )
				{ 
					SetImageFengWei( pDC, i );
				}

				if( m_wQuanWeiBanker != 255)
				{
					SetImageQuanFeng( pDC);
				}
				//流局
				if( m_bEndEmpty)
				{
					SetImageEmpty( pDC );
				}
			}
			DrawUserFace( pDC, pUserData->wFaceID, m_ptFace[i].x, m_ptFace[i].y, pUserData->cbUserStatus==US_OFFLINE );
		}	

			//test
#ifdef	DEBUG_GAME
			SetArtText( pDC, m_ptName[i].x, m_ptName[i].y, RGB(0,0,120),RGB(222, 233,2),  "玩家名字" );
			DrawUserFace( pDC, i, m_ptFace[i].x, m_ptFace[i].y, true );
			SetImageEmpty( pDC );
			//if( m_wQuanWeiBanker != 255)
			//{
			//	SetImageQuanFeng( pDC);
			//}
			//if( m_tDice>0 && m_tDice<12)
			//{
			//	ActionDice1( pDC,m_tDice);
			//}
			//else
			//{
			//	if( m_tDice== 12)
			//	{
			//		ActionDice2( pDC,2,4);
			//		//Sleep( 1000);
			//		//m_tDice= 0;
			//	}
			//}

			if( m_bEnd )
			{
				//SetImageTMj( pDC,i ); 
				//SetImageFMj( pDC, i );	
				//显牌
				//SetImageQuanFeng( pDC);
				SetImageEnd( pDC, i );
				SetImageFMj( pDC, i );
				
				//SetImageTMj( pDC,i ); 
				SetImageOMj( pDC, i );
				SetImageHMj( pDC, i );
			}
			//if( m_nChiMode )
			else { //SetImageQuanFeng( pDC);
				 SetImageFMj( pDC, i );
				
				 SetImageTMj( pDC,i ); 
				 SetImageOMj( pDC, i );
				 SetImageHMj( pDC, i );
				 if( pUserData!=NULL)
				 {		
					 char str[100];
					 sprintf( str, "%l金币",  pUserData->lGold);
					pDC->TextOut( 100,200,str );
				 }
				 
				 if( m_byFengWei[i]!=255 )
				 { 
					 SetImageFengWei( pDC, i );
				 }
			  }	 
#endif 
	}
	//BYTE test[14] = { 0x01, 0x02, 0x03,4,5,33,23,29,20,20,11,15,16,24 };
	if( m_nChiMode)
	{
		SetImageSelectChi( pDC,m_byHandMjs[2], HAND_MJ_MAX, m_nChiMode, m_byOutMj );
	}
	else if( m_nGangMode )
	{
		SetImageSeletGang( pDC, m_byHandMjs[2], HAND_MJ_MAX, m_nGangMode, m_byGangMj );
	}
	else
	{
		SetImageSelfMj( pDC, m_byHandMjs[2], HAND_MJ_MAX );
	}

	//绘画界面
	//pDC->FillSolidRect(0,0,nWidth,nHeight,RGB(255,0,0));
	//WORD b = timeGetTime()-a;
	//char str[100];
	//sprintf( str, "%dms帧", b);
	//pDC->TextOut( 100,100, str);
	return;
}

//用户顺序
void CGameClientView::SetUserOrder( bool bDeasilOrder )
{
	//状态判断
	if( m_bDeasilOrder == bDeasilOrder ) return;

	//设置变量
	m_bDeasilOrder = bDeasilOrder;

}

//////////////////////////////////////////////////////////////////////////
//开始按扭
void CGameClientView::OnStart()
{	
	AfxGetMainWnd()->SendMessage( IDM_START, 0, 0 );
	return;
}

//放弃按扭
void CGameClientView::OnGiveUp()
{
	
	AfxGetMainWnd()->SendMessage( IDM_GIVEUP, 0, 0 );
	UpdateGameView( NULL );
	return;
}
//吃牌按扭
void CGameClientView::OnChiMj()
{
	AfxGetMainWnd()->SendMessage( IDM_CHI_MJ, 1, 1 );
	UpdateGameView( NULL );
	return;
}

//碰牌按扭
void CGameClientView::OnPengMj()
{
	AfxGetMainWnd()->SendMessage( IDM_PENG_MJ, 2, 2 );
	UpdateGameView( NULL );
	return;
}

//杠牌按扭
void CGameClientView::OnGangMj()
{
	AfxGetMainWnd()->SendMessage( IDM_GANG_MJ, 3, 3 );
	UpdateGameView( NULL );
	return;
}

//听牌按扭
void CGameClientView::OnTingMj()
{
	AfxGetMainWnd()->SendMessage( IDM_TING_MJ, 4, 4 );
	UpdateGameView( NULL );
	return;
}
//胡牌按扭
void CGameClientView::OnHuMj()
{	
	AfxGetMainWnd()->SendMessage( IDM_HU_MJ, 5, 5 );
	UpdateGameView( NULL );
	return;
}
//双击鼠标左击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//CGameParrentView::OnLButtonDblClk(nFlags, point);
	__super::OnLButtonDblClk(nFlags, point);
	return;
}

//点击鼠标右建
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint point)
{
	//CGameParrentView::OnRButtonDown(nFlags, point);
	__super::OnRButtonDown( nFlags, point );
	return;
}

void CGameClientView::OnTest()
{
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
}

void CGameClientView::OnTest2()
{
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
}

void CGameClientView::OnTest3()
{
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
}
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{	
//m_bActionOut= true;
//UpdateGameView(NULL);

	if( m_bPlay && m_iUpMj != -1 &&m_nChiMode<=0 && m_nGangMode<=0 )
	{	
		if( m_byHandMjs[2][m_iUpMj]==0)
			return ;
		m_bActionOut= true;
		//m_tOutStartTime=timeGetTime();
		//SetTimer(ID_OUT_TIME, 50, 0);
		Sleep( 300 );
		AfxGetMainWnd()->SendMessage( IDM_OUT_MJ, 1, 1 );
		//m_iUpMj= -1;
	}
	else if( m_nChiMode>0 && m_iUpMj!=-1 )
	{
		if( m_byHandMjs[2][m_iUpMj]==0)
			return ;
		AfxGetMainWnd()->SendMessage( IDM_SELECT_CHI, 1, 1 );
		m_iUpMj = -1;
		m_nChiMode= 0;
		m_nChiType= 0;
	}
	else if( m_nGangMode>0 && m_iUpMj!=-1 )
	{
		if( m_byHandMjs[2][m_iUpMj]==0)
			return ;
		AfxGetMainWnd()->SendMessage( IDM_SELECT_GANG, 1, 1 );
		m_iUpMj = -1;
		m_nGangMode= 0;
		m_nGangType= 0;
		for( int i=0; i<(HAND_MJ_MAX-2)/3-1; i++ )
		{
			m_byGangMj[i]= -1;
		}
	}
	//CGameParrentView::OnLButtonDown(nFlags, point);
}

void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if( m_bPlay &&m_nChiMode<=0 &&m_nGangMode<=0 )
	{
		POINT csPoint;
		GetCursorPos( &csPoint );
		ScreenToClient( &csPoint );
		LONG x, y;
		x = csPoint.x;
		y = csPoint.y;

		if( x>m_ptFaceMj[2].x && y>m_ptFaceMj[2].y && x<m_ptFaceMj[2].x+m_csFaceMj[2].cx*COUNT_MJ_FACE+REVISE_MJ_SELF && y<m_ptFaceMj[2].y+m_csFaceMj[2].cy )
		{
			if( x>m_ptFaceMj[2].x+m_csFaceMj[2].cx*COUNT_MJ_FACE-1 )
			{
				m_iUpMj = COUNT_MJ_FACE-1;
			}
			else
			{
				m_iUpMj = (x-m_ptFaceMj[2].x)/m_csFaceMj[2].cx;
			}	
			if( m_iOldUpMj != m_iUpMj )
			{	
				m_iOldUpMj = m_iUpMj;
				UpdateGameView( NULL );
			}
		}
		else if(  m_iUpMj != -1 )
		{
			m_iUpMj = -1;
			UpdateGameView( NULL );
		}
	}

	//吃牌选择模式
	if( m_nChiMode )
	{
		POINT csPoint;
		GetCursorPos( &csPoint );
		ScreenToClient( &csPoint );
		LONG x, y;
		x = csPoint.x;
		y = csPoint.y;

		if( x>m_ptFaceMj[2].x && y>m_ptFaceMj[2].y && x<m_ptFaceMj[2].x+m_csFaceMj[2].cx*(COUNT_MJ_FACE-1) && y<m_ptFaceMj[2].y+m_csFaceMj[2].cy )
		{
			m_iUpMj = (x-m_ptFaceMj[2].x)/m_csFaceMj[2].cx;
			//遮罩不相关部分
			if( m_byHandMjs[2][m_iUpMj]<m_byOutMj-2 || m_byHandMjs[2][m_iUpMj]>m_byOutMj+2 || m_byHandMjs[2][m_iUpMj]==m_byOutMj )
			{
				m_iUpMj = -1;
			}
			UpdateGameView( NULL );
		}
	}
	//杠牌选择
	if( m_nGangMode)
	{
		POINT csPoint;
		GetCursorPos( &csPoint );
		ScreenToClient( &csPoint );
		LONG x, y;
		x = csPoint.x;
		y = csPoint.y;

		if( x>m_ptFaceMj[2].x && y>m_ptFaceMj[2].y && x<m_ptFaceMj[2].x+m_csFaceMj[2].cx*(COUNT_MJ_FACE-1) && y<m_ptFaceMj[2].y+m_csFaceMj[2].cy )
		{
			if( x>m_ptFaceMj[2].x+m_csFaceMj[2].cx*COUNT_MJ_FACE-1 )
			{
				m_iUpMj = COUNT_MJ_FACE-1;
			}
			else
			{
				m_iUpMj = (x-m_ptFaceMj[2].x)/m_csFaceMj[2].cx;
			}	
			//遮罩不相关部分
			bool bGangMj =false ;
			for( int i=0; i<(HAND_MJ_MAX-2)/3-1; i++ )
			{
				if( m_byHandMjs[2][m_iUpMj] == m_byGangMj[i])
				{
					bGangMj = true;
				}
			}
			if( !bGangMj )
			{
				m_iUpMj = -1;
			}
			UpdateGameView( NULL );
		}
	}
//	CGameParrentView::OnMouseMove(nFlags, point);
}

//获取牌面麻将
void CGameClientView::GetHandMj( BYTE byMj[], BYTE byMjCount )
{
	if( sizeof(byMj)>sizeof(m_byHandMjs[2] ) )
	{
		MessageBox( "数量不对" );
		return;
	}
	CopyMemory( &m_byHandMjs[2], byMj, sizeof( BYTE )*byMjCount );
}


////////////////辅助///////////////////////////////

//禁用按扭
void CGameClientView::HideButton()
{
	m_btHu.EnableWindow( false );
	//m_btTing.EnableWindow( false );
	m_btGang.EnableWindow( false );
	m_btPeng.EnableWindow( false );
	m_btChi.EnableWindow( false );
	m_btGiveUp.EnableWindow( false );
}

//描边字体
void CGameClientView::SetArtText( CDC *pDC, int x, int y, COLORREF nColor, COLORREF nBkColor, LPCTSTR lpszString )
{
	pDC->SetTextCharacterExtra( 3 );
	pDC->SetTextColor( nBkColor );
	//pDC->TextOut( x, y, lpszString );
	pDC->TextOut( x, y+1, lpszString );
	pDC->TextOut( x, y-1, lpszString );
	pDC->TextOut( x+1, y, lpszString );
	pDC->TextOut( x+1, y-1, lpszString );
	pDC->TextOut( x+1, y+1, lpszString );
	pDC->TextOut( x-1, y+1, lpszString );
	pDC->TextOut( x-1, y, lpszString );
	pDC->TextOut( x-1, y-1, lpszString );

	pDC->SetTextColor( nColor );
	pDC->TextOut( x, y, lpszString );
}

//清空视图
void CGameClientView::CleanUpView()
{
	//配置变量
	m_bDeasilOrder=		true;
	m_bLookOn=			true;
	m_bPlay=			false;
	m_bAction=			false;
	m_bActionOut=		false;
	m_nActionOutFlag=	-1;

	m_nChiMode=		0;
	m_nGangMode=	0;
	m_nChiType=		0;
	m_nGangType=	0;

	m_iUpMj		   = -1;
	m_wLastUser    = -1;
	m_wCurrentUser = -1;

	m_nDiceCount2= -1;
	m_nDiceCount1= -1;

	m_tDice= -1;
	m_tOut= -1;
	m_tOutFlag=-1;

	m_bEndEmpty= false;
	m_bEnd= false;

	for( BYTE i=0; i<3; i++ )
	{
		m_byGangMj[i] = -1;
	}

	//test
	//初始化手中麻牌
	for( int i=0; i<4; i++)
	{
		for( BYTE j=0; j<14; j++ )
		{
			m_byHandMjs[i][j] = 0;
		}
	}
	for( int i=0; i<4; i++ )
	{
		m_byHandMjCount[i] =14;
	}
	for( BYTE i=0; i<GAME_PLAYER; i++ )
	{
		m_byTouchMjCount[i] = COUNT_MJ_TOUCH;
		m_vcOutMjs[i].clear();
		//风位初始化
		m_byFengWei[i] = 255;
	}
	m_wQuanWei= 0;
	m_wQuanWeiBanker= 255;

	memset( m_hu, 0, sizeof(MJ_HU)*4 );
	m_scoreView.ShowWindow( SW_HIDE );
}
void CGameClientView::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch( nIDEvent)
	{
	case ID_DICE_TIME:
		{
			if( m_tDice<12)
			{
				m_tDice ++;
				UpdateGameView( NULL );
			}
			else
			{
				Sleep( 1000 );
				m_Timer.ShowWindow( SW_SHOW );
				m_tDice= 0;
				SetTimer(ID_OUTFLAG_TIME, 150,(TIMERPROC) NULL);
				UpdateGameView( NULL );	
				KillTimer( ID_DICE_TIME);
			}
		}
		break;
	case ID_OUT_TIME:
		{
			if(m_tOut<9)
			{
				m_tOut++;
				m_bPlay=false;
				UpdateGameView( NULL);
			}
			else
			{
				m_tOut= -1;
				m_iUpMj = -1;
				m_bActionOut=false;
				KillTimer( ID_OUT_TIME);
				UpdateGameView( NULL);
			}
		}
		break;
	case ID_OUTFLAG_TIME:
		{
			if(m_tOutFlag<15)
			{
				m_tOutFlag++;
				UpdateGameView(NULL);
			}
			else
			{
				m_tOutFlag= 0;
				//KillTimer( ID_OUTFLAG_TIME);
				UpdateGameView(NULL);
			}
		}
		break;
	}

//	CGameParrentView::OnTimer(nIDEvent);
}

void CGameClientView::OnCancel()
{
	//AfxGetMainWnd()->SendMessage( IDM_CANCEL2, 0, 0 );
	AfxGetMainWnd()->PostMessage( WM_CLOSE );
}

void CGameClientView::OnShowMini()
{
	AfxGetMainWnd()->SendMessage( IDM_SHOWMINI, 0, 0);
}

void CGameClientView::OnGameSet()
{
	AfxGetMainWnd()->SendMessage( IDM_GAMESET, 0, 0 );
}