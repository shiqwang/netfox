#ifndef SOCKET_MODULE_HEAD_FILE
#define SOCKET_MODULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\组件接口\IUnknownEx.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//代理类型
#define PROXY_NONE					0									//没有代理
#define PROXY_SOCKS4				1									//SOCKS4 代理
#define PROXY_SOCKS5				2									//SOCKS5 代理
#define PROXY_HTTP					3									//HTTP 代理

//模块定义
#ifdef _DEBUG
	#define SOCKET_MODULE_DLL_NAME	TEXT("SocketModuleD.dll")			//组件 DLL 名字
#else
	#define SOCKET_MODULE_DLL_NAME	TEXT("SocketModule.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//枚举定义

//连接状态定义
enum enSocketState
{
	SocketState_NoConnect,			//没有连接
	SocketState_Connecting,			//正在连接
	SocketState_Connected,			//成功连接
};

//////////////////////////////////////////////////////////////////////////
//结构定义

//代理信息结构
struct tagProxyInfo
{
	BYTE							cbProxyType;						//代理类型
	WORD							wProxyPort;							//代理端口
	CString							strProxyName;						//代理用户名
	CString							strProxyPass;						//代理密码
	CString							strProxyServer;						//代理地址
	CString							strHttpDomain;						//代理域名
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientSocket INTERFACE_VERSION(1,1)
static const GUID IID_IClientSocket={0x8b0e3918,0x5139,0x48da,0xbf,0x40,0xba,0x95,0xad,0xce,0x59,0x64};

//网络组件接口
interface IClientSocket : public IUnknownEx
{
	//取消代理
	virtual void __cdecl CancelProxyServer()=NULL;
	//代理服务器
	virtual void __cdecl SetProxyServer(tagProxyInfo & ProxyInfo)=NULL;
	//设置接口
	virtual bool __cdecl SetSocketSink(IUnknownEx * pIUnknownEx)=NULL;
	//获取接口
	virtual void  * __cdecl GetSocketSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取发送间隔
	virtual DWORD __cdecl GetLastSendTick()=NULL;
	//获取接收间隔
	virtual DWORD __cdecl GetLastRecvTick()=NULL;
	//获取发送数目
	virtual DWORD __cdecl GetSendPacketCount()=NULL;
	//获取接收数目
	virtual DWORD __cdecl GetRecvPacketCount()=NULL;
	//获取状态
	virtual enSocketState __cdecl GetConnectState()=NULL;
	//连接服务器
	virtual bool __cdecl Connect(DWORD dwServerIP, WORD wPort)=NULL;
	//连接服务器
	virtual bool __cdecl Connect(const LPCTSTR szServerIP, WORD wPort)=NULL;
	//发送函数
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//关闭连接
	virtual bool __cdecl CloseSocket(bool bNotify)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientSocketSink INTERFACE_VERSION(1,1)
static const GUID IID_IClientSocketSink={0x55fcf8aa,0x526a,0x44c2,0x9a,0xd1,0x4,0xe,0x7,0xd6,0x8a,0x2a};

//网络钩子接口
interface IClientSocketSink : public IUnknownEx
{
	//网络连接消息
	virtual bool __cdecl OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocke)=NULL;
	//网络读取消息
	virtual bool __cdecl OnSocketRead(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocke)=NULL;
	//网络关闭消息
	virtual bool __cdecl OnSocketClose(IClientSocket * pIClientSocke, bool bCloseByServer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//网络组件辅助类
class CClientSocketHelper : public CTempldateHelper<IClientSocket>
{
	//函数定义
public:
	//构造函数
	CClientSocketHelper(void) : CTempldateHelper<IClientSocket>(IID_IClientSocket,
		VER_IClientSocket,SOCKET_MODULE_DLL_NAME,TEXT("CreateClientSocket"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

#endif