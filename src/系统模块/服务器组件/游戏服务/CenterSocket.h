#ifndef CENTER_SOCKET_HEAD_FILE
#define CENTER_SOCKET_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////

#define EVENT_CENTER_CONNECT		0x0007								//中心连接
#define EVENT_CENTER_READ			0x0008								//中心读取
#define EVENT_CENTER_CLOSE			0x0009								//中心关闭

//连接通知
struct NTY_CenterConnect
{
	int									iErrorCode;						//错误代码
};

//读取通知
struct NTY_CenterRead
{
	CMD_Command							Command;						//命令信息
	WORD								wDataSize;						//数据大小
	BYTE								cbReadBuffer[SOCKET_PACKAGE];	//网络数据
};

//关闭通知
struct NTY_CenterClose
{
	bool								bCloseByServer;					//关闭模式
};

//////////////////////////////////////////////////////////////////////////

//中心连接类
class CCenterSocket : public IAsynchronismEngineSink, public IClientSocketSink
{
	//变量定义
protected:
	IEventService						* m_pIEventService;				//事件服务
	IQueueService						* m_pIQueueService;				//队列服务
	IAsynchronismEngine					* m_pIAsynchronismEngine;		//异步引擎

	//组件变量
protected:
	CClientSocketHelper					m_ClientSocketHelper;			//网络连接

	//函数定义
public:
	//构造函数
	CCenterSocket();
	//析构函数
	virtual ~CCenterSocket();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CCenterSocket))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool __cdecl OnAsynchronismEngineStart();
	//停止事件
	virtual bool __cdecl OnAsynchronismEngineStop();
	//异步请求
	virtual bool __cdecl OnAsynchronismRequest(WORD wRequestID, void * pBuffer, WORD wDataSize);

	//网络接口
public:
	//网络连接消息
	virtual bool __cdecl OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocke);
	//网络读取消息
	virtual bool __cdecl OnSocketRead(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocke);
	//网络关闭消息
	virtual bool __cdecl OnSocketClose(IClientSocket * pIClientSocke, bool bCloseByServer);

	//配置函数
public:
	//设置事件
	bool SetEventService(IUnknownEx * pIUnknownEx);
	//设置队列
	bool SetQueueService(IUnknownEx * pIUnknownEx);
	//设置异步
	bool SetAsynchronismEngine(IUnknownEx * pIUnknownEx);

	//功能函数
public:
	//连接服务器
	bool Connect(DWORD dwServerIP, WORD wPort);
	//发送函数
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//关闭连接
	bool CloseSocket(bool bNotify);
};

//////////////////////////////////////////////////////////////////////////

#endif