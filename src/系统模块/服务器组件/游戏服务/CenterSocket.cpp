#include "StdAfx.h"
#include "CenterSocket.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//请求标识
#define IREQ_CONNECT					1								//连接请求
#define IREQ_SEND_DATA					2								//发送请求
#define IREQ_CLOSE_SOCKET				3								//关闭请求

//////////////////////////////////////////////////////////////////////////
//结构体定义

//连接请求
struct tagReqConnect
{
	WORD								wPort;							//连接端口
	DWORD								dwServerIP;						//连接地址
};

//发送请求
struct tagReqSendData
{
	WORD								wMainCmdID;						//主命令码
	WORD								wSubCmdID;						//子命令码
	WORD								wDataSize;						//数据大小
	BYTE								cbDataBuffer[SOCKET_PACKAGE];	//发送数据
};

//关闭请求
struct tagReqCloseSocket
{
	bool								bNotify;						//是否通知
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CCenterSocket::CCenterSocket(void)
{
	//接口变量
	m_pIEventService=NULL;
	m_pIQueueService=NULL;
	m_pIAsynchronismEngine=NULL;

	return;
}

//析构函数
CCenterSocket::~CCenterSocket(void)
{
}

//接口查询
void * __cdecl CCenterSocket::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE(IAsynchronismEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAsynchronismEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool __cdecl CCenterSocket::OnAsynchronismEngineStart()
{
	//效验参数
	ASSERT(m_pIEventService!=NULL);
	ASSERT(m_pIQueueService!=NULL);
	ASSERT(m_pIAsynchronismEngine!=NULL);

	//连接对象
	if (m_ClientSocketHelper.GetInterface()==NULL)
	{
		//创建对象
		bool bSuccess=m_ClientSocketHelper.CreateInstance();
		if (bSuccess==false)
		{
			ASSERT(m_pIEventService!=NULL);
			m_pIEventService->ShowEventNotify(TEXT("中心服务器连接对象创建失败"),Level_Exception);
			return false;
		}

		//设置接口
		IClientSocketSink * pIClientSocketSink=GET_MYSELF_INTERFACE(IClientSocketSink);
		ASSERT(pIClientSocketSink!=NULL);
		if (m_ClientSocketHelper->SetSocketSink(pIClientSocketSink)==false)
		{
			ASSERT(m_pIEventService!=NULL);
			m_pIEventService->ShowEventNotify(TEXT("中心服务器连接对象回调接口设置失败"),Level_Exception);
			return false;
		}
	}

	return true;
}

//停止事件
bool __cdecl CCenterSocket::OnAsynchronismEngineStop()
{
	//关闭连接
	if (m_ClientSocketHelper.GetInterface()!=NULL) 
	{
		m_ClientSocketHelper->CloseSocket(false);
		m_ClientSocketHelper.CloseInstance();
	}

	return true;
}

//异步请求
bool __cdecl CCenterSocket::OnAsynchronismRequest(WORD wRequestID, void * pBuffer, WORD wDataSize)
{
	switch (wRequestID)
	{
	case IREQ_CONNECT:		//连接请求
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagReqConnect));
			tagReqConnect * pConnect=(tagReqConnect *)pBuffer;

			//请求处理
			ASSERT(m_ClientSocketHelper.GetInterface()!=NULL);
			m_ClientSocketHelper->Connect(pConnect->dwServerIP,pConnect->wPort);

			return true;
		}
	case IREQ_SEND_DATA:	//发送请求
		{
			//效验数据
			tagReqSendData * pSendData=(tagReqSendData *)pBuffer;
			ASSERT(wDataSize>=(sizeof(tagReqSendData)-sizeof(pSendData->cbDataBuffer)));
			ASSERT(wDataSize==(sizeof(tagReqSendData)-sizeof(pSendData->cbDataBuffer)+pSendData->wDataSize));

			//请求处理
			ASSERT(m_ClientSocketHelper.GetInterface()!=NULL);
			m_ClientSocketHelper->SendData(pSendData->wMainCmdID,pSendData->wSubCmdID,pSendData->cbDataBuffer,pSendData->wDataSize);

			return true;
		}
	case IREQ_CLOSE_SOCKET:	//关闭请求
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagReqCloseSocket));
			tagReqCloseSocket * pCloseCocket=(tagReqCloseSocket *)pBuffer;

			//请求处理
			ASSERT(m_ClientSocketHelper.GetInterface()!=NULL);
			m_ClientSocketHelper->CloseSocket(pCloseCocket->bNotify);

			return true;
		};
	default: 
		{
			ASSERT(FALSE);
		}
	}

	return false;
}

//网络连接消息
bool __cdecl CCenterSocket::OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocke)
{
	//效验参数
	ASSERT(m_pIQueueService!=NULL);

	//构造数据
	NTY_CenterConnect CenterSocketConnect;
	CenterSocketConnect.iErrorCode=iErrorCode;

	//投递消息
	m_pIQueueService->AddToQueue(EVENT_CENTER_CONNECT,&CenterSocketConnect,sizeof(CenterSocketConnect));

	return true;
}

//网络读取消息
bool __cdecl CCenterSocket::OnSocketRead(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocke)
{
	//效验参数
	ASSERT(m_pIQueueService!=NULL);

	//构造数据
	NTY_CenterRead CenterSocketRead;
	CenterSocketRead.Command=Command;
	CenterSocketRead.wDataSize=wDataSize;
	if (wDataSize>0)
	{
		ASSERT(pBuffer!=NULL);
		ASSERT(wDataSize<=sizeof(CenterSocketRead.cbReadBuffer));
		CopyMemory(CenterSocketRead.cbReadBuffer,pBuffer,wDataSize);
	}

	//投递消息
	WORD wQueueSize=sizeof(CenterSocketRead)-sizeof(CenterSocketRead.cbReadBuffer)+wDataSize;
	m_pIQueueService->AddToQueue(EVENT_CENTER_READ,&CenterSocketRead,wQueueSize);

	return true;
}

//网络关闭消息
bool __cdecl CCenterSocket::OnSocketClose(IClientSocket * pIClientSocke, bool bCloseByServer)
{
	//效验参数
	ASSERT(m_pIQueueService!=NULL);

	//构造数据
	NTY_CenterClose CenterSocketClose;
	CenterSocketClose.bCloseByServer=bCloseByServer;

	//投递消息
	m_pIQueueService->AddToQueue(EVENT_CENTER_CLOSE,&CenterSocketClose,sizeof(CenterSocketClose));

	return true;
}

//设置事件
bool CCenterSocket::SetEventService(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);
	return (m_pIEventService!=NULL);
}

//设置队列
bool CCenterSocket::SetQueueService(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIQueueService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IQueueService);
	ASSERT(m_pIQueueService!=NULL);
	return (m_pIQueueService!=NULL);
}

//设置异步
bool CCenterSocket::SetAsynchronismEngine(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIAsynchronismEngine=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IAsynchronismEngine);
	ASSERT(m_pIAsynchronismEngine!=NULL);
	return (m_pIAsynchronismEngine!=NULL);
}

//连接服务器
bool CCenterSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	//构造数据
	tagReqConnect ReqConnect;
	ReqConnect.wPort=wPort;
	ReqConnect.dwServerIP=dwServerIP;

	//投递请求
	ASSERT(m_pIAsynchronismEngine!=NULL);
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	m_pIAsynchronismEngine->InsertRequest(IREQ_CONNECT,&ReqConnect,sizeof(ReqConnect),pIUnknownEx);

	return true;
}

//发送函数
bool CCenterSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//构造数据
	tagReqSendData ReqSendData;
	ReqSendData.wMainCmdID=wMainCmdID;
	ReqSendData.wSubCmdID=wSubCmdID;
	ReqSendData.wDataSize=wDataSize;
	if (wDataSize>0)
	{
		ASSERT(wDataSize<=sizeof(ReqSendData.cbDataBuffer));
		if (wDataSize>sizeof(ReqSendData.cbDataBuffer)) return false;
		CopyMemory(ReqSendData.cbDataBuffer,pData,wDataSize);
	}

	//投递请求
	ASSERT(m_pIAsynchronismEngine!=NULL);
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	WORD wSendSize=sizeof(ReqSendData)-sizeof(ReqSendData.cbDataBuffer)+ReqSendData.wDataSize;
	m_pIAsynchronismEngine->InsertRequest(IREQ_SEND_DATA,&ReqSendData,wSendSize,pIUnknownEx);

	return true;
}

//关闭连接
bool CCenterSocket::CloseSocket(bool bNotify)
{
	//构造数据
	tagReqCloseSocket ReqCloseSocket;
	ReqCloseSocket.bNotify=bNotify;

	//投递请求
	ASSERT(m_pIAsynchronismEngine!=NULL);
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	m_pIAsynchronismEngine->InsertRequest(IREQ_CLOSE_SOCKET,&ReqCloseSocket,sizeof(ReqCloseSocket),pIUnknownEx);

	return true;
}

//////////////////////////////////////////////////////////////////////////
