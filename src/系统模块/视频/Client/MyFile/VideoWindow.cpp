#include "StdAfx.h"
#include ".\videowindow.h"
#include "UserProcess.h"

HDRAWDIB CVideoWindow::m_HDrawDib = NULL;
PBITMAPINFO CVideoWindow::m_BmpInfo = NULL;


CVideoWindow::CVideoWindow(void)
{
	if( !m_HDrawDib )
		m_HDrawDib = DrawDibOpen();

	if( !m_BmpInfo )
	{
		m_BmpInfo=new BITMAPINFO;
		m_BmpInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		m_BmpInfo->bmiHeader.biWidth=IMAGE_WIDTH;
		m_BmpInfo->bmiHeader.biHeight=IMAGE_HEIGHT;
		m_BmpInfo->bmiHeader.biPlanes=1;
		m_BmpInfo->bmiHeader.biBitCount=24;
		m_BmpInfo->bmiHeader.biCompression=0;
		m_BmpInfo->bmiHeader.biSizeImage=0;
		m_BmpInfo->bmiHeader.biXPelsPerMeter=0;
		m_BmpInfo->bmiHeader.biYPelsPerMeter=0;
		m_BmpInfo->bmiHeader.biClrUsed=0;
		m_BmpInfo->bmiHeader.biClrImportant=0;
	}
}

CVideoWindow::~CVideoWindow(void)
{
    m_hWnd = NULL;	
}

void CVideoWindow::OutVideo( char* pData, int len )
{
	HDC hdc = GetDC( m_hWnd );
	DrawDibDraw( m_HDrawDib,
				hdc,
				0,	//左偏移
				0,	//顶偏移
				-1, //缩放x
				-1,	//缩放y
				&m_BmpInfo->bmiHeader, //图象头
				pData,	//图象数据
				//图象大小
				0,	// src :left
				0,	// src :top
				IMAGE_WIDTH,  // src : width
				IMAGE_HEIGHT, // src : height
				//画出模式
				0	// use prev params....
				);
	DeleteDC( hdc );
}

void CVideoWindow::OutString( char* pchar )
{
	try
	{
		HDC hdc = GetDC( m_hWnd );
		TextOut(  hdc, 0, 0, pchar, strlen( pchar ) );
		DeleteDC( hdc );
	}
	catch(...)
	{
	}
}