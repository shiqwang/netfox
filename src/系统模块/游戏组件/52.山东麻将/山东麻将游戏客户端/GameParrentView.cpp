#include "StdAfx.h"
#include "Resource.h"
#include "GameParrentView.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const int			CGameParrentView::m_nXFace=32;						//头像高度
const int			CGameParrentView::m_nYFace=32;						//头像宽度
const int			CGameParrentView::m_nXTimer=15;						//定时器宽
const int			CGameParrentView::m_nYTimer=21;						//定时器高
const int			CGameParrentView::m_nXBorder=6;						//定时器高
const int			CGameParrentView::m_nYBorder=6;						//定时器高

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameParrentView, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameParrentView::CGameParrentView(bool bDoubleBuf ,UINT uColorCount, CThreadDraw * pThreadDraw) 
: m_bDoubleBuf(bDoubleBuf), m_uColorCount(uColorCount), m_pThreadDraw(pThreadDraw)
{
	//设置变量
	m_bReDraw=true;
	m_pReserve=NULL;
	m_pIUserFaceRes=NULL;
	memset(&m_wTimer,0,sizeof(m_wTimer));
	memset(&m_ptName,0,sizeof(m_ptName));
	memset(&m_ptFace,0,sizeof(m_ptFace));
	memset(&m_ptTimer,0,sizeof(m_ptTimer));
	memset(&m_ptReady,0,sizeof(m_ptReady));
	memset(&m_pUserItem,0,sizeof(m_pUserItem));

	//加载资源
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageReady.SetLoadInfo(IDB_USER_READY,hInstance);
	//m_ImageTimer.SetLoadInfo(IDB_USER_TIMER,hInstance);

	//m_ImageViewT.SetLoadInfo(IDB_VIEW_T,hInstance);
	//m_ImageViewB.SetLoadInfo(IDB_VIEW_B,hInstance);
	//m_ImageViewL.SetLoadInfo(IDB_VIEW_L,hInstance);
	//m_ImageViewR.SetLoadInfo(IDB_VIEW_R,hInstance);
	//m_ImageViewBL.SetLoadInfo(IDB_VIEW_BL,hInstance);
	//m_ImageViewTL.SetLoadInfo(IDB_VIEW_TL,hInstance);
	//m_ImageViewBR.SetLoadInfo(IDB_VIEW_BR,hInstance);
	//m_ImageViewTR.SetLoadInfo(IDB_VIEW_TR,hInstance);

	return;
}

//析构函数
CGameParrentView::~CGameParrentView()
{
}

//接口查询
void * __cdecl CGameParrentView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameFrameView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameFrameView,Guid,dwQueryVer);
	return NULL;
}

//更新界面
void CGameParrentView::UpdateGameView(const CRect * pRect)
{
	if (m_bDoubleBuf==true) m_bReDraw=true;
	if (pRect!=NULL) InvalidateRect(pRect,FALSE);
	else Invalidate(FALSE);
	return;
}

//获取界面
bool CGameParrentView::GetGameViewImage(CImage & ImageBuffer, int nXPos, int nYPos, int nWidth, int nHeight)
{
	ASSERT(ImageBuffer.IsNull()==false);
	ASSERT(m_ImageBuffer.IsNull()==false);
	if (m_ImageBuffer.IsNull()==false)
	{
		m_ImageBuffer.BitBlt(ImageBuffer.GetDC(),nXPos,nYPos,nWidth,nHeight,nXPos,nYPos);
		ImageBuffer.ReleaseDC();
		return true;
	}
	return false;
}

//获取时间
WORD CGameParrentView::GetUserTimer(WORD wChairID)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return 0;
	return m_wTimer[wChairID];
}

//获取玩家
const tagUserData * CGameParrentView::GetUserInfo(WORD wChairID)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return NULL;
	return m_pUserItem[wChairID];
}

//绘画背景
void CGameParrentView::DrawViewImage(CDC * pDC, CSkinImage & SkinImage, enImageMode ImageMode)
{
	//获取位置
	RECT rcClient;
	GetClientRect(&rcClient);

	//绘画位图
	switch (ImageMode)
	{
	case enMode_ElongGate:	//拉伸模式
		{
			CImageHandle ImageHandle(&SkinImage);
			int nImageWidth=ImageHandle->GetWidth();
			int nImageHeight=ImageHandle->GetHeight();
			ImageHandle->StretchBlt(pDC->m_hDC,0,0,rcClient.right,rcClient.bottom,0,0,nImageWidth,nImageHeight);
			return;
		}
	case enMode_Centent:	//居中模式
		{
			CImageHandle ImageHandle(&SkinImage);
			int nXPos=(rcClient.right-ImageHandle->GetWidth())/2;
			int nYPos=(rcClient.bottom-ImageHandle->GetHeight())/2;
			ImageHandle->BitBlt(pDC->m_hDC,nXPos,nYPos);
			return;
		}
	case enMode_Spread:		//平铺模式
		{
			CImageHandle ImageHandle(&SkinImage);
			int nImageWidth=ImageHandle->GetWidth();
			int nImageHeight=ImageHandle->GetHeight();
			for (int nXPos=0;nXPos<rcClient.right;nXPos+=nImageWidth)
			{
				for (int nYPos=0;nYPos<rcClient.bottom;nYPos+=nImageHeight)
				{
					ImageHandle->BitBlt(pDC->m_hDC,nXPos,nYPos);
				}
			}
			return;
		}
	}
	return;
}

//绘画头像
void CGameParrentView::DrawUserFace(CDC * pDC, WORD wFaceID, int nXPos, int nYPos, bool bOffLine)
{
	ASSERT(m_pIUserFaceRes!=NULL);
	m_pIUserFaceRes->DrawNormalFace(pDC,nXPos,nYPos,wFaceID);
	return;
}

//绘画头像
void CGameParrentView::DrawUserSmallFace(CDC * pDC, WORD wFaceID, int nXPos, int nYPos, bool bOffLine)
{
	ASSERT(m_pIUserFaceRes!=NULL);
	m_pIUserFaceRes->DrawSmallFace(pDC,nXPos,nYPos,wFaceID);
	return;
}

//绘画准备
void CGameParrentView::DrawUserReady(CDC * pDC, int nXPos, int nYPos)
{
	CImageHandle ImageHandle(&m_ImageReady);
	int nImageWidth=ImageHandle->GetWidth();
	int nImageHeight=ImageHandle->GetHeight();
	AlphaDrawImage(pDC,nXPos-nImageWidth/2,nYPos-nImageHeight/2,nImageWidth,nImageHeight,ImageHandle.GetBitmapHandle(),0,0,RGB(255,0,255));
	return;
}

//绘画时间
void CGameParrentView::DrawUserTimer(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea)
{
	if ((wTime>0)&&(wTimerArea>=wTime))
	{
		//创建 DC
		CDC DCBuffer;
		DCBuffer.CreateCompatibleDC(pDC);
		CImageHandle ImageHandle(&m_ImageTimer);
		HGDIOBJ hOldGdiObj=DCBuffer.SelectObject(ImageHandle.GetBitmapHandle());

		//绘画定时器
		int nDrawPos=nXPos;
		for (WORD nArea=10000;nArea>0;nArea/=10)
		{
			if (wTimerArea<nArea) continue;
			int nTimerPos=wTime/nArea;
			AlphaDrawImage(pDC,nDrawPos,nYPos,m_nXTimer,m_nYTimer,&DCBuffer,nTimerPos*m_nXTimer,0,RGB(255,0,255));
			wTime-=nTimerPos*nArea;
			nDrawPos+=m_nXTimer;
		}

		//清理资源
		DCBuffer.SelectObject(hOldGdiObj);
		DCBuffer.DeleteDC();
	}
	return;
}

//透明绘画
void CGameParrentView::AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, CDC * pScrDC, int nXScr, int nYScr, COLORREF crTransColor)
{
	::AlphaDrawImage(pDesDC,nXDes,nYDes,nDesWidth,nDesHeight,pScrDC,nXScr,nYScr,nDesWidth,nDesHeight,crTransColor);
	return;
}

//绘画透明图
void CGameParrentView::AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, HBITMAP hBitBmp, int nXScr, int nYScr, COLORREF crTransColor)
{
	//创建 DC
	CDC DCBuffer;
	DCBuffer.CreateCompatibleDC(NULL);
	HGDIOBJ hOldGdiObj=DCBuffer.SelectObject(hBitBmp);

	//绘画位图
	::AlphaDrawImage(pDesDC,nXDes,nYDes,nDesWidth,nDesHeight,&DCBuffer,nXScr,nYScr,nDesWidth,nDesHeight,crTransColor);

	//清理资源
	DCBuffer.SelectObject(hOldGdiObj);
	DCBuffer.DeleteDC();

	return;
}

//重置界面
void CGameParrentView::ResetData()
{
	//设置变量
	memset(m_wTimer,0,sizeof(m_wTimer));
	memset(m_pUserItem,0,sizeof(m_pUserItem));

	//重置界面
	ResetGameView();
	UpdateGameView(NULL);

	return;
}

//设置接口
bool CGameParrentView::SetUserFaceRes(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIUserFaceRes=(IUserFaceRes *)pIUnknownEx->QueryInterface(IID_IUserFaceRes,VER_IUserFaceRes);
	return (m_pIUserFaceRes!=NULL);
}

//设置时间
bool CGameParrentView::SetUserTimer(WORD wChairID, WORD wTimer)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return false;
	m_wTimer[wChairID]=wTimer;
	UpdateGameView(NULL);
	return true;
}

//设置用户
bool CGameParrentView::SetUserInfo(WORD wChairID, tagUserData * pUserItem)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return false;
	m_pUserItem[wChairID]=pUserItem;
	UpdateGameView(NULL);
	return true;
}

//绘画边框
//bool CGameParrentView::DrawGameViewFrame(CDC * pDC, int nWidth, int nHeight)
//{
//	//加载资源
//	int nXPos=0,nYPos=0;
//	CImageHandle BorderTHandle(&m_ImageViewT);
//	CImageHandle BorderBHandle(&m_ImageViewB);
//	CImageHandle BorderLHandle(&m_ImageViewL);
//	CImageHandle BorderRHandle(&m_ImageViewR);
//	CImageHandle BorderBLHandle(&m_ImageViewBL);
//	CImageHandle BorderTLHandle(&m_ImageViewTL);
//	CImageHandle BorderBRHandle(&m_ImageViewBR);
//	CImageHandle BorderTRHandle(&m_ImageViewTR);
//
//	//绘画左上角
//	m_ImageViewTL.AlphaDrawImage(pDC,0,0,RGB(0,0,0));
//
//	//绘画左边框
//	for (int i=m_ImageViewTL.GetHeight();i<nHeight;i+=m_ImageViewL.GetHeight())	
//		m_ImageViewL.AlphaDrawImage(pDC,0,i,RGB(0,0,0));
//
//	//绘画左下角
//	m_ImageViewBL.AlphaDrawImage(pDC,0,nHeight-m_ImageViewBL.GetHeight(),RGB(0,0,0));
//
//	//绘画上下框架
//	nXPos=nHeight-m_ImageViewB.GetHeight();
//	for (int i=m_ImageViewTL.GetWidth();i<nWidth;i+=m_ImageViewT.GetWidth())
//	{
//		m_ImageViewT.AlphaDrawImage(pDC,i,0,RGB(0,0,0));
//		m_ImageViewB.AlphaDrawImage(pDC,i,nXPos,RGB(0,0,0));
//	}
//
//	//绘画左上角
//	m_ImageViewTR.AlphaDrawImage(pDC,nWidth-m_ImageViewTR.GetWidth(),0,RGB(0,0,0));
//
//	//绘画左边框
//	nXPos=nWidth-m_ImageViewR.GetWidth();
//	for (int i=m_ImageViewTR.GetHeight();i<nHeight;i+=m_ImageViewR.GetHeight())	
//		m_ImageViewR.AlphaDrawImage(pDC,nXPos,i,RGB(0,0,0));
//
//	//绘画左下角
//	m_ImageViewBR.AlphaDrawImage(pDC,nWidth-m_ImageViewBR.GetWidth(),nHeight-m_ImageViewBL.GetHeight(),RGB(0,0,0));
//
//	return true;
//}

//创建消息
int CGameParrentView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//初始化
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	if (m_pThreadDraw!=NULL) m_pThreadDraw->BeginThread();

	return 0;
}

//绘画函数
void CGameParrentView::OnPaint()
{
	CPaintDC dc(this);

	//判断模式
	if (m_pThreadDraw!=NULL) return;

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//重画缓冲区
	if ((m_bDoubleBuf==true)&&(m_ImageBuffer.IsNull()==false))
	{
		if (m_bReDraw==true)
		{
			//创建字体
			CFont DrawFont;
			DrawFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
			CDC * pDC=CDC::FromHandle(m_ImageBuffer.GetDC());
			CFont * pOldFont=pDC->SelectObject(&DrawFont);

			//更新缓冲
			m_bReDraw=false;
			pDC->SetBkMode(TRANSPARENT);
			pDC->FillSolidRect(0,0,ClientRect.Width(),ClientRect.Height(),RGB(0,0,150));
			DrawGameView(pDC,ClientRect.Width(),ClientRect.Height());
//			DrawGameViewFrame(pDC,ClientRect.Width(),ClientRect.Height());

			//清理资源
			pDC->SelectObject(pOldFont);
			m_ImageBuffer.ReleaseDC();
			DrawFont.DeleteObject();
		}

		//绘画界面
		CRect rcClip;
		dc.GetClipBox(&rcClip);
		m_ImageBuffer.BitBlt(dc,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),rcClip.left,rcClip.top);
	}
	else 
	{
		//创建字体
		CFont DrawFont;
		DrawFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
		CFont * pOldFont=dc.SelectObject(&DrawFont);

		//绘画界面
		dc.SetBkMode(TRANSPARENT);
		DrawGameView(&dc,ClientRect.Width(),ClientRect.Height());
//		DrawGameViewFrame(&dc,ClientRect.Width(),ClientRect.Height());

		//清理资源
		dc.SelectObject(pOldFont);
		DrawFont.DeleteObject();
	}

	return;
}

//位置变化
void CGameParrentView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//效验状态
	if ((cx==0)||(cy==0)) return;
	if (GetSystemMetrics(SM_CXSCREEN)<cx) return;

	//更改缓冲图
	if ((m_bDoubleBuf==true)&&(nType!=SIZE_MINIMIZED))
	{
		if ((cx>m_ImageSize.cx)||(cy>m_ImageSize.cy))
		{
			m_bReDraw=true;
			m_ImageBuffer.Destroy();
			m_ImageSize.cx=__max(cx,m_ImageSize.cx);
			m_ImageSize.cy=__max(cy,m_ImageSize.cy);
			m_ImageBuffer.Create(m_ImageSize.cx,m_ImageSize.cy,m_uColorCount);
		}
	}

	//更新视图
	RectifyGameView(cx,cy);

	//设置绘画数据
	if (m_pThreadDraw!=NULL)
	{
		m_pThreadDraw->SetClientRange(cx,cy);
		if (nType==SIZE_MINIMIZED) m_pThreadDraw->SuspendDrawThread();
		else m_pThreadDraw->ResumeDrawThread();
	}
	else UpdateGameView(NULL);

	return;
}

//鼠标消息
void CGameParrentView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint ClientPoint;
	GetCursorPos( &ClientPoint);
	ScreenToClient( &ClientPoint);
	if (ClientPoint.y<20)
	AfxGetMainWnd()->PostMessage( WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(ClientPoint.x,ClientPoint.y));
	SetFocus();
	__super::OnLButtonDown(nFlags, point);
}

//销毁消息
void CGameParrentView::OnDestroy()
{
	__super::OnDestroy();

	//清理线程
	if (m_pThreadDraw!=NULL) m_pThreadDraw->EndThread();

	return;
}

//////////////////////////////////////////////////////////////////////////
