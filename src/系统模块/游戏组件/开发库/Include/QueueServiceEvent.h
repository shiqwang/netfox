#ifndef QUEUE_SERVICE_EVENT_HEAD_FILE
#define QUEUE_SERVICE_EVENT_HEAD_FILE

#pragma once

#include "ServerKernel.h"

//////////////////////////////////////////////////////////////////////////

//数据队列事件
class CQueueServiceEvent
{
	//变量定义
public:
	CThreadLock						m_BufferLock;						//同步锁定
	IQueueService *					m_pIQueueService;					//队列接口
	BYTE							m_cbBuffer[MAX_QUEUE_PACKET];		//接收缓冲

	//函数定义
public:
	//构造函数
	CQueueServiceEvent() { m_pIQueueService=NULL; }
	//析构函数
	virtual ~CQueueServiceEvent() {}

	//管理函数
public:
	//设置接口
	bool SetQueueService(IUnknownEx * pIUnknownEx)
	{
		ASSERT(pIUnknownEx!=NULL);
		m_pIQueueService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IQueueService);
		ASSERT(m_pIQueueService!=NULL);
		return (m_pIQueueService!=NULL);
	}
	//获取接口
	void * GetQueueService(const IID & Guid, DWORD dwQueryVer)
	{
		if (m_pIQueueService==NULL) return NULL;
		return m_pIQueueService->QueryInterface(Guid,dwQueryVer);
	}
	//是否有效
	bool IsValid() { return (m_pIQueueService!=NULL); }

	//通知函数
public:
	//定时器事件
	bool PostTimerEvent(WORD wTimerID, WPARAM wBindParam)
	{
		//效验参数
		ASSERT(m_pIQueueService!=NULL);
		if (m_pIQueueService==NULL) return false;

		//缓冲锁定
		CThreadLockHandle BufferLockHandle(&m_BufferLock);

		//投递消息
		NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)m_cbBuffer;
		pTimerEvent->wTimerID=wTimerID;
		pTimerEvent->wBindParam=wBindParam;
		m_pIQueueService->AddToQueue(EVENT_TIMER,m_cbBuffer,sizeof(NTY_TimerEvent));

		return true;
	}
	//数据库事件
	bool PostDataBaseEvent(WORD wRequestID, WORD wIndex, WORD wRoundID, const void * pDataBuffer, WORD wDataSize)
	{
		//效验参数
		ASSERT(m_pIQueueService!=NULL);
		ASSERT((wDataSize+sizeof(NTY_DataBaseEvent))<=MAX_QUEUE_PACKET);
		if (m_pIQueueService==NULL) return false;
		if ((wDataSize+sizeof(NTY_DataBaseEvent))>MAX_QUEUE_PACKET) return false;

		//缓冲锁定
		CThreadLockHandle BufferLockHandle(&m_BufferLock);

		//投递消息
		NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)m_cbBuffer;
		pDataBaseEvent->wIndex=wIndex;
		pDataBaseEvent->wRequestID=wRequestID;
		pDataBaseEvent->wRoundID=wRoundID;
		if (wDataSize>0)
		{
			ASSERT(pDataBuffer!=NULL);
			CopyMemory(m_cbBuffer+sizeof(NTY_DataBaseEvent),pDataBuffer,wDataSize);
		}
		m_pIQueueService->AddToQueue(EVENT_DATABASE,m_cbBuffer,sizeof(NTY_DataBaseEvent)+wDataSize);

		return true;
	}
	//网络应答事件
	bool PostSocketAcceptEvent(WORD wIndex, WORD wRoundID, DWORD dwClientIP)
	{
		//效验参数
		ASSERT(m_pIQueueService!=NULL);
		if (m_pIQueueService==NULL) return false;

		//缓冲锁定
		CThreadLockHandle BufferLockHandle(&m_BufferLock);

		//投递消息
		NTY_SocketAcceptEvent * pSocketAcceptEvent=(NTY_SocketAcceptEvent *)m_cbBuffer;
		pSocketAcceptEvent->wIndex=wIndex;
		pSocketAcceptEvent->wRoundID=wRoundID;
		pSocketAcceptEvent->dwClientIP=dwClientIP;
		m_pIQueueService->AddToQueue(EVENT_SOCKET_ACCEPT,m_cbBuffer,sizeof(NTY_SocketAcceptEvent));

		return true;
	}
	//网络读取事件
	bool PostSocketReadEvent(WORD wIndex, WORD wRoundID, CMD_Command Command, const void * pDataBuffer, WORD wDataSize)
	{
		//效验参数
		ASSERT(m_pIQueueService!=NULL);
		ASSERT((wDataSize+sizeof(NTY_SocketReadEvent))<=MAX_QUEUE_PACKET);
		if (m_pIQueueService==NULL) return false;
		if ((wDataSize+sizeof(NTY_SocketReadEvent))>MAX_QUEUE_PACKET) return false;

		//缓冲锁定
		CThreadLockHandle BufferLockHandle(&m_BufferLock);

		//投递消息
		NTY_SocketReadEvent * pSocketReadEvent=(NTY_SocketReadEvent *)m_cbBuffer;
		pSocketReadEvent->wIndex=wIndex;
		pSocketReadEvent->wRoundID=wRoundID;
		pSocketReadEvent->Command=Command;
		pSocketReadEvent->wDataSize=wDataSize;
		if (wDataSize>0)
		{
			ASSERT(pDataBuffer!=NULL);
			CopyMemory(m_cbBuffer+sizeof(NTY_SocketReadEvent),pDataBuffer,wDataSize);
		}
		m_pIQueueService->AddToQueue(EVENT_SOCKET_READ,m_cbBuffer,sizeof(NTY_SocketReadEvent)+wDataSize);

		return false;
	}
	//网络关闭事件
	bool PostSocketCloseEvent(WORD wIndex, WORD wRoundID, DWORD dwClientIP, DWORD dwConnectSecond)
	{
		//效验参数
		ASSERT(m_pIQueueService!=NULL);
		if (m_pIQueueService==NULL) return false;

		//缓冲锁定
		CThreadLockHandle BufferLockHandle(&m_BufferLock);

		//投递消息
		NTY_SocketCloseEvent * pSocketCloseEvent=(NTY_SocketCloseEvent *)m_cbBuffer;
		pSocketCloseEvent->wIndex=wIndex;
		pSocketCloseEvent->wRoundID=wRoundID;
		pSocketCloseEvent->dwClientIP=dwClientIP;
		pSocketCloseEvent->dwConnectSecond=dwConnectSecond;
		m_pIQueueService->AddToQueue(EVENT_SOCKET_CLOSE,m_cbBuffer,sizeof(NTY_SocketCloseEvent));

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////

#endif