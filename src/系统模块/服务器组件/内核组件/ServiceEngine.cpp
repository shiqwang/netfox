#include "StdAfx.h"
#include "EventService.h"
#include "ServiceEngine.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceEngine::CServiceEngine()
{
	m_bService=false;
	return;
}

//析构函数
CServiceEngine::~CServiceEngine()
{
	//停止服务
	StopService();

	return;
}

//接口查询
void * __cdecl CServiceEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServiceEngine,Guid,dwQueryVer);
	return NULL;
}

//获取定时器接口
void * __cdecl CServiceEngine::GetTimerEngine(const IID & Guid, DWORD dwQueryVer)
{
	return m_TimerEngine.QueryInterface(Guid,dwQueryVer);
}

//获取调度引擎接口
void * __cdecl CServiceEngine::GetAttemperEngine(const IID & Guid, DWORD dwQueryVer)
{
	return m_AttemperEngine.QueryInterface(Guid,dwQueryVer);
}

//获取网络引擎接口
void * __cdecl CServiceEngine::GetTCPSocketEngine(const IID & Guid, DWORD dwQueryVer)
{
	return m_TCPSocketEngine.QueryInterface(Guid,dwQueryVer);
}

//获取数据库引擎接口
void * __cdecl CServiceEngine::GetDataBaseEngine(const IID & Guid, DWORD dwQueryVer)
{
	return m_DataBaseEngine.QueryInterface(Guid,dwQueryVer);
}

//获取异步引擎接口
void * __cdecl CServiceEngine::GetAsynchronismEngine(const IID & Guid, DWORD dwQueryVer)
{
	return m_AsynchronismEngine.QueryInterface(Guid,dwQueryVer);
}

//获取数据库队列接口
void * __cdecl CServiceEngine::GetDataBaseQueueService(const IID & Guid, DWORD dwQueryVer)
{
	return m_DataBaseEngine.GetQueueService(Guid,dwQueryVer);
}

//获取调度队列接口
void * __cdecl CServiceEngine::GetAttemperQueueService(const IID & Guid, DWORD dwQueryVer)
{
	return m_AttemperEngine.GetQueueService(Guid,dwQueryVer);
}

//外部控制
bool __cdecl CServiceEngine::ControlService(void * pBuffer, WORD wDataSize)
{
	//效验状态
	if (m_bService==false) 
	{
		CEventTrace::ShowEventNotify(TEXT("服务引擎处于停止状态中，控制操作忽略"),Level_Exception);
		return false;
	}

	//插入数据
	IQueueService * pIQueueService=(IQueueService *)m_AttemperEngine.GetQueueService(IID_IQueueService,VER_IQueueService);
	if (pIQueueService==NULL) 
	{
		CEventTrace::ShowEventNotify(TEXT("获取调度引擎触发接口失败"),Level_Exception);
		return false;
	}

	//投递指令
	if (pIQueueService->AddToQueue(EVENT_CONTROL,pBuffer,wDataSize)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("投递控制消息指令失败"),Level_Exception);
		return false;
	}

	return true;
}

//启动服务
bool __cdecl CServiceEngine::StartService()
{
	//判断状态
	if (m_bService==true)
	{
		CEventTrace::ShowEventNotify(TEXT("服务引擎重复启动，启动操作忽略"),Level_Warning);
		return true;
	}

	//接口查询
	IUnknownEx * pISocketEngine=GET_OBJECT_INTERFACE(m_TCPSocketEngine,IUnknownEx);
	IUnknownEx * pIAttemperEngine=(IUnknownEx *)m_AttemperEngine.GetQueueService(IID_IUnknownEx,VER_IUnknownEx);

	//绑定组件
	if (m_AttemperEngine.SetSocketEngine(pISocketEngine)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎与调度引擎绑定失败"),Level_Exception);
		return false;
	}
	if (m_TimerEngine.SetTimerEngineSink(pIAttemperEngine)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("定时器引擎与调度引擎绑定失败"),Level_Exception);
		return false;
	}
	if (m_TCPSocketEngine.SetSocketEngineSink(pIAttemperEngine)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎与调度引擎绑定失败"),Level_Exception);
		return false;
	}

	//启动引擎服务
	if (m_TimerEngine.StartService()==false) return false;
	if (m_DataBaseEngine.StartService()==false) return false;
	if (m_AsynchronismEngine.StartService()==false) return false;
	if (m_AttemperEngine.StartService()==false) return false;
	if (m_TCPSocketEngine.StartService()==false) return false;

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool __cdecl CServiceEngine::StopService()
{
	//设置变量
	m_bService=false;

	//删除定时器
	m_TimerEngine.KillAllTimer();

	//停止服务引擎
	m_AttemperEngine.StopService();
	m_DataBaseEngine.StopService();
	m_TimerEngine.StopService();
	m_TCPSocketEngine.StopService();
	m_AsynchronismEngine.StopService();

	return true;
}

//设置事件
bool __cdecl CServiceEngine::SetEventService(IUnknownEx * pIUnknownEx)
{
	return CEventTrace::SetEventService(pIUnknownEx);
}

//注册钩子
bool __cdecl CServiceEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
{
	return m_AttemperEngine.SetAttemperEngineSink(pIUnknownEx);
}

//注册钩子
bool __cdecl CServiceEngine::SetDataBaseSink(IUnknownEx * pIUnknownEx)
{
	return m_DataBaseEngine.SetDataBaseSink(pIUnknownEx);
}

//注册钩子
bool __cdecl CServiceEngine::RegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx)
{
	return m_AsynchronismEngine.RegisterAsynchronismEngineSink(pIUnknownEx);
}

//设置网络
bool __cdecl CServiceEngine::InitServiceEngine(WORD wListenPort, WORD wMaxSocketItem)
{
	if (m_TCPSocketEngine.SetServicePort(wListenPort)==false) return false;
	if (m_TCPSocketEngine.SetMaxSocketItem(wMaxSocketItem)==false) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateServiceEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CServiceEngine * pServiceEngine=NULL;
	try
	{
		pServiceEngine=new CServiceEngine();
		if (pServiceEngine==NULL) throw TEXT("创建失败");
		void * pObject=pServiceEngine->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pServiceEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
