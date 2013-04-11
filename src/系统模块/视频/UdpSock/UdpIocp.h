#pragma once
#include "..\Include\SocketBase.h"
#include "..\Include\Common.h"

#include <Queue>
#include <vector>
using namespace std;
typedef vector<HANDLE> HANDLE_VECTOR;

//封包标志
#define PACKET_START1		0XAA
#define PACKET_START2		0XBB
#define PACKET_END1			0XBB
#define PACKET_END2			0XAA

#pragma pack(1)
typedef struct _CHECK_SUM
{
	WORD Head;
	WORD Length;
	char Data;
}CHECK_SUM, *PCHECK_SUM;
#pragma pack()

class CUdpSocket : public CSocketBase
{
protected:
	int m_Sid;
	sockaddr_in m_FromAddr;

	WSAOVERLAPPED m_RecvOverlapped; //接收事件结构
	WSAOVERLAPPED m_SendOverlapped; //发送事件结构
public:
	CUdpSocket(void);
	virtual ~CUdpSocket(void);

	virtual int Create( int type, int protocol, int mode=0 );
	// 创建Socket
	int Create( int id=0 );

	int GetSocketID() { return m_Sid; }; //得到ID

	virtual void Init( LPVOID p ){}; //初始化
	virtual void ReceivedData( int length ); //接收数据处理
	virtual BOOL CheckSum( int length ); //检测封包
	virtual int Recv(); //接收
	virtual int Send( char* pBuf, long length, LPCTSTR lpTo=NULL, unsigned int Port=0, int dwFlag=0  ); //发送
};

typedef queue<CUdpSocket*> QUEUE_RECV;
class CUdpIocp
{
protected:
	DWORD m_dwNumberOfWorkers; //工作线程数
	DWORD m_dwConcurrency; //cpu数
	HANDLE_VECTOR m_ThreadHandleVector;
	QUEUE_RECV m_QueueRecv;
public:
	HANDLE m_hServerIOCPort; //外部完成端口句柄
	int m_ServerSocketSize; //数量
	CUdpSocket **m_pSocketArray; //动态Socket数组指针
public:
	CUdpIocp(void);
	virtual ~CUdpIocp(void);
	// 初始化
	int Init( int ServerSocketSize=1, int WorkerNum=0 );
	void Release();

	//初始化socket
	template <typename T>
	void CreateSocket( int port )
	{
		for( int i=0;i<m_ServerSocketSize;i++ )
		{
			m_pSocketArray[i] = (CUdpSocket *)new T();
			if( m_pSocketArray[i] )
			{
				m_pSocketArray[i]->Init( this );
				m_pSocketArray[i]->Create( i );
				m_pSocketArray[i]->Listen( port );
				Associate( m_pSocketArray[i], m_hServerIOCPort );
				//PushSocket( m_pSocketArray[i] );
				m_pSocketArray[i]->Recv();
			}
		}
		//Recv();
	};

	//完成端口和ID绑定
	bool Associate(CUdpSocket* pSocket, HANDLE hPort);
	//压入接收
	void PushSocket( CUdpSocket* p );
	//发送
	void Send( char* Buf, int len, char* IP, unsigned int Port );
	CUdpSocket* GetActiveSocket();
	CUdpSocket* GetSocket( int i );
	// 接收
	void Recv(void);
	// 创建接收线程
	void CreateReceiveWorkerThread( int WorkerNum=0 );
};