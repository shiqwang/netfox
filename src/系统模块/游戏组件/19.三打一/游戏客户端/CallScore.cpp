#include "StdAfx.h"
#include "CallScore.h"

//按钮标识

//游戏按钮
#define IDC_SCORE_80						500								//开始按钮
#define IDC_SCORE_75						501								//开始按钮
#define IDC_SCORE_70						502								//开始按钮
#define IDC_SCORE_65						503								//开始按钮
#define IDC_SCORE_60						504								//开始按钮
#define IDC_SCORE_55						505								//开始按钮
#define IDC_SCORE_50						506								//开始按钮
#define IDC_SCORE_45						507								//开始按钮
#define IDC_SCORE_40						508								//开始按钮
#define IDC_SCORE_35						509								//开始按钮
#define IDC_SCORE_30						510								//开始按钮
#define IDC_SCORE_25						511								//开始按钮
#define IDC_SCORE_20						512								//开始按钮
#define IDC_SCORE_15						513								//开始按钮
#define IDC_SCORE_10						514								//开始按钮
#define IDC_SCORE_5							515								//开始按钮
#define IDC_GIVE_UP							516								//不叫
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCallScore, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SCORE_80,OnCallScore80)
	ON_BN_CLICKED(IDC_SCORE_75,OnCallScore75)
	ON_BN_CLICKED(IDC_SCORE_70,OnCallScore70)
	ON_BN_CLICKED(IDC_SCORE_65,OnCallScore65)
	ON_BN_CLICKED(IDC_SCORE_60,OnCallScore60)
	ON_BN_CLICKED(IDC_SCORE_55,OnCallScore55)
	ON_BN_CLICKED(IDC_SCORE_50,OnCallScore50)
	ON_BN_CLICKED(IDC_SCORE_45,OnCallScore45)
	ON_BN_CLICKED(IDC_SCORE_40,OnCallScore40)
	ON_BN_CLICKED(IDC_SCORE_35,OnCallScore35)
	ON_BN_CLICKED(IDC_SCORE_30,OnCallScore30)
	ON_BN_CLICKED(IDC_SCORE_25,OnCallScore25)
	ON_BN_CLICKED(IDC_SCORE_20,OnCallScore20)
	ON_BN_CLICKED(IDC_SCORE_15,OnCallScore15)
	ON_BN_CLICKED(IDC_SCORE_10,OnCallScore10)
	ON_BN_CLICKED(IDC_SCORE_5,OnCallScore5)
	ON_BN_CLICKED(IDC_GIVE_UP, OnGiveUpScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//构造函数
CCallScore::CCallScore()
{
	return;
}

//析构函数
CCallScore::~CCallScore()
{
}

//建立消息
int CCallScore::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect rcCreate(-999,0,0,0);

	//创建按钮
	m_Score80.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_80);
	m_Score75.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_75);
	m_Score70.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_70);
	m_Score65.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_65);
	m_Score60.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_60);
	m_Score55.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_55);
	m_Score50.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_50);
	m_Score45.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_45);
	m_Score40.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_40);
	m_Score35.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_35);
	m_Score30.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_30);
	m_Score25.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_25);
	m_Score20.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_20);
	m_Score15.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_15);
	m_Score10.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_10);
	m_Score5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE_5);
	m_btGiveUpScore.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_GIVE_UP);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_Score80.SetButtonImage(IDB_80,hInstance,false);
	m_Score75.SetButtonImage(IDB_75,hInstance,false);
	m_Score70.SetButtonImage(IDB_70,hInstance,false);
	m_Score65.SetButtonImage(IDB_65,hInstance,false);
	m_Score60.SetButtonImage(IDB_60,hInstance,false);
	m_Score55.SetButtonImage(IDB_55,hInstance,false);
	m_Score50.SetButtonImage(IDB_50,hInstance,false);
	m_Score45.SetButtonImage(IDB_45,hInstance,false);
	m_Score40.SetButtonImage(IDB_40,hInstance,false);
	m_Score35.SetButtonImage(IDB_35,hInstance,false);
	m_Score30.SetButtonImage(IDB_30,hInstance,false);
	m_Score25.SetButtonImage(IDB_25,hInstance,false);
	m_Score20.SetButtonImage(IDB_20,hInstance,false);
	m_Score15.SetButtonImage(IDB_15,hInstance,false);
	m_Score10.SetButtonImage(IDB_10,hInstance,false);
	m_Score5.SetButtonImage(IDB_5,hInstance,false);

	m_btGiveUpScore.SetButtonImage(IDB_BU_JIAO,hInstance,false);

	

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//加载资源
	m_ImageScoreBack.SetLoadInfo(IDB_CALL_SCORE,hInstance);     //背景
	//移动窗口
	CImageHandle ImageHandle(&m_ImageScoreBack);
	SetWindowPos(NULL,0,0,m_ImageScoreBack.GetWidth(),m_ImageScoreBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return true;
}

//重画函数
void CCallScore::OnPaint() 
{
	CPaintDC dc(this);
	
	CImageHandle HandleBack(&m_ImageScoreBack);
	
	m_ImageScoreBack.BitBlt(dc,0,0);

	ReloadView();

	return;
}

//重新改变按钮状态
void CCallScore::ReloadView() 
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	int nWidth=rcClient.Width();
	int nHeight=rcClient.Height();

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	DeferWindowPos(hDwp,m_Score80,NULL,10,40,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score75,NULL,50,40,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score70,NULL,90,40,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score65,NULL,130,40,0,0,uFlags);
	
	DeferWindowPos(hDwp,m_Score60,NULL,10,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score55,NULL,50,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score50,NULL,90,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score45,NULL,130,70,0,0,uFlags);

	DeferWindowPos(hDwp,m_Score40,NULL,10,100,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score35,NULL,50,100,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score30,NULL,90,100,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score25,NULL,130,100,0,0,uFlags);

	DeferWindowPos(hDwp,m_Score20,NULL,10,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score15,NULL,50,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score10,NULL,90,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_Score5,NULL,130,130,0,0,uFlags);

	DeferWindowPos(hDwp,m_btGiveUpScore,NULL,nWidth/2-35,165,0,0,uFlags);
	
	//移动控件
	EndDeferWindowPos(hDwp);
	m_Score80.EnableWindow((80<m_cbPriScore)?true:false);
	m_Score75.EnableWindow((75<m_cbPriScore)?true:false);
	m_Score70.EnableWindow((70<m_cbPriScore)?true:false);
	m_Score65.EnableWindow((65<m_cbPriScore)?true:false);
	m_Score60.EnableWindow((60<m_cbPriScore)?true:false);
	m_Score55.EnableWindow((55<m_cbPriScore)?true:false);
	m_Score50.EnableWindow((50<m_cbPriScore)?true:false);
	m_Score45.EnableWindow((45<m_cbPriScore)?true:false);
	m_Score40.EnableWindow((40<m_cbPriScore)?true:false);
	m_Score35.EnableWindow((35<m_cbPriScore)?true:false);
	m_Score30.EnableWindow((30<m_cbPriScore)?true:false);
	m_Score25.EnableWindow((25<m_cbPriScore)?true:false);
	m_Score20.EnableWindow((20<m_cbPriScore)?true:false);
	m_Score15.EnableWindow((15<m_cbPriScore)?true:false);
	m_Score10.EnableWindow((10<m_cbPriScore)?true:false);
	m_Score5.EnableWindow((5<m_cbPriScore)?true:false);

}

void CCallScore::OnCallScore80()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,80,80);
	return;
}
void CCallScore::OnCallScore75()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,75,75);
	return;
}
void CCallScore::OnCallScore70()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,70,70);
	return;
}
void CCallScore::OnCallScore65()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,65,65);
	return;
}
void CCallScore::OnCallScore60()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,60,60);
	return;
}
void CCallScore::OnCallScore55()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,55,55);
	return;
}
void CCallScore::OnCallScore50()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,50,50);
	return;
}
void CCallScore::OnCallScore45()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,45,45);
	return;
}
void CCallScore::OnCallScore40()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,40,40);
	return;
}
void CCallScore::OnCallScore35()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,35,35);
	return;
}
void CCallScore::OnCallScore30()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,30,30);
	return;
}
void CCallScore::OnCallScore25()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,25,25);
	return;
}
void CCallScore::OnCallScore20()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,20,20);
	return;
}

void CCallScore::OnCallScore15()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,15,15);
	return;
}

void CCallScore::OnCallScore10()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,10,10);
	return;
}
void CCallScore::OnCallScore5()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,5,5);
	return;
}
void CCallScore::OnGiveUpScore()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,250,250);
	return;
}



//////////////////////////////////////////////////////////////////////////
