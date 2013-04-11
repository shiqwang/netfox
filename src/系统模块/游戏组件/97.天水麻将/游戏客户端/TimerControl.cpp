#include "StdAfx.h"
#include "TimerControl.h"
#include "Struct.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTimerControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CTimerControl::CTimerControl()
{
	//配置变量
	m_bDisplay=false;

	//位置变量
	m_BenchmarkPos.SetPoint(0,0);

	//运行变量
	m_dwCurrentIndex=0xFFFFFFFF;

	//内部数据
	m_pSinkWindow=NULL;

	m_nTimer=-1;

	m_bNotify=FALSE;

	m_bPause=FALSE;

	m_bDes=FALSE;
	return;
}

//析构函数
CTimerControl::~CTimerControl()
{
}

void CTimerControl::SetBitmap(UINT uIdBack,UINT uIdArrow)
{
	m_ImageBack.SetLoadInfo(uIdBack,AfxGetInstanceHandle());
	m_ImageArrow.SetLoadInfo(uIdArrow,AfxGetInstanceHandle());
	CImageHandle HandleBack(&m_ImageBack);
	m_Size.cx=m_ImageBack.GetWidth();
	m_Size.cy=m_ImageBack.GetHeight();
}

//设置窗口
void CTimerControl::SetSinkWindow(CWnd * pSinkWindow)
{
	//设置变量
	m_pSinkWindow=pSinkWindow;
}

//设置倒数时钟,direction为方向,secord为秒数,bNotify是到时是否通知
void CTimerControl::SetTimerUp(int direction,int secord,BOOL bNotify)
{
	m_dwCurrentIndex=direction;
	m_nTimer=secord*2;
	m_bNotify=bNotify;
	SetPause(false);
	ShowTimer(true);
}

void CTimerControl::ShowTimer(bool bDisplay)
{
	//状态判断
	if (m_bDisplay==bDisplay) return;

	//设置变量
	m_bDisplay=bDisplay;

	if (m_bDisplay==false)
		m_nTimer=-1;

	//重画控件
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//基准位置
void CTimerControl::SetBenchmarkPos(int nXPos, int nYPos)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	//调整控件
	if (m_hWnd!=NULL) RectifyControl();
}

//调整位置
void CTimerControl::RectifyControl()
{
	int nXPos=0;
	int nYPos=0;
	nXPos=m_BenchmarkPos.x-m_Size.cx/2;
	nYPos=m_BenchmarkPos.y-m_Size.cy/2;
	//移动位置
	SetWindowPos(NULL,nXPos,nYPos,m_Size.cx,m_Size.cy,SWP_NOZORDER);

	//重画界面
	Invalidate(TRUE);
	return;
}

//重画函数
void CTimerControl::OnPaint()
{
	CPaintDC dc(this);
	SetupRgn( &dc, m_ImageBack, RGB( 255, 0, 255) );
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载位图
	CImageHandle HandleBack(&m_ImageBack);
	CImageHandle HandleArrow(&m_ImageArrow);

	//创建位图
	CBitmap BufferImage;
	if (!BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height())) return;

	//创建 DC
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(&BufferImage);

	//m_ImageBack.BitBlt(BufferDC,0,0);
	m_ImageBack.AlphaDrawImage( &BufferDC, 0, 0, COLORTRANS );
	switch(m_dwCurrentIndex)
	{
		case 0:
			m_ImageArrow.AlphaDrawImage(&BufferDC,rcClient.Width()/2-m_ImageArrow.GetWidth()/4/2,0,16,16,0,0,RGB(255,0,255));
			break;
		case 3:
			if (m_bDes)
				m_ImageArrow.AlphaDrawImage(&BufferDC,rcClient.Width()-m_ImageArrow.GetWidth()/4,rcClient.Height()/2-m_ImageArrow.GetHeight()/2,16,16,48,0,RGB(255,0,255));
			else
				m_ImageArrow.AlphaDrawImage(&BufferDC,0,rcClient.Height()/2-m_ImageArrow.GetHeight()/2,16,16,16,0,RGB(255,0,255));
			break;
		case 2:
			m_ImageArrow.AlphaDrawImage(&BufferDC,rcClient.Width()/2-m_ImageArrow.GetWidth()/4/2,rcClient.Height()-m_ImageArrow.GetHeight(),16,16,32,0,RGB(255,0,255));
			break;
		case 1:
			if (m_bDes)
				m_ImageArrow.AlphaDrawImage(&BufferDC,0,rcClient.Height()/2-m_ImageArrow.GetHeight()/2,16,16,16,0,RGB(255,0,255));
			else
				m_ImageArrow.AlphaDrawImage(&BufferDC,rcClient.Width()-m_ImageArrow.GetWidth()/4,rcClient.Height()/2-m_ImageArrow.GetHeight()/2,16,16,48,0,RGB(255,0,255));
			break;
	}

	BufferDC.SetTextColor(RGB(220,220,220));
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextAlign(TA_CENTER);
	CString str;
	if (m_nTimer!=-1)
		str.Format("%02d",m_nTimer/2);
	else
		str="00";
	BufferDC.TextOut(rcClient.Width()/2+2,rcClient.Height()/2-5,str);

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();

	return;
}

//建立消息
int CTimerControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	SetTimer(1245,500,NULL);
	return 0;
}

void CTimerControl::OnTimer(UINT_PTR nIDEvent)
{
	if (m_nTimer!=-1 && m_bPause==FALSE)
	{
		m_nTimer--;
		Invalidate();
		if (m_nTimer==-1 && m_bNotify && m_pSinkWindow)
			m_pSinkWindow->SendMessage(IDM_TIMER_OUT,0,0);
	}
}

void CTimerControl::SetPause(BOOL bPause)
{
	m_bPause=bPause;
	//m_nTimer= 0;
}

void CTimerControl::SetDesOrder(bool bDes)
{
	m_bDes=bDes;
}

//透明处理
void CTimerControl::SetupRgn(CDC *pDC,CSkinImage bImage,COLORREF TransColor){	//bImage.GetWidth()/5;
	tagImageLoadInfo ImageIDB;
	bImage.GetLoadInfo(ImageIDB);
	CBitmap cBitmap;
	cBitmap.LoadBitmap(ImageIDB.uResourceID);//这个BITMAP是5倍长的，所以会有问题死机。
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
			while (iX <= bit.bmWidth  && memDC.GetPixel(iX, y) == TransColor)
				iX++;

			//记住这个起始点
			int iLeftX = iX;

			//寻找下个透明色的点
			while (iX <= bit.bmWidth  && memDC.GetPixel(iX, y) != TransColor)
				++iX;

			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp.CreateRectRgn(iLeftX, y, iX, y+1);

			//合并到主"region".
			wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_OR);

			//删除临时"region",否则下次创建时和出错
			rgnTemp.DeleteObject();
		}while(iX <bit.bmWidth );
		iX = 0;
	}
	if(pOldMemBmp)
		memDC.SelectObject(pOldMemBmp);
	CWnd * pWnd = pDC->GetWindow();
	pWnd->SetWindowRgn(wndRgn,TRUE);
	pWnd->SetForegroundWindow();   } 