#include "StdAfx.h"
#include "TimerEngine.h"
#include "EventService.h"

//宏定义
#define NO_TIME_LEFT						DWORD(-1)				//不响应时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CTimerThread::CTimerThread(void)
{
	m_dwTimerSpace=0L;
	m_pTimerEngine=NULL;
}

//析构函数
CTimerThread::~CTimerThread(void)
{
}

//配置函数
bool CTimerThread::InitThread(CTimerEngine * pTimerEngine, DWORD dwTimerSpace)
{
	//效益参数
	ASSERT(dwTimerSpace>=10L);
	ASSERT(pTimerEngine!=NULL);
	if (pTimerEngine==NULL) return false;

	//设置变量
	m_dwTimerSpace=dwTimerSpace;
	m_pTimerEngine=pTimerEngine;

	return true;
}

//运行函数
bool CTimerThread::RepetitionRun()
{
	ASSERT(m_pTimerEngine!=NULL);
	Sleep(m_dwTimerSpace);
	m_pTimerEngine->OnTimerThreadSink();
	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTimerEngine::CTimerEngine(void)
{
	m_bService=false;
	m_dwTimePass=0L;
	m_dwTimerSpace=25L;
	m_dwTimeLeave=NO_TIME_LEFT;
}

//析构函数
CTimerEngine::~CTimerEngine(void)
{
	INT_PTR i = 0;
	//停止服务
	StopService();

	//清理内存
	tagTimerItem * pTimerItem=NULL;
	for (i=0;i<m_TimerItemFree.GetCount();i++)
	{
		pTimerItem=m_TimerItemFree[i];
		ASSERT(pTimerItem!=NULL);
		SafeDelete(pTimerItem);
	}
	for (i=0;i<m_TimerItemActive.GetCount();i++)
	{
		pTimerItem=m_TimerItemActive[i];
		ASSERT(pTimerItem!=NULL);
		SafeDelete(pTimerItem);
	}
	m_TimerItemFree.RemoveAll();
	m_TimerItemActive.RemoveAll();

	return;
}

//接口查询
void * __cdecl CTimerEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITimerEngine,Guid,dwQueryVer);
	QUERYINTERFACE(ITimerEngineManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITimerEngine,Guid,dwQueryVer);
	return NULL;
}

//设置定时器
bool __cdecl CTimerEngine::SetTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wParam)
{
	//锁定资源
	CThreadLockHandle LockHandle(&m_ThreadLock);

	//效验参数
	ASSERT(dwRepeat>0L);
	if (dwRepeat==0) return false;
	dwElapse=(dwElapse+m_dwTimerSpace-1)/m_dwTimerSpace*m_dwTimerSpace;

	//查找定时器
	bool bTimerExist=false;
	tagTimerItem * pTimerItem=NULL;
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		pTimerItem=m_TimerItemActive[i];
		ASSERT(pTimerItem!=NULL);
		if (pTimerItem->wTimerID==wTimerID) 
		{
			bTimerExist=true;
			break;
		}
	}

	//创建定时器
	if (bTimerExist==false)
	{
		INT_PTR nFreeCount=m_TimerItemFree.GetCount();
		if (nFreeCount>0)
		{
			pTimerItem=m_TimerItemFree[nFreeCount-1];
			ASSERT(pTimerItem!=NULL);
			m_TimerItemFree.RemoveAt(nFreeCount-1);
		}
		else
		{
			try
			{
				pTimerItem=new tagTimerItem;
				ASSERT(pTimerItem!=NULL);
				if (pTimerItem==NULL) return false;
			}
			catch (...) { return false; }
		}
	}

	//设置参数
	ASSERT(pTimerItem!=NULL);
	pTimerItem->wTimerID=wTimerID;
	pTimerItem->wBindParam=wParam;
	pTimerItem->dwElapse=dwElapse;
	pTimerItem->dwRepeatTimes=dwRepeat;
	pTimerItem->dwTimeLeave=dwElapse+m_dwTimePass;

	//激活定时器
	m_dwTimeLeave=__min(m_dwTimeLeave,dwElapse);
	if (bTimerExist==false) m_TimerItemActive.Add(pTimerItem);

	return false;
}

//删除定时器
bool __cdecl CTimerEngine::KillTimer(WORD wTimerID)
{
	//锁定资源
	CThreadLockHandle LockHandle(&m_ThreadLock);

	//查找定时器
	tagTimerItem * pTimerItem=NULL;
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		pTimerItem=m_TimerItemActive[i];
		ASSERT(pTimerItem!=NULL);
		if (pTimerItem->wTimerID==wTimerID) 
		{
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemFree.Add(pTimerItem);
			if (m_TimerItemActive.GetCount()==0) 
			{
				m_dwTimePass=0L;
				m_dwTimeLeave=NO_TIME_LEFT;
			}
			return true;;
		}
	}

	return false;
}

//删除定时器
bool __cdecl CTimerEngine::KillAllTimer()
{
	//锁定资源
	CThreadLockHandle LockHandle(&m_ThreadLock);

	//删除定时器
	m_TimerItemFree.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//设置变量
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEFT;

	return true;
}

//开始服务
bool __cdecl CTimerEngine::StartService()
{
	//效验状态
	if (m_bService==true) 
	{
		CEventTrace::ShowEventNotify(TEXT("定时器引擎重复启动，启动操作忽略"),Level_Warning);
		return true;
	}

	//判断绑定
	if (m_AttemperEvent.IsValid()==false)
	{
		CEventTrace::ShowEventNotify(TEXT("定时器引擎未与触发服务绑定"),Level_Exception);
		return false;
	}

	//设置变量
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEFT;
	if (m_TimerThread.InitThread(this,m_dwTimerSpace)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("定时器引擎线程服务初始化失败"),Level_Exception);
		return false;
	}

	//启动服务
	if (m_TimerThread.StartThead()==false)
	{
		CEventTrace::ShowEventNotify(TEXT("定时器引擎线程服务启动失败"),Level_Exception);
		return false;
	}

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool __cdecl CTimerEngine::StopService()
{
	//设置变量
	m_bService=false;

	//停止线程
	m_TimerThread.StopThread();

	//设置变量
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEFT;
	m_TimerItemFree.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	return true;
}

//设置接口
bool __cdecl CTimerEngine::SetTimerEngineSink(IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(m_bService==false);
	if (m_bService==true) return false;
	if (pIUnknownEx==NULL) return false;

	//设置接口
	if (m_AttemperEvent.SetQueueService(pIUnknownEx)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("定时器引擎与触发服务绑定失败"),Level_Exception);
		return false;
	}
	
	return true;
}

//定时器通知
void CTimerEngine::OnTimerThreadSink()
{
	//锁定资源
	CThreadLockHandle LockHandle(&m_ThreadLock);

	//倒计时间
	if (m_dwTimeLeave==NO_TIME_LEFT) 
	{
		ASSERT(m_TimerItemActive.GetCount()==0);
		return;
	}

	//减少时间
	ASSERT(m_dwTimeLeave>=m_dwTimerSpace);
	m_dwTimeLeave-=m_dwTimerSpace;
	m_dwTimePass+=m_dwTimerSpace;

	//查询定时器
	if (m_dwTimeLeave==0)
	{
		bool bKillTimer=false;
		tagTimerItem * pTimerItem=NULL;
		DWORD dwTimeLeave=NO_TIME_LEFT;
		for (INT_PTR i=0;i<m_TimerItemActive.GetCount();)
		{
			//效验参数
			pTimerItem=m_TimerItemActive[i];
			ASSERT(pTimerItem!=NULL);
			ASSERT(pTimerItem->dwTimeLeave>=m_dwTimePass);

			//定时器处理
			bKillTimer=false;
			pTimerItem->dwTimeLeave-=m_dwTimePass;
			if (pTimerItem->dwTimeLeave==0L)
			{
				//发送通知
				m_AttemperEvent.PostTimerEvent(pTimerItem->wTimerID,pTimerItem->wBindParam);

				//设置次数
				if (pTimerItem->dwRepeatTimes!=TIMER_REPEAT_TIMER)
				{
					ASSERT(pTimerItem->dwRepeatTimes>0);
					if (pTimerItem->dwRepeatTimes==1L)
					{
						bKillTimer=true;
						m_TimerItemActive.RemoveAt(i);
						m_TimerItemFree.Add(pTimerItem);
					}
					else pTimerItem->dwRepeatTimes--;
				}

				//设置时间
				if (bKillTimer==false) pTimerItem->dwTimeLeave=pTimerItem->dwElapse;
			}

			//增加索引
			if (bKillTimer==false) 
			{
				i++;
				dwTimeLeave=__min(dwTimeLeave,pTimerItem->dwTimeLeave);
				ASSERT(dwTimeLeave%m_dwTimerSpace==0);
			}
		}

		//设置响应
		m_dwTimePass=0L;
		m_dwTimeLeave=dwTimeLeave;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateTimerEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CTimerEngine * pTimerEngine=NULL;
	try
	{
		pTimerEngine=new CTimerEngine();
		if (pTimerEngine==NULL) throw TEXT("创建失败");
		void * pObject=pTimerEngine->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTimerEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
