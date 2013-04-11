#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_START						100								//开始按钮
#define IDC_ADD_GOLD					101								//加注按钮
#define IDC_GIVE_UP						102								//放弃按钮
#define IDC_FOLLOW						103								//跟注按钮
#define IDC_SHOW_HAND					104								//梭哈按钮
#define IDC_KANPAI                      105                             //看牌按钮         //add
#define IDC_KAIPAI                      106                             //开牌按钮         //add
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_KANPAI, KANPAI)                   //add
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_FOLLOW, OnFollow)
	ON_BN_CLICKED(IDC_GIVE_UP, OnGiveUp)
	ON_BN_CLICKED(IDC_ADD_GOLD, OnAddGold)
	ON_BN_CLICKED(IDC_SHOW_HAND, OnShowHand)
	ON_BN_CLICKED(IDC_KAIPAI, KaiPai)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//设置变量
	m_lMaxGold=0L;
	m_lBasicGold=0L;
	m_Banker=INVALID_CHAIR;        //add

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageTitle.SetLoadInfo(IDB_TITLE,hInstance);
	m_ImageBarMid.SetLoadInfo(IDB_BAR_MID,hInstance);
	m_ImageBarLeft.SetLoadInfo(IDB_BAR_LEFT,hInstance);
	m_ImageBarRight.SetLoadInfo(IDB_BAR_RIGHT,hInstance);
	m_ImageGlodTable.SetLoadInfo(IDB_GLOD_TABLE,hInstance);
	m_ImageBackGround.SetLoadInfo(IDB_BACK_GROUND,hInstance);
	m_ImageBack.SetLoadInfo(IDB_BACK,hInstance);
	m_BankerView.SetLoadInfo(IDB_BANKER,hInstance);

	m_ImageKanPaiLog.SetLoadInfo(IDB_KAN_PAI_LOG,hInstance);
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
	m_GoldControl.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CreateRect,this,8,NULL);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,9);
	for (int i=0;i<GAME_PLAYER;i++) m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,10+i);

	//创建按钮
	m_btStart.Create(NULL,WS_CHILD,CreateRect,this,IDC_START);
	m_btFollow.Create(NULL,WS_CHILD,CreateRect,this,IDC_FOLLOW);
	m_btGiveUp.Create(NULL,WS_CHILD,CreateRect,this,IDC_GIVE_UP);
	m_btAddGold.Create(NULL,WS_CHILD,CreateRect,this,IDC_ADD_GOLD);
	m_btShowHand.Create(NULL,WS_CHILD,CreateRect,this,IDC_SHOW_HAND);
	m_btKanPai.Create(NULL,WS_CHILD,CreateRect,this,IDC_KANPAI);                                    //add
	m_btKaiPai.Create(NULL,WS_CHILD,CreateRect,this,IDC_KAIPAI);                                    //add
	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_STAR,hInstance,false);
	m_btFollow.SetButtonImage(IDB_BT_FOLLOW,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_BT_GIVE_UP,hInstance,false);
	m_btAddGold.SetButtonImage(IDB_BT_ADD_GOLD,hInstance,false);
	m_btShowHand.SetButtonImage(IDB_BT_SHOW_HAND,hInstance,false);
	m_btKanPai.SetButtonImage(IDB_KANPAI,hInstance,false);                                           //add
	m_btKaiPai.SetButtonImage(IDB_KAIPAI,hInstance,false);                                           //add
	return 0;
}

//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

//跟注按钮
void CGameClientView::OnFollow()
{
	AfxGetMainWnd()->SendMessage(IDM_FOLLOW,0,0);
	return;
}

//放弃按钮
void CGameClientView::OnGiveUp()
{
	AfxGetMainWnd()->SendMessage(IDM_GIVE_UP,0,0);
	return;
}

//加注按钮
void CGameClientView::OnAddGold()
{
	AfxGetMainWnd()->SendMessage(IDM_ADD_GOLD,0,0);
	return;
}

//梭哈按钮
void CGameClientView::OnShowHand()
{
	AfxGetMainWnd()->SendMessage(IDM_SHOWHAND,0,0);
	return;
}

/////////////////////////////////////////////////////////         add
//看牌按钮
void CGameClientView::KANPAI()
{
	AfxGetMainWnd()->SendMessage(IDM_KANPAI,0,0);
	m_CardControl[3].SetShowCardFlag(true);
	UpdateGameView(NULL);
	return;
}
//开牌按钮
void CGameClientView::KaiPai()
{
	AfxGetMainWnd()->SendMessage(IDM_KAIPAI,0,0);
	return;
}
///////////////////////////////////////////////////////                                            //add
//重置界面
void CGameClientView::ResetGameView()
{
	//设置变量
	m_lMaxGold=0L;
	m_lBasicGold=0L;

	//设置控件
	m_btKanPai.ShowWindow(SW_HIDE);                                                                //add
	m_btKaiPai.ShowWindow(SW_HIDE);                                                                //add
	m_btStart.ShowWindow(SW_HIDE);
	m_btFollow.ShowWindow(SW_HIDE);
	m_btGiveUp.ShowWindow(SW_HIDE);
	m_btAddGold.ShowWindow(SW_HIDE);
	m_btShowHand.ShowWindow(SW_HIDE);
	m_ScoreView.ShowWindow(SW_HIDE);
	m_GoldControl.ShowWindow(SW_HIDE);
	for (int i=0;i<CountArray(m_GoldView);i++) m_GoldView[i].SetGold(0L);
	for (int i=0;i<GAME_PLAYER;i++) m_CardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	
	//设置坐标
	m_ptFace[0].x=nWidth/2-m_nXFace;                                                               //上面头像 
	m_ptFace[0].y=5+m_nYBorder;
	m_ptName[0].x=nWidth/2+5;
	m_ptName[0].y=5+m_nYBorder;
	m_ptTimer[0].x=nWidth/2-m_nXFace-m_nXTimer*2-5;
	m_ptTimer[0].y=5+m_nYBorder;
	m_ptReady[0].x=nWidth/2-m_nXFace-m_nXTimer-5;
	m_ptReady[0].y=5+m_nYBorder+m_nYTimer/2;
	m_UserFlagPos[0].x=nWidth/2-m_nXFace+70;														
	m_UserFlagPos[0].y=1+m_nYBorder;																

	m_ptFace[4].x=nWidth-m_nXBorder-m_nXFace-5;													   //右下角头像
	m_ptFace[4].y=nHeight/2-m_nYFace-100+200+30;
	m_ptName[4].x=nWidth-m_nXBorder-m_nXFace-30;
	m_ptName[4].y=nHeight/2-m_nYFace-50+200+30;
	m_ptTimer[4].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptTimer[4].y=nHeight/2-m_nYFace-55-m_nYTimer+10+200+30;
	m_ptReady[4].x=nWidth-m_nXBorder-m_nXTimer-5;
	m_ptReady[4].y=nHeight/2-m_nYFace-55-m_nYTimer/2+200+30;
	m_UserFlagPos[4].x=nWidth-m_nXBorder-m_nXFace-50;												
	m_UserFlagPos[4].y=nHeight/2-m_nYFace-80+200+30;														

	m_ptFace[3].x=nWidth/2+72;																	   //下面头像
	m_ptFace[3].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[3].x=nWidth/2+77+m_nXFace;
	m_ptName[3].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptTimer[3].x=nWidth/2+72;
	m_ptTimer[3].y=nHeight-m_nYBorder-m_nYFace-10-m_nYTimer;
	m_ptReady[3].x=nWidth/2+72+m_nXTimer;
	m_ptReady[3].y=nHeight-m_nYBorder-m_nYFace-10-m_nYTimer/2;
	m_UserFlagPos[3].x=nWidth/2+72+50;           												    
	m_UserFlagPos[3].y=nHeight-m_nYBorder-m_nYFace-5-20;											

	m_ptFace[2].x=m_nXBorder+5;																	   //左下角头像
	m_ptFace[2].y=nHeight/2-m_nYFace-100+200+30;
	m_ptName[2].x=m_nXBorder+m_nXFace+10;
	m_ptName[2].y=nHeight/2-m_nYFace-50+200+30;
	m_ptTimer[2].x=m_nXBorder+5;
	m_ptTimer[2].y=nHeight/2-m_nYFace-55-m_nYTimer+10+200+30;
	m_ptReady[2].x=m_nXBorder+5+m_nXTimer;
	m_ptReady[2].y=nHeight/2-m_nYFace-55-m_nYTimer/2+200+30;
	m_UserFlagPos[2].x=m_nXBorder+45;           												     
	m_UserFlagPos[2].y=nHeight/2-m_nYFace-100+200+30;	

	m_ptFace[5].x=nWidth-m_nXBorder-m_nXFace-5;													   //右上角头像
	m_ptFace[5].y=nHeight/2-m_nYFace-100-20-20;
	m_ptName[5].x=nWidth-m_nXBorder-m_nXFace-30;
	m_ptName[5].y=nHeight/2-m_nYFace-50-20-20;
	m_ptTimer[5].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptTimer[5].y=nHeight/2-m_nYFace-55-m_nYTimer+10-20-20;
	m_ptReady[5].x=nWidth-m_nXBorder-m_nXTimer-5;
	m_ptReady[5].y=nHeight/2-m_nYFace-55-m_nYTimer/2-20-20;
	m_UserFlagPos[5].x=nWidth-m_nXBorder-m_nXFace-50;												
	m_UserFlagPos[5].y=nHeight/2-m_nYFace-80-20-20;	

	m_ptFace[1].x=m_nXBorder+5;																	   //左上角头像
	m_ptFace[1].y=nHeight/2-m_nYFace-100-20-20;
	m_ptName[1].x=m_nXBorder+m_nXFace+30;
	m_ptName[1].y=nHeight/2-m_nYFace-50-20-20;
	m_ptTimer[1].x=m_nXBorder+5;
	m_ptTimer[1].y=nHeight/2-m_nYFace-55-m_nYTimer+10-20-20;
	m_ptReady[1].x=m_nXBorder+5+m_nXTimer;
	m_ptReady[1].y=nHeight/2-m_nYFace-55-m_nYTimer/2-20-20;
	m_UserFlagPos[1].x=m_nXBorder+45;           												     
	m_UserFlagPos[1].y=nHeight/2-m_nYFace-100-20-20;											               

	//筹码控件
	int nXCenterPos=nWidth/2;
	int nYCenterPos=nHeight/2;
	if (GetSystemMetrics(SM_CXSCREEN)>=1024)
	{
		m_GoldStation[0].SetPoint(nXCenterPos-32,nYCenterPos-92-60);
		m_GoldStation[1].SetPoint(nXCenterPos+28,nYCenterPos-92-60);
		m_GoldStation[2].SetPoint(nXCenterPos-94-60,nYCenterPos-67);
		m_GoldStation[3].SetPoint(nXCenterPos-94-60,nYCenterPos-127);
		m_GoldStation[4].SetPoint(nXCenterPos-94-60,nYCenterPos+113);
		m_GoldStation[5].SetPoint(nXCenterPos-94-60,nYCenterPos+57);
		m_GoldStation[6].SetPoint(nXCenterPos+28,nYCenterPos+93+80);
		m_GoldStation[7].SetPoint(nXCenterPos-32,nYCenterPos+93+80);
		m_GoldStation[8].SetPoint(nXCenterPos+96+60,nYCenterPos+57);
		m_GoldStation[9].SetPoint(nXCenterPos+96+60,nYCenterPos+123);
		m_GoldStation[10].SetPoint(nXCenterPos+96+60,nYCenterPos-127);
		m_GoldStation[11].SetPoint(nXCenterPos+96+60,nYCenterPos-63);		
	}
	else
	{	
		m_GoldStation[0].SetPoint(nXCenterPos-32,nYCenterPos-92-60);
		m_GoldStation[1].SetPoint(nXCenterPos+28,nYCenterPos-92-60);
		m_GoldStation[2].SetPoint(nXCenterPos-94-60,nYCenterPos-67);
		m_GoldStation[3].SetPoint(nXCenterPos-94-60,nYCenterPos-127);
		m_GoldStation[4].SetPoint(nXCenterPos-94-60,nYCenterPos+113);
		m_GoldStation[5].SetPoint(nXCenterPos-94-60,nYCenterPos+57);
		m_GoldStation[6].SetPoint(nXCenterPos+28,nYCenterPos+93+80);
		m_GoldStation[7].SetPoint(nXCenterPos-32,nYCenterPos+93+80);
		m_GoldStation[8].SetPoint(nXCenterPos+96+60,nYCenterPos+57);
		m_GoldStation[9].SetPoint(nXCenterPos+96+60,nYCenterPos+123);
		m_GoldStation[10].SetPoint(nXCenterPos+96+60,nYCenterPos-127);
		m_GoldStation[11].SetPoint(nXCenterPos+96+60,nYCenterPos-63);	
	}

	//按钮控件
	HDWP hDwp=BeginDeferWindowPos(5);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth-m_nXBorder-110,nHeight-m_nYBorder-85,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddGold,NULL,nWidth-m_nXBorder-105,nHeight-m_nYBorder-113,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFollow,NULL,nWidth-m_nXBorder-85+42,nHeight-m_nYBorder-100,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth-m_nXBorder-43,nHeight-m_nYBorder-48,0,0,uFlags);
	DeferWindowPos(hDwp,m_btShowHand,NULL,nWidth-m_nXBorder-155,nHeight-m_nYBorder-71,0,0,uFlags);
	DeferWindowPos(hDwp,m_btKanPai,NULL,nWidth-m_nXBorder-106,nHeight-m_nYBorder-50,0,0,uFlags);       //add
	DeferWindowPos(hDwp,m_btKaiPai,NULL,nWidth-m_nXBorder-185,nHeight-m_nYBorder-66,0,0,uFlags);       //add
	EndDeferWindowPos(hDwp);

	//成绩窗口
	CRect rcScoreView;
	m_ScoreView.GetWindowRect(&rcScoreView);
	m_ScoreView.MoveWindow((nWidth-rcScoreView.Width())/2,(nHeight-rcScoreView.Height())/2-10+20,rcScoreView.Width(),rcScoreView.Height());  //delete
	//m_ScoreView.MoveWindow((nWidth-rcScoreView.Width())/2-200,(nHeight-rcScoreView.Height())/2-10-200,rcScoreView.Width(),rcScoreView.Height());  //add

	//加注窗口
	m_GoldControl.SetBasicPoint(nWidth-80-m_nXBorder,nHeight-85-m_nYBorder);

	//扑克控件
	//m_CardControl[0].SetBasicStation(CPoint(nWidth/2,42+m_nYBorder),enXCenter,enYTop);                      //上面扑克
	//m_CardControl[3].SetBasicStation(CPoint(m_nXBorder+5,nHeight/2),enXLeft,enYCenter);                     //右下角扑克
	//m_CardControl[2].SetBasicStation(CPoint(nWidth/2,nHeight-5-m_nYBorder),enXCenter,enYBottom);
	//m_CardControl[1].SetBasicStation(CPoint(nWidth-m_nXBorder-5,nHeight/2),enXRight,enYCenter);             //下边扑克
	

	m_CardControl[0].SetBasicStation(CPoint(nWidth/2,42+m_nYBorder),enXCenter,enYTop);                       //上面扑克
	m_CardControl[5].SetBasicStation(CPoint(nWidth-m_nXBorder-5-80,nHeight/2-100),enXRight,enYCenter);       //右上角扑克
	m_CardControl[4].SetBasicStation(CPoint(nWidth-m_nXBorder-5-80,nHeight/2+80),enXRight,enYCenter);        //右下角扑克
	m_CardControl[3].SetBasicStation(CPoint(nWidth/2,nHeight-5-m_nYBorder),enXCenter,enYBottom);             //下边扑克
	m_CardControl[1].SetBasicStation(CPoint(m_nXBorder+5+80,nHeight/2-100),enXLeft,enYCenter);	             //左上角扑克
	m_CardControl[2].SetBasicStation(CPoint(m_nXBorder+5+80,nHeight/2+80),enXLeft,enYCenter);	             //左下角扑克
	
	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sDrawGameView.log",strTime);

			strTemp.Format("DrawGameView");
			theApp.WriteLog(strFile, strTemp);

	//变量定义
	CImageHandle HandleTitle(&m_ImageTitle);
	CImageHandle HandleBarMid(&m_ImageBarMid);
	CImageHandle HandleBarLeft(&m_ImageBarLeft);
	CImageHandle HandleBarRight(&m_ImageBarRight);
	CImageHandle HandleGlodTable(&m_ImageGlodTable);	

	CImageHandle HandleKanPaiLog(&m_ImageKanPaiLog);
	
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);
	DrawViewImage(pDC,m_ImageBackGround,enMode_Centent);
	m_ImageTitle.BitBlt(pDC->m_hDC,m_nXBorder,m_nYBorder);


	if(m_Banker!=INVALID_CHAIR)
	{
		CImageHandle HandleBanker(&m_BankerView);
		int Width=m_BankerView.GetWidth()/4;
		int Height=m_BankerView.GetHeight();
		m_BankerView.BitBlt(pDC->m_hDC,m_UserFlagPos[m_Banker].x,m_UserFlagPos[m_Banker].y,Width,Height,0,0);	
	}		
		
//	m_ImageGlodTable.BitBlt(pDC->m_hDC,(nWidth-m_ImageGlodTable.GetWidth())/2,(nHeight-m_ImageGlodTable.GetHeight())/2);

	//控制区域
/*	m_ImageBarLeft.BitBlt(pDC->m_hDC,m_nXBorder,nHeight-m_ImageBarLeft.GetHeight()-m_nYBorder);
	m_ImageBarMid.StretchBlt(pDC->m_hDC,m_nXBorder+m_ImageBarLeft.GetWidth(),nHeight-m_ImageBarMid.GetHeight()-m_nYBorder,
		nWidth-2*m_nXBorder-m_ImageBarLeft.GetWidth()-m_ImageBarRight.GetWidth(),m_ImageBarMid.GetHeight());
	m_ImageBarRight.BitBlt(pDC->m_hDC,nWidth-m_nXBorder-m_ImageBarRight.GetWidth(),nHeight-m_ImageBarRight.GetHeight()-m_nYBorder);*/

	//绘画用户
	WORD wUserTimer=0;
	TCHAR szBuffer[64];
	pDC->SetTextColor(RGB(255,255,0));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign((i==1)?(TA_RIGHT|TA_TOP):(TA_LEFT|TA_TOP));
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));

		
			if (myKanPaiBool[i])
			{
				//画看牌标记
				strTemp.Format("画看牌标记 i=%d", i);
				theApp.WriteLog(strFile, strTemp);
				m_ImageKanPaiLog.BitBlt(pDC->m_hDC,m_ptName[i].x,m_ptName[i].y+31);
			}
			else
			{
				strTemp.Format("NO I=%d", i);
				theApp.WriteLog(strFile, strTemp);
			}

			//用户金币
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("￥%ld"),pUserData->lScore);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y+16,szBuffer,lstrlen(szBuffer));

			//其他信息
			wUserTimer=GetUserTimer(i);
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData->wFaceID,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}//End if (pUserData!=NULL)
	}//End for (WORD i=0;i<GAME_PLAYER;i++)

	//游戏信息
	const UINT uDrawFormat=DT_END_ELLIPSIS|DT_CENTER|DT_SINGLELINE|DT_VCENTER;
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(255,255,255));

	//最大
	if (m_lMaxGold!=0) 
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("￥%ld"),m_lMaxGold);
		//DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(86,18,162,33),uDrawFormat); //delete
		DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(86,25,162,50),uDrawFormat);   //add

	}

	//底注
	if (m_lBasicGold!=0) 
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("￥%ld"),m_lBasicGold);
		//DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(86,35,162,50),uDrawFormat); //delete
		DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(86,15,162,100),uDrawFormat);
	}//End if

	//绘画筹码
	LONG lTotalGold=0L;
	for (DWORD i=0;i<CountArray(m_GoldView);i++)
	{
		lTotalGold+=m_GoldView[i].GetGold();
		m_GoldView[i].DrawGoldView(pDC,m_GoldStation[i].x,m_GoldStation[i].y);
	}//End for

	//总注信息
	if (lTotalGold!=0L)
	{
		CFont ViewFont;
		ViewFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));
		CFont * pOldFont=pDC->SelectObject(&ViewFont);
		//pDC->SetTextColor(RGB(0,0,2500));	   //设置字体为蓝色
		pDC->SetTextColor(RGB(255,255,0));     //设置字体为黄色
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),lTotalGold);
		pDC->TextOut(nWidth/2+20,nHeight/2+60,szBuffer,lstrlen(szBuffer));
		pDC->SelectObject(pOldFont);
		ViewFont.DeleteObject();
	}//End if (lTotalGold!=0L)

	return;
}
//金币标题
void CGameClientView::SetGoldTitleInfo(DWORD lMaxGold, DWORD lBasicGold)
{
	//设置变量
	m_lMaxGold=lMaxGold;
	m_lBasicGold=lBasicGold;

	//更新视图
	UpdateGameView(NULL);

	return;
}

//用户筹码
void CGameClientView::SetUserGoldInfo(WORD wViewChairID, bool bTableGold, DWORD dwGold)
{
	//参数判断
	WORD wGoldPos=wViewChairID*2+((bTableGold==true)?1:0);
	if (wGoldPos>=CountArray(m_GoldStation)) return;

	//设置变量
	DWORD m_dwAllGold=0L;
	m_GoldView[wGoldPos].SetGold(dwGold);                      
	for (int i=0;i<CountArray(m_GoldStation);i++)
		m_dwAllGold+=m_GoldView[i].GetGold();         

	//更新视图
	UpdateGameView(NULL);

	return;
}

//获取庄家用户
void CGameClientView::GetCurrentUser(WORD MyCurrentUser)
{
	m_Banker=MyCurrentUser;
	UpdateGameView(NULL);
	return ;
}    

//计算某玩家放入的数
int CGameClientView::GetOneUserInputAllMoney(int userInt)
{
int resultInt = -1;

resultInt = m_GoldView[userInt].GetGold();

return resultInt;
}
//////////////////////////////////////////////////////////////////////////
