#include "StdAfx.h"
#include "EventService.h"
#include "AttemperEngine.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngine::CAttemperEngine(void)
{
	m_bService=false;
	m_pITCPSocketEngine=NULL;
	m_pIAttemperEngineSink=NULL;
	return;
}

//析构函数
CAttemperEngine::~CAttemperEngine(void)
{
}

//接口查询
void * __cdecl CAttemperEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngine,Guid,dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngine,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool __cdecl CAttemperEngine::StartService()
{
	//判断状态
	if (m_bService==true) 
	{
		CEventTrace::ShowEventNotify(TEXT("调度引擎重复启动，启动操作忽略"),Level_Warning);
		return true;
	}

	//外挂接口
	if (m_pIAttemperEngineSink==NULL)
	{
		CEventTrace::ShowEventNotify(TEXT("调度引擎外挂服务不存在"),Level_Exception);
		return false;
	}

	//设置队列
	if (m_RequestQueueService.SetQueueServiceSink(GET_MYSELF_INTERFACE(IUnknownEx))==false)
	{
		CEventTrace::ShowEventNotify(TEXT("调度引擎与队列服务绑定失败"),Level_Exception);
		return false;
	}

	//启动外挂
	if (m_pIAttemperEngineSink->StartService(GET_MYSELF_INTERFACE(IUnknownEx))==false)
	{
		CEventTrace::ShowEventNotify(TEXT("调度引擎外挂服务启动失败"),Level_Exception);
		return false;
	}

	//启动队列
	if (m_RequestQueueService.StartService()==false)
	{
		CEventTrace::ShowEventNotify(TEXT("调度引擎队列服务启动失败"),Level_Exception);
		return false;
	}

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool __cdecl CAttemperEngine::StopService()
{
	//设置变量
	m_bService=false;

	//停止请求队列
	m_RequestQueueService.StopService();

	//停止外挂
	if (m_pIAttemperEngineSink!=NULL)
	{
		m_pIAttemperEngineSink->StopService(GET_MYSELF_INTERFACE(IUnknownEx));
	}

	return true;
}

//设置网络
bool __cdecl CAttemperEngine::SetSocketEngine(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pITCPSocketEngine=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ITCPSocketEngine);
	ASSERT(m_pITCPSocketEngine!=NULL);
	return (m_pITCPSocketEngine!=NULL);
}

//注册钩子
bool __cdecl CAttemperEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(m_pIAttemperEngineSink==NULL);
	if (pIUnknownEx==NULL) return false;
	if (m_pIAttemperEngineSink!=NULL) return false;

	//查询接口
	m_pIAttemperEngineSink=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IAttemperEngineSink);
	if (m_pIAttemperEngineSink==NULL)
	{
		CEventTrace::ShowEventNotify(TEXT("调度引擎外挂服务接口获取失败，挂接操作失败"),Level_Exception);
		return false;
	}

	return true;
}

//获取接口
void * __cdecl CAttemperEngine::GetQueueService(const IID & Guid, DWORD dwQueryVer)
{
	return m_RequestQueueService.QueryInterface(Guid,dwQueryVer);
}

//队列接口
void __cdecl CAttemperEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize, DWORD dwInsertTime)
{
	//内核事件
	ASSERT(m_pIAttemperEngineSink!=NULL);
	switch (wIdentifier)
	{
	case EVENT_TIMER:			//定时器事件
		{
			//效验参数
			ASSERT(wDataSize==sizeof(NTY_TimerEvent));
			if (wDataSize!=sizeof(NTY_TimerEvent)) return;

			//处理消息
			NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)pBuffer;
			m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->wTimerID,pTimerEvent->wBindParam);

			return;
		}
	case EVENT_DATABASE:		//数据库事件
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(NTY_DataBaseEvent));
			if (wDataSize<sizeof(NTY_DataBaseEvent)) return;

			//处理消息
			NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)pBuffer;
			m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent+1,wDataSize-sizeof(NTY_DataBaseEvent),pDataBaseEvent);

			return;
		}
	case EVENT_SOCKET_ACCEPT:	//网络应答事件
		{
			//效验大小
			ASSERT(wDataSize==sizeof(NTY_SocketAcceptEvent));
			if (wDataSize!=sizeof(NTY_SocketAcceptEvent)) return;

			//处理消息
			NTY_SocketAcceptEvent * pSocketAcceptEvent=(NTY_SocketAcceptEvent *)pBuffer;
			m_pIAttemperEngineSink->OnEventSocketAccept(pSocketAcceptEvent);

			return;
		}
	case EVENT_SOCKET_READ:		//网络读取事件
		{
			//效验大小
			NTY_SocketReadEvent * pSocketReadEvent=(NTY_SocketReadEvent *)pBuffer;
			ASSERT(wDataSize>=sizeof(NTY_SocketReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_SocketReadEvent)+pSocketReadEvent->wDataSize));
			if (wDataSize<sizeof(NTY_SocketReadEvent)) return;
			if (wDataSize!=(sizeof(NTY_SocketReadEvent)+pSocketReadEvent->wDataSize)) return;

			//处理消息
			bool bSuccess=false;
			try 
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventSocketRead(pSocketReadEvent->Command,pSocketReadEvent+1,pSocketReadEvent->wDataSize,pSocketReadEvent);
			}
			catch (...)	{ }
			if (bSuccess==false) m_pITCPSocketEngine->CloseSocket(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);

			return;
		}
	case EVENT_SOCKET_CLOSE:	//网络关闭事件
		{
			//效验大小
			ASSERT(wDataSize==sizeof(NTY_SocketCloseEvent));
			if (wDataSize!=sizeof(NTY_SocketCloseEvent)) return;

			//处理消息
			NTY_SocketCloseEvent * pSocketCloseEvent=(NTY_SocketCloseEvent *)pBuffer;
			m_pIAttemperEngineSink->OnEventSocketClose(pSocketCloseEvent);

			return;
		}
	}

	//其他事件
	m_pIAttemperEngineSink->OnAttemperEvent(wIdentifier,pBuffer,wDataSize,dwInsertTime); 

	return;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateAttemperEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CAttemperEngine * pAttemperEngine=NULL;
	try
	{
		pAttemperEngine=new CAttemperEngine();
		if (pAttemperEngine==NULL) throw TEXT("创建失败");
		void * pObject=pAttemperEngine->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pAttemperEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
