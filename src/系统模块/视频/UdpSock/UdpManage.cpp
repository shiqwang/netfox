#include "StdAfx.h"
#include ".\udpmanage.h"

DWORD WINAPI WorkerThread(LPVOID lp);

CRITICAL_SECTION m_EventQueueCritical; //临界变量

CUdpManage::CUdpManage(void)
{
	m_hWorkerIOCPort = NULL;
	InitializeCriticalSection( &m_EventQueueCritical );
}

CUdpManage::~CUdpManage(void)
{
	if( m_hWorkerIOCPort )
		CloseHandle( m_hWorkerIOCPort );
	DeleteCriticalSection( &m_EventQueueCritical ); //删掉临界变量
}

int CUdpManage::Init( int ServerSocketSize, int WorkerNum )
{
	int r = CUdpIocp::Init( ServerSocketSize, WorkerNum );

	//创建外部完成端口
	m_hWorkerIOCPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

	CreateWorkerThread( 4 );
	return r;
}

void CUdpManage::CreateWorkerThread( int WorkerNum )
{
	if( WorkerNum==0 )
        WorkerNum = 1;

	HANDLE tHandle;
	//创建内部线程
	for(int i = 0; i < (int)WorkerNum; i++)
	{
		tHandle = CreateThread(NULL,0,WorkerThread,(LPVOID)this,0,NULL);
		m_ThreadHandleVector.push_back( tHandle );
		//SetThreadPriority( tHandle, THREAD_PRIORITY_TIME_CRITICAL);
	}
}

DWORD WINAPI WorkerThread(LPVOID lp)
{
	CUdpManage* pUdpManage = (CUdpManage*)lp;
	CUser* pUser = NULL;
	
	LPOVERLAPPED	pOvl; //返回结构
	BOOL	bRet; //返回值
	DWORD	nbytes; //返回大小
	DWORD	WorkIndex; //ID
	char *pData = NULL;
	int len = 0; //长度

	while (1)
	{
//		Sleep(1);
		//监听完成端口,有数据才会往下运行
		bRet = GetQueuedCompletionStatus( pUdpManage->m_hWorkerIOCPort, &nbytes, &WorkIndex, &pOvl, INFINITE );

		if( pUdpManage->m_EventQueue.empty() )
			continue;

		EnterCriticalSection( &m_EventQueueCritical ); //Lock
		pUser = pUdpManage->m_EventQueue.front();
		pUdpManage->m_EventQueue.pop();
		LeaveCriticalSection( &m_EventQueueCritical );
		if( pUser )
		{
//			try
			{
				while (pUser->PullOutCore(pData, len))
				{
					if(pData)
					{
						//处理结果
						pUser->Parsing( pData, len );
						delete pData; //删除数据
						pData = NULL; //置空
					}
				}
			}
//			catch(...)
			{
			}
		}	
	}
	return 0;
}

void CUdpManage::SendAllUser( char* Tmp, int len, char* lpTo, UINT port )
{
	for( int i=0; i<m_ServerSocketSize; i++ )
	{
		if( m_pSocketArray[i]!=NULL )
		{
			m_pSocketArray[i]->Send( Tmp, len, lpTo, port );
		}
	}
}