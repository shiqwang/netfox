#pragma once

#include "windows.h"
#include "..\Include\STLMap.h"
#include "DeskID.h"

typedef CSTLMap<DWORD,CDeskID> DESK_ID_LIST;

class CRoomID
{
private:
	DESK_ID_LIST m_DeskIdList;
public:
	CRoomID(void);
	~CRoomID(void);

	void AddDeskID( DWORD id, CDeskID* pDeskID );
    CDeskID* GetDeskID( DWORD id );
	BOOL DeleteUser( DWORD DeskID, PUSER_INFO pUserInfo );
	int GetDeskCount(void){ return (int)m_DeskIdList.GetSize(); };
};
