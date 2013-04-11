#include "StdAfx.h"
#include "EventService.h"
#include "AsynchronismEngine.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define WM_ASYN_REQUEST					WM_USER+10						//异步请求

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CControlWnd, CWnd)
	ON_MESSAGE(WM_ASYN_REQUEST,OnAsynRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CControlWnd::CControlWnd()
{
	m_pAsynchronismEngine=NULL;
}

//析构函数
CControlWnd::~CControlWnd()
{
}

//请求消息
HRESULT CControlWnd::OnAsynRequest(WPARAM wParam, LPARAM lParam)
{
	//效验变量
	ASSERT(m_pAsynchronismEngine!=NULL);

	//获取参数
	WORD wRequestID=(WORD)wParam;
	IAsynchronismEngineSink * pIAsynchronismEngineSink=(IAsynchronismEngineSink *)lParam;

	//请求处理
	ASSERT(pIAsynchronismEngineSink!=NULL);
	m_pAsynchronismEngine->OnAsynchronismRequest(wRequestID,pIAsynchronismEngineSink);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CMessageThread::CMessageThread()
{
	m_pAsynchronismEngine=NULL;
}

//析构函数
CMessageThread::~CMessageThread()
{
}

//运行函数
bool CMessageThread::RepetitionRun()
{
	MSG Message;
	if (GetMessage(&Message,NULL,0,0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		return true;
	}
	return false;
}

//停止线程
bool CMessageThread::StopThread(DWORD dwWaitSeconds)
{
	PostThreadMessage(WM_QUIT,0,0);
	return __super::StopThread(dwWaitSeconds);
}

//中止线程
bool CMessageThread::TerminateThread(DWORD dwExitCode)
{
	PostThreadMessage(WM_QUIT,0,0);
	return __super::TerminateThread(dwExitCode);
}

//开始事件
bool CMessageThread::OnThreadStratEvent()
{
	ASSERT(m_pAsynchronismEngine!=NULL);
	return m_pAsynchronismEngine->OnMessageThreadStart();
}

//停止事件
bool CMessageThread::OnThreadStopEvent()
{
	ASSERT(m_pAsynchronismEngine!=NULL);
	return m_pAsynchronismEngine->OnMessageThreadStop();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CAsynchronismEngine::CAsynchronismEngine(void)
{
	//设置变量
	m_bService=false;
	memset(m_cbBuffer,0,sizeof(m_cbBuffer));

	//设置组件
	m_ControlWnd.m_pAsynchronismEngine=this;
	m_MessageThread.m_pAsynchronismEngine=this;

	return;
}

//析构函数
CAsynchronismEngine::~CAsynchronismEngine(void)
{
}

//接口查询
void * __cdecl CAsynchronismEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAsynchronismEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAsynchronismEngine,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool __cdecl CAsynchronismEngine::StartService()
{
	//启动线程
	if (m_MessageThread.StartThead()==false) return false;

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool __cdecl CAsynchronismEngine::StopService()
{
	//设置变量
	m_bService=false;

	//停止线程
	m_MessageThread.StopThread();

	//清理数据
	m_DataStorage.RemoveData(false);

	return true;
}

//插入请求
bool __cdecl CAsynchronismEngine::InsertRequest(WORD wRequestID, void * const pBuffer, WORD wDataSize, IUnknownEx * pIUnknownEx)
{
	//效验状态
	ASSERT(wDataSize<=sizeof(m_cbBuffer));
	if (m_bService==false) return false;
	if (wDataSize>sizeof(m_cbBuffer)) return false;

	//查询接口
	IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IAsynchronismEngineSink);
	ASSERT(pIAsynchronismEngineSink!=NULL);
	if (pIAsynchronismEngineSink==NULL) return false;

	//插入数据
	CThreadLockHandle ThreadLockHandle(&m_ThreadLock);
	if (m_DataStorage.AddData(wRequestID,pBuffer,wDataSize)==false) return false;

	//投递消息
	PostMessage(m_ControlWnd.m_hWnd,WM_ASYN_REQUEST,wRequestID,(LPARAM)pIAsynchronismEngineSink);
	
	return true;
}

//注册钩子
bool __cdecl CAsynchronismEngine::RegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx)
{
	//获取接口
	IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IAsynchronismEngineSink);
	ASSERT(pIAsynchronismEngineSink!=NULL);
	if (pIAsynchronismEngineSink==NULL) return false;

	//查找现存
	IAsynchronismEngineSink * pIAsynchronismEngineSinkTemp=NULL;
	for (INT_PTR i=0;i<m_AsynchronismEngineSinkArray.GetCount();i++)
	{
		pIAsynchronismEngineSinkTemp=m_AsynchronismEngineSinkArray[0];
		ASSERT(pIAsynchronismEngineSinkTemp!=NULL);
		if (pIAsynchronismEngineSinkTemp==pIAsynchronismEngineSink) return false;
	}

	//注册钩子
	m_AsynchronismEngineSinkArray.Add(pIAsynchronismEngineSink);

	return true;
}

//取消注册
bool __cdecl CAsynchronismEngine::UnRegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx)
{
	//获取接口
	IAsynchronismEngineSink * pIAsynchronismEngineSink=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IAsynchronismEngineSink);
	ASSERT(pIAsynchronismEngineSink!=NULL);
	if (pIAsynchronismEngineSink==NULL) return false;

	//删除钩子
	IAsynchronismEngineSink * pIAsynchronismEngineSinkTemp=NULL;
	for (INT_PTR i=0;i<m_AsynchronismEngineSinkArray.GetCount();i++)
	{
		pIAsynchronismEngineSinkTemp=m_AsynchronismEngineSinkArray[0];
		ASSERT(pIAsynchronismEngineSinkTemp!=NULL);
		if (pIAsynchronismEngineSinkTemp==pIAsynchronismEngineSink)
		{
			m_AsynchronismEngineSinkArray.RemoveAt(i);
			return true;
		}
	}

	return false;
}

//线程启动
bool CAsynchronismEngine::OnMessageThreadStart()
{
	//创建窗口
	if (m_ControlWnd.m_hWnd==NULL)
		m_ControlWnd.Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),CWnd::GetDesktopWindow(),10);

	//事件通知
	bool bSuccsee=true;
	IAsynchronismEngineSink * pIAsynchronismEngineSink=NULL;
	for (INT_PTR i=0;i<m_AsynchronismEngineSinkArray.GetCount();i++)
	{
		pIAsynchronismEngineSink=m_AsynchronismEngineSinkArray[0];
		ASSERT(pIAsynchronismEngineSink!=NULL);
		try 
		{ 
			if (pIAsynchronismEngineSink->OnAsynchronismEngineStart()==false) bSuccsee=false;
		}
		catch (...)	{ bSuccsee=false; }
	}

	return bSuccsee;
}

//线程停止
bool CAsynchronismEngine::OnMessageThreadStop()
{
	//关闭窗口
	m_ControlWnd.DestroyWindow();

	//事件通知
	bool bSuccsee=true;
	IAsynchronismEngineSink * pIAsynchronismEngineSink=NULL;
	for (INT_PTR i=0;i<m_AsynchronismEngineSinkArray.GetCount();i++)
	{
		pIAsynchronismEngineSink=m_AsynchronismEngineSinkArray[0];
		ASSERT(pIAsynchronismEngineSink!=NULL);
		try	
		{ 
			if (pIAsynchronismEngineSink->OnAsynchronismEngineStop()==false) bSuccsee=false;
		}
		catch (...)	{ bSuccsee=false; }
	}

	return bSuccsee;
}

//异步请求
void CAsynchronismEngine::OnAsynchronismRequest(WORD wRequestID, IAsynchronismEngineSink * pIAsynchronismEngineSink)
{
	//锁定资源
	CThreadLockHandle ThreadLockHandle(&m_ThreadLock);

	//提取数据
	tagDataHead DataHead;
	m_DataStorage.GetData(DataHead,m_cbBuffer,sizeof(m_cbBuffer));

	//消息处理
	try
	{
		ASSERT(DataHead.wIdentifier==wRequestID);
		pIAsynchronismEngineSink->OnAsynchronismRequest(wRequestID,m_cbBuffer,DataHead.wDataSize);
	}
	catch (...) {}

	return;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateAsynchronismEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CAsynchronismEngine * pAsynchronismEngine=NULL;
	try
	{
		pAsynchronismEngine=new CAsynchronismEngine();
		if (pAsynchronismEngine==NULL) throw TEXT("创建失败");
		void * pObject=pAsynchronismEngine->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pAsynchronismEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
