#include "StdAfx.h"
#include "CenterService.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CCenterService::CCenterService()
{
	m_pIEventService=NULL;
}

//析构函数
CCenterService::~CCenterService()
{
}

//开始服务
bool CCenterService::StartService()
{
	try
	{
		//变量定义
		bool bSuccess=false;

		//创建服务对象
		if (m_ServiceEngine.IsValid()==false)
		{
			if (m_ServiceEngine.CreateInstance()==false) 
			{
				throw m_ServiceEngine.GetErrorMessage();
			}
			if (m_pIEventService!=NULL) 
			{
				m_ServiceEngine->SetEventService(m_pIEventService);
			}

			//注册调度外挂服务
			IUnknownEx * pIAttemperEngine=GET_OBJECT_INTERFACE(m_AttemperSink,IUnknownEx);
			if (m_ServiceEngine->SetAttemperEngineSink(pIAttemperEngine)==false) 
			{
				throw 0;
			}

			//注册数据库外挂服务
			IUnknownEx * pIDataBaseSink=GET_OBJECT_INTERFACE(m_DataBaseSink,IUnknownEx);
			if (m_ServiceEngine->SetDataBaseSink(pIDataBaseSink)==false) 
			{
				throw 0;
			}
		}

		//判断状态
		if (m_ServiceEngine->IsService()==true)
		{
			TraceEventString(TEXT("中心服务已经了启动了"),Level_Warning);
			return true;
		}

		//配置服务
		m_InitParamter.LoadInitParamter();
		if (m_DataBaseSink.InitDataBaseSink(&m_InitParamter,m_ServiceEngine.GetInterface())==false) 
		{
			throw 0;
		}
		if (m_AttemperSink.InitAttemperSink(&m_InitParamter,m_ServiceEngine.GetInterface())==false) 
		{
			throw 0;
		}
		if (m_ServiceEngine->InitServiceEngine(m_InitParamter.m_wListenPort,m_InitParamter.m_wMaxConnect)==false) 
		{
			throw 0;
		}

		//启动服务
		if (m_ServiceEngine->StartService()==false) 
		{
			throw 0;
		}

		return true;
	}
	catch (LPCTSTR pszErrorMsg)
	{
		TraceEventString(pszErrorMsg,Level_Exception); 
		TraceEventString(TEXT("中心服务启动失败"),Level_Exception); 
		StopService();
	}
	catch (...)
	{ 
		TraceEventString(TEXT("中心服务启动失败"),Level_Exception); 
		StopService();
	}

	return false;
}

//停止服务
bool CCenterService::StopService()
{
	try
	{
		if (m_ServiceEngine.GetInterface()!=NULL) 
		{
			m_ServiceEngine->StopService();
		}
		return true;
	}
	catch (LPCTSTR pszErrorMsg)
	{
		TraceEventString(pszErrorMsg,Level_Exception); 
		TraceEventString(TEXT("中心服务停止失败"),Level_Exception); 
	}
	catch (...)
	{ 
		TraceEventString(TEXT("中心服务停止失败"),Level_Exception); 
	}

	return false;
}

//服务状态
bool CCenterService::IsService()
{
	if (m_ServiceEngine.IsValid()==false) 
	{
		return false;
	}
	return m_ServiceEngine->IsService();
}

//设置事件
bool CCenterService::SetEventService(IUnknownEx * pIEventService)
{
	//获取接口
	ASSERT(pIEventService!=NULL);
	m_pIEventService=(IEventService *)pIEventService->QueryInterface(IID_IEventService,VER_IEventService);
	ASSERT(m_pIEventService!=NULL);

	//设置接口
	m_DataBaseSink.SetEventService(pIEventService);
	m_AttemperSink.SetEventService(pIEventService);
	if (m_ServiceEngine.IsValid()) 
	{
		m_ServiceEngine->SetEventService(pIEventService);
	}

	return (m_pIEventService!=NULL);
}

//显示错误
void CCenterService::TraceEventString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	if (m_pIEventService!=NULL)
	{
		m_pIEventService->ShowEventNotify(pszString,TraceLevel);
	}
	
	return ;
}

//////////////////////////////////////////////////////////////////////////
