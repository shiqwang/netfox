#pragma once

#include "UdpIocp.h"
#include "User.h"

#include <Queue>
typedef queue<CUser*> EVENT_QUEUE; //事件堆栈

//udp管理类
class CUdpManage : public CUdpIocp
{
public:
	CUdpManage(void);
	virtual ~CUdpManage(void);

	EVENT_QUEUE m_EventQueue; //事件集合
	HANDLE m_hWorkerIOCPort; //事件完成端口句柄

	int Init( int ServerSocketSize=1, int WorkerNum=0 ); //初始化

	//创建处理进程
	void CreateWorkerThread( int WorkerNum=0 );
	void SendAllUser( char* Tmp, int len, char* lpTo, UINT port );
};
