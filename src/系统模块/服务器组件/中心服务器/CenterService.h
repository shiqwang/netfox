#ifndef CENTER_SERVICE_HEAD_FILE
#define CENTER_SERVICE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//中心服务类
class CCenterService
{
	//变量定义
protected:
	CDataBaseSink					m_DataBaseSink;						//数据库钩子
	CAttemperEngineSink				m_AttemperSink;						//调度钩子
	CServiceEngineHelper			m_ServiceEngine;					//服务引擎

	//配置变量
protected:
	CInitParamter					m_InitParamter;						//配置参数
	IEventService					* m_pIEventService;					//事件服务

	//函数定义
public:
	//构造函数
	CCenterService();
	//析构函数
	virtual ~CCenterService();

	//功能函数
public:
	//开始服务
	bool StartService();
	//停止服务
	bool StopService();
	//服务状态
	bool IsService();
	//设置事件
	bool SetEventService(IUnknownEx * pIEventService);

	//内部函数
private:
	//显示错误
	void TraceEventString(LPCTSTR pszString, enTraceLevel TraceLevel);
};


//////////////////////////////////////////////////////////////////////////

#endif
