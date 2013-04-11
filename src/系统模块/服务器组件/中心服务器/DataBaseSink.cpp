#include "StdAfx.h"
#include "Afxinet.h"
#include "DataBaseSink.h"

//构造函数
CDataBaseSink::CDataBaseSink()
{
	m_pInitParamter=NULL;
	m_pIEventService=NULL;
}

CDataBaseSink::~CDataBaseSink()
{
}

//接口查询
void * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CDataBaseSink::StartService(IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(m_pInitParamter!=NULL);
	ASSERT(m_pIEventService!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	//创建实例
	if ((m_CenterDataBase.IsValid()==false)&&(m_CenterDataBase.CreateInstance()==false))
	{
		m_pIEventService->ShowEventNotify(TEXT("中心数据库对象创建失败"),
									Level_Exception);
		return false;
	}

	try
	{
		//连接中心数据库
		m_CenterDataBase->SetConnectionInfo(m_pInitParamter->m_szServerDataBaseAddr,
									m_pInitParamter->m_wServerDataBasePort,
									m_pInitParamter->m_szServerDataBaseName,
									m_pInitParamter->m_szServerDataBaseUser,
									m_pInitParamter->m_szServerDataBasePass);
		m_CenterDataBase->OpenConnection();

		return true;
	}
	catch (IADOError * pIADOError)
	{
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return false;
}

//调度模块关闭
bool __cdecl CDataBaseSink::StopService(IUnknownEx * pIUnknownEx)
{
	return false;
}

//数据操作处理
bool __cdecl CDataBaseSink::OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, 
											  void * pDataBuffer, 
											  WORD wDataSize)
{
	return false;
}

//设置事件
bool CDataBaseSink::SetEventService(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);
	return (m_pIEventService!=NULL);
}

//配置函数
bool CDataBaseSink::InitDataBaseSink(CInitParamter * pInitParamter, 
									 IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pInitParamter!=NULL);

	//设置变量
	m_pInitParamter=pInitParamter;

	//查询接口
	IServiceEngine * pIServiceEngine=(IServiceEngine *)pIUnknownEx->QueryInterface(IID_IServiceEngine,VER_IServiceEngine);
	ASSERT(pIServiceEngine!=NULL);
	if (pIServiceEngine==NULL) 
	{
		throw TEXT("服务引擎接口查询失败");
	}

	//获取逻辑引擎
	IAttemperEngine * pIAttemperEngine=(IAttemperEngine *)pIServiceEngine->GetAttemperEngine(IID_IAttemperEngine,VER_IAttemperEngine);
	ASSERT(pIAttemperEngine!=NULL);
	if (pIAttemperEngine==NULL) 
	{
		throw TEXT("调度引擎接口查询失败");
	}

	//设置通知组件
	IUnknownEx * pIQueueService=(IUnknownEx *)pIAttemperEngine->GetQueueService(IID_IQueueService,VER_IQueueService);
	if (m_AttemperEvent.SetQueueService(pIQueueService)==false) 
	{
		throw TEXT("逻辑引擎通知接口设置失败");
	}

	return true;
}
