#ifndef QUEUE_SERVICE_HEAD_FILE
#define QUEUE_SERVICE_HEAD_FILE

#include "ServerKernel.h"

#pragma once

//////////////////////////////////////////////////////////////////////////

//队列线程类
class CQueueServiceThread : public CServiceThread
{
	//友员定义
	friend class CQueueService;

	//配置定义
protected:
	HANDLE							m_hCompletionPort;					//完成端口

	//辅助变量
private:
	BYTE							m_cbBuffer[MAX_QUEUE_PACKET];		//接收缓冲

	//函数定义
protected:
	//构造函数
	CQueueServiceThread(void);
	//析构函数
	virtual ~CQueueServiceThread(void);

	//功能函数
public:
	//配置函数
	bool InitThread(HANDLE hCompletionPort);
	//取消配置
	bool UnInitThread();

	//重载函数
private:
	//运行函数
	virtual bool RepetitionRun();
};

//////////////////////////////////////////////////////////////////////////

//数据队列类
class CQueueService : public IQueueServiceEngine, public IQueueService 
{
	friend class CQueueServiceThread;

	//变量定义
protected:
	bool							m_bService;							//服务标志
	HANDLE							m_hCompletionPort;					//完成端口
	IQueueServiceSink				* m_pIQueueServiceSink;				//回调接口

	//组件变量
protected:
	CThreadLock						m_ThreadLock;						//线程锁
	CDataStorage					m_DataStorage;						//数据存储
	CQueueServiceThread				m_QueueServiceThread;				//队列线程

	//函数定义
public:
	//构造函数
	CQueueService(void);
	//析构函数
	virtual ~CQueueService(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CQueueService))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//队列接口
public:
	//加入数据
	virtual bool __cdecl AddToQueue(WORD wIdentifier, void * const pBuffer, WORD wDataSize);

	//管理接口
public:
	//开始服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl StopService();
	//设置接口
	virtual bool __cdecl SetQueueServiceSink(IUnknownEx * pIUnknownEx);
	//负荷信息
	virtual bool __cdecl GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//辅助函数
private:
	//提取数据
	bool GetData(tagDataHead & DataHead, void * pBuffer, WORD wBufferSize);
	//数据消息
	void OnQueueServiceThread(const tagDataHead & DataHead, void * pBuffer, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif