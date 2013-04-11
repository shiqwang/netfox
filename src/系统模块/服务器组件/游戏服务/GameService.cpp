#include "StdAfx.h"
#include "GameService.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameService::CGameService()
{
	m_szModule[0]=0;
	m_hDllInstance=NULL;
	m_pIEventService=NULL;
	m_pIGameServiceManager=NULL;
	memset(&m_GameUserDBInfo,0,sizeof(m_GameUserDBInfo));
	memset(&m_GameScoreDBInfo,0,sizeof(m_GameScoreDBInfo));
	memset(&m_GameServiceAttrib,0,sizeof(m_GameServiceAttrib));
	memset(&m_GameServiceOption,0,sizeof(m_GameServiceOption));
}

//析构函数
CGameService::~CGameService()
{
	UnLoadGameServiceModule();
	return;
}

//接口查询
void * __cdecl CGameService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameService,Guid,dwQueryVer);
	return NULL;
}

//开始服务
bool __cdecl CGameService::StartService(tagGameServiceParameter * pGameServiceParameter)
{
	//效验参数
	ASSERT(pGameServiceParameter!=NULL);
	if (pGameServiceParameter==NULL) return false;

	try
	{
		//设置变量
		bool bSuccess=false;
		lstrcpyn(m_szModule,pGameServiceParameter->szModule,CountArray(m_szModule));
		CopyMemory(&m_GameUserDBInfo,&pGameServiceParameter->GameUserDBInfo,sizeof(m_GameUserDBInfo));
		CopyMemory(&m_GameScoreDBInfo,&pGameServiceParameter->GameScoreDBInfo,sizeof(m_GameScoreDBInfo));
		CopyMemory(&m_GameServiceOption,&pGameServiceParameter->GameServiceOption,sizeof(m_GameServiceOption));

		//创建服务对象
		if (m_ServiceEngine.GetInterface()==NULL)
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
			ShowErrorMessasge(TEXT("游戏服务已经了启动了"),Level_Warning);
			return true;
		}

		//加载模块
		ASSERT(m_szModule[0]!=0);
		if (LoadGameServiceModule(m_szModule)==false)
		{
			ShowErrorMessasge(TEXT("游戏服务模块组件加载失败"),Level_Exception);
			throw 0;
		}

		//调整参数
		WORD wLessConnect=m_GameServiceOption.wTableCount*m_GameServiceAttrib.wChairCount+50;
		m_GameServiceOption.wMaxConnect=__max(m_GameServiceOption.wMaxConnect,wLessConnect);

		//配置网络
		if (m_ServiceEngine->InitServiceEngine(m_GameServiceOption.wServerPort,m_GameServiceOption.wMaxConnect)==false) throw 0;

		//配置数据库
		tagDataBaseSinkParameter DataBaseSinkParameter;
		memset(&DataBaseSinkParameter,0,sizeof(DataBaseSinkParameter));
		DataBaseSinkParameter.pGameUserDBInfo=&m_GameUserDBInfo;
		DataBaseSinkParameter.pGameScoreDBInfo=&m_GameScoreDBInfo;
		DataBaseSinkParameter.pGameServiceAttrib=&m_GameServiceAttrib;
		DataBaseSinkParameter.pGameServiceOption=&m_GameServiceOption;
		if (m_DataBaseSink.InitDataBaseSink(&DataBaseSinkParameter,m_ServiceEngine.GetInterface())==false)
		{
			ShowErrorMessasge(TEXT("数据库引擎外挂服务配置失败"),Level_Exception);
			throw 0;
		}

		//配置调度
		tagAttemperSinkParameter AttemperSinkParameter;
		memset(&AttemperSinkParameter,0,sizeof(AttemperSinkParameter));
		AttemperSinkParameter.pCenterSocket=&m_CenterSocket;
		AttemperSinkParameter.pGameServiceAttrib=&m_GameServiceAttrib;
		AttemperSinkParameter.pGameServiceOption=&m_GameServiceOption;
		AttemperSinkParameter.pIGameServiceManager=m_pIGameServiceManager;
		AttemperSinkParameter.dwCenterServer=pGameServiceParameter->dwCenterAddr;
		if (m_AttemperSink.InitAttemperSink(&AttemperSinkParameter,m_ServiceEngine.GetInterface())==false)
		{
			ShowErrorMessasge(TEXT("调度引擎外挂服务配置失败"),Level_Exception);
			throw 0;
		}

		//获取调度引擎
		IAttemperEngine * pIAttemperEngine=(IAttemperEngine *)m_ServiceEngine->GetAttemperEngine(IID_IAttemperEngine,VER_IAttemperEngine);
		ASSERT(pIAttemperEngine!=NULL);
		if (pIAttemperEngine==NULL)
		{
			ShowErrorMessasge(TEXT("调度引擎接口查询失败"),Level_Exception);
			throw 0;
		}

		//获取调度队列
		IQueueService * pIQueueService=(IQueueService *)pIAttemperEngine->GetQueueService(IID_IQueueService,VER_IQueueService);
		ASSERT(pIQueueService!=NULL);
		if (pIQueueService==NULL) 
		{
			ShowErrorMessasge(TEXT("调度引擎队列服务接口查询失败"),Level_Exception);
			throw 0;
		}

		//获取异步引擎
		IAsynchronismEngine * pIAsynchronismEngine=(IAsynchronismEngine *)m_ServiceEngine->GetAsynchronismEngine(IID_IAsynchronismEngine,VER_IAsynchronismEngine);
		ASSERT(pIAsynchronismEngine!=NULL);
		if (pIAsynchronismEngine==NULL) 
		{
			ShowErrorMessasge(TEXT("异步引擎队务接口查询失败"),Level_Exception);
			throw 0;
		}

		//中心连接
		if (m_CenterSocket.SetQueueService(pIQueueService)==false) throw 0;
		if (m_CenterSocket.SetAsynchronismEngine(pIAsynchronismEngine)==false) throw 0;

		//注册对象
		IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECT_INTERFACE(m_CenterSocket,IAsynchronismEngineSink);
		if (pIAsynchronismEngine->RegisterAsynchronismEngineSink(pIAsynchronismEngineSink)==false) 
		{
			ShowErrorMessasge(TEXT("中心连接异步对象注册失败"),Level_Exception);
			throw 0;
		}

		//启动服务
		if (m_ServiceEngine->StartService()==false) throw 0;

		return true;
	}
	catch (LPCTSTR pszErrorMsg)
	{
		ShowErrorMessasge(pszErrorMsg,Level_Exception); 
		ShowErrorMessasge(TEXT("游戏服务启动失败"),Level_Exception); 
		StopService();
	}
	catch (...)
	{ 
		ShowErrorMessasge(TEXT("产生异常错误，游戏服务启动失败"),Level_Exception); 
		StopService();
	}

	return false;
}

//停止服务
bool __cdecl CGameService::StopService()
{
	try
	{
		//停止服务
		if (m_ServiceEngine.GetInterface()!=NULL) 
		{
			//停止服务
			m_ServiceEngine->StopService();

			//注销对象
			IAsynchronismEngine * pIAsynchronismEngine=(IAsynchronismEngine *)m_ServiceEngine->GetAsynchronismEngine(IID_IAsynchronismEngine,VER_IAsynchronismEngine);
			IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECT_INTERFACE(m_CenterSocket,IAsynchronismEngineSink);
			if (pIAsynchronismEngine!=NULL) pIAsynchronismEngine->UnRegisterAsynchronismEngineSink(pIAsynchronismEngineSink);
		}

		//卸载模块
		UnLoadGameServiceModule();

		return true;
	}
	catch (LPCTSTR pszErrorMsg)
	{
		ShowErrorMessasge(pszErrorMsg,Level_Exception); 
		ShowErrorMessasge(TEXT("游戏服务停止失败"),Level_Exception); 
	}
	catch (...)
	{ 
		ShowErrorMessasge(TEXT("游戏服务停止失败"),Level_Exception); 
	}

	return false;
}

//服务状态
bool CGameService::IsService()
{
	if (m_ServiceEngine.GetInterface()!=NULL==false) return false;
	return m_ServiceEngine->IsService();
}

//设置事件
bool CGameService::SetEventService(IUnknownEx * pIEventService)
{
	//获取接口
	ASSERT(pIEventService!=NULL);
	m_pIEventService=(IEventService *)pIEventService->QueryInterface(IID_IEventService,VER_IEventService);
	ASSERT(m_pIEventService!=NULL);

	//设置接口
	m_CenterSocket.SetEventService(pIEventService);
	m_DataBaseSink.SetEventService(pIEventService);
	m_AttemperSink.SetEventService(pIEventService);
	if (m_ServiceEngine.GetInterface()!=NULL) m_ServiceEngine->SetEventService(pIEventService);

	return (m_pIEventService!=NULL);
}

//释放模块
bool CGameService::UnLoadGameServiceModule()
{
	try
	{
		if (m_pIGameServiceManager!=NULL)
		{
			m_pIGameServiceManager->Release();
			m_pIGameServiceManager=NULL;
		}
		if (m_hDllInstance!=NULL)
		{
			AfxFreeLibrary(m_hDllInstance);
			m_hDllInstance=NULL;
		}
		return true;
	}
	catch (...) {}

	return false;
}

//加载模块
bool CGameService::LoadGameServiceModule(LPCTSTR pszModuleName)
{
	//效验参数
	ASSERT(pszModuleName!=NULL);
	ASSERT(m_hDllInstance==NULL);

	try
	{
		//加载模块
		m_hDllInstance=AfxLoadLibrary(pszModuleName);
		if (m_hDllInstance==NULL) 
		{
			TCHAR szDescribe[256]=TEXT("");
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("“%s”模块加载失败"),pszModuleName);
			ShowErrorMessasge(szDescribe,Level_Exception);
			throw 0;
		}
		ModuleCreateFunc * CreateFunc=(ModuleCreateFunc *)GetProcAddress(m_hDllInstance,TEXT("CreateGameServiceManager"));
		if (CreateFunc==NULL) 
		{
			TCHAR szDescribe[256]=TEXT("");
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("“%s”创建函数不存在"),pszModuleName);
			ShowErrorMessasge(szDescribe,Level_Exception);
			throw 0;
		}
		m_pIGameServiceManager=(IGameServiceManager *)CreateFunc(IID_IGameServiceManager,VER_IGameServiceManager);
		if (m_pIGameServiceManager==NULL) 
		{
			TCHAR szDescribe[256]=TEXT("");
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("“%s”游戏管理接口创建失败"),pszModuleName);
			ShowErrorMessasge(szDescribe,Level_Exception);
			throw 0;
		}

		//获取属性
		m_pIGameServiceManager->GetGameServiceAttrib(m_GameServiceAttrib);

		return true;
	}
	catch (...)	{ UnLoadGameServiceModule(); }

	return false;
}

//显示错误
void CGameService::ShowErrorMessasge(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pszString,TraceLevel);
	return;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateGameService(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CGameService * pGameService=NULL;
	try
	{
		pGameService=new CGameService();
		if (pGameService==NULL) throw TEXT("创建失败");
		void * pObject=pGameService->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pGameService);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
