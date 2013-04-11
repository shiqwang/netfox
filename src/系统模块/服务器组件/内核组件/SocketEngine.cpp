#include "StdAfx.h"
#include "EventService.h"
#include "SocketEngine.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define TIME_BREAK_READY			9000L								//中断时间
#define TIME_BREAK_CONNECT			4000L								//中断时间
#define TIME_DETECT_SOCKET			20000L								//监测时间

//////////////////////////////////////////////////////////////////////////

//动作定义
#define QUEUE_SEND_REQUEST			1									//发送标识
#define QUEUE_SAFE_CLOSE			2									//安全关闭
#define QUEUE_ALLOW_BATCH			3									//允许群发
#define QUEUE_DETECT_SOCKET			4									//检测连接

//发送请求结构
struct tagSendDataRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
	WORD							wDataSize;							//数据大小
	BYTE							cbSendBuf[SOCKET_PACKAGE];			//发送缓冲
};

//设置群发
struct tagAllowBatchSend
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
	BYTE							cbAllow;							//允许标志
};

//安全关闭
struct tagSafeCloseSocket
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
};

//////////////////////////////////////////////////////////////////////////

//发送字节映射表
const BYTE g_SendByteMap[256]=				
{
	0x70,0x2F,0x40,0x5F,0x44,0x8E,0x6E,0x45,0x7E,0xAB,0x2C,0x1F,0xB4,0xAC,0x9D,0x91,
	0x0D,0x36,0x9B,0x0B,0xD4,0xC4,0x39,0x74,0xBF,0x23,0x16,0x14,0x06,0xEB,0x04,0x3E,
	0x12,0x5C,0x8B,0xBC,0x61,0x63,0xF6,0xA5,0xE1,0x65,0xD8,0xF5,0x5A,0x07,0xF0,0x13,
	0xF2,0x20,0x6B,0x4A,0x24,0x59,0x89,0x64,0xD7,0x42,0x6A,0x5E,0x3D,0x0A,0x77,0xE0,
	0x80,0x27,0xB8,0xC5,0x8C,0x0E,0xFA,0x8A,0xD5,0x29,0x56,0x57,0x6C,0x53,0x67,0x41,
	0xE8,0x00,0x1A,0xCE,0x86,0x83,0xB0,0x22,0x28,0x4D,0x3F,0x26,0x46,0x4F,0x6F,0x2B,
	0x72,0x3A,0xF1,0x8D,0x97,0x95,0x49,0x84,0xE5,0xE3,0x79,0x8F,0x51,0x10,0xA8,0x82,
	0xC6,0xDD,0xFF,0xFC,0xE4,0xCF,0xB3,0x09,0x5D,0xEA,0x9C,0x34,0xF9,0x17,0x9F,0xDA,
	0x87,0xF8,0x15,0x05,0x3C,0xD3,0xA4,0x85,0x2E,0xFB,0xEE,0x47,0x3B,0xEF,0x37,0x7F,
	0x93,0xAF,0x69,0x0C,0x71,0x31,0xDE,0x21,0x75,0xA0,0xAA,0xBA,0x7C,0x38,0x02,0xB7,
	0x81,0x01,0xFD,0xE7,0x1D,0xCC,0xCD,0xBD,0x1B,0x7A,0x2A,0xAD,0x66,0xBE,0x55,0x33,
	0x03,0xDB,0x88,0xB2,0x1E,0x4E,0xB9,0xE6,0xC2,0xF7,0xCB,0x7D,0xC9,0x62,0xC3,0xA6,
	0xDC,0xA7,0x50,0xB5,0x4B,0x94,0xC0,0x92,0x4C,0x11,0x5B,0x78,0xD9,0xB1,0xED,0x19,
	0xE9,0xA1,0x1C,0xB6,0x32,0x99,0xA3,0x76,0x9E,0x7B,0x6D,0x9A,0x30,0xD6,0xA9,0x25,
	0xC7,0xAE,0x96,0x35,0xD0,0xBB,0xD2,0xC8,0xA2,0x08,0xF3,0xD1,0x73,0xF4,0x48,0x2D,
	0x90,0xCA,0xE2,0x58,0xC1,0x18,0x52,0xFE,0xDF,0x68,0x98,0x54,0xEC,0x60,0x43,0x0F
};

//接收字节映射表
const BYTE g_RecvByteMap[256]=				
{
	0x51,0xA1,0x9E,0xB0,0x1E,0x83,0x1C,0x2D,0xE9,0x77,0x3D,0x13,0x93,0x10,0x45,0xFF,
	0x6D,0xC9,0x20,0x2F,0x1B,0x82,0x1A,0x7D,0xF5,0xCF,0x52,0xA8,0xD2,0xA4,0xB4,0x0B,
	0x31,0x97,0x57,0x19,0x34,0xDF,0x5B,0x41,0x58,0x49,0xAA,0x5F,0x0A,0xEF,0x88,0x01,
	0xDC,0x95,0xD4,0xAF,0x7B,0xE3,0x11,0x8E,0x9D,0x16,0x61,0x8C,0x84,0x3C,0x1F,0x5A,
	0x02,0x4F,0x39,0xFE,0x04,0x07,0x5C,0x8B,0xEE,0x66,0x33,0xC4,0xC8,0x59,0xB5,0x5D,
	0xC2,0x6C,0xF6,0x4D,0xFB,0xAE,0x4A,0x4B,0xF3,0x35,0x2C,0xCA,0x21,0x78,0x3B,0x03,
	0xFD,0x24,0xBD,0x25,0x37,0x29,0xAC,0x4E,0xF9,0x92,0x3A,0x32,0x4C,0xDA,0x06,0x5E,
	0x00,0x94,0x60,0xEC,0x17,0x98,0xD7,0x3E,0xCB,0x6A,0xA9,0xD9,0x9C,0xBB,0x08,0x8F,
	0x40,0xA0,0x6F,0x55,0x67,0x87,0x54,0x80,0xB2,0x36,0x47,0x22,0x44,0x63,0x05,0x6B,
	0xF0,0x0F,0xC7,0x90,0xC5,0x65,0xE2,0x64,0xFA,0xD5,0xDB,0x12,0x7A,0x0E,0xD8,0x7E,
	0x99,0xD1,0xE8,0xD6,0x86,0x27,0xBF,0xC1,0x6E,0xDE,0x9A,0x09,0x0D,0xAB,0xE1,0x91,
	0x56,0xCD,0xB3,0x76,0x0C,0xC3,0xD3,0x9F,0x42,0xB6,0x9B,0xE5,0x23,0xA7,0xAD,0x18,
	0xC6,0xF4,0xB8,0xBE,0x15,0x43,0x70,0xE0,0xE7,0xBC,0xF1,0xBA,0xA5,0xA6,0x53,0x75,
	0xE4,0xEB,0xE6,0x85,0x14,0x48,0xDD,0x38,0x2A,0xCC,0x7F,0xB1,0xC0,0x71,0x96,0xF8,
	0x3F,0x28,0xF2,0x69,0x74,0x68,0xB7,0xA3,0x50,0xD0,0x79,0x1D,0xFC,0xCE,0x8A,0x8D,
	0x2E,0x62,0x30,0xEA,0xED,0x2B,0x26,0xB9,0x81,0x7C,0x46,0x89,0x73,0xA2,0xF7,0x72
};

//数据加密密钥
const static DWORD g_dwPacketKey=0xA55AA55A;

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	memset(&m_WSABuffer,0,sizeof(m_WSABuffer));
	memset(&m_OverLapped,0,sizeof(m_OverLapped));
}

//析构函数
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedSend::COverLappedSend() : COverLapped(OperationType_Send)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=(char *)m_cbBuffer;
}

//析构函数
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerSocketItem::CServerSocketItem(WORD wIndex, IServerSocketItemSink * pIServerSocketItemSink) 
	: m_wIndex(wIndex), m_pIServerSocketItemSink(pIServerSocketItemSink)
{
	m_wRountID=0;
	m_wRecvSize=0;
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_bNotify=true;
	m_bRecvIng=false;
	m_bCloseIng=false;
	m_bAllowBatch=false;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;
	m_dwClientAddr=0;
	m_dwConnectTime=0;
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_dwSendPacketCount=0;
	m_dwRecvPacketCount=0;
	m_hSocket=INVALID_SOCKET;
}

//析够函数
CServerSocketItem::~CServerSocketItem(void)
{
	//删除空闲重叠 IO
	INT_PTR iCount=m_OverLappedSendFree.GetCount();
	for (INT_PTR i=0;i<iCount;i++) delete m_OverLappedSendFree[i];
	m_OverLappedSendFree.RemoveAll();

	//删除活动重叠 IO
	iCount=m_OverLappedSendActive.GetCount();
	for (INT_PTR i=0;i<iCount;i++) delete m_OverLappedSendActive[i];
	m_OverLappedSendActive.RemoveAll();

	return;
}

//随机映射
WORD CServerSocketItem::SeedRandMap(WORD wSeed)
{
	DWORD dwHold=wSeed;
	return (WORD)((dwHold=dwHold*241103L+2533101L)>>16);
}

//映射发送数据
BYTE CServerSocketItem::MapSendByte(BYTE const cbData)
{
	BYTE cbMap=g_SendByteMap[(BYTE)(cbData+m_cbSendRound)];
	m_cbSendRound+=3;
	return cbMap;
}

//映射接收数据
BYTE CServerSocketItem::MapRecvByte(BYTE const cbData)
{
	BYTE cbMap=g_RecvByteMap[cbData]-m_cbRecvRound;
	m_cbRecvRound+=3;
	return cbMap;
}

//获取发送结构
COverLappedSend * CServerSocketItem::GetSendOverLapped()
{
	//寻找空闲结构
	COverLappedSend * pOverLappedSend=NULL;
	INT_PTR nFreeCount=m_OverLappedSendFree.GetCount();
	if (nFreeCount>0)
	{
		pOverLappedSend=m_OverLappedSendFree[nFreeCount-1];
		ASSERT(pOverLappedSend!=NULL);
		m_OverLappedSendFree.RemoveAt(nFreeCount-1);
		m_OverLappedSendActive.Add(pOverLappedSend);
		return pOverLappedSend;
	}

	//新建发送结构
	try
	{
		pOverLappedSend=new COverLappedSend;
		ASSERT(pOverLappedSend!=NULL);
		m_OverLappedSendActive.Add(pOverLappedSend);
		return pOverLappedSend;
	}
	catch (...) { }
	return NULL;
}

//绑定对象
DWORD CServerSocketItem::Attach(SOCKET hSocket, DWORD dwClientAddr)
{
	//效验数据
	ASSERT(dwClientAddr!=0);
	ASSERT(m_bRecvIng==false);
	ASSERT(IsValidSocket()==false);
	ASSERT(hSocket!=INVALID_SOCKET);

	//设置变量
	m_bNotify=false;
	m_bRecvIng=false;
	m_bCloseIng=false;
	m_hSocket=hSocket;
	m_dwClientAddr=dwClientAddr;
	m_dwRecvTickCount=GetTickCount();
	m_dwConnectTime=(DWORD)time(NULL);

	//发送通知
	m_pIServerSocketItemSink->OnSocketAcceptEvent(this);

	return GetIdentifierID();
}

//发送函数
bool CServerSocketItem::SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验状态
	if (m_bCloseIng==true) return false;
	if (m_wRountID!=wRountID) return false;
	if (m_dwRecvPacketCount==0) return false;
	if (IsValidSocket()==false) return false;

	//寻找发送结构
	COverLappedSend * pOverLappedSend=GetSendOverLapped();
	ASSERT(pOverLappedSend!=NULL);
	if (pOverLappedSend==NULL) return false;

	//构造数据
	CMD_Head * pHead=(CMD_Head *)pOverLappedSend->m_cbBuffer;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	WORD wSendSize=EncryptBuffer(pOverLappedSend->m_cbBuffer,sizeof(CMD_Head),sizeof(pOverLappedSend->m_cbBuffer));
	pOverLappedSend->m_WSABuffer.len=wSendSize;

	//发送数据
	if (m_OverLappedSendActive.GetCount()==1)
	{
		DWORD dwThancferred=0;
		int iRetCode=WSASend(m_hSocket,&pOverLappedSend->m_WSABuffer,1,&dwThancferred,0,&pOverLappedSend->m_OverLapped,NULL);
		if ((iRetCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
		{
			OnSendCompleted(pOverLappedSend,0L);
			return false;
		}
	}

	return true;
}

//发送函数
bool CServerSocketItem::SendData(void * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验参数
	ASSERT(wDataSize<=SOCKET_PACKAGE);

	//效验状态
	if (m_bCloseIng==true) return false;
	if (m_wRountID!=wRountID) return false;
	if (m_dwRecvPacketCount==0) return false;
	if (IsValidSocket()==false) return false;
	if (wDataSize>SOCKET_PACKAGE) return false;

	//寻找发送结构
	COverLappedSend * pOverLappedSend=GetSendOverLapped();
	ASSERT(pOverLappedSend!=NULL);
	if (pOverLappedSend==NULL) return false;

	//构造数据
	CMD_Head * pHead=(CMD_Head *)pOverLappedSend->m_cbBuffer;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		memcpy(pHead+1,pData,wDataSize);
	}
	WORD wSendSize=EncryptBuffer(pOverLappedSend->m_cbBuffer,sizeof(CMD_Head)+wDataSize,sizeof(pOverLappedSend->m_cbBuffer));
	pOverLappedSend->m_WSABuffer.len=wSendSize;

	//发送数据
	if (m_OverLappedSendActive.GetCount()==1)
	{
		DWORD dwThancferred=0;
		int iRetCode=WSASend(m_hSocket,&pOverLappedSend->m_WSABuffer,1,&dwThancferred,0,&pOverLappedSend->m_OverLapped,NULL);
		if ((iRetCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
		{
			OnSendCompleted(pOverLappedSend,0L);
			return false;
		}
	}

	return true;
}

//投递接收
bool CServerSocketItem::RecvData()
{
	//效验变量
	ASSERT(m_bRecvIng==false);
	ASSERT(m_hSocket!=INVALID_SOCKET);

	//判断关闭
	if (m_bCloseIng==true)
	{
		if (m_OverLappedSendActive.GetCount()==0) CloseSocket(m_wRountID);
		return false;
	}

	//接收数据
	m_bRecvIng=true;
	DWORD dwThancferred=0,dwFlags=0;
	int iRetCode=WSARecv(m_hSocket,&m_OverLappedRecv.m_WSABuffer,1,&dwThancferred,&dwFlags,&m_OverLappedRecv.m_OverLapped,NULL);
	if ((iRetCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
	{
		m_bRecvIng=false;
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

//关闭连接
bool CServerSocketItem::CloseSocket(WORD wRountID)
{
	//状态判断
	if (m_wRountID!=wRountID) return false;

	//关闭连接
	if (m_hSocket!=INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
	}

	//判断关闭
	if ((m_bRecvIng==false)&&(m_OverLappedSendActive.GetCount()==0)) OnCloseCompleted();

	return true;
}

//设置关闭
bool CServerSocketItem::ShutDownSocket(WORD wRountID)
{
	return true;

	//状态判断
	if (m_wRountID!=wRountID) return false;
	if (m_hSocket==INVALID_SOCKET) return false;

	//设置变量
	if (m_bCloseIng==false)
	{
		m_bCloseIng=true;
		if (m_OverLappedSendActive.GetCount()==0) CloseSocket(wRountID);
	}

	return true;
}

//允许群发
bool CServerSocketItem::AllowBatchSend(WORD wRountID, bool bAllowBatch)
{
	//状态判断
	if (m_wRountID!=wRountID) return false;
	if (m_hSocket==INVALID_SOCKET) return false;

	//设置变量
	m_bAllowBatch=bAllowBatch;

	return true;
}

//重置变量
void CServerSocketItem::ResetSocketData()
{
	//效验状态
	ASSERT(m_hSocket==INVALID_SOCKET);

	//重置数据
	m_wRountID++;
	m_wRecvSize=0;
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;
	m_dwClientAddr=0;
	m_dwConnectTime=0;
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_dwSendPacketCount=0;
	m_dwRecvPacketCount=0;

	//状态变量
	m_bNotify=true;
	m_bRecvIng=false;
	m_bCloseIng=false;
	m_bAllowBatch=false;
	m_OverLappedSendFree.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();

	return;
}

//发送完成函数
bool CServerSocketItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//效验变量
	ASSERT(pOverLappedSend!=NULL);
	ASSERT(m_OverLappedSendActive.GetCount()>0);
	ASSERT(pOverLappedSend==m_OverLappedSendActive[0]);

	//释放发送结构
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendFree.Add(pOverLappedSend);

	//设置变量
	if (dwThancferred!=0) m_dwSendTickCount=GetTickCount();

	//判断关闭
	if (m_hSocket==INVALID_SOCKET)
	{
		m_OverLappedSendFree.Append(m_OverLappedSendActive);
		m_OverLappedSendActive.RemoveAll();
		CloseSocket(m_wRountID);
		return true;
	}

	//继续发送数据
	if (m_OverLappedSendActive.GetCount()>0)
	{
		DWORD dwThancferred=0;
		pOverLappedSend=m_OverLappedSendActive[0];
		ASSERT(pOverLappedSend!=NULL);
		int iRetCode=WSASend(m_hSocket,&pOverLappedSend->m_WSABuffer,1,&dwThancferred,0,&pOverLappedSend->m_OverLapped,NULL);
		if ((iRetCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
		{
			m_OverLappedSendFree.Append(m_OverLappedSendActive);
			m_OverLappedSendActive.RemoveAll();
			CloseSocket(m_wRountID);
			return false;
		}
		return true;
	}

	//判断关闭
	if (m_bCloseIng==true) 
		CloseSocket(m_wRountID);

	return true;
}

//接收完成函数
bool CServerSocketItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//效验数据
	ASSERT(m_bRecvIng==true);

	//设置变量
	m_bRecvIng=false;
	m_dwRecvTickCount=GetTickCount();

	//判断关闭
	if (m_hSocket==INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//接收数据
	int iRetCode=recv(m_hSocket,(char *)m_cbRecvBuf+m_wRecvSize,sizeof(m_cbRecvBuf)-m_wRecvSize,0);
	if (iRetCode<=0)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//接收完成
	m_wRecvSize+=iRetCode;
	BYTE cbBuffer[SOCKET_BUFFER];
	CMD_Head * pHead=(CMD_Head *)m_cbRecvBuf;

	//处理数据
	try
	{
		while (m_wRecvSize>=sizeof(CMD_Head))
		{
			//效验数据
			WORD wPacketSize=pHead->CmdInfo.wDataSize;
			if (wPacketSize>SOCKET_BUFFER) throw TEXT("数据包超长");
			if (wPacketSize<sizeof(CMD_Head)) throw TEXT("数据包非法");
			if (pHead->CmdInfo.cbMessageVer!=SOCKET_VER) throw TEXT("数据包版本错误");
			if (m_wRecvSize<wPacketSize) break;

			//提取数据
			CopyMemory(cbBuffer,m_cbRecvBuf,wPacketSize);
			WORD wRealySize=CrevasseBuffer(cbBuffer,wPacketSize);
			ASSERT(wRealySize>=sizeof(CMD_Head));
			m_dwRecvPacketCount++;

			//解释数据
			WORD wDataSize=wRealySize-sizeof(CMD_Head);
			void * pDataBuffer=cbBuffer+sizeof(CMD_Head);
			CMD_Command Command=((CMD_Head *)cbBuffer)->CommandInfo;

			//内核命令
			if (Command.wMainCmdID==MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
				case SUB_KN_DETECT_SOCKET:	//网络检测
					{
						break;
					}
				default: throw TEXT("非法命令码");
				}
			}
			else 
			{
				//消息处理
				m_pIServerSocketItemSink->OnSocketReadEvent(Command,pDataBuffer,wDataSize,this);			
			}

			//删除缓存数据
			m_wRecvSize-=wPacketSize;
			MoveMemory(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);
		}
	}
	catch (...)
	{ 
		CloseSocket(m_wRountID);
		return false;
	}

	return RecvData();
}

//关闭完成通知
bool CServerSocketItem::OnCloseCompleted()
{
	//效验状态
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_OverLappedSendActive.GetCount()==0);

	//关闭事件
	ASSERT(m_bNotify==false);
	if (m_bNotify==false)
	{
		m_bNotify=true;
		m_pIServerSocketItemSink->OnSocketCloseEvent(this);
	}

	//状态变量
	ResetSocketData();

	return true;
}

//加密数据
WORD CServerSocketItem::EncryptBuffer(BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize)
{
	WORD i = 0;
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_Head));
	ASSERT(wDataSize<=(sizeof(CMD_Head)+SOCKET_PACKAGE));
	ASSERT(wBufferSize>=(wDataSize+2*sizeof(DWORD)));

	//调整长度
	WORD wEncryptSize=wDataSize-sizeof(CMD_Info),wSnapCount=0;
	if ((wEncryptSize%sizeof(DWORD))!=0)
	{
		wSnapCount=sizeof(DWORD)-wEncryptSize%sizeof(DWORD);
		memset(pcbDataBuffer+sizeof(CMD_Info)+wEncryptSize,0,wSnapCount);
	}

	//效验码与字节映射
	BYTE cbCheckCode=0;
	for (i=sizeof(CMD_Info);i<wDataSize;i++) 
	{
		cbCheckCode+=pcbDataBuffer[i];
		pcbDataBuffer[i]=MapSendByte(pcbDataBuffer[i]);
	}

	//填写信息头
	CMD_Head * pHead=(CMD_Head *)pcbDataBuffer;
	pHead->CmdInfo.cbCheckCode=~cbCheckCode+1;
	pHead->CmdInfo.wDataSize=wDataSize;
	pHead->CmdInfo.cbMessageVer=SOCKET_VER;

	//加密数据
	DWORD dwXorKey=m_dwSendXorKey;
	WORD * pwSeed=(WORD *)(pcbDataBuffer+sizeof(CMD_Info));
	DWORD * pdwXor=(DWORD *)(pcbDataBuffer+sizeof(CMD_Info));
	WORD wEncrypCount=(wEncryptSize+wSnapCount)/sizeof(DWORD);
	for (i=0;i<wEncrypCount;i++)
	{
		*pdwXor++^=dwXorKey;
		dwXorKey=SeedRandMap(*pwSeed++);
		dwXorKey|=((DWORD)SeedRandMap(*pwSeed++))<<16; 
		dwXorKey^=g_dwPacketKey;
	}

	//设置变量
	m_dwSendPacketCount++;
	m_dwSendXorKey=dwXorKey;

	return wDataSize;
}

//解密数据
WORD CServerSocketItem::CrevasseBuffer(BYTE pcbDataBuffer[], WORD wDataSize)
{
	WORD i = 0;
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_Head));
	ASSERT(((CMD_Head *)pcbDataBuffer)->CmdInfo.wDataSize==wDataSize);

	//调整长度
	WORD wSnapCount=0;
	if ((wDataSize%sizeof(DWORD))!=0)
	{
		wSnapCount=sizeof(DWORD)-wDataSize%sizeof(DWORD);
		memset(pcbDataBuffer+wDataSize,0,wSnapCount);
	}

	//提取密钥
	if (m_dwRecvPacketCount==0)
	{
		ASSERT(wDataSize>=(sizeof(CMD_Head)+sizeof(DWORD)));
		if (wDataSize<(sizeof(CMD_Head)+sizeof(DWORD))) throw TEXT("数据包解密长度错误");
		m_dwRecvXorKey=*(DWORD *)(pcbDataBuffer+sizeof(CMD_Head));
		m_dwSendXorKey=m_dwRecvXorKey;
		MoveMemory(pcbDataBuffer+sizeof(CMD_Head),pcbDataBuffer+sizeof(CMD_Head)+sizeof(DWORD),
			wDataSize-sizeof(CMD_Head)-sizeof(DWORD));
		wDataSize-=sizeof(DWORD);
		((CMD_Head *)pcbDataBuffer)->CmdInfo.wDataSize-=sizeof(DWORD);
	}

	//解密数据
	DWORD dwXorKey=m_dwRecvXorKey;
	DWORD * pdwXor=(DWORD *)(pcbDataBuffer+sizeof(CMD_Info));
	WORD  * pwSeed=(WORD *)(pcbDataBuffer+sizeof(CMD_Info));
	WORD wEncrypCount=(wDataSize+wSnapCount-sizeof(CMD_Info))/4;
	for (i=0;i<wEncrypCount;i++)
	{
		if ((i==(wEncrypCount-1))&&(wSnapCount>0))
		{
			BYTE * pcbKey=((BYTE *)&m_dwRecvXorKey)+sizeof(DWORD)-wSnapCount;
			CopyMemory(pcbDataBuffer+wDataSize,pcbKey,wSnapCount);
		}
		dwXorKey=SeedRandMap(*pwSeed++);
		dwXorKey|=((DWORD)SeedRandMap(*pwSeed++))<<16;
		dwXorKey^=g_dwPacketKey;
		*pdwXor++^=m_dwRecvXorKey;
		m_dwRecvXorKey=dwXorKey;
	}

	//效验码与字节映射
	CMD_Head * pHead=(CMD_Head *)pcbDataBuffer;
	BYTE cbCheckCode=pHead->CmdInfo.cbCheckCode;;
	for (i=sizeof(CMD_Info);i<wDataSize;i++)
	{
		pcbDataBuffer[i]=MapRecvByte(pcbDataBuffer[i]);
		cbCheckCode+=pcbDataBuffer[i];
	}
	if (cbCheckCode!=0) throw TEXT("数据包效验码错误");

	return wDataSize;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerSocketRSThread::CServerSocketRSThread(void)
{
	m_hCompletionPort=NULL;
}

//析构函数
CServerSocketRSThread::~CServerSocketRSThread(void)
{
}

//配置函数
bool CServerSocketRSThread::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort!=NULL);
	m_hCompletionPort=hCompletionPort;
	return true;
}

//运行函数
bool CServerSocketRSThread::RepetitionRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);

	//变量定义
	DWORD dwThancferred=0;					
	OVERLAPPED * pOverLapped=NULL;
	COverLapped * pSocketLapped=NULL;
	CServerSocketItem * pServerSocketItem=NULL;

	//等待完成端口
	BOOL bSuccess=GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pServerSocketItem,&pOverLapped,INFINITE);
	if ((bSuccess==FALSE)&&(GetLastError()!=ERROR_NETNAME_DELETED)) return false;
	if ((pServerSocketItem==NULL)&&(pOverLapped==NULL)) return false;

	//处理操作
	ASSERT(pOverLapped!=NULL);
	ASSERT(pServerSocketItem!=NULL);
	pSocketLapped=CONTAINING_RECORD(pOverLapped,COverLapped,m_OverLapped);
	switch (pSocketLapped->GetOperationType())
	{
	case OperationType_Recv:	//SOCKET 数据读取
		{
			COverLappedRecv * pOverLappedRecv=(COverLappedRecv *)pSocketLapped;
			CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnRecvCompleted(pOverLappedRecv,dwThancferred);
			break;
		}
	case OperationType_Send:	//SOCKET 数据发送
		{
			COverLappedSend * pOverLappedSend=(COverLappedSend *)pSocketLapped;
			CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnSendCompleted(pOverLappedSend,dwThancferred);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSocketAcceptThread::CSocketAcceptThread(void)
{
	m_hCompletionPort=NULL;
	m_pTCPSocketManager=NULL;
	m_hListenSocket=INVALID_SOCKET;
}

//析构函数
CSocketAcceptThread::~CSocketAcceptThread(void)
{
}

//配置函数
bool CSocketAcceptThread::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPSocketEngine * pTCPSocketManager)
{
	ASSERT(hCompletionPort!=NULL);
	ASSERT(pTCPSocketManager!=NULL);
	ASSERT(hListenSocket!=INVALID_SOCKET);
	m_hListenSocket=hListenSocket;
	m_hCompletionPort=hCompletionPort;
	m_pTCPSocketManager=pTCPSocketManager;
	return true;
}

//运行函数
bool CSocketAcceptThread::RepetitionRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);
	ASSERT(m_pTCPSocketManager!=NULL);

	//设置变量
	SOCKADDR_IN	SocketAddr;
	CServerSocketItem * pServerSocketItem=NULL;
	SOCKET hConnectSocket=INVALID_SOCKET;
	int nBufferSize=sizeof(SocketAddr);

	try
	{
		//监听连接
		hConnectSocket=WSAAccept(m_hListenSocket,(SOCKADDR *)&SocketAddr,&nBufferSize,NULL,NULL);
		if (hConnectSocket==INVALID_SOCKET) return false;

		//获取连接
		pServerSocketItem=m_pTCPSocketManager->ActiveSocketItem();
		if (pServerSocketItem==NULL) throw TEXT("申请连接对象失败");

		//激活对象
		CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
		pServerSocketItem->Attach(hConnectSocket,SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket,m_hCompletionPort,(ULONG_PTR)pServerSocketItem,0);
		pServerSocketItem->RecvData();
	}
	catch (...)
	{
		//清理对象
		ASSERT(pServerSocketItem==NULL);
		if (hConnectSocket!=INVALID_SOCKET)	closesocket(hConnectSocket);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSocketDetectThread::CSocketDetectThread(void)
{
	m_dwTickCount=0;;
	m_pTCPSocketManager=NULL;
}

//析构函数
CSocketDetectThread::~CSocketDetectThread(void)
{
}

//配置函数
bool CSocketDetectThread::InitThread(CTCPSocketEngine * pTCPSocketManager)
{
	//效验参数
	ASSERT(pTCPSocketManager!=NULL);

	//设置变量
	m_dwTickCount=0L;
	m_pTCPSocketManager=pTCPSocketManager;

	return true;
}

//运行函数
bool CSocketDetectThread::RepetitionRun()
{
	//效验参数
	ASSERT(m_pTCPSocketManager!=NULL);

	//设置间隔
	Sleep(500);
	m_dwTickCount+=500L;

	//检测连接
	if (m_dwTickCount>20000L)
	{
		m_dwTickCount=0L;
		m_pTCPSocketManager->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPSocketEngine::CTCPSocketEngine(void)
{
	m_bService=false;
	m_wListenPort=0;
	m_dwLastDetect=0;
	m_wMaxSocketItem=0;
	m_hCompletionPort=NULL;
	m_hServerSocket=INVALID_SOCKET;

	return;
}

//析构函数
CTCPSocketEngine::~CTCPSocketEngine(void)
{
	//停止服务
	StopService();

	//释放存储连接
	CServerSocketItem * pSocketItem=NULL;
	for (INT_PTR i=0;i<m_StorageSocketItem.GetCount();i++)
	{
		pSocketItem=m_StorageSocketItem[i];
		ASSERT(pSocketItem!=NULL);
		SafeDelete(pSocketItem);
	}
	m_StorageSocketItem.RemoveAll();

	return;
}

//接口查询
void * __cdecl CTCPSocketEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketEngine,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketEngineManager,Guid,dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketEngine,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CTCPSocketEngine::SetSocketEngineSink(IUnknownEx * pIUnknownEx)
{
	//状态判断
	if (m_bService==true) 
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎处于服务状态，绑定操作忽略"),Level_Exception);
		return false;
	}

	//设置接口
	if (m_AttemperEvent.SetQueueService(pIUnknownEx)==false)
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎与触发服务绑定失败"),Level_Exception);
		return false;
	}
	
	return true;
}

//设置端口
bool __cdecl CTCPSocketEngine::SetServicePort(WORD wListenPort)
{
	//效验状态
	if (m_bService==true) 
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎处于服务状态，端口绑定操作忽略"),Level_Exception);
		return false;
	}

	//判断参数
	if (wListenPort==0)
	{
		CEventTrace::ShowEventNotify(TEXT("网络端口错误，端口绑定操作失败"),Level_Exception);
		return false;
	}

	//设置变量
	m_wListenPort=wListenPort;

	return true;
}

//设置数目
bool __cdecl CTCPSocketEngine::SetMaxSocketItem(WORD wMaxSocketItem)
{
	m_wMaxSocketItem=wMaxSocketItem;
	return true;
}

//启动服务
bool __cdecl CTCPSocketEngine::StartService()
{
	DWORD i = 0;
	//效验状态
	if (m_bService==true) 
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎重复启动，启动操作忽略"),Level_Warning);
		return true;
	}

	//判断端口
	if (m_wListenPort==0)
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎监听端口无效"),Level_Exception);
		return false;
	}

	//绑定对象
	if (m_SendQueueService.SetQueueServiceSink(GET_MYSELF_INTERFACE(IUnknownEx))==false)
	{
		CEventTrace::ShowEventNotify(TEXT("网络引擎与触发服务绑定失败"),Level_Exception);
		return false;
	}

	try
	{
		//获取系统信息
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadCount=SystemInfo.dwNumberOfProcessors;

		//建立完成端口
		m_hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
		if (m_hCompletionPort==NULL) throw TEXT("网络引擎完成端口创建失败");

		//建立监听SOCKET
		struct sockaddr_in SocketAddr;
		memset(&SocketAddr,0,sizeof(SocketAddr));
		SocketAddr.sin_addr.s_addr=INADDR_ANY;
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(m_wListenPort);
		m_hServerSocket=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
		if (m_hServerSocket==INVALID_SOCKET) throw TEXT("网络引擎监听 SOCKET 创建失败");
		int iErrorCode=bind(m_hServerSocket,(SOCKADDR*)&SocketAddr,sizeof(SocketAddr));
		if (iErrorCode==SOCKET_ERROR) throw TEXT("网络引擎监听端口被占用，端口绑定失败");
		iErrorCode=listen(m_hServerSocket,200);
		if (iErrorCode==SOCKET_ERROR) throw TEXT("网络引擎监听端口被占用，端口监听失败");

		//启动发送队列
		bool bSuccess=m_SendQueueService.StartService();
		if (bSuccess==false) throw TEXT("网络引擎发送队列服务启动失败");

		//建立读写线程
		for (i=0;i<dwThreadCount;i++)
		{
			CServerSocketRSThread * pServerSocketRSThread=new CServerSocketRSThread();
			if (pServerSocketRSThread==NULL) throw TEXT("网络引擎读写线程服务创建失败");
			bSuccess=pServerSocketRSThread->InitThread(m_hCompletionPort);
			if (bSuccess==false) throw TEXT("网络引擎读写线程服务配置失败");
			m_SocketRSThreadArray.Add(pServerSocketRSThread);
		}

		//建立应答线程
		bSuccess=m_SocketAcceptThread.InitThread(m_hCompletionPort,m_hServerSocket,this);
		if (bSuccess==false) throw TEXT("网络引擎网络监听线程服务配置");

		//运行读写线程
		for (i=0;i<dwThreadCount;i++)
		{
			CServerSocketRSThread * pServerSocketRSThread=m_SocketRSThreadArray[i];
			ASSERT(pServerSocketRSThread!=NULL);
			bSuccess=pServerSocketRSThread->StartThead();
			if (bSuccess==false) throw TEXT("网络引擎读写线程服务启动失败");
		}

		//网络检测线程
		m_SocketDetectThread.InitThread(this);
		bSuccess=m_SocketDetectThread.StartThead();
		if (bSuccess==false) throw TEXT("网络引检测线程服务启动失败");

		//运行应答线程
		bSuccess=m_SocketAcceptThread.StartThead();
		if (bSuccess==false) throw TEXT("网络引擎监听线程服务启动失败");

		//设置变量
		m_bService=true;
	}
	catch (LPCTSTR pszError)
	{
		CEventTrace::ShowEventNotify(pszError,Level_Exception);
		return false;
	}

	return true;
}

//停止服务
bool __cdecl CTCPSocketEngine::StopService()
{
	//设置变量
	m_bService=false;
	m_dwLastDetect=0L;

	//停止检测线程
	m_SocketDetectThread.StopThread();

	//终止应答线程
	if (m_hServerSocket!=INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket=INVALID_SOCKET;
	}
	m_SocketAcceptThread.StopThread();

	//停止发送队列
	m_SendQueueService.StopService();

	//释放读写线程
	INT_PTR nCount=m_SocketRSThreadArray.GetCount(),i=0;
	if (m_hCompletionPort!=NULL)
	{
		for (i=0;i<nCount;i++) PostQueuedCompletionStatus(m_hCompletionPort,0,NULL,NULL);
	}
	for (i=0;i<nCount;i++)
	{
		CServerSocketRSThread * pSocketThread=m_SocketRSThreadArray[i];
		ASSERT(pSocketThread!=NULL);
		pSocketThread->StopThread();
		SafeDelete(pSocketThread);
	}
	m_SocketRSThreadArray.RemoveAll();

	//关闭完成端口
	if (m_hCompletionPort!=NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort=NULL;
	}

	//关闭连接
	CServerSocketItem * pServerSocketItem=NULL;
	for (i=0;i<m_ActiveSocketItem.GetCount();i++)
	{
		pServerSocketItem=m_ActiveSocketItem[i];
		ASSERT(pServerSocketItem!=NULL);
		pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
		pServerSocketItem->ResetSocketData();
	}
	m_FreeSocketItem.Append(m_ActiveSocketItem);
	m_ActiveSocketItem.RemoveAll();

	return true;
}

//应答消息
bool CTCPSocketEngine::OnSocketAcceptEvent(CServerSocketItem * pServerSocketItem)
{
	//效验数据
	ASSERT(pServerSocketItem!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	//投递消息
	if (m_bService==false) return false;
	m_AttemperEvent.PostSocketAcceptEvent(pServerSocketItem->GetIndex(),pServerSocketItem->GetRountID(),pServerSocketItem->GetClientAddr());

	return true;
}

//网络读取消息
bool CTCPSocketEngine::OnSocketReadEvent(CMD_Command Command, void * pBuffer, WORD wDataSize, CServerSocketItem * pServerSocketItem)
{
	//效验数据
	ASSERT(pServerSocketItem!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	//效验状态
	if (m_bService==false) return false;
	m_AttemperEvent.PostSocketReadEvent(pServerSocketItem->GetIndex(),pServerSocketItem->GetRountID(),Command,pBuffer,wDataSize);

	return true;
}

//网络关闭消息
bool CTCPSocketEngine::OnSocketCloseEvent(CServerSocketItem * pServerSocketItem)
{
	//效验参数
	ASSERT(pServerSocketItem!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	try
	{
		//效验状态
		if (m_bService==false) return false;

		//计算时间
		WORD wIndex=pServerSocketItem->GetIndex();
		WORD wRountID=pServerSocketItem->GetRountID();
		DWORD dwClientAddr=pServerSocketItem->GetClientAddr();
		DWORD dwConnectTime=pServerSocketItem->GetConnectTime();
		m_AttemperEvent.PostSocketCloseEvent(wIndex,wRountID,dwClientAddr,dwConnectTime);

		//释放连接
		FreeSocketItem(pServerSocketItem);
	}
	catch (...) {}

	return true;
}

//通知回调函数（发送队列线程调用）
void __cdecl CTCPSocketEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize, DWORD dwInsertTime)
{
	switch (wIdentifier)
	{
	case QUEUE_SEND_REQUEST:		//发送请求
		{
			//效验数据
			tagSendDataRequest * pSendDataRequest=(tagSendDataRequest *)pBuffer;
			ASSERT(wDataSize>=(sizeof(tagSendDataRequest)-sizeof(pSendDataRequest->cbSendBuf)));
			ASSERT(wDataSize==(pSendDataRequest->wDataSize+sizeof(tagSendDataRequest)-sizeof(pSendDataRequest->cbSendBuf)));

			//发送数据
			if (pSendDataRequest->wIndex==INDEX_ALL_SOCKET)
			{
				//获取活动项
				CThreadLockHandle ItemLockedHandle(&m_ItemLocked);
				m_TempSocketItem.Copy(m_ActiveSocketItem);
				ItemLockedHandle.UnLock();

				//循环发送数据
				CServerSocketItem * pServerSocketItem=NULL;
				for (INT_PTR i=0;i<m_TempSocketItem.GetCount();i++)
				{
					pServerSocketItem=m_TempSocketItem[i];
					ASSERT(pServerSocketItem!=NULL);
					CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
					if (pServerSocketItem->IsAllowBatch())
					{
						pServerSocketItem->SendData(pSendDataRequest->cbSendBuf,pSendDataRequest->wDataSize,pSendDataRequest->wMainCmdID,
							pSendDataRequest->wSubCmdID,pServerSocketItem->GetRountID());
					}
				}
			}
			else
			{
				//单项发送
				CServerSocketItem * pServerSocketItem=EnumSocketItem(pSendDataRequest->wIndex);
				CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
				pServerSocketItem->SendData(pSendDataRequest->cbSendBuf,pSendDataRequest->wDataSize,pSendDataRequest->wMainCmdID,
					pSendDataRequest->wSubCmdID,pSendDataRequest->wRountID);
			}

			break;
		}
	case QUEUE_SAFE_CLOSE:		//安全关闭
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagSafeCloseSocket));
			tagSafeCloseSocket * pSafeCloseSocket=(tagSafeCloseSocket *)pBuffer;

			//安全关闭
			CServerSocketItem * pServerSocketItem=EnumSocketItem(pSafeCloseSocket->wIndex);
			CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
			pServerSocketItem->ShutDownSocket(pSafeCloseSocket->wRountID);

			break;
		}
	case QUEUE_ALLOW_BATCH:		//允许群发
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend=(tagAllowBatchSend *)pBuffer;

			//设置群发
			CServerSocketItem * pServerSocketItem=EnumSocketItem(pAllowBatchSend->wIndex);
			CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
			pServerSocketItem->AllowBatchSend(pAllowBatchSend->wRountID,pAllowBatchSend->cbAllow?true:false);

			break;
		}
	case QUEUE_DETECT_SOCKET:	//检测连接
		{
			//获取活动项
			CThreadLockHandle ItemLockedHandle(&m_ItemLocked);
			m_TempSocketItem.Copy(m_ActiveSocketItem);
			ItemLockedHandle.UnLock();

			//构造数据
			CMD_KN_DetectSocket DetectSocket;
			ZeroMemory(&DetectSocket,sizeof(DetectSocket));

			//变量定义
			WORD wRountID=0;
			DWORD dwNowTickCount=GetTickCount();
			DWORD dwBreakTickCount=__max(dwNowTickCount-m_dwLastDetect,TIME_BREAK_READY);

			//设置变量
			m_dwLastDetect=GetTickCount();

			//检测连接
			for (INT_PTR i=0;i<m_TempSocketItem.GetCount();i++)
			{
				//变量定义
				CServerSocketItem * pServerSocketItem=m_TempSocketItem[i];
				DWORD dwRecvTickCount=pServerSocketItem->GetRecvTickCount();
				CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());

				//间隔检查
				if (dwRecvTickCount>=dwNowTickCount) continue;

				//检测连接
				if (pServerSocketItem->IsReadySend()==true)
				{
					if ((dwNowTickCount-dwRecvTickCount)>dwBreakTickCount)
					{
						pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
						continue;
					}
				}
				else
				{
					if ((dwNowTickCount-dwRecvTickCount)>TIME_BREAK_CONNECT)
					{
						pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
						continue;
					}
				}

				//发送数据
				if (pServerSocketItem->IsReadySend()==true)
				{
					wRountID=pServerSocketItem->GetRountID();
					DetectSocket.dwSendTickCount=GetTickCount();
					pServerSocketItem->SendData(&DetectSocket,sizeof(DetectSocket),MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,wRountID);
				}
			}

			break;
		}
	default: { ASSERT(FALSE); }
	}

	return;
}

//获取空闲对象
CServerSocketItem * CTCPSocketEngine::ActiveSocketItem()
{
	CThreadLockHandle ItemLockedHandle(&m_ItemLocked,true);

	//获取空闲对象
	CServerSocketItem * pServerSocketItem=NULL;
	if (m_FreeSocketItem.GetCount()>0)
	{
		INT_PTR nItemPostion=m_FreeSocketItem.GetCount()-1;
		pServerSocketItem=m_FreeSocketItem[nItemPostion];
		ASSERT(pServerSocketItem!=NULL);
		m_FreeSocketItem.RemoveAt(nItemPostion);
		m_ActiveSocketItem.Add(pServerSocketItem);
	}

	//创建新对象
	if (pServerSocketItem==NULL)
	{
		WORD wStorageCount=(WORD)m_StorageSocketItem.GetCount();
		if (wStorageCount<m_wMaxSocketItem)
		{
			try
			{
				pServerSocketItem=new CServerSocketItem(wStorageCount,this);
				if (pServerSocketItem==NULL) return NULL;
				m_StorageSocketItem.Add(pServerSocketItem);
				m_ActiveSocketItem.Add(pServerSocketItem);
			}
			catch (...) { return NULL; }
		}
	}

	return pServerSocketItem;
}

//获取连接对象
CServerSocketItem * CTCPSocketEngine::EnumSocketItem(WORD wIndex)
{
	CThreadLockHandle ItemLockedHandle(&m_ItemLocked,true);
	if (wIndex<m_StorageSocketItem.GetCount())
	{
		CServerSocketItem * pServerSocketItem=m_StorageSocketItem[wIndex];
		ASSERT(pServerSocketItem!=NULL);
		return pServerSocketItem;
	}
	return NULL;
}

//释放连接对象
bool CTCPSocketEngine::FreeSocketItem(CServerSocketItem * pServerSocketItem)
{
	//效验参数
	ASSERT(pServerSocketItem!=NULL);

	//释放对象
	CThreadLockHandle ItemLockedHandle(&m_ItemLocked,true);
	INT_PTR nActiveCount=m_ActiveSocketItem.GetCount();
	for (int i=0;i<nActiveCount;i++)
	{
		if (pServerSocketItem==m_ActiveSocketItem[i])
		{
			m_ActiveSocketItem.RemoveAt(i);
			m_FreeSocketItem.Add(pServerSocketItem);
			return true;
		}
	}

	//释放失败
	ASSERT(FALSE);
	return false;
}

//检测连接
bool __cdecl CTCPSocketEngine::DetectSocket()
{
	return m_SendQueueService.AddToQueue(QUEUE_DETECT_SOCKET,NULL,0);
}


//发送函数
bool __cdecl CTCPSocketEngine::SendData(WORD wIndex, WORD wRountID, WORD wMainCmdID, WORD wSubCmdID)
{
	//效益状态
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID=wMainCmdID;
	SendRequest.wSubCmdID=wSubCmdID;
	SendRequest.wIndex=wIndex;
	SendRequest.wRountID=wRountID;
	SendRequest.wDataSize=0;

	//加入发送队列
	WORD wSendSize=sizeof(SendRequest)-sizeof(SendRequest.cbSendBuf);
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST,&SendRequest,wSendSize);
}

//发送函数
bool __cdecl CTCPSocketEngine::SendData(WORD wIndex, WORD wRountID, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效益状态
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//效益数据
	ASSERT((wDataSize+sizeof(CMD_Head))<=SOCKET_PACKAGE);
	if ((wDataSize+sizeof(CMD_Head))>SOCKET_PACKAGE) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID=wMainCmdID;
	SendRequest.wSubCmdID=wSubCmdID;
	SendRequest.wIndex=wIndex;
	SendRequest.wRountID=wRountID;
	SendRequest.wDataSize=wDataSize;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SendRequest.cbSendBuf,pData,wDataSize);
	}

	//加入发送队列
	WORD wSendSize=sizeof(SendRequest)-sizeof(SendRequest.cbSendBuf)+wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST,&SendRequest,wSendSize);
}

//批量发送
bool __cdecl CTCPSocketEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效益状态
	if (m_bService==false) return false;

	//效益数据
	ASSERT((wDataSize+sizeof(CMD_Head))<=SOCKET_PACKAGE);
	if ((wDataSize+sizeof(CMD_Head))>SOCKET_PACKAGE) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID=wMainCmdID;
	SendRequest.wSubCmdID=wSubCmdID;
	SendRequest.wIndex=INDEX_ALL_SOCKET;
	SendRequest.wRountID=0;
	SendRequest.wDataSize=wDataSize;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SendRequest.cbSendBuf,pData,wDataSize);
	}

	//加入发送队列
	WORD wSendSize=sizeof(SendRequest)-sizeof(SendRequest.cbSendBuf)+wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST,&SendRequest,wSendSize);
}

//关闭连接
bool __cdecl CTCPSocketEngine::CloseSocket(WORD wIndex, WORD wRountID)
{
	CServerSocketItem * pServerSocketItem=EnumSocketItem(wIndex);
	if (pServerSocketItem==NULL) return false;
	CThreadLockHandle SocketLockHandle(pServerSocketItem->GetSignedLock());
	return pServerSocketItem->CloseSocket(wRountID);
}

//设置关闭
bool __cdecl CTCPSocketEngine::ShutDownSocket(WORD wIndex, WORD wRountID)
{
	tagSafeCloseSocket SafeCloseSocket;
	SafeCloseSocket.wIndex=wIndex;
	SafeCloseSocket.wRountID=wRountID;
	return m_SendQueueService.AddToQueue(QUEUE_SAFE_CLOSE,&SafeCloseSocket,sizeof(SafeCloseSocket));
}

//允许群发
bool __cdecl CTCPSocketEngine::AllowBatchSend(WORD wIndex, WORD wRountID, bool bAllow)
{
	tagAllowBatchSend AllowBatchSend;
	AllowBatchSend.wIndex=wIndex;
	AllowBatchSend.wRountID=wRountID;
	AllowBatchSend.cbAllow=bAllow;
	return m_SendQueueService.AddToQueue(QUEUE_ALLOW_BATCH,&AllowBatchSend,sizeof(AllowBatchSend));
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateTCPSocketEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CTCPSocketEngine * pTCPSocketEngine=NULL;
	try
	{
		pTCPSocketEngine=new CTCPSocketEngine();
		if (pTCPSocketEngine==NULL) throw TEXT("创建失败");
		void * pObject=pTCPSocketEngine->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTCPSocketEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
