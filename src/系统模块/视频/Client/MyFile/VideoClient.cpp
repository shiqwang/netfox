#include "StdAfx.h"
#include ".\videoclient.h"
#include "UserProcess.h"

#ifdef _DEBUG
	#pragma comment( lib, "strmbasd.lib" )
#else
	//#pragma comment( lib, "STRMBASE.lib" )
#endif

//安全释放
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

CVideoClient* g_pVideoClient = NULL;
extern CRITICAL_SECTION g_CheckCritical;
extern USER_INFO m_ServerIP;

DWORD CLIENT_PORT;

extern HDRAWDIB m_HDrawDib;
extern PBITMAPINFO m_BmpInfo;

CVideoClient::CVideoClient(void)
{
	g_pVideoClient = this;
	srand( (unsigned)time( NULL ) );
	CLIENT_PORT = 0; //rand();
	InitializeCriticalSection( &g_CheckCritical );
}

CVideoClient::~CVideoClient(void)
{
	g_pVideoClient = NULL;

	DrawDibClose( CVideoWindow::m_HDrawDib );
	if( CVideoWindow::m_BmpInfo )
	{
		delete CVideoWindow::m_BmpInfo;
		CVideoWindow::m_BmpInfo = NULL;
	}
	Release();
	DeleteCriticalSection( &g_CheckCritical ); //删掉临界变量
}

void CVideoClient::Init(void)
{
	Release();
	UdpManage.Init(11,4);
	UdpManage.CreateSocket<CMyUserManage>( CLIENT_PORT );

	OutMin.Init();
	InMin.Init();
}

void CVideoClient::Release(void)
{
//	Quit();
	UdpManage.Release();
	InMin.Release();
	OutMin.Release();
}

BOOL CVideoClient::Login( char* ServerIP, int Port, DWORD RoomID, DWORD DeskID, DWORD Pos )
{
	m_Room = RoomID;
	m_Desk = DeskID;
	char Tmp[MAX_PATH];
	if( gethostname( Tmp, MAX_PATH )==0 )
	{
		PHOSTENT HostInfo;
		HostInfo = gethostbyname( Tmp );
		if( HostInfo && HostInfo->h_addr_list[0] )
		{
			DWORD LocalIP = (*(struct in_addr *)HostInfo->h_addr_list[0]).S_un.S_addr;
			int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_LOGIN, RoomID, DeskID, Pos, LocalIP, UdpManage.GetSocket(0)->GetPort() );
			if( len>0 )
			{
				UdpManage.Send( Tmp, len , ServerIP, Port );
				//UdpManage.SendAllUser( Tmp, len , ServerIP, Port );
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CVideoClient::SendAllUser( char* Buffer, int len )
{
	/*for( int i=0;i<UdpManage.m_ServerSocketSize;i++ )
	{
		if( UdpManage.m_pSocketArray[i] )
			UdpManage.m_pSocketArray[i]->Send( Buffer, len );
	}*/
	try
	{
		CMyUserManage* p = (CMyUserManage*)UdpManage.GetActiveSocket();
		if( p )
			p->SendAllUser( Buffer, len );
	}
	catch(...)
	{
	}
}

void CVideoClient::OutSound( char* pData, int len )
{
	OutMin.OutSound( pData, len );
}

void CVideoClient::OutVideo( DWORD Pos, char* pData, int len )
{
	if( Pos == INFINITE )
	{
		if( CUserProcess::m_ServerIP.POS!=0 )
		{
			m_VideoWindow[CUserProcess::m_ServerIP.POS-1].OutVideo( pData, len );
		}
	}
	else
        m_VideoWindow[Pos].OutVideo( pData, len );
}

void CVideoClient::SaveSoundData( char* Buffer, int len )
{
    CMyUserManage* p = (CMyUserManage*)UdpManage.GetActiveSocket();
	if( p )
		p->SaveSoundData( Buffer, len );
}

void CVideoClient::OutString( DWORD Pos, char* pchar )
{
	if( Pos>0 && Pos<10 )
	{
		m_VideoWindow[Pos-1].OutString( pchar );
	}
}

CUdpSocket* CVideoClient::GetSocket(int i)
{
	return UdpManage.GetSocket(i);
}

void CVideoClient::Quit()
{
	char tmp[MAX_PATH];
	int len = m_MyPacket.GetPacket( (char*)tmp, MAX_PATH, PACKET_QUIT, 1, NULL );
	SendAllUser( tmp, len );
}

void CVideoClient::SendDebugMsg(char* p)
{
//#ifdef _DEBUG
	HWND hWnd = FindWindow( NULL, "VideoDebug" );
	if( hWnd )
	{
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.cbData = strlen(p)+1;
		cds.lpData = p;
		SendMessage(hWnd,WM_COPYDATA,0,(LPARAM)&cds);
	}
//#endif
}