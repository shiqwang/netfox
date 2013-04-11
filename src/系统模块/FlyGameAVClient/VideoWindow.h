#pragma once

#include "vfw.h"
#pragma comment( lib, "Vfw32.lib" )
//ÊÓÆµ´°¿Ú
class CVideoWindow
{
private:
	HWND m_hWnd;
public:
	CVideoWindow(void);
	virtual ~CVideoWindow(void);

	static HDRAWDIB m_HDrawDib;
	static PBITMAPINFO m_BmpInfo;

	void SetHwnd( HWND hWnd ){ m_hWnd = hWnd; };

	void OutVideo( char* pData, int len );
	void OutString( char* pchar );
};
