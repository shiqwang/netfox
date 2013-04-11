#include "StdAfx.h"
#include "CardControl.h"
#include "GameScoreWnd.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameScoreWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameScoreWnd::CGameScoreWnd()
{
	//设置变量
	ZeroMemory(&m_GameScoreInfo,sizeof(m_GameScoreInfo));
	m_ImageBack.SetLoadInfo(IDB_GAME_SCORE,AfxGetInstanceHandle());

	return;
}

//析构函数
CGameScoreWnd::~CGameScoreWnd()
{
}

//建立消息
int CGameScoreWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//移动窗口
	CImageHandle BackImageHandle(&m_ImageBack);
	
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOREPOSITION);

	

	return TRUE;
}

//重画函数
void CGameScoreWnd::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC BufferDC;
	CBitmap ImageBuffer;
	BufferDC.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	BufferDC.SelectObject(&ImageBuffer);

	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(BufferDC,0,0);

	//设置 DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextColor(RGB(230,230,230));
	BufferDC.SelectObject(CSkinAttribute::m_DefaultFont);

	//绘画扑克
	g_CardResource.m_ImageUserBottom.DrawCardItem(&BufferDC,m_GameScoreInfo.cbChiHuCard,275,71);

	//用户成绩
	CRect rcDraw;
	TCHAR szBuffer[128];
	BYTE bUserCount=0;
	BYTE bRowShow=0;

	//输出胡牌玩家 
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		bRowShow = 0;

		//过滤玩家 
		if (m_GameScoreInfo.wCardType[i]==0)
			BufferDC.SetTextColor(RGB(0,0,0));
		else
			BufferDC.SetTextColor(RGB(0,0,255));

		//用户名字
		rcDraw.left=90+i*120;
		rcDraw.right=90+i*120+120;
		rcDraw.top = 150+bRowShow*20;
		rcDraw.bottom = 150+bRowShow*20+20;
		BufferDC.DrawText(m_GameScoreInfo.szUserName[i],lstrlen(m_GameScoreInfo.szUserName[i]),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		
		bRowShow++;

		//输出庄家 
		if(m_GameScoreInfo.wBankerUser==i)
		{			
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("庄家	X %d"),m_GameScoreInfo.bGoOnBankerTime);
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		if(m_GameScoreInfo.wCardType[i]!=0)//胡牌玩家
		{
			//胡牌信息
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			BufferDC.DrawText(m_GameScoreInfo.m_cHu,lstrlen(m_GameScoreInfo.m_cHu),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;
		}
		else if(m_GameScoreInfo.wProvideUser==i)//点炮玩家
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("点炮"));
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		//金牌信息
		if(m_GameScoreInfo.bGoldCount[i]>0)
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("金牌	 %d 翻"),m_GameScoreInfo.bGoldCount[i]);
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		//杠牌信息
		if(m_GameScoreInfo.bGangCount[i]>0)
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("杠牌	 %d 翻"),m_GameScoreInfo.bGangCount[i]);
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		//花牌信息
		if(m_GameScoreInfo.bFlowerCount[i]>0)
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("花牌	 %d 翻"),m_GameScoreInfo.bFlowerCount[i]);
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		//东西南北
		if(m_GameScoreInfo.bAllSidesCard[i]==true)
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("东西南北	 1 翻"));
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		//梅兰竹菊
		if(m_GameScoreInfo.bAllPlantCard[i]==true)
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("梅兰竹菊	 1 翻"));
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}

		//春夏秋冬
		if(m_GameScoreInfo.bAllSeasonCard[i]==true)
		{
			rcDraw.top = 150+bRowShow*20;
			rcDraw.bottom = 150+bRowShow*20+20;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("春夏秋冬	 1 翻"));
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			bRowShow++;

		}


		//总盘数
		BYTE bTotalPan = m_GameScoreInfo.bGoldCount[i]+m_GameScoreInfo.bFlowerCount[i]+m_GameScoreInfo.bGangCount[i]+
			(m_GameScoreInfo.bAllPlantCard[i]?1:0)+(m_GameScoreInfo.bAllSeasonCard[i]?1:0)+(m_GameScoreInfo.bAllSidesCard[i]?1:0);
	
		rcDraw.top = 340;
		rcDraw.bottom =360;

		//合计
		if(m_GameScoreInfo.wCardType[i]!=0)//胡牌玩家
		{			

			LONG lScore = (bTotalPan+m_GameScoreInfo.lCellScore)*m_GameScoreInfo.bGoOnBankerTime;

			_snprintf(szBuffer,sizeof(szBuffer),TEXT("(%d翻+%d底)X%d =%d"),bTotalPan,m_GameScoreInfo.lCellScore,m_GameScoreInfo.bGoOnBankerTime,lScore);

			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			
		}
		else//非胡牌玩家
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d翻"),bTotalPan);

			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

		}

		//玩家分数

		rcDraw.top = 360;
		rcDraw.bottom = 380;

		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_GameScoreInfo.lScore[i]);

		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	



	}
	/*for (int i=0;i<GAME_PLAYER;i++)
	{
		//用户名字
		rcDraw.left=25;
		rcDraw.right=125;
		rcDraw.top=125+i*25;
		rcDraw.bottom=rcDraw.top+12;
		BufferDC.DrawText(m_GameScoreInfo.szUserName[i],lstrlen(m_GameScoreInfo.szUserName[i]),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=130;
		rcDraw.right=170;
		rcDraw.top=125+i*25;
		rcDraw.bottom=rcDraw.top+12;
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_GameScoreInfo.lScore[i]);
		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		//用户状态
		if (m_GameScoreInfo.wCardType[i]!=0) BufferDC.TextOut(180,125+i*25,TEXT("胡牌"),4);
		else if (m_GameScoreInfo.wProvideUser==i) BufferDC.TextOut(180,125+i*25,TEXT("放炮"),4);

		//其他信息
		if (m_GameScoreInfo.wBankerUser==i) BufferDC.TextOut(228,125+i*25,TEXT("是"),2);

		if(m_GameScoreInfo.bFlowerCount[i]>0)
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_GameScoreInfo.bFlowerCount[i]);
			BufferDC.TextOut(270,125+i*25,szBuffer,lstrlen(szBuffer));
		}

		if(m_GameScoreInfo.bGoldCount[i]>0)
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_GameScoreInfo.bGoldCount[i]);
			BufferDC.TextOut(300,125+i*25,szBuffer,lstrlen(szBuffer));
		}
	}
	rcDraw.left=24;
	rcDraw.right=300;
	rcDraw.top=218;
	rcDraw.bottom=rcDraw.top+12;
	BufferDC.DrawText(m_GameScoreInfo.m_cHu,lstrlen(m_GameScoreInfo.m_cHu),&rcDraw,DT_VCENTER|DT_END_ELLIPSIS);*/

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CGameScoreWnd::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CGameScoreWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//设置积分
void CGameScoreWnd::SetScoreInfo(tagGameScoreInfo & GameScoreInfo)
{
	//设置变量
	m_GameScoreInfo=GameScoreInfo;

	//显示窗口
	Invalidate(NULL);
	ShowWindow(SW_SHOW);

	return;
}

//////////////////////////////////////////////////////////////////////////
