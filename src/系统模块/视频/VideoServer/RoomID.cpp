#include "StdAfx.h"
#include ".\roomid.h"

CRoomID::CRoomID(void)
{
}

CRoomID::~CRoomID(void)
{
	m_DeskIdList.DeleteAllData();
}

void CRoomID::AddDeskID( DWORD id, CDeskID* pDeskID )
{
	m_DeskIdList.PutData( id, pDeskID );
}

CDeskID* CRoomID::GetDeskID( DWORD id )
{
	return m_DeskIdList.GetData( id );
}

BOOL CRoomID::DeleteUser( DWORD DeskID, PUSER_INFO pUserInfo )
{
	CDeskID* pDeskID = m_DeskIdList.GetData( DeskID );
	if( pDeskID )
	{
		if( pDeskID->DeleteUser( pUserInfo ) )
		{
			if( pDeskID->GetUserCount()==0 )
			{
				m_DeskIdList.DeleteData( DeskID );
			}
			return TRUE;
		}
	}
	return FALSE;
}