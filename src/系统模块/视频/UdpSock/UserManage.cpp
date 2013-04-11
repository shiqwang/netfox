#include "StdAfx.h"
#include ".\usermanage.h"

DWORD WINAPI CheckUser(LPVOID lp);
USER_MAP CUserManage::m_UserMap;
HANDLE CUserManage::m_CheckThreadHandle = NULL;

CRITICAL_SECTION g_CheckCritical; //临界变量

CUserManage::CUserManage(void)
{
	m_pUdpManage = NULL;
}

CUserManage::~CUserManage(void)
{
	m_UserMap.DeleteAllData();
	if( m_CheckThreadHandle )
	{
		TerminateThread( m_CheckThreadHandle, 0 );
	}
}

void CUserManage::Init( LPVOID p )
{
	m_pUdpManage = (CUdpManage*)p;
	CreateCheckUserThread();
}; //初始化

void CUserManage::ReceivedData( int length )
{
	//检验数据
	if( !CheckSum( length ) )
		return;

	char* p = &m_pRecvBuff[4];

	char Index[MAX_PATH];
	sprintf( Index, "%s:%d", inet_ntoa( m_FromAddr.sin_addr ), htons(m_FromAddr.sin_port) );
	CUser* pUser = m_UserMap.GetData( Index );
	if( !pUser )
	{
		pUser = new CUser;
		pUser->Init( this, inet_ntoa( m_FromAddr.sin_addr ), htons(m_FromAddr.sin_port) );
		m_UserMap.PutData( Index, pUser );
	}
	pUser->PutData( m_pRecvBuff, length );
	m_pUdpManage->m_EventQueue.push( pUser );
	PostQueuedCompletionStatus( m_pUdpManage->m_hWorkerIOCPort, (DWORD)0, m_Sid, NULL/*pOvl*/ );
}

void CUserManage::CreateCheckUserThread(void)
{
	if( m_CheckThreadHandle==NULL )
		m_CheckThreadHandle = CreateThread(NULL,0,::CheckUser,(LPVOID)this,0,NULL);
}

DWORD WINAPI CheckUser(LPVOID lp)
{
	CUserManage* pUserManage = (CUserManage*)lp;
	while( true )
	{
		Sleep(5000);
		pUserManage->CheckUser(GetTickCount());
	}
}

void CUserManage::CheckUser(DWORD Time)
{
	USER_MAP::Iterator Iter1;
	USER_MAP::Iterator Iter2;
	try
	{
		EnterCriticalSection( &g_CheckCritical ); //Lock
	}
	catch(...)
	{
		return;
	}
	try
	{
		Iter1 = m_UserMap.m_UserTypeMap.begin();
		Iter2 = m_UserMap.m_UserTypeMap.end();
		CUser* pUser;
		for( ;Iter1!=Iter2;Iter1++ )
		{
			pUser = (*Iter1).second;
			if( pUser && pUser->Check(Time) )
			{
				m_UserMap.DeleteData( Iter1->first );
				Iter1 = m_UserMap.m_UserTypeMap.begin();
				Iter2 = m_UserMap.m_UserTypeMap.end();
			}
		}
	}
	catch(...)
	{
		OutInfo( "Error:CheckUser" );
	}
	LeaveCriticalSection( &g_CheckCritical ); //UnLock
}

void CUserManage::SendAllUser( char* Buffer, int len )
{
	EnterCriticalSection( &g_CheckCritical ); //Lock
	USER_MAP::Iterator Iter1 = m_UserMap.m_UserTypeMap.begin();
	USER_MAP::Iterator Iter2 = m_UserMap.m_UserTypeMap.end();
	CUser* pUser;
	for( ;Iter1!=Iter2;Iter1++ )
	{
		pUser = (*Iter1).second;
		if( pUser )
		{
			pUser->Send( Buffer, len );
		}
	}
	LeaveCriticalSection( &g_CheckCritical ); //UnLock
}