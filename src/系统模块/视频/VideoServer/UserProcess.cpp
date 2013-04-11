#include "StdAfx.h"
#include ".\userprocess.h"

ROOM_ID_LIST m_RoomIDList;
ROOM_ID_LIST m_SwapRoomIDList;

CMyPacket CUserProcess::m_MyPacket;

CUserProcess::CUserProcess(void)
{
	m_pUserInfo = NULL;
	m_RoomID = 0;
	m_DeskID = 0;
}

CUserProcess::~CUserProcess(void)
{
	CRoomID* pRoomID = m_RoomIDList.GetData( m_RoomID );
	if( pRoomID )
	{
		if( pRoomID->DeleteUser( m_DeskID, m_pUserInfo ) )
		{
			printf( "用户退出 房间:%d 桌号:%d 外网:%s:%d\n", m_RoomID, m_DeskID, m_IP, m_Port );
			if( pRoomID->GetDeskCount()==0 )
			{
				m_RoomIDList.DeleteData( m_RoomID );
			}
		}
	}
	pRoomID = m_SwapRoomIDList.GetData( m_RoomID );
	if( pRoomID )
	{
		if( pRoomID->DeleteUser( m_DeskID, m_pUserInfo ) )
		{
			printf( "删除交换 房间:%d 桌号:%d 外网:%s:%d\n", m_RoomID, m_DeskID, m_IP, m_Port );
			if( pRoomID->GetDeskCount()==0 )
			{
				m_SwapRoomIDList.DeleteData( m_RoomID );
			}
		}
	}
}

void CUserProcess::Parsing( char* pData, int length )
{
	WORD Packet = *PWORD(pData);
	switch( Packet )
	{
	case PACKET_LOGIN: //登陆
		{
			//用户登陆
			DWORD IP = 0, PORT = 0, Pos = 0;
			if( m_MyPacket.GetValue( pData, length, Packet, &m_RoomID, &m_DeskID, &Pos, &IP, &PORT ) > 0 )
			{
				char Tmp[MAX_PATH];
//				sprintf( Tmp,"用户登陆 房间:%d 桌号:%d 外网:%s:%d 内网%s:%d", m_RoomID, m_DeskID, m_IP, m_Port, inet_ntoa(*(in_addr*)&IP),PORT );
				printf( "用户登陆 房间:%d 桌号:%d 外网:%s:%d 内网%s:%d\n", m_RoomID, m_DeskID, m_IP, m_Port, inet_ntoa(*(in_addr*)&IP),PORT );
				int len=0;
				DWORD W_IP = inet_addr(m_IP);
				CRoomID* pRoomID = m_RoomIDList.GetData( m_RoomID );
				if( !pRoomID )
				{
                    pRoomID = new CRoomID;
					m_RoomIDList.PutData( m_RoomID, pRoomID );
					CDeskID* pDeskID = new CDeskID;
					pRoomID->AddDeskID( m_DeskID, pDeskID );
					m_pUserInfo = pDeskID->AddUser( W_IP, m_Port, IP, PORT, Pos );
					len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_LOGIN_OK, inet_addr(m_IP), m_Port, Pos );
					Send( Tmp, len, m_IP, m_Port );
				}
				else
				{
					CDeskID* pDeskID = pRoomID->GetDeskID( m_DeskID );
					if( !pDeskID )
					{
						pDeskID = new CDeskID;
						pRoomID->AddDeskID( m_DeskID, pDeskID );
					}
					m_pUserInfo = pDeskID->AddUser( W_IP, m_Port, IP, PORT, Pos );
					if( m_pUserInfo )
					{
						len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_LOGIN_OK, inet_addr(m_IP), m_Port, Pos );
						Send( Tmp, len, m_IP, m_Port );
						len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_ADDCLIENT, W_IP, m_Port, IP, PORT, Pos );
						int index = 0;
						PUSER_INFO pUserInfo;
						char Tmp1[MAX_PATH];
						while( (pUserInfo = pDeskID->GetUser( index++ )) != NULL )
						{
							if( pUserInfo->W_IP == W_IP && pUserInfo->W_PORT == m_Port && pUserInfo->N_IP == IP && pUserInfo->N_PORT == PORT )
								continue;
							Send( Tmp, len, inet_ntoa( *((in_addr*)&pUserInfo->W_IP) ), pUserInfo->W_PORT );
							int nlen = m_MyPacket.GetPacket( Tmp1, MAX_PATH, PACKET_ADDCLIENT, pUserInfo->W_IP, pUserInfo->W_PORT, pUserInfo->N_IP, pUserInfo->N_PORT, pUserInfo->POS );						
							if( nlen>0 )
							{
								Send( Tmp1, nlen, m_IP, m_Port );
							}
						}
					}
					else
					{
						len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_LOGIN_ERR, 5 );
						Send( Tmp, len, m_IP, m_Port );
					}
				}
			}            
		}
		break;
	case PACKET_SWAP_CONNECT:
		{
			//用户登陆
			DWORD IP = 0, PORT = 0, Pos = 0, wPos = 0;
			if( m_MyPacket.GetValue( pData, length, Packet, &m_RoomID, &m_DeskID, &Pos, &wPos ) > 0 )
			{
				char Tmp[MAX_PATH];
//				sprintf( Tmp,"用户登陆 房间:%d 桌号:%d 外网:%s:%d 内网%s:%d", m_RoomID, m_DeskID, m_IP, m_Port, inet_ntoa(*(in_addr*)&IP),PORT );
				printf( "交换记录 房间:%d 桌号:%d IP:%s:%d POS:%d 连接POS:%d\n", m_RoomID, m_DeskID, m_IP, m_Port, Pos, wPos );
				int len=0;
				CRoomID* pRoomID = m_SwapRoomIDList.GetData( m_RoomID );
				if( !pRoomID )
				{
                    pRoomID = new CRoomID;
					m_SwapRoomIDList.PutData( m_RoomID, pRoomID );
					CDeskID* pDeskID = new CDeskID;
					pRoomID->AddDeskID( m_DeskID, pDeskID );
					m_pUserInfo = pDeskID->AddUser( inet_addr(m_IP), m_Port, (DWORD)this, wPos, Pos );
					if( !m_pUserInfo )
					{
						len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_LOGIN_ERR, 3 );
						Send( Tmp, len, m_IP, m_Port );
					}
				}
				else
				{
					CDeskID* pDeskID = pRoomID->GetDeskID( m_DeskID );
					if( !pDeskID )
					{
						pDeskID = new CDeskID;
						pRoomID->AddDeskID( m_DeskID, pDeskID );
					}
					m_pUserInfo = pDeskID->AddUser( inet_addr(m_IP), m_Port, (DWORD)this, wPos, Pos );
					if( m_pUserInfo )
					{
						PUSER_INFO pUserInfo;
						int index = 0;
						while( (pUserInfo = pDeskID->GetUser( index++ )) != NULL )
						{
							if( pUserInfo->N_PORT == m_pUserInfo->POS && pUserInfo->POS == m_pUserInfo->N_PORT )
							{
								printf( "交换IP\n" );
								int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_SWAP_CONNECT, pUserInfo->W_IP,  pUserInfo->W_PORT, pUserInfo->POS, 1 );
								Send( Tmp, len );
                                len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_SWAP_CONNECT, m_pUserInfo->W_IP,  m_pUserInfo->W_PORT, m_pUserInfo->POS, 1 );
								Send( Tmp, len, inet_ntoa( *(in_addr*)&pUserInfo->W_IP ), pUserInfo->W_PORT );
								CUserProcess* p = (CUserProcess*)pUserInfo->N_IP;
								p->CheckTime = 0;
								CheckTime = 0;
								break;
							}
						}
					}
					else
					{
						len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_LOGIN_ERR, 3 );
						Send( Tmp, len, m_IP, m_Port );
					}
				}
			}
		}
		break;
	case PACKET_QUIT:
		{
			CheckTime = 0;
		}
		break;
	}
}