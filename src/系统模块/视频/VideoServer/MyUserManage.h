#pragma once
#include "..\UdpSock\UserManage.h"

typedef struct
{
    unsigned int IP;
	int Port;
}SERVER_INFO;

class CMyUserManage : public CUserManage
{
public:
	CMyUserManage(void);
	~CMyUserManage(void);

	SERVER_INFO m_HallServer;

	virtual void ReceivedData( int length );
};
