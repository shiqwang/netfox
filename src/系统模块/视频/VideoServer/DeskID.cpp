#include "StdAfx.h"
#include ".\deskid.h"

#include <algorithm>

#define MAX_USER 10

CDeskID::CDeskID(void)
{
	m_UserList.reserve( 5 );
}

CDeskID::~CDeskID(void)
{
	PUSER_INFO pUserInfo;
	for( int i=0; i<(int)m_UserList.size(); i++ )
	{
        pUserInfo = m_UserList[i];
		delete pUserInfo;
	}
	m_UserList.clear();
}

PUSER_INFO CDeskID::AddUser( DWORD W_IP, DWORD W_PORT, DWORD N_IP, DWORD N_PORT, DWORD& Pos )
{
	USER_INFO UserInfo = { W_IP, W_PORT, N_IP, N_PORT, Pos };
	PUSER_INFO pUserInfo, r = NULL;
	for( int i=0; i<(int)m_UserList.size(); i++ )
	{
        pUserInfo = m_UserList[i];
		if( pUserInfo )
		{
			if( memcmp( pUserInfo, &UserInfo, sizeof(UserInfo)-4 )==0 )
			{
				Pos = pUserInfo->POS;
				return pUserInfo;
			}
		}
	}
	//检查位置
	BYTE b[MAX_USER];
	ZeroMemory( b, MAX_USER );
	for( int i=0; i<(int)m_UserList.size(); i++ )
	{
        pUserInfo = m_UserList[i];
		if( pUserInfo )
		{
			if( pUserInfo->POS == Pos )
			{
				Pos = MAX_USER;
			}
			if( pUserInfo->POS>0 && pUserInfo->POS<MAX_USER )
			{
				b[pUserInfo->POS-1] = 1;
			}
		}
	}
	//找位置
	if( Pos >= MAX_USER )
	{
        for( int i=0; i<MAX_USER; i++ )
		{
            if( b[i]==0 )
			{
				Pos = i+1;
				UserInfo.POS = Pos;
				break;
			}
		}
	}
	if( Pos<MAX_USER ) //找到位置
	{
		pUserInfo = new USER_INFO;
		*pUserInfo = UserInfo;
		m_UserList.push_back( pUserInfo );
		return pUserInfo;
	}
	else
	{
		return NULL;
	}
}

PUSER_INFO CDeskID::GetUser( int index )
{
	int Count = (int)m_UserList.size();
	if( index < (int)m_UserList.size() )
	{
		return m_UserList[index];
	}
	return NULL;
}

BOOL CDeskID::DeleteUser( PUSER_INFO pUserInfo )
{
	USER_LIST::iterator iter = find( m_UserList.begin(), m_UserList.end(), pUserInfo );
	if( iter != m_UserList.end() )
	{
		m_UserList.erase( iter );
        delete pUserInfo;
		pUserInfo = NULL;
		return TRUE;
	}
	return FALSE;
}