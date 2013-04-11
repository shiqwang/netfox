#include "StdAfx.h"
#include "SocketBase.h"
//#include <ws2tcpip.h>

CSocketBase::CSocketBase(void)
{
	pSendOvl = NULL;
	pRecvOvl = NULL;
	m_Socket = INVALID_SOCKET;
	m_SocketType = 0;
	m_nSocketErr = 0;
	m_nPending = 0;
	m_nWouldblock = 0;
	WSADATA WSAData;
	WSAStartup( MAKEWORD( 2, 2 ), &WSAData );
}

CSocketBase::~CSocketBase(void)
{
	Close();
	WSACleanup();
}

//创建Socket
int CSocketBase::Create( int type, int protocol, int mode )
{
	//初始化socket
	m_Socket = socket( AF_INET, type/*SOCK_DGRAM*/, protocol/*IPPROTO_UDP*/ );
	if( m_Socket == INVALID_SOCKET )
		return WSAGetLastError(); //返回错误代码
/*
	BOOL bop = true;
	if( setsockopt( m_Socket, IPPROTO_IP, IP_HDRINCL, (char*)&bop, sizeof(bop) ) == SOCKET_ERROR )
		return WSAGetLastError(); //返回错误代码
*/

	if( mode )
	{
		int iMode = 1;
		if( SOCKET_ERROR == ioctlsocket( m_Socket, FIONBIO, (u_long FAR*) &iMode ) )
		{
			return WSAGetLastError(); //返回错误代码
		}
	}

	return 0;
}

void CSocketBase::Set( SOCKET s, int type )
{
	if( s == INVALID_SOCKET )
		return;
	m_Socket = s;
	m_SocketType = type;
}

// 释放
void CSocketBase::Close(void)
{
	if( m_Socket!=INVALID_SOCKET )
	{
		shutdown( m_Socket, SD_BOTH );
		closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}
}

// 绑定端口
int CSocketBase::Bind( int port )
{
	if( m_Socket == INVALID_SOCKET )
		return -1;

	sockaddr_in addr; //地址结构
	//bing地址结构
	ZeroMemory( &addr, sizeof(addr) );
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	addr.sin_port			= htons(port);
	
	//允许多个socket 绑定端口
/*	int opt = 1; //操作符
	if( setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR )
	{
		return WSAGetLastError();
	}
*/
	//设置超时时间
//	linger lingerOpt; //延迟操作符
//	lingerOpt.l_onoff = 0;
//	lingerOpt.l_linger = 0;
//	setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerOpt, sizeof(lingerOpt));
	
	//bing端口
	if ( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		return WSAGetLastError(); //返回错误代码
	}

	return 0;
}

// 监听端口
int CSocketBase::Listen(int port)
{
	int Ret = Bind( port );
	if( Ret )
		return Ret;

	sockaddr addr;
	int len = sizeof(addr);
	ZeroMemory( &addr, sizeof(addr) );
	if( getsockname( m_Socket, &addr, &len ) )
//	if( getpeername( m_Socket, &addr, &len ) )
	{
		len = WSAGetLastError();
	}

	//htons
	m_Port = htons( ((sockaddr_in*)&addr)->sin_port );

	int socklen, err;
	//设置接收缓存
	socklen = SOCKET_BUFF_SIZE*4;
	setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&socklen, sizeof(socklen));
	//检查接收缓存
	len = sizeof(socklen);
	err = getsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&socklen, &len);
	if (err == SOCKET_ERROR)
	{
		return FALSE;
	}
	//设置发送缓存
	socklen = SOCKET_BUFF_SIZE*4;
	setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&socklen, sizeof(socklen));
	//检查发送缓存
	len = sizeof(socklen);
	err = getsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&socklen, &len);
	if (err == SOCKET_ERROR)
	{
		return FALSE;
	}

	if( m_SocketType == SOCK_STREAM )
	{
		//如果是TCP就监听
		Ret = listen( m_Socket, 5 );
		if( Ret == SOCKET_ERROR )
		{
			Ret = WSAGetLastError();
			//Close();
		}
	}

	return Ret;
}

//接受连接
CSocketBase* CSocketBase::Accept( sockaddr * paddr, int * plen )
{
	if( m_Socket == INVALID_SOCKET )
		return NULL;

	//接受连接
	ZeroMemory( paddr, sizeof(sockaddr) );
	*plen = sizeof(sockaddr);
	SOCKET s = accept( m_Socket, paddr, plen );
	if( s!=INVALID_SOCKET )
	{
		CSocketBase* pSocketBase = new CSocketBase;
		pSocketBase->Set( s, SOCK_STREAM );
		return pSocketBase;
	}
	return NULL;
}

//连接
int CSocketBase::Connect( LPCTSTR lpszHostAddress, unsigned int nHostPort )
{
	if( m_Socket == INVALID_SOCKET )
		return -1;

	sockaddr_in addr; //地址结构
	//初始化地址
	ZeroMemory( &addr, sizeof(addr) );
	addr.sin_family = AF_INET;
#ifdef _UNICODE
	USES_CONVERSION;
	addr.sin_addr.s_addr = inet_addr(W2A(lpszHostAddress));
#else
	addr.sin_addr.s_addr = inet_addr(lpszHostAddress);
#endif
	addr.sin_port = htons(nHostPort);

	//连接
	int Ret = connect( m_Socket,(struct sockaddr *)&addr,sizeof(addr) );
	//检查返回值
	if ( Ret == SOCKET_ERROR )
	{
		Ret = WSAGetLastError();
		Close();
		return FALSE;
	}

	return Ret;
}

//接收数据
int CSocketBase::Recv( sockaddr* lpFrom )
{
	int Ret; //返回值
	sockaddr addr;
	int len = 0;

	if( m_Socket == INVALID_SOCKET )
		return -1;

	//清空缓存
	ZeroMemory( m_pRecvBuff, MAX_PACKET_SIZE );
		
	WSABUF Buf; //接收缓存
	Buf.len = MAX_PACKET_SIZE; //缓存大小
	Buf.buf = m_pRecvBuff; //缓存

	DWORD dwRecvSize=0, dwFlag=0;

	//接收
	if( m_SocketType == SOCK_STREAM )
	{
		Ret = WSARecv( m_Socket, &Buf, 1, &dwRecvSize, &dwFlag, pRecvOvl, NULL );
	}
	else
	{
		if( lpFrom==NULL )
		{
			lpFrom = &addr;
		}

		len = sizeof(addr);
RECV_LOOP:
		Ret = WSARecvFrom( m_Socket, &Buf, 1, &dwRecvSize, &dwFlag, lpFrom, &len, pRecvOvl, NULL );
	}

	//判断返回值
 	if ( Ret == SOCKET_ERROR )
	{
		Ret = WSAGetLastError(); //得到错误
		switch( Ret )
		{
		case WSAECONNRESET:
			goto RECV_LOOP;
			break;
		case WSA_IO_PENDING:
			//累计错误
			m_nPending++; 
			//大于3次结束连接
			//if( m_nPending > 3 )
				//Close();
			break;
		case WSAEWOULDBLOCK:
			//累计错误
			m_nWouldblock++;
			//大于3次结束连接
			//if( m_nWouldblock > 3 )
				//Close();
			break;
		default:
			//累计错误
			m_nSocketErr++;
			//大于3次结束连接
			//if( m_nSocketErr == 2 )
				//Close();
			break;
		}
	}

	return Ret;
}

//发送数据
int CSocketBase::Send( char* pBuf, long length, LPCTSTR lpTo, unsigned int Port, int dwFlag )
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
	//ZeroMemory( pTBuf, MAX_PACKET_SIZE );
	memcpy( pTBuf, pBuf, length );

	Buf.buf = pTBuf;
	Buf.len = length;

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