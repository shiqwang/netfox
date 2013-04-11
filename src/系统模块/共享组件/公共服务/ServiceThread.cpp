#include "StdAfx.h"
#include <Process.h>
#include "ServiceThread.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//启动参数
struct tagThreadParameter
{
	bool							bSuccess;							//是否错误
	HANDLE							hEventFinish;						//事件句柄
	CServiceThread					* m_pServiceThread;					//线程指针
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CThreadLockHandle::CThreadLockHandle(IThreadLock * pIThreadLock, bool bAutoLock)
{
	ASSERT(pIThreadLock!=NULL);
	m_nLockCount=0;
	m_pIThreadLock=pIThreadLock;
	if (bAutoLock) Lock();
	return;
}

//析构函数
CThreadLockHandle::~CThreadLockHandle()
{
	while (m_nLockCount>0) UnLock();
}

//锁定函数
void CThreadLockHandle::Lock()
{
	//效验状态
	ASSERT(m_nLockCount>=0);
	ASSERT(m_pIThreadLock!=NULL);

	//锁定对象
	m_nLockCount++;
	m_pIThreadLock->Lock();
}

//解锁函数 
void CThreadLockHandle::UnLock()
{
	//效验状态
	ASSERT(m_nLockCount>0);
	ASSERT(m_pIThreadLock!=NULL);

	//解除锁定
	m_nLockCount--;
	m_pIThreadLock->UnLock();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceThread::CServiceThread(void)
{
	m_bRun=false;
	m_uThreadID=0;
	m_hThreadHandle=NULL;
}

//析构函数
CServiceThread::~CServiceThread(void)
{
	StopThread(INFINITE);
}

//状态判断
bool CServiceThread::IsRuning()
{
	if (m_hThreadHandle!=NULL)
	{
		DWORD dwRetCode=WaitForSingleObject(m_hThreadHandle,0);
		if (dwRetCode==WAIT_TIMEOUT) return true;
	}
	return false;
}

//启动线程
bool CServiceThread::StartThead()
{
	//效验状态
	if (IsRuning()) return false;

	//清理变量
	if (m_hThreadHandle!=NULL) 
	{
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle=NULL;
		m_uThreadID=0;
	}

	//构造参数
	tagThreadParameter ThreadParameter;
	ThreadParameter.bSuccess=false;
	ThreadParameter.m_pServiceThread=this;
	ThreadParameter.hEventFinish=CreateEvent(NULL,FALSE,FALSE,NULL);
	if (ThreadParameter.hEventFinish==NULL) return false;

	//启动线程
	m_bRun=true;
	m_hThreadHandle=(HANDLE)::_beginthreadex(NULL,0,ThreadFunction,&ThreadParameter,0,&m_uThreadID);

	//等待事件
	WaitForSingleObject(ThreadParameter.hEventFinish,INFINITE);

	//关闭事件
	CloseHandle(ThreadParameter.hEventFinish);

	//判断错误
	if (ThreadParameter.bSuccess==false) 
	{
		StopThread();
		return false;
	}

	return true;
}

//停止线程
bool CServiceThread::StopThread(DWORD dwWaitSeconds)
{
	//停止线程
	if (IsRuning()==true)
	{
		m_bRun=false;
		DWORD dwRetCode=WaitForSingleObject(m_hThreadHandle,dwWaitSeconds);
		if (dwRetCode==WAIT_TIMEOUT) return false;
	}

	//设置变量
	if (m_hThreadHandle!=NULL)
	{
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle=NULL;
		m_uThreadID=0;
	}

	return true;
}

//中止线程
bool CServiceThread::TerminateThread(DWORD dwExitCode)
{
	//停止线程
	if (IsRuning()==true)
	{
		::TerminateThread(m_hThreadHandle,dwExitCode);
	}

	//设置变量
	if (m_hThreadHandle!=NULL)
	{
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle=NULL;
		m_uThreadID=0;
	}

	return true;
}

//投递消息
bool CServiceThread::PostThreadMessage(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	//状态效验
	if (m_uThreadID==0) return false;

	//投递消息
	BOOL bSuccess=::PostThreadMessage(m_uThreadID,uMessage,wParam,lParam);
	return bSuccess?true:false;
}

//线程函数
unsigned __stdcall CServiceThread::ThreadFunction(LPVOID pThreadData)
{
	//获取参数
	ASSERT(pThreadData!=NULL);
	tagThreadParameter * pThreadParameter=(tagThreadParameter *)pThreadData;
	CServiceThread * pThread=pThreadParameter->m_pServiceThread;

	//随机事件
	srand((DWORD)time(NULL));

	//启动事件
	try 
	{
		pThreadParameter->bSuccess=pThread->OnThreadStratEvent(); 
	} 
	catch (...) 
	{ 
		pThreadParameter->bSuccess=false; 
	}

	//设置事件
	bool bSuccess=pThreadParameter->bSuccess;
	ASSERT(pThreadParameter->hEventFinish!=NULL);
	SetEvent(pThreadParameter->hEventFinish);

	//运行线程
	if (bSuccess==true)
	{
		while (pThread->m_bRun)
		{
			try
			{
				if (pThread->RepetitionRun()==false) break;
			}
			catch (...) {}
		}
	}

	//停止事件
	try 
	{ 
		pThread->OnThreadStopEvent(); 
	} 
	catch (...) 
	{
	}

	//中止线程
	_endthreadex(0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
