#pragma once

#include <winsock2.h>
#pragma comment( lib, "ws2_32.lib" )

//封包大小
#define SOCKET_BUFF_SIZE	(1024*16) //Socket大小
#define MAX_PACKET_SIZE		(1024*8) //封包大小

//socket基础类
class CSocketBase
{
protected:
	SOCKET m_Socket;
	int m_Port;
	int m_SocketType;
	char m_pRecvBuff[SOCKET_BUFF_SIZE]; //接收缓存
	char m_pSendBuff[MAX_PACKET_SIZE]; //发送缓存

public:
	LPWSAOVERLAPPED pRecvOvl;
	LPWSAOVERLAPPED pSendOvl;

	//错误记数器
	short m_nSocketErr;
	short m_nPending;
	short m_nWouldblock;

public:
	CSocketBase(void);
	virtual ~CSocketBase(void);

	HANDLE GetSocketHandle() { return (HANDLE)m_Socket; };

	int GetPort(){ return m_Port; };
	// 创建Socket
	virtual int Create( int type, int protocol=0, int mode=0 );
	// Socket
	void Set( SOCKET s, int type );
	// 释放
	void Close(void);
	// 绑定端口
	int Bind( int port );
	// 监听端口
	int Listen( int port );
	//接受连接
	CSocketBase* Accept( sockaddr * paddr, int * plen );
	//连接
	int Connect( LPCTSTR lpszHostAddress, unsigned int nHostPort );
	//接收
	virtual int Recv( sockaddr* lpFrom=NULL );
	//发送
	virtual int Send( char* pBuf, long length, LPCTSTR lpTo=NULL, unsigned int Port=0, int dwFlag=0  );
};
