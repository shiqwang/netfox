#pragma once
#include "MyUserManage.h"
#include "..\udpsock\user.h"
#include "..\UdpSock\MyPacket.h"
#include "RoomID.h"

#include "..\Include\STLMap.h"
typedef CSTLMap<DWORD,CRoomID> ROOM_ID_LIST;

class CUserProcess : public CUser
{
protected:
	static CMyPacket m_MyPacket;
	DWORD m_RoomID, m_DeskID;
	PUSER_INFO m_pUserInfo;
public:
	CUserProcess(void);
	virtual ~CUserProcess(void);

	virtual void Parsing( char* pData, int length );
};
