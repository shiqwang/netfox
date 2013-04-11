// LabelEx.cpp : implementation file
//

#include "stdafx.h"
//#include "TestLabelEx.h"
#include "Resource.h"
#include "LabelEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelEx

CLabelEx::CLabelEx()
{
	djObjW=OBJ_W;
	djObjH=OBJ_H;
	djObjBlank = OBJ_BLANK;
	djObjNumber = 6;
	djStartX=OBJ_START_X;
	djStartY=OBJ_START_Y;
	
	m_bClicked=FALSE;
	m_bOver=FALSE;
	m_bUnderLine=FALSE;
	m_bAutoUnderLine=FALSE;
	m_bBorder=FALSE;
	m_bHighLight=TRUE;
	m_bBackUnderLine=FALSE;
	
	m_crText=RGB(0,0,0);
	m_crBorder=RGB(0,0,0);
	m_crBG=RGB(255,255,255);
	m_crUnderLine=RGB(0,0,255);
	
	m_hHandCur=AfxGetApp()->LoadCursor(IDC_JIAN);

	m_hXCur=AfxGetApp()->LoadCursor(IDC_X);
	
	showModeInt = 0;
//	InitObjListData();
	//	InitDrawKuang();
	isCreatePen = false;
	isActiveBool = false;
	
}

CLabelEx::~CLabelEx()
{
	if(m_BGBm.GetSafeHandle()!=NULL)
		m_BGBm.DeleteObject();
	if(m_MouseOverBGBm.GetSafeHandle()!=NULL)
		m_MouseOverBGBm.DeleteObject();
	if(m_ClickedBGBm.GetSafeHandle()!=NULL)
		m_ClickedBGBm.DeleteObject();
	if(m_LabelBm.GetSafeHandle()!=NULL)
		m_LabelBm.DeleteObject();
	if(m_MouseOverLabelBm.GetSafeHandle()!=NULL)
		m_MouseOverLabelBm.DeleteObject();
	if(m_ClickedLabelBm.GetSafeHandle()!=NULL)
		m_ClickedLabelBm.DeleteObject();
}


BEGIN_MESSAGE_MAP(CLabelEx, CStatic)
//{{AFX_MSG_MAP(CLabelEx)
ON_WM_PAINT()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelEx message handlers

void CLabelEx::OnPaint() 
{
	
	CPaintDC dc(this); // device context for painting
	dc.SetTextColor(m_crText);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(this->GetFont());
	///准备工作
	CRect rect;
	CDC MemDC;
	CPen BorderPen,*pOldPen,UnderLinePen;
	CBrush BGBrush,*pOldBrush;
	BITMAP bm;
	int nTextLeft=0,nTextTop=0; //文字输出的位置
	
	this->GetClientRect(&rect);
	
	MemDC.CreateCompatibleDC(&dc);
	MemDC.SetMapMode(dc.GetMapMode());
	
	///画边框
	if(m_bBorder)
	{
		BorderPen.CreatePen(PS_SOLID,1,m_crBorder);
		BGBrush.CreateSolidBrush(m_crBG);
		
		pOldPen=dc.SelectObject(&BorderPen);
		pOldBrush=dc.SelectObject(&BGBrush);
		
		dc.Rectangle(&rect);
		
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
		
		rect.DeflateRect(1,1);
	}
	///贴背景图
	if(m_bClicked && m_ClickedBGBm.GetSafeHandle()!=NULL)
	{
		MemDC.SelectObject(m_ClickedBGBm);
		dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,SRCCOPY);
	}
	else if(m_bOver && m_MouseOverBGBm.GetSafeHandle()!=NULL)//鼠标经过的时候
	{
		MemDC.SelectObject(m_MouseOverBGBm);
		dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,SRCCOPY);
	}
	else if(m_BGBm.GetSafeHandle()!=NULL)
	{
		MemDC.SelectObject(m_BGBm);
		dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,SRCCOPY);
	}
	///贴标签图片
	if(m_bClicked && m_ClickedLabelBm.GetSafeHandle()!=NULL)
	{
		m_ClickedLabelBm.GetBitmap(&bm);
		double fScal=bm.bmWidth*1.0/bm.bmHeight;
		nTextLeft=int(rect.Height()*fScal)+4;
		MemDC.SelectObject(m_ClickedLabelBm);
		dc.StretchBlt(rect.left,rect.top,int(rect.Height()*fScal),rect.Height(),
			&MemDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	}
	else if(m_bOver && m_MouseOverLabelBm.GetSafeHandle()!=NULL)
	{
		m_MouseOverLabelBm.GetBitmap(&bm);
		double fScal=bm.bmWidth*1.0/bm.bmHeight;
		nTextLeft=int(rect.Height()*fScal)+4;
		MemDC.SelectObject(m_MouseOverLabelBm);
		dc.StretchBlt(rect.left,rect.top,int(rect.Height()*fScal),rect.Height(),
			&MemDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	}
	else if(m_LabelBm.GetSafeHandle()!=NULL)
	{
		m_LabelBm.GetBitmap(&bm);
		double fScal=bm.bmWidth*1.0/bm.bmHeight;
		nTextLeft=int(rect.Height()*fScal)+4;
		MemDC.SelectObject(m_LabelBm);
		dc.StretchBlt(rect.left,rect.top,int(rect.Height()*fScal),rect.Height(),
			&MemDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	}
	else
	{
		nTextLeft=4;
	}
	///输出文字
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	CString strText;
	this->GetWindowText(strText);
	nTextTop=rect.top+(rect.Height()-tm.tmHeight)/2;
	if(strText.GetLength()>0)
	{	
		dc.TextOut(nTextLeft,nTextTop,strText);
	}
	
	///画下划线
	if(m_bUnderLine)
	{
		nTextLeft-=2;
		nTextTop=nTextTop+tm.tmHeight+1;
		UnderLinePen.CreatePen(PS_SOLID,1,m_crUnderLine);
		pOldPen=dc.SelectObject(&UnderLinePen);
		dc.MoveTo(nTextLeft,nTextTop);
		dc.LineTo(nTextLeft+tm.tmAveCharWidth*strText.GetLength(),nTextTop);
	}
	
	//		DrawAllKuangPic();
}

void CLabelEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	
	SetFont(GetParent()->GetFont());
	CStatic::PreSubclassWindow();
}
//鼠标移动
void CLabelEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( !isCreatePen)
	{
//		InitDrawKuang();
		isCreatePen = true;
	}

	// TODO: Add your message handler code here and/or call default
	/*
	if (m_bOver)        // Cursor is currently over control
    {
	CRect rect;
	GetClientRect(rect);
	
	  if (!rect.PtInRect(point))
	  {
	  m_bOver = FALSE;
	  if(m_bAutoUnderLine)  ///自动下划线
	  {
	  this->SetUnderLine(FALSE,RGB(0,0,0));
	  }
	  if(m_bHighLight)   //自动高亮
	  {
	  ///恢复原来的字体颜色
	  this->SetTextColor(m_crBackText);
	  }
	  RedrawWindow();
	  ReleaseCapture();
	  return;
	  }
	  }
	  else                      // Cursor has just moved over control
	  {
	  m_bOver = TRUE;
	  if(m_bAutoUnderLine)
	  {
	  this->SetUnderLine(TRUE,RGB(0,0,255));
	  }
	  if(m_bHighLight)
	  {
	  m_crBackText=m_crText;
	  this->SetTextColor(RGB(0,0,255));
	  }
	  RedrawWindow();
	  SetCapture();
	  ::SetCursor(m_hHandCur);
	  }
	*/
	//重画界面
//	Invalidate(FALSE);

	int x=point.x,y=point.y;
	
	GetOpSelectObj(  x,   y);
	//
	//	if ( IsInRect( x, y))
	
	
	CStatic::OnMouseMove(nFlags, point);
}

void CLabelEx::SetTextColor(COLORREF crText)
{
	m_crText=crText;
}

void CLabelEx::SetUnderLine(BOOL bUnderLine, COLORREF crUnderLine)
{
	m_bUnderLine=bUnderLine;
	m_crUnderLine=crUnderLine;
	
}

void CLabelEx::SetBorder(BOOL bBorder, COLORREF crBorder)
{
	m_bBorder=bBorder;
	m_crBorder=crBorder;
}

void CLabelEx::SetBkColor(COLORREF crBkColor)
{
	m_crBG=crBkColor;
	
}

void CLabelEx::EnableAutoUnderLine(BOOL bAutoUnderLine)
{
	m_bAutoUnderLine=bAutoUnderLine;
	if(m_bAutoUnderLine)   ///设置了自动下划线
	{
		m_bBackUnderLine=m_bUnderLine;//保存原来下划线的状态
		m_bUnderLine=FALSE;
	}
	else   ///取消了自动下划线
	{
		m_bUnderLine=m_bBackUnderLine;//回复原来下划线的状态
	}
}

void CLabelEx::RedrawWindow()
{
	CStatic::RedrawWindow();
	//RECT rect;
	//this->GetWindowRect(&rect);
	//GetParent()->ScreenToClient(&rect);
	//GetParent()->InvalidateRect(&rect);
}

void CLabelEx::SetBGBitmap(UINT ID)
{
	if(m_BGBm.GetSafeHandle()!=NULL)
		m_BGBm.DeleteObject();
	m_BGBm.LoadBitmap(ID);
	
}

void CLabelEx::SetMouseOverBGBitmap(UINT ID)
{
	if(m_MouseOverBGBm.GetSafeHandle()!=NULL)
		m_MouseOverBGBm.DeleteObject();
	m_MouseOverBGBm.LoadBitmap(ID);
}

void CLabelEx::SetClickedBGBitmap(UINT ID)
{
	if(m_ClickedBGBm.GetSafeHandle()!=NULL)
		m_ClickedBGBm.DeleteObject();
	m_ClickedBGBm.LoadBitmap(ID);
	
}

void CLabelEx::SetLabelBitmap(UINT ID)
{
	if(m_LabelBm.GetSafeHandle()!=NULL)
		m_LabelBm.DeleteObject();
	m_LabelBm.LoadBitmap(ID);
	
}


void CLabelEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//	m_bClicked=!m_bClicked;
	//	RedrawWindow();
	// TODO: Add your message handler code here and/or call default
	int x=point.x,y=point.y;
	int opType = 1;
	GetOpSelectObj( x,  y,  opType);
	
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CLabelEx::SetMouseOverLabelBitmap(UINT ID)
{
	if(m_MouseOverLabelBm.GetSafeHandle()!=NULL)
		m_MouseOverLabelBm.DeleteObject();
	m_MouseOverLabelBm.LoadBitmap(ID);
	
}

void CLabelEx::SetClickedLabelBitmap(UINT ID)
{
	if(m_ClickedLabelBm.GetSafeHandle()!=NULL)
		m_ClickedLabelBm.DeleteObject();
	m_ClickedLabelBm.LoadBitmap(ID);
	
}

///自动感应字体颜色
void CLabelEx::EnableHighLight(BOOL bHighLight)
{
	m_bHighLight=bHighLight;
}


void CLabelEx::DrawOneKuangPic(int showX, int showY)
{
	CClientDC dc(this); // device context for painting
	
	mdc->SelectObject(bgbmp);
	RECT myrect;
	GetClientRect(&myrect);
	int w,h;
	w = djObjW*OBJ_NUM;
	h = djObjH;
	
	dc.BitBlt( showX , showY , w, h , mdc , 0 , 0 , SRCCOPY );
	
}
void CLabelEx::InitDrawKuang()
{
	//建立画笔
	pen0.CreatePen(PS_SOLID,1, COLOR_TYPE_BLUE/*RGB(0x0,0xf0,0xf0)*/);
	pen1.CreatePen(PS_SOLID,1,COLOR_TYPE_RED /*RGB(0x60,0x60,0x60)*/);	
	pen2.CreatePen(PS_SOLID,1,COLOR_TYPE_GREEN /*RGB(0x60,0x60,0x60)*/);
	
	//doctor
	CClientDC dc(this);	
	GetClientRect(&rect);
	mdc=new CDC;
	//背景的粘贴
	bgbmp= new CBitmap;
	
	mdc->CreateCompatibleDC( &dc );  
	bgbmp->LoadBitmap(IDB_BITMAP_KUANG);	/// 加载背景
	
}

//
int CLabelEx::GetSelectObjInt(int x,int showX, int showY)
{
	int resultInt=-1;
	resultInt =  (x-showX) / (djObjW+djObjBlank);
	return resultInt;	
}
//画选择的OBJ
void CLabelEx::DrawSelectOneObj(int objInt ,int opType, int showX, int showY)
{
	//	DrawAllObj(m_showX, m_showY);
	DrawAllKuangObj();
	CPen *old;
	CClientDC dc(this); // device context for painting
	if ( 1 == opType)
	{
		old=dc.SelectObject(&pen1);
	}
	else
	{
		old=dc.SelectObject(&pen2);
	}
	int x,y=0,w=djObjW,h=djObjH;
	x= showX;//2;	
	y= showY;//HEIGHT+18;

	int textXInt;
	//for(int j=0;j< 6;j++)
	{
		x=showX+objInt*(djObjW+djObjBlank);
		textXInt = showX-djObjW/2;
		dc.MoveTo(x,y);
		dc.LineTo(x+w,y);	 
		dc.LineTo(x+w,y+h);
		dc.LineTo(x,y+h);	 
		dc.LineTo(x,y);
	}

	//显示提示信息
	dc.SetTextColor(RGB(255,255,255));

	CRect rcDraw;
	CString showMsg;


	int textW = 25;

	if ( djObjNumber < 5)
		textW = 26;
	rcDraw.left= textXInt;
	rcDraw.right= rcDraw.left+ textW*djObjNumber;
	rcDraw.top= showY - 15;
	rcDraw.bottom=rcDraw.top+15;


	for (int i=0; i < djObjNumber; i++)
	{
		showMsg += "位置";
	}

	dc.DrawText( showMsg, showMsg.GetLength(),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	dc.SetTextColor(RGB(255,0,0));
	rcDraw.left= x ;
	rcDraw.right= rcDraw.left+(30);
	rcDraw.top= showY - 15;
	rcDraw.bottom=rcDraw.top+15;
	dc.DrawText( objNameStr[objInt],objNameStr[objInt].GetLength(),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

}
void CLabelEx::DrawAllKuangObj()
{
	for (int i=0; i <maxUseObjNum ; i++)
	{
		int showX=objList[i].m_showX ;
		int showY=objList[i].m_showY;
		
		//	DrawAllObj( showX,  showY);
		DrawAllDJObj( showX,  showY);
	}	
}
void CLabelEx::DrawAllDJObj(int showX, int showY)
{
	
	CPen *old;
	CClientDC dc(this); // device context for painting
	
	old=dc.SelectObject(&pen0);
	int x,y=0,w=djObjW,h=djObjH;
	x= showX;//2;	
	y= showY;//HEIGHT+18;
	
	for(int j=0;j< djObjNumber && j< OBJ_NUM;j++)
	{
		x= showX+j*((djObjW+djObjBlank));
		dc.MoveTo(x,y);
		dc.LineTo(x+w,y);	 
		dc.LineTo(x+w,y+h);
		dc.LineTo(x,y+h);	 
		dc.LineTo(x,y);
	}	
}
/*
void CLabelEx::DrawAllObj(int showX, int showY)
{
CPen *old;
CClientDC dc(this); // device context for painting

  old=dc.SelectObject(&pen0);
  int x,y=0,w=OBJ_W,h=OBJ_H;
  x= showX;//2;	
  y= showY;//HEIGHT+18;
  
	for(int j=0;j< OBJ_NUM;j++)
	{
	x= showX+j*(OBJ_W+OBJ_BLANK);
	dc.MoveTo(x,y);
	dc.LineTo(x+w,y);	 
	dc.LineTo(x+w,y+h);
	dc.LineTo(x,y+h);	 
	dc.LineTo(x,y);
	}
	}
*/
//设置DJ数据
void CLabelEx::InitDJDataList(DRAW_KUANG_OBJ *myItem, int djUseNum)
{

	for (int i=0; i < djUseNum && i < djObjNumber; i++)
	{
		
		objList[i].objInt = myItem[i].objInt;		
		memcpy( objList[i].djObjName , myItem[i].djObjName, 50);
	}
	
	
}
void CLabelEx::InitMyselfObjListData()
{
	int i;
	for (i=0; i < OBJ_MAX_NUM-2; i++)
	{
		
		memset( (char *)&objList[i] , 0, sizeof(DRAW_KUANG_OBJ));
	}
	
	int objInt;
	int showX;
	int showY;
	
	for ( i=0; i <OBJ_MAX_NUM-2; i++)
	{
		showX=djStartX + i* (djObjW+djObjBlank);
		showY=djStartY + i*40;
		SetOneObjData( i,  showX,  showY);
	}
	
	maxUseObjNum = 1;
	
	int j=0;
	objNameStr[j]="语音";
		j++;
	objNameStr[j]="视频";
		j++;
	objNameStr[j]="缩放";
		j++;
	objNameStr[j]="菜单";
		j++;

	djObjNumber = 4;

}
void CLabelEx::InitObjListData()
{
	int i;
	for (i=0; i < OBJ_MAX_NUM; i++)
	{
		
		memset( (char *)&objList[i] , 0, sizeof(DRAW_KUANG_OBJ));
	}
	
	int objInt;
	int showX;
	int showY;
	
	for ( i=0; i <OBJ_MAX_NUM; i++)
	{
		showX=djStartX + i* (djObjW+djObjBlank);
		showY=djStartY + i*40;
		SetOneObjData( i,  showX,  showY);
	}
	
	maxUseObjNum = 1;
	
	int j=0;
	objNameStr[j]="礼物";
	j++;
	objNameStr[j]="好友";
		j++;
	objNameStr[j]="语音";
		j++;
	objNameStr[j]="视频";
		j++;
	objNameStr[j]="缩放";
		j++;
	objNameStr[j]="菜单";
		j++;
}
void CLabelEx::SetOneObjData(int objInt, int showX, int showY)
{
	objList[objInt].m_showX = showX;
	objList[objInt].m_showY = showY;
}

int CLabelEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
//	InitDrawKuang();	
	return 0;
}
void CLabelEx::DrawAllKuangPic()
{

	for (int i=0; i < maxUseObjNum ; i++)
	{
		int showX=objList[i].m_showX ;
		int showY=objList[i].m_showY;
		
		DrawOneKuangPic( showX,  showY);
	}
}
//获取被选择的对象
//输入:0=MOUSE MOVE, 1=DOWN
//返回:
void CLabelEx::GetOpSelectObj(int x, int y, int opType)
{
	if ( ! isActiveBool)
	{
		
	::SetCursor(m_hXCur);
		return;
	}




	int objID = GetSelectObjID( x,  y);
	if ( objID > -1)
	{
		int showX,showY ;
		showX = objList[objID].m_showX ;
		showY = objList[objID].m_showY ;
		int objInt = 	GetSelectObjInt( x, showX, showY);
		if ( objInt > -1 && objInt < OBJ_NUM && objInt <djObjNumber)
		{
			::SetCursor(m_hHandCur);
			switch (opType)
			{ 
			case 0:
				{
					DrawSelectOneObj( objInt, 0, showX, showY);
				}
				break;
				//发送信息
			case 1:
				{
					DrawSelectOneObj( objInt, 1, showX, showY);
					
					//发送消息 objID大范围, objInt范围
					CString showMsg;
					showMsg.Format("choose big=%d, small=%s",
						objID,
						objNameStr[objInt]);
					//	AfxMessageBox( showMsg);
					::SendMessage(my_hWnd /*AfxGetMainWnd()->m_hWnd*/,MESSAGE_CHOOSE_OBJ, labelID, objList[objInt].objInt);
				}
				break;
			}
			
		}
	}	
	
	
	
	
	
}
int CLabelEx::GetSelectObjID(int x, int y)
{
	int resultInt =-1;
	for (int i=0; i < maxUseObjNum; i++)
	{
		if (IsInRect( x, y, objList[i].m_showX, objList[i].m_showY))
		{
			return i;
		}
	}
	return resultInt;	
}

bool CLabelEx::IsInRect(int x,int y , int showX, int showY)
{
	bool resultBool = false;
	if ( x>= showX && x <= (((djObjW+djObjBlank)*djObjNumber)+showX) &&  
		y>= showY && y <= (OBJ_H+showY))
	{
		resultBool=true;
	}
	return resultBool;
}

BOOL CLabelEx::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
//	InitDrawKuang();	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

