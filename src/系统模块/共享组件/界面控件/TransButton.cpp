#include "Stdafx.h"
#include "Resource.h"
#include "TransButton.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(TransButton, CButton)

BEGIN_MESSAGE_MAP(TransButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//宏定义
#define DEF_TEXT_COLOR				RGB(10,10,10)							//默认颜色
#define DEF_TEXT_COLOR_EX			RGB(10,10,10)							//默认颜色

//静态变量
TransButtonAttribute				TransButton::m_SkinAttribute;			//按钮属性

//////////////////////////////////////////////////////////////////////////

//构造函数
TransButtonAttribute::TransButtonAttribute() : CSkinAttribute(TEXT("TransButton"))
{
	//初始化变量
	m_crTextColor=DEF_TEXT_COLOR;

	//注册属性
	RegisterAttribute(SKIN_BUTTON_CRTEXTCOLOR,&m_crTextColor,false);
	RegisterAttribute(SKIN_BUTTON_STRIMAGEPATH,&m_strImagePath,false);

	return;
}

//析构函数
TransButtonAttribute::~TransButtonAttribute() 
{
}

//加载配置
bool __cdecl TransButtonAttribute::LoadSkinOption()
{
	//读取字体颜色
	m_crTextColor=ReadColor(SKIN_BUTTON_CRTEXTCOLOR,DEF_TEXT_COLOR);
	m_ImageBack.SetLoadInfo(IDB_SKIN_BUTTON_BACK,GetModuleHandle(SKIN_CONTROL_DLL_NAME));

	return true;
}

//保存配置
bool __cdecl TransButtonAttribute::SaveSkinOption()
{
	WriteColor(SKIN_BUTTON_CRTEXTCOLOR,m_crTextColor);
	WriteString(SKIN_BUTTON_STRIMAGEPATH,m_strImagePath);
	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
TransButton::TransButton()
{
	m_hIcon=NULL;
	m_bExpand=true;
	m_bHovering=false;
	m_pIMouseEvent=NULL;
	m_crTextColor=m_SkinAttribute.m_crTextColor;
}

//析构函数
TransButton::~TransButton()
{
}

//对象附加到现有窗口
void TransButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)m_SkinAttribute.GetDefaultFont(),TRUE);
	return;
}

//建立消息
int TransButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)m_SkinAttribute.GetDefaultFont(),TRUE);
	return 0;
}

//设置颜色
bool TransButton::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor=crTextColor;
	if (GetSafeHwnd()) Invalidate(FALSE);
	return true;
}

//设置图标
bool TransButton::SetButtonIcon(HICON hIcon)
{
	m_hIcon=hIcon;
	if (GetSafeHwnd()) Invalidate(FALSE);
	return true;
}

//加载位图
bool TransButton::SetButtonImage(LPCTSTR pszFileName, bool bExpandImage)
{
	//效验参数
	ASSERT(pszFileName);
	if (pszFileName==NULL) return false;

	//加载位图
	m_bExpand=bExpandImage;
	m_ImageBack.SetLoadInfo(pszFileName);

	//调整位置
	if (m_bExpand==false) FixButtonSize();
	if (GetSafeHwnd()) Invalidate(FALSE);

	return true;
}

//加载位图
bool TransButton::SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage)
{
	//加载位图
	ASSERT(uBitmapID!=0);
	if (uBitmapID==0) return false;
	m_bExpand=bExpandImage;
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);

	//调整位置
	if (m_bExpand==false) FixButtonSize();
	if (GetSafeHwnd()) Invalidate(FALSE);

	return true;
}

//获取位图参数
bool TransButton::GetButtonLoadInfo(tagImageLoadInfo & LoadInfo)
{
	if (m_ImageBack.IsSetLoadInfo()) return m_ImageBack.GetLoadInfo(LoadInfo);
	return m_SkinAttribute.m_ImageBack.GetLoadInfo(LoadInfo);
}

//默认按钮
void TransButton::SetDefaultButton(bool bResetIcon, bool bResetColor, bool bResetImage)
{
	//恢复默认
	if (bResetImage) 
	{
		m_bExpand=true;
		m_ImageBack.RemoveLoadInfo();
	}
	if (bResetIcon)	m_hIcon=NULL;
	if (bResetColor) m_crTextColor=m_SkinAttribute.m_crTextColor;

	//刷新界面
	if (GetSafeHwnd()) Invalidate(FALSE);

	return;
}

//调整位置
bool TransButton::FixButtonSize()
{
	if (m_ImageBack.IsSetLoadInfo()&&GetSafeHwnd())
	{
		CImageHandle ImageHandle(&m_ImageBack);
		SetWindowPos(NULL,0,0,ImageHandle->GetWidth()/5,ImageHandle->GetHeight(),SWP_NOMOVE);
		return true;
	}
	return false;
}

//鼠标移动消息
void TransButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bHovering==false)
	{
		//注册消息
		m_bHovering=true;
		Invalidate(FALSE);
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
	}
	__super::OnMouseMove(nFlags, point);
}

//鼠标离开消息
LRESULT TransButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	Invalidate(FALSE);
	return 0;
}

//背景函数
BOOL TransButton::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//界面绘画函数
void TransButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//定义变量
	CRect ClientRect;
	GetClientRect(&ClientRect);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//设置 DC
	HDC hDC=lpDrawItemStruct->hDC;
	///////////////////////////////////////////////////////////////////////

	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//目前m_ImageBack是没有用到的参数
	//在SetupRgn中以GetButtonLoadInfo(buttonInfo)得到了按钮的位图信息
	SetupRgn(pDC,m_ImageBack,RGB(255,0,255));

	//CBitmap cBitmap;
	//cBitmap.LoadBitmap(IDB_SKIN_MASK);
	//此IDB_SKIN_MASK为手动裁剪的一个按钮的1/5.目前按钮透明没有问题。
	//如能将此CPP中的m_ImageBack裁剪下1/5，则透明按钮可共用

	///////////////////////////////////////////////////////////////////////
	//获取文字
	CString strText;
	GetWindowText(strText);

	//加载背景图
	CImageHandle ImageHandle;
	if (m_ImageBack.IsSetLoadInfo()) ImageHandle.AttachResource(&m_ImageBack);
	else ImageHandle.AttachResource(&m_SkinAttribute.m_ImageBack);
	if (ImageHandle.IsResourceValid())
	{
		//计算位图位置
		int iPartWidth=ImageHandle->GetWidth()/5,iDrawPos=0;
		if (bDisable) iDrawPos=iPartWidth*4;
		else if (bButtonDown) iDrawPos=iPartWidth;
		else if (m_bHovering) iDrawPos=iPartWidth*3;
		else if (lpDrawItemStruct->itemState&ODS_FOCUS)	iDrawPos=iPartWidth*2;

		//绘画背景图
		if (m_bExpand==false) 
		{
			//ImageHandle->BitBlt(hDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,SRCCOPY);
			ImageHandle->AlphaDrawImage(pDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,RGB(255,0,255));
		}
		else 
			ImageHandle->StretchBlt(hDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,iPartWidth,ImageHandle->GetHeight(),SRCCOPY);
	}
	else
	{
		//绘画默认界面
		CDC * pDC=CDC::FromHandle(hDC);
		pDC->FillSolidRect(&ClientRect,GetSysColor(COLOR_BTNFACE));
		if (bButtonDown) pDC->Draw3dRect(&ClientRect,GetSysColor(COLOR_WINDOWFRAME),GetSysColor(COLOR_3DHILIGHT));
		else pDC->Draw3dRect(&ClientRect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_WINDOWFRAME));
	}

	//绘画图标
	if (bButtonDown) ClientRect.top+=2;
	if (m_hIcon)
	{
		DrawIconEx(hDC,ClientRect.left+6,ClientRect.top+(ClientRect.Height()-16)/2+1,m_hIcon,16,16,0,NULL,DI_NORMAL);
		ClientRect.left+=22;
	}

	//绘画字体
	ClientRect.top+=1;
	::SetBkMode(hDC,TRANSPARENT);
	if (bDisable) ::SetTextColor(hDC,GetSysColor(COLOR_GRAYTEXT));
	else ::SetTextColor(hDC,m_crTextColor);
	DrawText(hDC,strText,strText.GetLength(),ClientRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}
void TransButton::SetupRgn(CDC *pDC,				//窗体的DC指针
						   //CBitmap &cBitmap,		//含有窗体形状的位图对象
						   CSkinImage bImage,
						   COLORREF TransColor)	//透明色
{
	///////////////////////////////////////////////////////////////
	//bimage中可能不包含IDB信息。。。。此路不通
	//能否寻找到cskinimage到bmp的转换?
	////从CSkinImage中得到资源IDB
	//tagImageLoadInfo imageIDB;
	//bImage.GetLoadInfo(imageIDB.uResourceID);
	//从IDB中得到BMP
	//CBitmap cBitmap;
	//cBitmap.LoadBitmap(imageIDB);

	//int IWidth,IHeight;
	//IWidth=bImage.GetWidth()/5;
	//IHeight=bImage.GetHeight();

	//CBitmap cBitmap;
	//cBitmap.LoadBitmap(IDB_SKIN_MASK);

	//从按钮装载信息将按钮图片载入BMP，根据BMP确定透明区域
	tagImageLoadInfo buttonInfo;
	GetButtonLoadInfo(buttonInfo);

	CBitmap cBitmap;
	if((buttonInfo.strFileName)!="")
		cBitmap.LoadBitmap(buttonInfo.strFileName);
	else if((buttonInfo.uResourceID)!=0)
		cBitmap.LoadBitmap(buttonInfo.uResourceID);

	//////////////////////////////////////////////////////////////////


	CDC memDC;
	//创建与传入DC兼容的临时DC
	memDC.CreateCompatibleDC(pDC);

	CBitmap *pOldMemBmp=NULL;
	//将位图选入临时DC
	pOldMemBmp=memDC.SelectObject(&cBitmap);

	CRgn wndRgn;
	//创建总的窗体区域，初始region为0
	wndRgn.CreateRectRgn(0,0,0,0);


	BITMAP bit;   
	cBitmap.GetBitmap (&bit);//取得位图参数，这里要用到位图的长和宽     

	int y;
	for(y=0;y<=bit.bmHeight  ;y++)
	{
		CRgn rgnTemp; //保存临时region

		int iX = 0;
		do
		{
			//跳过透明色找到下一个非透明色的点.
			while (iX <= bit.bmWidth/5  && memDC.GetPixel(iX, y) == TransColor)
				iX++;

			//记住这个起始点
			int iLeftX = iX;

			//寻找下个透明色的点
			while (iX <= bit.bmWidth/5  && memDC.GetPixel(iX, y) != TransColor)
				++iX;

			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp.CreateRectRgn(iLeftX, y, iX, y+1);

			//合并到主"region".
			wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_OR);

			//删除临时"region",否则下次创建时和出错
			rgnTemp.DeleteObject();
		}while(iX <bit.bmWidth/5 );
		iX = 0;
	}
	if(pOldMemBmp)
		memDC.SelectObject(pOldMemBmp);
	CWnd * pWnd = pDC->GetWindow();
	pWnd->SetWindowRgn(wndRgn,TRUE);    
	pWnd->SetForegroundWindow();    
}

//默认回调函数
LRESULT TransButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pIMouseEvent)
	{
		switch (message)
		{
		case WM_MOUSELEAVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseLeft(uButtonID,wParam,lParam);
				break;
			}
		case WM_MOUSEMOVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseMove(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonUp(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonUp(uButtonID,wParam,lParam);
				break;
			}
		}
	}
	return __super::DefWindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
