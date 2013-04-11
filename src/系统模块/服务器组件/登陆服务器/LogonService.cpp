#include "StdAfx.h"
#include "LogonService.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CLogonService::CLogonService(void)
{
	m_pIEventService=NULL;
}

//析构函数
CLogonService::~CLogonService(void)
{
}

//服务状态
bool CLogonService::IsService()
{
	if (m_ServiceEngine.GetInterface()!=NULL==false) return false;
	return m_ServiceEngine->IsService();
}

//开始服务
bool CLogonService::StartService()
{
	try
	{
		//变量定义
		bool bSuccess=false;

		//创建服务对象
		if (m_ServiceEngine.GetInterface()==false)
		{
			if (m_ServiceEngine.CreateInstance()==false) throw m_ServiceEngine.GetErrorMessage();
			if (m_pIEventService!=NULL) m_ServiceEngine->SetEventService(m_pIEventService);

			//注册调度外挂服务
			IUnknownEx * pIAttemperEngine=GET_OBJECT_INTERFACE(m_AttemperSink,IUnknownEx);
			if (m_ServiceEngine->SetAttemperEngineSink(pIAttemperEngine)==false) throw 0;

			//注册数据库外挂服务
			IUnknownEx * pIDataBaseSink=GET_OBJECT_INTERFACE(m_DataBaseSink,IUnknownEx);
			if (m_ServiceEngine->SetDataBaseSink(pIDataBaseSink)==false) throw 0;
		}

		//判断状态
		if (m_ServiceEngine->IsService()==true)
		{
			TraceEventString(TEXT("登录服务已经了启动了"),Level_Warning);
			return true;
		}

		//配置服务
		m_InitParamter.LoadInitParamter();
		if (m_DataBaseSink.InitDataBaseSink(&m_InitParamter,m_ServiceEngine.GetInterface())==false) throw 0;
		if (m_AttemperSink.InitAttemperSink(&m_InitParamter,&m_CenterSocket,m_ServiceEngine.GetInterface())==false) throw 0;
		if (m_ServiceEngine->InitServiceEngine(m_InitParamter.m_wListenPort,m_InitParamter.m_wMaxConnect)==false) throw 0;

		//获取调度引擎
		IAttemperEngine * pIAttemperEngine=(IAttemperEngine *)m_ServiceEngine->GetAttemperEngine(IID_IAttemperEngine,VER_IAttemperEngine);
		ASSERT(pIAttemperEngine!=NULL);
		if (pIAttemperEngine==NULL) throw TEXT("调度引擎接口查询失败");

		//获取调度队列
		IQueueService * pIQueueService=(IQueueService *)pIAttemperEngine->GetQueueService(IID_IQueueService,VER_IQueueService);
		ASSERT(pIQueueService!=NULL);
		if (pIQueueService==NULL) throw TEXT("调度引擎队列服务接口查询失败");

		//获取异步引擎
		IAsynchronismEngine * pIAsynchronismEngine=(IAsynchronismEngine *)m_ServiceEngine->GetAsynchronismEngine(IID_IAsynchronismEngine,VER_IAsynchronismEngine);
		ASSERT(pIAsynchronismEngine!=NULL);
		if (pIAsynchronismEngine==NULL) throw TEXT("异步引擎队务接口查询失败");

		//中心连接
		if (m_CenterSocket.SetQueueService(pIQueueService)==false) throw 0;
		if (m_CenterSocket.SetAsynchronismEngine(pIAsynchronismEngine)==false) throw 0;

		//注册对象
		IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECT_INTERFACE(m_CenterSocket,IAsynchronismEngineSink);
		if (pIAsynchronismEngine->RegisterAsynchronismEngineSink(pIAsynchronismEngineSink)==false) throw TEXT("中心连接异步对象注册失败");

		//启动服务
		if (m_ServiceEngine->StartService()==false) throw 0;

		return true;
	}
	catch (LPCTSTR pszErrorMsg)
	{
		TraceEventString(pszErrorMsg,Level_Exception); 
		TraceEventString(TEXT("登录服务启动失败"),Level_Exception); 
		StopService();
	}
	catch (...)
	{ 
		TraceEventString(TEXT("登录服务启动失败"),Level_Exception); 
		StopService();
	}

	return false;
}

//停止服务
bool CLogonService::StopService()
{
	try
	{
		if (m_ServiceEngine.GetInterface()!=NULL) 
		{
			//停止服务
			m_ServiceEngine->StopService();
		
			//获取异步引擎
			IAsynchronismEngine * pIAsynchronismEngine=(IAsynchronismEngine *)m_ServiceEngine->GetAsynchronismEngine(IID_IAsynchronismEngine,VER_IAsynchronismEngine);
			ASSERT(pIAsynchronismEngine!=NULL);
			if (pIAsynchronismEngine==NULL) return false;

			//注销组件
			IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECT_INTERFACE(m_CenterSocket,IAsynchronismEngineSink);
			pIAsynchronismEngine->UnRegisterAsynchronismEngineSink(pIAsynchronismEngineSink);
		}

		return true;
	}
	catch (LPCTSTR pszErrorMsg)
	{
		TraceEventString(pszErrorMsg,Level_Exception); 
		TraceEventString(TEXT("登录服务停止失败"),Level_Exception); 
	}
	catch (...)
	{ 
		TraceEventString(TEXT("登录服务停止失败"),Level_Exception); 
	}

	return false;
}

//设置事件
bool CLogonService::SetEventService(IUnknownEx * pIEventService)
{
	//获取接口
	ASSERT(pIEventService!=NULL);
	m_pIEventService=(IEventService *)pIEventService->QueryInterface(IID_IEventService,VER_IEventService);
	ASSERT(m_pIEventService!=NULL);

	//设置接口
	m_CenterSocket.SetEventService(pIEventService);
	m_DataBaseSink.SetEventService(pIEventService);
	m_AttemperSink.SetEventService(pIEventService);
	if (m_ServiceEngine.GetInterface()) m_ServiceEngine->SetEventService(pIEventService);

	return (m_pIEventService!=NULL);
}

//显示错误
void CLogonService::TraceEventString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pszString,TraceLevel);
	return;
}

//////////////////////////////////////////////////////////////////////////
