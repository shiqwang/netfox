#include "StdAfx.h"
#include "UdpIocp.h"

//事件
#define OVL_RECEIVE			0X01 //接收
#define OVL_SEND			0X02 //发送
#define OVL_CLOSE			0X03 //关闭

DWORD WINAPI ReceiveWorkerThread(LPVOID lp);

CUdpSocket::CUdpSocket(void)
{
	m_Sid = 0;
	ZeroMemory( &m_RecvOverlapped, sizeof(m_RecvOverlapped) );
	ZeroMemory( &m_SendOverlapped, sizeof(m_SendOverlapped) );
	pRecvOvl = &m_RecvOverlapped;
	pSendOvl = &m_SendOverlapped;

	pRecvOvl->Offset = OVL_RECEIVE; //接收事件
	pSendOvl->Offset = OVL_SEND;
}

CUdpSocket::~CUdpSocket(void)
{
}

void CUdpSocket::ReceivedData( int length )
{
	//检查长度
	if(!length)
		return; //返回错误
}

BOOL CUdpSocket::CheckSum( int length )
{
	if( length<=6 )
		return FALSE;
	PCHECK_SUM pCheckSum = (PCHECK_SUM)m_pRecvBuff;
	if( pCheckSum->Head != ((DWORD)PACKET_START2<<8) + PACKET_START1 )
		return FALSE;
	if( pCheckSum->Length+6 != length )
		return FALSE;
	if( *PWORD(&m_pRecvBuff[pCheckSum->Length+4]) != ((DWORD)PACKET_END2<<8) + PACKET_END1 )
		return FALSE;
	return TRUE;
}

int CUdpSocket::Recv()
{
	return CSocketBase::Recv( (sockaddr*)&m_FromAddr );
}

int CUdpSocket::Send( char* pBuf, long length, LPCTSTR lpTo, unsigned int Port, int dwFlag )
{
	int Ret; //返回值

	if( m_Socket == INVALID_SOCKET )
		return -1;

	//检查长度
	if( length > MAX_PACKET_SIZE )
		return 0;

	WSABUF Buf; //接收缓存
	DWORD dwSentSize = 0;

	//缓存
	char pTBuf[MAX_PACKET_SIZE];
	int index = 0;
	//封包格式
	pTBuf[index++] = (BYTE)PACKET_START1;
	pTBuf[index++] = (BYTE)PACKET_START2;
	*PWORD( &pTBuf[index] ) = (WORD)length;
	index += 2;
	memcpy( pTBuf+index, pBuf, length );
	index += length;
	pTBuf[index++] = (BYTE)PACKET_END1;
	pTBuf[index++] = (BYTE)PACKET_END2;

	Buf.buf = pTBuf;
	Buf.len = index;

	if(pSendOvl)
		pSendOvl->OffsetHigh = Buf.len;
	//发送
	if( m_SocketType == SOCK_STREAM )
	{
		Ret = WSASend( m_Socket, &Buf, 1, &dwSentSize, dwFlag, pSendOvl, NULL);
	}
	else
	{
		sockaddr_in addr; //地址结构
		//初始化地址
		//ZeroMemory( &addr, sizeof(addr) );
		addr.sin_family = AF_INET;
#ifdef _UNICODE
		USES_CONVERSION;
		addr.sin_addr.s_addr = inet_addr(W2A(lpTo));
#else
		addr.sin_addr.s_addr = inet_addr(lpTo);
#endif
		addr.sin_port = htons(Port);

		Ret = WSASendTo( m_Socket, &Buf, 1, &dwSentSize, dwFlag, (sockaddr*)&addr, sizeof(addr), pSendOvl, NULL);
	}

	//判断返回值
 	if ( Ret == SOCKET_ERROR )
	{
		Ret = WSAGetLastError(); //得到错误
		switch( Ret )
		{
		case WSA_IO_PENDING:
			//累计错误
			m_nPending++; 
			//大于3次结束连接
			if( m_nPending > 3 )
				//Close();
			break;
		case WSAEWOULDBLOCK:
			//累计错误
			m_nWouldblock++;
			//大于3次结束连接
			if( m_nWouldblock > 3 )
				//Close();
			break;
		default:
			//累计错误
			m_nSocketErr++;
			//大于3次结束连接
			if( m_nSocketErr == 2 )
				//Close();
			break;
		}
	}

	return Ret;
}

int CUdpSocket::Create( int type, int protocol, int mode )
{
	return CSocketBase::Create( type, protocol, mode );
}

int CUdpSocket::Create( int id )
{
	m_Sid = id;
	//初始化socket
	return CSocketBase::Create( SOCK_DGRAM, IPPROTO_UDP );
//	m_Socket = WSASocket( AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, NULL, WSA_FLAG_OVERLAPPED );
//	m_Socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
//	if( m_Socket == INVALID_SOCKET )
//		return WSAGetLastError(); //返回错误代码

//	return 0;
}

CUdpIocp::CUdpIocp(void)
{
	m_dwNumberOfWorkers = 0; //工作线程数
	m_dwConcurrency = 0; //cpu数
	m_ServerSocketSize = 0;
	m_hServerIOCPort = NULL;
	
	m_ThreadHandleVector.reserve( 1 );
	
	m_pSocketArray = NULL;
}

CUdpIocp::~CUdpIocp(void)
{
	Release();
}

int CUdpIocp::Init( int ServerSocketSize, int WorkerNum )
{
	//初始化ID
	int i;
	m_ServerSocketSize = ServerSocketSize;

	m_pSocketArray = new CUdpSocket* [ServerSocketSize];
	for( i = 0; i<ServerSocketSize; i++ )
		m_pSocketArray[i] = NULL;

	//创建外部接收线程
	CreateReceiveWorkerThread(WorkerNum);

	return 0;
}

void CUdpIocp::Release()
{
	int i;
	//释放线程
	for( i=0; i<(int)m_ThreadHandleVector.size();i++ )
	{
		TerminateThread( m_ThreadHandleVector[i], 0 );
	}
	m_ThreadHandleVector.clear();
	//释放Socket
	if( m_pSocketArray )
	{
		for( i=0;i<m_ServerSocketSize;i++ )
			delete m_pSocketArray[i];
		delete[] m_pSocketArray;
		m_pSocketArray = NULL;
	}
	//关闭句柄
	if( m_hServerIOCPort )
	{
		CloseHandle( m_hServerIOCPort );
		m_hServerIOCPort = NULL;
	}
	m_ServerSocketSize = 0;
}

//完成端口和ID绑定
bool CUdpIocp::Associate(CUdpSocket* pSocket, HANDLE hPort)
{
	//检查完成端口
	if (!hPort) 
	{
		return FALSE; //返回错误
	}
	
	//ID和完成端口绑定
	HANDLE hTemp;
	hTemp = CreateIoCompletionPort( pSocket->GetSocketHandle(), hPort, (DWORD)pSocket->GetSocketID(), 1/*m_dwConcurrency*/ );

	//返回
	return (hTemp == hPort);
}

void CUdpIocp::PushSocket( CUdpSocket* p )
{
	m_QueueRecv.push( p );
}

CUdpSocket* CUdpIocp::GetActiveSocket()
{
	for( int i=0; i<m_ServerSocketSize; i++ )
	{
		if( m_pSocketArray[i]!=NULL )
		{
			return m_pSocketArray[i];
		}
	}
	return NULL;
}

CUdpSocket* CUdpIocp::GetSocket( int i )
{
	if( i>=0 && i<m_ServerSocketSize )
	{
		if( m_pSocketArray[i]!=NULL )
		{
			return m_pSocketArray[i];
		}
	}
	return NULL;
}

//发送
void CUdpIocp::Send( char* Buf, int len, char* IP, unsigned int Port )
{
	for( int i=0; i<m_ServerSocketSize; i++ )
	{
		if( m_pSocketArray[i]!=NULL )
		{
			m_pSocketArray[i]->Send( Buf, len, IP, Port );
			break;
		}
	}
}

// 接收
void CUdpIocp::Recv(void)
{
	while( m_QueueRecv.empty() )
	{
		Sleep( 1 );
	}
	CUdpSocket* pUdpSocket = m_QueueRecv.front();
	m_QueueRecv.pop();
	pUdpSocket->Recv();
}

void CUdpIocp::CreateReceiveWorkerThread( int WorkerNum )
{
	SYSTEM_INFO SystemInfo; //系统信息
	// 设置线程权限 15:高
//	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	// 得到系统信息
	GetSystemInfo ( &SystemInfo );
	//是否自动设置使用资源
	if( !WorkerNum )
		//CPU的2倍,属于优化
		m_dwNumberOfWorkers = 2 * SystemInfo.dwNumberOfProcessors;
	else
		//用户设置
		m_dwNumberOfWorkers = WorkerNum;
	//保存CPU数
	m_dwConcurrency = SystemInfo.dwNumberOfProcessors;

	//创建外部完成端口
	m_hServerIOCPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, m_dwNumberOfWorkers );

	HANDLE tHandle;
	//创建内部线程
	for(int i = 0; i < (int)m_dwNumberOfWorkers; i++)
	{
		tHandle = CreateThread(NULL,0,ReceiveWorkerThread,(LPVOID)this,0,NULL);
		m_ThreadHandleVector.push_back( tHandle );
		//SetThreadPriority( tHandle, THREAD_PRIORITY_TIME_CRITICAL);
	}
}

//外部接收数据线程
DWORD WINAPI ReceiveWorkerThread(LPVOID lp)
{
	CUdpIocp* pUdpIocp = (CUdpIocp*)lp;
	BOOL	bRet = 0; //返回值
	DWORD	nbytes = 0; //返回大小
	DWORD	WorkIndex = 0; //ID
	LPOVERLAPPED	pOvl = NULL; //返回结构
	CUdpSocket*	pSocket = NULL; //socket
	while (1)
	{
		//监听完成端口,有数据才会往下运行
		bRet = GetQueuedCompletionStatus(pUdpIocp->m_hServerIOCPort,&nbytes,
										&WorkIndex,&pOvl,INFINITE);
		//有信息判断是否正确
		if(bRet || pOvl)
		{
			if(bRet) //如果信息返回正确
			{
				//检查是否超过最大处理数
				if( WorkIndex > (DWORD)pUdpIocp->m_ServerSocketSize )
					goto loop_pass; //继续

				//得到要处理的Socket
				pSocket = (CUdpSocket*)pUdpIocp->m_pSocketArray[WorkIndex];
				//检查pSocket是否为空
				if( !pSocket )
					goto loop_pass; //继续
				//判断是哪个消息
				switch( pOvl->Offset )
				{
				case OVL_RECEIVE: //接收
					//pUdpIocp->Recv();
					//检查是否出错
					if( !nbytes ) 
					{
						//pSocket->Close(); //关闭
						pSocket->Recv(); //继续接收
						//pUdpIocp->PushSocket( pSocket );
						goto loop_pass; //继续
					}
					//恢复错误计数器
					pSocket->m_nPending = 0;
					pSocket->m_nWouldblock = 0;
					pSocket->ReceivedData( (int)nbytes ); //处理接收的数据
					pSocket->Recv(); //继续接收
					//pUdpIocp->PushSocket( pSocket );
					break;
				case OVL_SEND: //发送
					//恢复错误计数器
					pSocket->m_nPending = 0;
					pSocket->m_nWouldblock = 0;
					break;
				case OVL_CLOSE: //关闭
					//pSocket->Close(); //关闭
					break;
				default:
					break;
				}
			}
			else
			{
				if( pOvl ) //关闭
				{
					//检查是否超过最大处理数
					if( WorkIndex > (DWORD)pUdpIocp->m_ServerSocketSize )
						goto loop_pass; //继续
					//得到要处理的Socket
					pSocket = (CUdpSocket*)pUdpIocp->m_pSocketArray[WorkIndex];
					//检查pSocket是否为空
					if( !pSocket )
						goto loop_pass; //继续
					//判断是哪个消息
					switch( pOvl->Offset )
					{
					case OVL_RECEIVE: //接收
						//pUdpIocp->Recv();
						//检查是否出错
						if( !nbytes ) 
						{
							pSocket->Recv(); //继续接收
							//pUdpIocp->PushSocket( pSocket );
							goto loop_pass; //继续
						}
						//恢复错误计数器
						pSocket->m_nPending = 0;
						pSocket->m_nWouldblock = 0;
						//pSocket->ReceivedData( (int)nbytes ); //处理接收的数据
						pSocket->Recv(); //继续接收
						//pUdpIocp->PushSocket( pSocket );
						break;
					case OVL_SEND: //发送
						//恢复错误计数器
						pSocket->m_nPending = 0;
						pSocket->m_nWouldblock = 0;
						break;
					case OVL_CLOSE: //关闭
						//pSocket->Close(); //关闭
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			OutInfo( "ERROR:GetQueuedCompletionStatus()" );
		}
loop_pass:
		continue; //继续循环
	}
	return 1; //返回1
}