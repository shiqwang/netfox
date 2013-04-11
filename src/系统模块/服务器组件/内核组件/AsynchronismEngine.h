#ifndef ASYNCHRONISM_ENGINE_HEAD_FILE
#define ASYNCHRONISM_ENGINE_HEAD_FILE

#pragma once

#include "ServerKernel.h"

//类说明
class CAsynchronismEngine;
typedef class CArrayTemplate<IAsynchronismEngineSink *> CAsynchronismEngineSinkArray;

//////////////////////////////////////////////////////////////////////////

//控制窗口
class CControlWnd : public CWnd
{
	//变量定义
public:
	CAsynchronismEngine				* m_pAsynchronismEngine;			//异步引擎

	//函数定义
public:
	//构造函数
	CControlWnd();
	//析构函数
	virtual ~CControlWnd();

	//消息映射
protected:
	//请求消息
	HRESULT OnAsynRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//消息线程
class CMessageThread : public CServiceThread
{
	//变量定义
public:
	CAsynchronismEngine				* m_pAsynchronismEngine;			//异步引擎

	//函数定义
public:
	//构造函数
	CMessageThread();
	//析构函数
	virtual ~CMessageThread();

	//功能函数
public:
	//停止线程
	virtual bool StopThread(DWORD dwWaitSeconds=INFINITE);
	//中止线程
	virtual bool TerminateThread(DWORD dwExitCode);

	//事件函数
private:
	//开始事件
	virtual bool OnThreadStratEvent();
	//停止事件
	virtual bool OnThreadStopEvent();

	//重载函数
private:
	//运行函数
	virtual bool RepetitionRun();
};

//////////////////////////////////////////////////////////////////////////

//异步引擎
class CAsynchronismEngine : public IAsynchronismEngine
{
	friend class CControlWnd;
	friend class CMessageThread;

	//内核变量
protected:
	bool							m_bService;							//服务标志
	BYTE							m_cbBuffer[MAX_QUEUE_PACKET];		//数据缓冲
	CAsynchronismEngineSinkArray	m_AsynchronismEngineSinkArray;		//异步钩子

	//组件变量
protected:
	CControlWnd						m_ControlWnd;						//控制窗口
	CThreadLock						m_ThreadLock;						//线程同步
	CDataStorage					m_DataStorage;						//数据存储
	CMessageThread					m_MessageThread;					//线程组件

	//函数定义
public:
	//构造函数
	CAsynchronismEngine(void);
	//析构函数
	virtual ~CAsynchronismEngine(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CAsynchronismEngine))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//启动服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl StopService();
	//插入请求
	virtual bool __cdecl InsertRequest(WORD wRequestID, void * const pBuffer, WORD wDataSize, IUnknownEx * pIUnknownEx);

	//功能接口
public:
	//注册钩子
	virtual bool __cdecl RegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx);
	//取消注册
	virtual bool __cdecl UnRegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx);

	//线程函数
protected:
	//线程启动
	bool OnMessageThreadStart();
	//线程停止
	bool OnMessageThreadStop();
	//异步请求
	void OnAsynchronismRequest(WORD wRequestID, IAsynchronismEngineSink * pIAsynchronismEngineSink);
};

//////////////////////////////////////////////////////////////////////////

#endif