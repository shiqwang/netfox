#include "Stdafx.h"
#include "AfxPriv.h"
#include "SkinDialog.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSkinDialog, CDialog)

BEGIN_MESSAGE_MAP(CSkinDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SETTINGCHANGE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSkinDialogEx, CSkinDialog)

BEGIN_MESSAGE_MAP(CSkinDialogEx,CSkinDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CRgnDialog, CDialog)

BEGIN_MESSAGE_MAP(CRgnDialog,CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinDialog::CSkinDialog(UINT nIDTemplate, CWnd * pParentWnd) 
	: CDialog(nIDTemplate,pParentWnd), CSkinWndObject(this)
{
}

//析构函数
CSkinDialog::~CSkinDialog()
{
}

//初始化消息
BOOL CSkinDialog::OnInitDialog()
{
	__super::OnInitDialog();

	//初始化
	InitSkinObject();
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return TRUE;
}

//激活消息
void CSkinDialog::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState,pWndOther,bMinimized);

	m_bActive=(nState!=WA_INACTIVE);
	DrawSkinView(NULL);

	return;
}

//激活消息
void CSkinDialog::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive,dwThreadID);

	m_bActive=bActive?true:false;
	DrawSkinView(NULL);

	return;
}

//鼠标消息
void CSkinDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags,point);
	OnLMouseDoubleEvent(point);
	return;
}

//鼠标消息
void CSkinDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);
	OnLMouseDownEvent(point);
	return;
}

//鼠标消息
void CSkinDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags,point);
	OnLMouseUpEvent(point);
	return;
}

//鼠标消息
void CSkinDialog::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnRButtonDblClk(nFlags,point);
	OnRMouseDoubleEvent(point);
	return;
}

//鼠标消息
void CSkinDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
	__super::OnRButtonDown(nFlags,point);
	OnRMouseDownEvent(point);
	return;
}

//鼠标消息
void CSkinDialog::OnRButtonUp(UINT nFlags, CPoint point)
{
	__super::OnRButtonUp(nFlags,point);
	OnRMouseUpEvent(point);
	return;
}

//鼠标消息
void CSkinDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags,point);
	OnMouseMoveEvent(point);
	return;
}

//绘画消息 
void CSkinDialog::OnPaint()
{
	CPaintDC dc(this);
	DrawSkinView(&dc);
	return;
}

//位置改变
void CSkinDialog::OnWindowPosChanged(WINDOWPOS * lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	//调整位置
	CRect rcClient;
	m_cbNowHotButton=255;
	m_cbNowDownButton=255;
	GetClientRect(&rcClient);
	AdjustTitleButton(rcClient.Width(),rcClient.Height());

	//重画界面
	DrawSkinView(NULL);

	return;
}

//设置改变
void CSkinDialog::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags, lpszSection);

	//调整框架大小
	CSkinWndObject::OnSettingChange();

	return;
}

//绘画背景
BOOL CSkinDialog::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//控件颜色
HBRUSH CSkinDialog::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(m_SkinAttribute.m_crControlTXColor);
			return m_SkinAttribute.m_brBackGround;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//标题消息
LRESULT	CSkinDialog::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);
	DrawSkinView(NULL);
	return lResult;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinDialogEx::CSkinDialogEx(UINT nIDTemplate, CWnd * pParentWnd) : CSkinDialog(nIDTemplate,pParentWnd)
{
}

//析构函数
CSkinDialogEx::~CSkinDialogEx()
{
}

//重画消息
void CSkinDialogEx::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//绘画标题
	DrawSkinView(&dc);

	//绘画背景
	int nXExcursion=GetXExcursionPos();
	int nYExcursion=GetYExcursionPos();
	dc.FillSolidRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,4,m_SkinAttribute.m_crBackFrame);
	dc.FillSolidRect(nXExcursion,ClientRect.Height()-8,ClientRect.Width()-2*nXExcursion,7,m_SkinAttribute.m_crBackFrame);
	dc.FillSolidRect(nXExcursion,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,m_SkinAttribute.m_crBackFrame);
	dc.FillSolidRect(ClientRect.Width()-nXExcursion-12,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,m_SkinAttribute.m_crBackFrame);

	//绘画内框
	CPen BorderPen(PS_SOLID,1,m_SkinAttribute.m_crInsideBorder);
	CPen * pOldPen=dc.SelectObject(&BorderPen);
	CRect rcDrawRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,ClientRect.Height());
	
	//COLORREF BGColor=RGB(255,171,11);

	//dc.SelectObject(m_SkinAttribute.m_brBackGround);
	//dc.SelectObject(BGColor);
	
	
	if (m_bMaxSize==true) rcDrawRect.DeflateRect(3,3,3,2);
	else rcDrawRect.DeflateRect(3,4,3,4);
	dc.RoundRect(&rcDrawRect,CPoint(8,8));
	dc.SelectObject(pOldPen);
	BorderPen.DeleteObject();

	return;
}

//位置消息
void CSkinDialogEx::OnSize(UINT nType, int cx, int cy)
{
	CSkinDialog::OnSize(nType, cx, cy);

	//刷新界面
	Invalidate(FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CRgnDialog::CRgnDialog(UINT uTemplate, CWnd * pParent) : CDialog(uTemplate,pParent)
{
}

//析构函数
CRgnDialog::~CRgnDialog()
{
}

//初始化函数
BOOL CRgnDialog::OnInitDialog()
{
	__super::OnInitDialog();

	//删除窗口标题和获取属性
	ModifyStyle(WS_CAPTION,0,SWP_DRAWFRAME);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return TRUE;
}

//重画函数
void CRgnDialog::OnPaint()
{
	CPaintDC dc(this);

	//绘画底图
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CImageHandle ImageHandle(&m_ImageBack);
	if (m_ImageBack.IsNull()==false) m_ImageBack.BitBlt(dc,0,0);

	return;
}

//加载位图
bool CRgnDialog::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);
	return CreateControlRgn(crTrans);
}

//加载位图
bool CRgnDialog::LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans)
{
	m_ImageBack.SetLoadInfo(pszFileName);
	return CreateControlRgn(crTrans);
}

//创建区域
bool CRgnDialog::CreateControlRgn(COLORREF crTrans)
{
	CImageHandle ImageHandle(&m_ImageBack);
	HRGN hRgn=m_ImageBack.CreateImageRegion(crTrans);
	if (hRgn!=NULL)
	{
		SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);
		SetWindowRgn(hRgn,TRUE);
		m_DialogRgn.DeleteObject();
		m_DialogRgn.Attach(hRgn);
		return true;
	}
	return false;
}

//鼠标右键按下消息
void CRgnDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//模拟按标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

//背景函数
BOOL CRgnDialog::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
