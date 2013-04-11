#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BT_OK,OnBtOK)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));
    m_szFang= new char[256];
	memset(m_szFang,0,sizeof(m_szFang));
	//加载资源
	//m_ImageBack.SetLoadInfo(IDB_GAME_END,AfxGetInstanceHandle());
	m_ImageBack.SetLoadInfo(IDB_TOL3,AfxGetInstanceHandle());
	m_ImageHead.SetLoadInfo(IDB_TOL,AfxGetInstanceHandle());
	m_ImageBody.SetLoadInfo(IDB_TOL2,AfxGetInstanceHandle());
	m_ImageLine.SetLoadInfo(IDB_TOL4,AfxGetInstanceHandle());

	
}

//析构函数
CScoreView::~CScoreView(void)
{
	delete m_szFang;
}

//初始化函数
BOOL CScoreView::OnInitDialog()
{
	__super::OnInitDialog();

	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHBack(&m_ImageBack);
	CImageHandle ImageHHead(&m_ImageHead);
	CImageHandle ImageHBody(&m_ImageBody);
	CImageHandle ImageHLine(&m_ImageLine);
	SetWindowPos(NULL,0,0,m_ImageHead.GetWidth(),m_ImageHead.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);


	CRect CreateRect( m_ImageHead.GetWidth()/2-30,m_ImageHead.GetHeight()-40, m_ImageHead.GetWidth()/2, m_ImageHead.GetHeight() );
	//创建按扭
	//m_ButtonOK.Create( TEXT(""),WS_CHILD, CreateRect, this, IDC_BT_OK );
	//设置按扭( 载图 )
	//m_ButtonOK.SetButtonImage( IDB_ENT, AfxGetInstanceHandle(), 0 );

	HDWP hDwp=BeginDeferWindowPos(7);
	//m_ButtonOK.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	//DeferWindowPos( hDwp, m_ButtonOK, NULL,m_ImageHead.GetWidth()/2-5,m_ImageHead.GetHeight(), 0,0, uFlags );	

	//m_ButtonOK.ShowWindow(SW_SHOW);
	return TRUE;
}
void CScoreView::OnBtOK()
{
	CDialog::OnOK();
}
//重画函数
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 


   // SetupRgn( &dc, m_ImageBack, RGB( 255, 0, 255) );

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(&CSkinAttribute::m_DefaultFont);

	//绘画背景
	CImageHandle ImageHBack(&m_ImageBack);
	CImageHandle ImageHHead(&m_ImageHead);
	CImageHandle ImageHBody(&m_ImageBody);
	CImageHandle ImageHLine(&m_ImageLine);
	//m_ImageBack.BitBlt(dc,0,0);
	//m_ImageBack.AlphaDrawImage( &dc, 0,0,RGB( 255, 0, 255 ) );
	m_ImageHead.AlphaDrawImage( &dc, 0,0,RGB( 255, 0, 255 ) );

	//for(int i=0;i<LINE_COUNT;i++)
	 // m_ImageBack.AlphaDrawImage( &dc, 0,m_ImageHead.GetHeight()+i*m_ImageBack.GetHeight(),RGB( 255, 0, 255 ) );

	// m_ImageLine.AlphaDrawImage( &dc, (m_ImageBack.GetWidth()-m_ImageLine.GetWidth())/2,m_ImageHead.GetHeight()+(LINE_COUNT-20)*m_ImageBack.GetHeight(),RGB( 255, 0, 255 ) );
	// m_ImageBody.AlphaDrawImage( &dc, 0,m_ImageHead.GetHeight()+LINE_COUNT*m_ImageBack.GetHeight(),RGB( 255, 0, 255 ) );
	

	//显示分数
	CRect rcDraw;
	TCHAR szBuffer[64]=TEXT("");

	CFont font;
	font.CreateFont(20,10,0,0,FW_THIN,true,false,false,
		CHINESEBIG5_CHARSET,OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,
		FF_MODERN,"宋体");
	/*dc.SetTextColor(RGB(0,255,0));

	rcDraw.left=90;
	rcDraw.right=194;
	rcDraw.top=130;
	rcDraw.bottom=rcDraw.top+82;
	dc.DrawText(_T("UserName"),lstrlen(_T("UserName")),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	rcDraw.left=200;
	rcDraw.right=m_ImageBack.GetWidth();
	dc.DrawText(_T("Score"),lstrlen(_T("Score")),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);*/

	dc.SetTextColor(RGB(255,255,255));

	WORD i;
	for ( i=0;i<CountArray(m_szUserName);i++)
	{
		//用户名字
		rcDraw.left=50;
		rcDraw.right=90;
		rcDraw.top=i*32+85;
		rcDraw.bottom=rcDraw.top+22;
		dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=90;
		rcDraw.right=m_ImageBack.GetWidth();
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lGameScore[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	}

	//显示税收
	/*rcDraw.left=90;
	rcDraw.right=194;
	rcDraw.top=i*22+150;
	rcDraw.bottom=rcDraw.top+82;
	LPCTSTR pszTax=TEXT("交易税");
	dc.DrawText(pszTax,lstrlen(pszTax),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);*/

	//显示税收
	rcDraw.left=140;
	rcDraw.right=170;
	rcDraw.top=i*22+160;
	rcDraw.bottom=rcDraw.top+32;
	_snprintf(szBuffer,sizeof(szBuffer),"%ld",m_lGameTax);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);


	//strcpy(m_szFang,"训诂花加 1 番，自摸加 1 番，清 胡加 1 番，");
	//dc.SelectObject( &font);
	dc.SetTextColor(RGB(255,255,255));
	rcDraw.left= 20;
	rcDraw.right=m_ImageBack.GetWidth();
	rcDraw.top=i*32+125;
	rcDraw.bottom=rcDraw.top+80;
	
	int max =lstrlen(m_szFang);
	if(lstrlen(m_szFang)<rcDraw.right/8)
	{
		dc.DrawText( m_szFang, lstrlen(m_szFang), &rcDraw,DT_LEFT|DT_WORDBREAK);
	}
	else
	{
		dc.DrawText( m_szFang, rcDraw.right/8, &rcDraw,DT_LEFT|DT_WORDBREAK);
		rcDraw.top+=22;
		rcDraw.bottom=rcDraw.top+80;
		dc.DrawText( m_szFang+rcDraw.right/8, lstrlen(m_szFang)-(rcDraw.right/8), &rcDraw,DT_LEFT|DT_WORDBREAK);
	}
	//dc.DrawTextEx()
	return;
}

//绘画背景
BOOL CScoreView::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CScoreView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//绘画视图
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetGameTax(LONG lGameTax)
{
	//设置变量
	if (m_lGameTax!=lGameTax)
	{
		m_lGameTax=lGameTax;
		Invalidate(FALSE);
	}

	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	//设置变量
	if (wChairID<CountArray(m_lGameScore))
	{
		m_lGameScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		Invalidate(FALSE);
	}

	return;
}
//透明处理
void CScoreView::SetupRgn(CDC *pDC,CSkinImage bImage,COLORREF TransColor)
{
	//bImage.GetWidth()/5;
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
	pWnd->SetForegroundWindow();   
} 

void CScoreView::SetFangText( char *pszFang )
{
	strcpy( m_szFang, pszFang);
}

//////////////////////////////////////////////////////////////////////////
