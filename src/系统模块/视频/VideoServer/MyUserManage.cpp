#include "StdAfx.h"
#include ".\myusermanage.h"
#include "UserProcess.h"

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
		pUser->Init( this, inet_ntoa( m_FromAddr.sin_addr ), htons(m_FromAddr.sin_port) );
		m_UserMap.PutData( Index, pUser );
	}
	pUser->PutData( m_pRecvBuff, length );
	m_pUdpManage->m_EventQueue.push( pUser );
	PostQueuedCompletionStatus( m_pUdpManage->m_hWorkerIOCPort, (DWORD)0, m_Sid, NULL/*pOvl*/ );
}