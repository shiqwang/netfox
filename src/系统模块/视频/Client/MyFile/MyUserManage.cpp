#include "StdAfx.h"
#include ".\myusermanage.h"
#include "UserProcess.h"

extern CRITICAL_SECTION g_CheckCritical;

CMyUserManage::CMyUserManage(void)
{
}

CMyUserManage::~CMyUserManage(void)
{
}

void CMyUserManage::ReceivedData( int length )
{
	//¼ìÑéÊý¾Ý
	if( !CheckSum( length ) )
		return;

	char* p = &m_pRecvBuff[4];

	char Index[MAX_PATH];
	sprintf( Index, "%s:%d", inet_ntoa( m_FromAddr.sin_addr ), htons(m_FromAddr.sin_port) );
	CUser* pUser = m_UserMap.GetData( Index );
	if( !pUser )
	{
		pUser = new CUserProcess;
		if( pUser )
		{
			pUser->Init( this, inet_ntoa( m_FromAddr.sin_addr ), htons(m_FromAddr.sin_port) );
			m_UserMap.PutData( Index, pUser );
		}
	}
	pUser->PutData( m_pRecvBuff, length );
	m_pUdpManage->m_EventQueue.push( pUser );
	PostQueuedCompletionStatus( m_pUdpManage->m_hWorkerIOCPort, (DWORD)0, m_Sid, NULL/*pOvl*/ );
}

void CMyUserManage::SendAllUser( char* Buffer, int len )
{
	EnterCriticalSection( &g_CheckCritical ); //Lock
	USER_MAP::Iterator Iter1 = m_UserMap.m_UserTypeMap.begin();
	USER_MAP::Iterator Iter2 = m_UserMap.m_UserTypeMap.end();
	CUserProcess* pUser;

	for( ;Iter1!=Iter2;Iter1++ )
	{
		pUser = (CUserProcess*)(*Iter1).second;
		if( /*!pUser->TestServer()*/pUser->GetWindowPos()!=0 )
		{
			if( pUser )
			{
				pUser->Send( Buffer, len );
			}
		}
	}
	LeaveCriticalSection( &g_CheckCritical ); //UnLock
}

void CMyUserManage::SaveSoundData( char* Buffer, int len )
{
	EnterCriticalSection( &g_CheckCritical ); //Lock
	USER_MAP::Iterator Iter1 = m_UserMap.m_UserTypeMap.begin();
	USER_MAP::Iterator Iter2 = m_UserMap.m_UserTypeMap.end();
	CUserProcess* pUser;
	for( ;Iter1!=Iter2;Iter1++ )
	{
		pUser = (CUserProcess*)(*Iter1).second;
		if( !pUser->TestServer() )
		{
			if( pUser )
			{
				pUser->SaveSoundData( Buffer, len );
			}
		}
	}
	LeaveCriticalSection( &g_CheckCritical ); //UnLock
}