#ifndef CLIENT_SOCKET_HEAD_FILE
#define CLIENT_SOCKET_HEAD_FILE

#pragma once

#include "SocketModule.h"

//////////////////////////////////////////////////////////////////////////

//网络连接类
class CClientSocket : public CWnd, public IClientSocket
{
	//状态变量
protected:
	bool							m_bCloseByServer;					//关闭方式
	tagProxyInfo					m_ProxyInfo;						//代理信息
	enSocketState					m_SocketState;						//连接状态
	IClientSocketSink				* m_pIClientSocketSink;				//回调接口

	//核心变量
protected:
	SOCKET							m_hSocket;							//连接句柄
	WORD							m_wRecvSize;						//接收长度
	BYTE							m_cbRecvBuf[SOCKET_BUFFER*10];		//接收缓冲

	//计数变量
protected:
	DWORD							m_dwSendTickCount;					//发送时间
	DWORD							m_dwRecvTickCount;					//接收时间
	DWORD							m_dwSendPacketCount;				//发送计数
	DWORD							m_dwRecvPacketCount;				//接受计数

	//加密数据
protected:
	BYTE							m_cbSendRound;						//字节映射
	BYTE							m_cbRecvRound;						//字节映射
	DWORD							m_dwSendXorKey;						//发送密钥
	DWORD							m_dwRecvXorKey;						//接收密钥
	static BYTE						m_SendByteMap[256];					//字节映射表
	static BYTE						m_RecvByteMap[256];					//字节映射表

	//函数定义
public:
	//构造函数
	CClientSocket();
	//析构函数
	virtual ~CClientSocket();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CClientSocket))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//取消代理
	virtual void __cdecl CancelProxyServer();
	//代理服务器
	virtual void __cdecl SetProxyServer(tagProxyInfo & ProxyInfo);
	//设置接口
	virtual bool __cdecl SetSocketSink(IUnknownEx * pIUnknownEx);
	//获取接口
	virtual void * __cdecl GetSocketSink(const IID & Guid, DWORD dwQueryVer);
	//获取发送间隔
	virtual DWORD __cdecl GetLastSendTick() { return m_dwSendTickCount; }
	//获取接收间隔
	virtual DWORD __cdecl GetLastRecvTick() { return m_dwRecvTickCount; }
	//获取发送数目
	virtual DWORD __cdecl GetSendPacketCount() { return m_dwSendPacketCount; }
	//获取接收数目
	virtual DWORD __cdecl GetRecvPacketCount() { return m_dwRecvPacketCount; }
	//获取状态
	virtual enSocketState __cdecl GetConnectState() { return m_SocketState; }
	//连接服务器
	virtual bool __cdecl Connect(DWORD dwServerIP, WORD wPort);
	//连接服务器
	virtual bool __cdecl Connect(const LPCTSTR szServerIP, WORD wPort);
	//发送函数
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//关闭连接
	virtual bool __cdecl CloseSocket(bool bNotify);

	//辅助函数
protected:
	//解释地址
	DWORD TranslateAddr(LPCTSTR pszServerAddr);
	//连接错误
	LPCTSTR GetConnectError(int iErrorCode, LPTSTR pszBuffer, WORD wBufferSize);
	//发送数据
	bool SendBuffer(void * pBuffer, WORD wSendSize);
	//加密数据
	WORD EncryptBuffer(BYTE * pcbDataBuffer, WORD wDataSize, WORD wBufferSize);
	//解密数据
	WORD CrevasseBuffer(BYTE pcbDataBuffer[], WORD wDataSize);

	//处理函数
protected:
	//网络连接
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);
	//网络读取
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//网络关闭
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);

	//内联函数
private:
	//随机映射
	inline WORD SeedRandMap(WORD wSeed);
	//映射发送数据
	inline BYTE MapSendByte(BYTE const cbData);
	//映射接收数据
	inline BYTE MapRecvByte(BYTE const cbData);

	//消息映射
protected:
	//SOCKET 消息处理程序
	LRESULT	OnSocketNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif