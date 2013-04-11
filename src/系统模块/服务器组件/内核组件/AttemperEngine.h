#ifndef ATTEMPER_ENGINE_HEAD_FILE
#define ATTEMPER_ENGINE_HEAD_FILE

#pragma once

//组件头文件
#include "ServerKernel.h"
#include "QueueService.h"

//////////////////////////////////////////////////////////////////////////

//调度管理类
class CAttemperEngine : public IAttemperEngine, public IQueueServiceSink
{
	//核心变量
protected:
	bool							m_bService;							//运行标志
	CQueueService					m_RequestQueueService;				//队列对象

	//接口变量
protected:
	ITCPSocketEngine				* m_pITCPSocketEngine;				//网络引擎
	IAttemperEngineSink				* m_pIAttemperEngineSink;			//挂接接口

	//函数定义
public:
	//构造函数
	CAttemperEngine(void);
	//析构函数
	virtual ~CAttemperEngine(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CAttemperEngine))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//启动服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl StopService();
	//设置网络
	virtual bool __cdecl SetSocketEngine(IUnknownEx * pIUnknownEx);
	//注册钩子
	virtual bool __cdecl SetAttemperEngineSink(IUnknownEx * pIUnknownEx);
	//获取接口
	virtual void * __cdecl GetQueueService(const IID & Guid, DWORD dwQueryVer);

	//队列接口
public:
	//触发接口
	virtual void __cdecl OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize, DWORD dwInsertTime);
};

//////////////////////////////////////////////////////////////////////////

#endif