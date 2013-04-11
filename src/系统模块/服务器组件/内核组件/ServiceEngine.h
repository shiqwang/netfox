#ifndef SERVICE_ENGINE_HEAD_FILE
#define SERVICE_ENGINE_HEAD_FILE

#pragma once

#include "ServerKernel.h"
#include "TimerEngine.h"
#include "SocketEngine.h"
#include "AttemperEngine.h"
#include "DataBaseEngine.h"
#include "AsynchronismEngine.h"

//////////////////////////////////////////////////////////////////////////

//服务引擎
class CServiceEngine : public IServiceEngine
{
	//变量定义
protected:
	bool								m_bService;						//运行标志

	//组件变量
protected:
	CTimerEngine						m_TimerEngine;					//定时器引擎
	CDataBaseEngine						m_DataBaseEngine;				//数据库引擎
	CAttemperEngine						m_AttemperEngine;				//调度引擎
	CTCPSocketEngine					m_TCPSocketEngine;				//网络引擎
	CAsynchronismEngine					m_AsynchronismEngine;			//异步引擎

	//函数定义
public:
	//构造函数
	CServiceEngine();
	//析构函数
	virtual ~CServiceEngine();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CServiceEngine))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl StopService();

	//配置接口
public:
	//设置事件
	virtual bool __cdecl SetEventService(IUnknownEx * pIUnknownEx);
	//设置钩子
	virtual bool __cdecl SetDataBaseSink(IUnknownEx * pIUnknownEx);
	//设置钩子
	virtual bool __cdecl SetAttemperEngineSink(IUnknownEx * pIUnknownEx);
	//注册钩子
	virtual bool __cdecl RegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx);
	//设置网络
	virtual bool __cdecl InitServiceEngine(WORD wListenPort, WORD wMaxSocketItem);

	//服务查询
public:
	//获取定时器引擎接口
	virtual void * __cdecl GetTimerEngine(const IID & Guid, DWORD dwQueryVer);
	//获取数据库引擎接口
	virtual void * __cdecl GetDataBaseEngine(const IID & Guid, DWORD dwQueryVer);
	//获取调度引擎接口
	virtual void * __cdecl GetAttemperEngine(const IID & Guid, DWORD dwQueryVer);
	//获取网络引擎接口
	virtual void * __cdecl GetTCPSocketEngine(const IID & Guid, DWORD dwQueryVer);
	//获取异步引擎接口
	virtual void * __cdecl GetAsynchronismEngine(const IID & Guid, DWORD dwQueryVer);
	//获取数据库队列接口
	virtual void * __cdecl GetDataBaseQueueService(const IID & Guid, DWORD dwQueryVer);
	//获取调度队列接口
	virtual void * __cdecl GetAttemperQueueService(const IID & Guid, DWORD dwQueryVer);

	//功能接口
public:
	//服务状态
	virtual bool __cdecl IsService() { return m_bService; }
	//外部控制
	virtual bool __cdecl ControlService(void * pBuffer, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif 