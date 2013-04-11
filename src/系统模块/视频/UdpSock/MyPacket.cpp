#include "StdAfx.h"
#include ".\mypacket.h"

CMyPacket::CMyPacket(void)
{
	//封包类型
	AddPacket( PACKET_LOGIN, T_DWORD/*房间ID*/, T_DWORD/*桌号ID*/, T_DWORD/*位置*/, T_DWORD/*内部IP*/, T_DWORD/*内部PORT*/, NULL );
	AddPacket( PACKET_LOGIN_OK, T_DWORD/*IP*/, T_DWORD/*PORT*/, T_DWORD/*位置*/, NULL );
	AddPacket( PACKET_LOGIN_ERR, T_DWORD/*错误号*/, NULL );
	AddPacket( PACKET_QUIT, T_DWORD, NULL );
    AddPacket( PACKET_ADDCLIENT, T_DWORD/*外部IP*/, T_DWORD/*外部PORT*/, T_DWORD/*内部IP*/, T_DWORD/*内部PORT*/, T_DWORD/*位置*/, NULL );
	AddPacket( PACKET_USER_CONNECT, T_DWORD/**/, NULL );
	AddPacket( PACKET_USER_CONNECT_OK, T_DWORD/**/, NULL );
	AddPacket( PACKET_USER_VEDIO, T_DWORD/*序列*/, T_LEN_DATA/*数据*/, NULL );
	AddPacket( PACKET_USER_VEDIO_OK, T_DWORD/*标志*/, T_DWORD/*长度*/, NULL );
	AddPacket( PACKET_USER_VEDIO_NEW, T_DWORD/*标志*/, NULL );
	AddPacket( PACKET_USER_GET_NEW_VEDIO, T_DWORD/*数据*/, NULL );
	AddPacket( PACKET_USER_GET_VEDIO, T_LEN_DATA/*数据*/, NULL );
	AddPacket( PACKET_USER_VEDIO_ERR, T_LEN_DATA/*数据*/, NULL );

	AddPacket( PACKET_USER_SOUND, T_DWORD/*序列*/, T_LEN_DATA/*数据*/, NULL );
	AddPacket( PACKET_USER_SOUND_OK, T_DWORD/*标志*/, T_DWORD/*长度*/, NULL );
	AddPacket( PACKET_USER_SOUND_NEXT, T_DWORD/*标志*/, NULL );
	AddPacket( PACKET_USER_GET_SOUND, T_LEN_DATA/*数据*/, NULL );
	AddPacket( PACKET_USER_SOUND_ERR, T_LEN_DATA/*数据*/, NULL );

	AddPacket( PACKET_SWAP_CONNECT, T_DWORD/*房间ID*/, T_DWORD/*桌号ID*/, T_DWORD/*位置*/, T_DWORD/*连接位置*/, NULL );
}

CMyPacket::~CMyPacket(void)
{
}