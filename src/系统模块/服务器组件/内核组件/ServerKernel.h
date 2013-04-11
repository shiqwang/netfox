#ifndef SERVER_KERNEL_HEAD_FILE
#define SERVER_KERNEL_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//系统头文件
#include <ICrsint.h>
#include <WinSock2.h>

//平台头文件
#include "..\..\模板库\Template.h"
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\组件接口\IUnknownEx.h"
#include "..\..\共享组件\公共服务\ComService.h"

//////////////////////////////////////////////////////////////////////////

//ADO 导入库
#import "MSADO15.DLL" rename_namespace("ADOCG") rename("EOF","EndOfFile")
using namespace ADOCG;

//COM 错误类型
typedef _com_error					CComError;							//COM 错误

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//模块定义
#ifdef _DEBUG
	#define SERVER_KERNEL_DLL_NAME	TEXT("ServerKernelD.dll")			//组件 DLL 名字
#else
	#define SERVER_KERNEL_DLL_NAME	TEXT("ServerKernel.dll")			//组件 DLL 名字
#endif

//常量宏定义
#define EVENT_LEVEL_COUNT			4									//事件等级
#define MAX_QUEUE_PACKET			10240								//最大队列
#define INDEX_ALL_SOCKET			0xFFFF								//所有连接
#define TIMER_REPEAT_TIMER			DWORD(-1)							//重复次数

//////////////////////////////////////////////////////////////////////////
//枚举定义

//输出等级
enum enTraceLevel
{
	Level_Normal					=0,									//普通消息
	Level_Warning					=1,									//警告消息
	Level_Exception					=2,									//异常消息
	Level_Debug						=3,									//调试消息
};

//数据库错误代码
enum enADOErrorType
{
	ErrorType_Nothing				=0,									//没有错误
	ErrorType_Connect				=1,									//连接错误
	ErrorType_Other					=2,									//其他错误
};

//////////////////////////////////////////////////////////////////////////
//事件定义

//事件标识
#define EVENT_CONTROL				0x0001								//控制消息
#define EVENT_TIMER					0x0002								//定时器引擎
#define EVENT_DATABASE				0x0003								//数据库请求
#define EVENT_SOCKET_ACCEPT			0x0004								//网络应答
#define EVENT_SOCKET_READ			0x0005								//网络读取
#define EVENT_SOCKET_CLOSE			0x0006								//网络关闭

//定时器事件
struct NTY_TimerEvent
{
	WORD							wTimerID;							//定时器 ID
	WPARAM							wBindParam;							//绑定参数
};

//数据库请求事件
struct NTY_DataBaseEvent
{
	WORD							wIndex;								//对象索引
	WORD							wRoundID;							//对象标识
	WORD							wRequestID;							//请求标识
};

//网络应答事件
struct NTY_SocketAcceptEvent
{
	WORD							wIndex;								//连接索引
	WORD							wRoundID;							//连接标识
	DWORD							dwClientIP;							//连接地址
};

//网络读取事件
struct NTY_SocketReadEvent
{
	WORD							wIndex;								//连接索引
	WORD							wRoundID;							//连接标识
	WORD							wDataSize;							//数据大小
	CMD_Command						Command;							//命令信息
};

//网络关闭事件
struct NTY_SocketCloseEvent
{
	WORD							wIndex;								//连接索引
	WORD							wRoundID;							//连接标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwConnectSecond;					//连接时间
};

//////////////////////////////////////////////////////////////////////////

#define VER_IADOError INTERFACE_VERSION(1,1)
static const GUID IID_IADOError={0x66463b5a,0x390c,0x42f9,0x85,0x19,0x13,0x31,0x39,0x36,0xfe,0x8f};

//数据库错误接口
interface IADOError : public IUnknownEx
{
	//错误描述
	virtual LPCTSTR __cdecl GetErrorDescribe()=NULL;
	//错误类型
	virtual enADOErrorType __cdecl GetErrorType()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDataBase INTERFACE_VERSION(1,1)
static const GUID IID_IDataBase={0x9e962173,0x2a9f,0x4ebd,0x8e,0x98,0x40,0xe9,0x96,0x57,0x24,0xfb};

//数据库连接接口
interface IDataBase : public IUnknownEx
{
	//打开连接
	virtual bool __cdecl OpenConnection()=NULL;
	//关闭记录
	virtual bool __cdecl CloseRecordset()=NULL;
	//关闭连接
	virtual bool __cdecl CloseConnection()=NULL;
	//重新连接
	virtual bool __cdecl TryConnectAgain(bool bFocusConnect, CComError * pComError)=NULL;
	//设置信息
	virtual bool __cdecl SetConnectionInfo(LPCTSTR szIP, WORD wPort, LPCTSTR szData, LPCTSTR szName, LPCTSTR szPass)=NULL;
	//是否连接错误
	virtual bool __cdecl IsConnectError()=NULL;
	//是否打开
	virtual bool __cdecl IsRecordsetOpened()=NULL;
	//往下移动
	virtual void __cdecl MoveToNext()=NULL;
	//移到开头
	virtual void __cdecl MoveToFirst()=NULL;
	//是否结束
	virtual bool __cdecl IsEndRecordset()=NULL;
	//获取数目
	virtual long __cdecl GetRecordCount()=NULL;
	//获取大小
	virtual long __cdecl GetActualSize(LPCTSTR pszParamName)=NULL;
	//绑定对象
	virtual bool __cdecl BindToRecordset(CADORecordBinding * pBind)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, WORD & wValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, INT & nValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, LONG & lValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DWORD & ulValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DOUBLE & dbValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, TCHAR szBuffer[], UINT uSize)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time)=NULL;
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, bool & bValue)=NULL;
	//设置存储过程
	virtual void __cdecl SetSPName(LPCTSTR pszSpName)=NULL;
	//插入参数
	virtual void __cdecl AddParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, ADOCG::DataTypeEnum Type, long lSize, _variant_t & vtValue)=NULL;
	//删除参数
	virtual void __cdecl ClearAllParameters()=NULL;
	//获得参数
	virtual void __cdecl GetParameterValue(LPCTSTR pszParamName, _variant_t & vtValue)=NULL;
	//获取返回数值
	virtual long __cdecl GetReturnValue()=NULL;
	//执行语句
	virtual bool __cdecl Execute(LPCTSTR pszCommand)=NULL;
	//执行命令
	virtual bool __cdecl ExecuteCommand(bool bRecordset)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDataBaseSink INTERFACE_VERSION(1,1)
static const GUID IID_IDataBaseSink={0x1287cdc2,0xcbda,0x4bb6,0x95,0xeb,0xd9,0xcc,0x6a,0x2b,0xef,0xd3};

//数据库钩子接口
interface IDataBaseSink : public IUnknownEx
{
	//数据库模块启动
	virtual bool __cdecl StartService(IUnknownEx * pIUnknownEx)=NULL;
	//数据库模块关闭
	virtual bool __cdecl StopService(IUnknownEx * pIUnknownEx)=NULL;
	//数据操作处理
	virtual bool __cdecl OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDataBaseEngine INTERFACE_VERSION(1,1)
static const GUID IID_IDataBaseEngine={0x9b00b2a6,0x620e,0x4791,0x97,0xad,0x28,0x32,0x16,0xf1,0x8c,0xdf};

//数据库引擎接口
interface IDataBaseEngine : public IUnknownEx
{
	//启动服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
	//注册钩子
	virtual bool __cdecl SetDataBaseSink(IUnknownEx * pIUnknownEx)=NULL;
	//获取接口
	virtual void * __cdecl GetQueueService(const IID & Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IQueueServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IQueueServiceSink={0x88b5bf59,0x3a98,0x43b1,0xac,0x85,0xf1,0x17,0x5b,0x29,0x69,0x40};

//数据队列类钩子接口
interface IQueueServiceSink : public IUnknownEx
{
	//通知回调函数
	virtual void __cdecl OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize, DWORD dwInsertTime)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IQueueService INTERFACE_VERSION(1,1)
static const GUID IID_IQueueService={0xcc5310b5,0x3709,0x40aa,0x85,0x24,0xd6,0xc5,0x87,0xb0,0x32,0x22};

//数据队列接口
interface IQueueService : public IUnknownEx
{
	//加入数据
	virtual bool __cdecl AddToQueue(WORD wIdentifier, void * const pBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IQueueServiceEngine INTERFACE_VERSION(1,1)
static const GUID IID_IQueueServiceEngine={0xba2c602e,0x7f5a,0x40ad,0xb7,0xa5,0x7d,0x1c,0x2a,0x8f,0xee,0x78};

//队列类引擎接口
interface IQueueServiceEngine : public IUnknownEx
{
	//开始服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
	//设置接口
	virtual bool __cdecl SetQueueServiceSink(IUnknownEx * pIUnknownEx)=NULL;
	//负荷信息
	virtual bool __cdecl GetBurthenInfo(tagBurthenInfo & BurthenInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IEventService INTERFACE_VERSION(1,1)
static const GUID IID_IEventService={0x729d0499,0xad36,0x49d5,0x8d,0x46,0xfd,0xd,0x78,0x82,0xa6,0x37};

//服务引擎接口
interface IEventService : public IUnknownEx
{
	//设置句柄
	virtual bool __cdecl SetRichEditHwnd(HWND hRichEdit)=NULL;
	//设置级别
	virtual void __cdecl ConfigEventService(enTraceLevel TraceLevel, bool bShow)=NULL;
	//事件通知
	virtual void __cdecl ShowEventNotify(LPCTSTR pszString, enTraceLevel TraceLevel)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITimerEngine INTERFACE_VERSION(1,1)
static const GUID IID_ITimerEngine={0x4a9b0421,0x1224,0x4e5e,0xa5,0x4f,0xb1,0x61,0x6e,0x62,0xdb,0x98};

//定时器引擎接口
interface ITimerEngine : public IUnknownEx
{
	//设置定时器
	virtual bool __cdecl SetTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wParam)=NULL; 
	//删除定时器
	virtual bool __cdecl KillTimer(WORD wTimerID)=NULL;
	//删除定时器
	virtual bool __cdecl KillAllTimer()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITimerEngineManager INTERFACE_VERSION(1,1)
static const GUID IID_ITimerEngineManager={0xcdbfa840,0xdc45,0x41f7,0xb6,0x3c,0xad,0x83,0x75,0x98,0xc6,0x3e};

//定时器引擎接口
interface ITimerEngineManager : public IUnknownEx
{
	//开始服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
	//设置接口
	virtual bool __cdecl SetTimerEngineSink(IUnknownEx * pIUnknownEx)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITCPSocketEngine INTERFACE_VERSION(1,1)
static const GUID IID_ITCPSocketEngine={0x9979ea68,0x5ee7,0x4c15,0xa4,0xdf,0x9f,0xe3,0x85,0x26,0x13,0xbb};

//TCP SOCKET 引擎接口
interface ITCPSocketEngine : public IUnknownEx
{
	//检测连接
	virtual bool __cdecl DetectSocket()=NULL;
	//发送函数
	virtual bool __cdecl SendData(WORD wIndex, WORD wRoundID, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendData(WORD wIndex, WORD wRoundID, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//批量发送
	virtual bool __cdecl SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//关闭连接
	virtual bool __cdecl CloseSocket(WORD wIndex, WORD wRoundID)=NULL;
	//关闭连接
	virtual bool __cdecl ShutDownSocket(WORD wIndex, WORD wRoundID)=NULL;
	//允许群发
	virtual bool __cdecl AllowBatchSend(WORD wIndex, WORD wRoundID, bool bAllow)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITCPSocketEngineManager INTERFACE_VERSION(1,1)
static const GUID IID_ITCPSocketEngineManager={0xf91b246f,0x3c97,0x4445,0xb3,0xeb,0xb9,0x42,0x3b,0x68,0x1b,0xed};

//TCP SOCKET 引擎管理接口
interface ITCPSocketEngineManager : public IUnknownEx
{
	//设置接口
	virtual bool __cdecl SetSocketEngineSink(IUnknownEx * pIUnknownEx)=NULL;
	//设置端口
	virtual bool __cdecl SetServicePort(WORD wListenPort)=NULL;
	//设置数目
	virtual bool __cdecl SetMaxSocketItem(WORD wMaxSocketItem)=NULL;
	//启动服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAttemperEngineSink INTERFACE_VERSION(1,1)
static const GUID IID_IAttemperEngineSink={0x82d2c5,0x4e6f,0x4031,0xae,0xd4,0x6f,0xee,0x5a,0x17,0x1b,0x8a};

//调度模块钩子接口
interface IAttemperEngineSink : public IUnknownEx
{
	//管理接口
public:
	//调度模块启动
	virtual bool __cdecl StartService(IUnknownEx * pIUnknownEx)=NULL;
	//调度模块关闭
	virtual bool __cdecl StopService(IUnknownEx * pIUnknownEx)=NULL;
	//事件处理接口
	virtual bool __cdecl OnAttemperEvent(WORD wIdentifier, void * pBuffer, WORD wDataSize, DWORD dwInsertTime)=NULL;

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(WORD wTimerID, WPARAM wBindParam)=NULL;
	//数据库事件
	virtual bool __cdecl OnEventDataBase(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)=NULL;
	//网络应答事件
	virtual bool __cdecl OnEventSocketAccept(NTY_SocketAcceptEvent * pSocketAcceptEvent)=NULL;
	//网络读取事件
	virtual bool __cdecl OnEventSocketRead(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)=NULL;
	//网络关闭事件
	virtual bool __cdecl OnEventSocketClose(NTY_SocketCloseEvent * pSocketCloseEvent)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAttemperEngine INTERFACE_VERSION(1,1)
static const GUID IID_IAttemperEngine={0x967b3e64,0x69af,0x434a,0xb8,0x6c,0x67,0x9b,0x72,0xbb,0x2e,0xe};

//调度引擎接口
interface IAttemperEngine : public IUnknownEx
{
	//管理接口
public:
	//启动服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
	//设置网络
	virtual bool __cdecl SetSocketEngine(IUnknownEx * pIUnknownEx)=NULL;
	//设置钩子
	virtual bool __cdecl SetAttemperEngineSink(IUnknownEx * pIUnknownEx)=NULL;
	//获取接口
	virtual void * __cdecl GetQueueService(const IID & Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IServiceEngine INTERFACE_VERSION(3,1)
static const GUID IID_IServiceEngine={0x74284fc0,0xc587,0x4c21,0x9b,0x7c,0x16,0x76,0x27,0x1a,0x7b,0xd5};

//服务引擎接口
interface IServiceEngine : public IUnknownEx
{
	//服务接口
public:
	//启动服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;

	//配置接口
public:
	//设置事件
	virtual bool __cdecl SetEventService(IUnknownEx * pIUnknownEx)=NULL;
	//设置钩子
	virtual bool __cdecl SetDataBaseSink(IUnknownEx * pIUnknownEx)=NULL;
	//设置钩子
	virtual bool __cdecl SetAttemperEngineSink(IUnknownEx * pIUnknownEx)=NULL;
	//注册钩子
	virtual bool __cdecl RegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx)=NULL;
	//设置网络引擎接口
	virtual bool __cdecl InitServiceEngine(WORD wListenPort, WORD wMaxSocketItem)=NULL;

	//服务查询
public:
	//获取定时器接口
	virtual void * __cdecl GetTimerEngine(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取数据库引擎接口
	virtual void * __cdecl GetDataBaseEngine(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取调度引擎接口
	virtual void * __cdecl GetAttemperEngine(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取网络引擎接口
	virtual void * __cdecl GetTCPSocketEngine(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取异步引擎接口
	virtual void * __cdecl GetAsynchronismEngine(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取接口
	virtual void * __cdecl GetDataBaseQueueService(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取接口
	virtual void * __cdecl GetAttemperQueueService(const IID & Guid, DWORD dwQueryVer)=NULL;

	//功能接口
public:
	//服务状态
	virtual bool __cdecl IsService()=NULL;
	//外部控制
	virtual bool __cdecl ControlService(void * pBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAsynchronismEngine INTERFACE_VERSION(1,1)
static const GUID IID_IAsynchronismEngine={0x3728c1a5,0x37a9,0x4ace,0x8e,0x31,0xd9,0xb1,0xce,0x63,0xd9,0x88};

//异步引擎接口
interface IAsynchronismEngine : public IUnknownEx
{
	//管理接口
public:
	//启动服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
	//插入请求
	virtual bool __cdecl InsertRequest(WORD wRequestID, void * const pBuffer, WORD wDataSize, IUnknownEx * pIUnknownEx)=NULL;

	//功能接口
public:
	//注册钩子
	virtual bool __cdecl RegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx)=NULL;
	//取消注册
	virtual bool __cdecl UnRegisterAsynchronismEngineSink(IUnknownEx * pIUnknownEx)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAsynchronismEngineSink INTERFACE_VERSION(1,1)
static const GUID IID_IAsynchronismEngineSink={0x6898f671,0x5c4b,0x4555,0x8c,0x82,0x49,0xc4,0xf3,0x2a,0xeb,0x4d};

//异步引擎钩子接口
interface IAsynchronismEngineSink : public IUnknownEx
{
	//启动事件
	virtual bool __cdecl OnAsynchronismEngineStart()=NULL;
	//停止事件
	virtual bool __cdecl OnAsynchronismEngineStop()=NULL;
	//异步请求
	virtual bool __cdecl OnAsynchronismRequest(WORD wRequestID, void * pBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出组件文件
#include "QueueServiceEvent.h"
#include "ServerKernelHelper.h"

//////////////////////////////////////////////////////////////////////////

#endif