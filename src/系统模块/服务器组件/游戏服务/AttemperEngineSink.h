#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServerList.h"
#include "CenterSocket.h"
#include "DataBaseSink.h"
#include "GameServiceExport.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////
//结构体定义

//调度参数
struct tagAttemperSinkParameter
{
	//配置变量
	DWORD							dwCenterServer;						//中心地址
	tagGameServiceAttrib			* pGameServiceAttrib;				//服务属性
	tagGameServiceOption			* pGameServiceOption;				//配置信息

	//组件变量
	CCenterSocket					* pCenterSocket;					//中心连接
	IGameServiceManager				* pIGameServiceManager;				//管理接口
};

//连接信息结构
struct tagConnectItemInfo
{
	bool							bConnect;							//是否连接
	WORD							wRoundID;							//循环索引
	DWORD							dwClientIP;							//连接地址
	IServerUserItem					* pIServerUserItem;					//用户指针
};

//////////////////////////////////////////////////////////////////////////

//类说明
class CTableFrame;

//调度引擎钩子
class CAttemperEngineSink : public IAttemperEngineSink, public IGameServiceFrame
{
	//变量定义
protected:
	WORD							m_wMaxUserItem;						//最大用户
	CTableFrame						* m_pTableFrame;					//桌子指针
	tagConnectItemInfo				* m_pConnectItemInfo;				//辅助信息

	//中心服务器
protected:
	enSocketState					m_SocketState;						//连接状态
	CCenterSocket					* m_pCenterSocket;					//中心连接

	//控制变量
protected:
	bool							m_bShallClose;						//关闭标志
	bool							m_bAllowWisper;						//私聊标志
	bool							m_bAllowRoomChat;					//聊天标志
	bool							m_bAllowGameChat;					//聊天标志
	bool							m_bAllowEnterRoom;					//进入标志
	bool							m_bAllowEnterGame;					//进入标志

	bool							m_bFZBRoom;					//防作弊房间标志
	//配置信息
protected:
	DWORD							m_dwCenterServer;					//中心地址
	tagGameServiceAttrib			* m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption			* m_pGameServiceOption;				//服务配置

	//游戏组件
protected:
	HINSTANCE						m_hDllInstance;						//游戏实例
	IGameServiceManager				* m_pIGameServiceManager;			//管理接口

	//列表解释
protected:
	CMD_GR_ColumnInfo				m_ColumnInfo;						//列表解释
	WORD							m_wColumnInfoSize;					//列表长度

	//在线人数
protected:
	WORD							m_wOnLineInfoSize;					//信息长度
	tagOnLineCountInfo				m_OnLineCountInfo[64];				//在线信息

	//组件变量
protected:
	CServerList						m_ServerList;						//服务器列表
	CQueueServiceEvent				m_DataBaseEvent;					//数据库通知
	CServerUserManager				m_ServerUserManager;				//用户管理

	//接口变量
protected:
	ITimerEngine					* m_pITimerEngine;					//定时器接口
	IEventService					* m_pIEventService;					//事件接口
	ITCPSocketEngine				* m_pITCPSocketEngine;				//网络接口

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CAttemperEngineSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//调度接口
public:
	//调度模块启动
	virtual bool __cdecl StartService(IUnknownEx * pIUnknownEx);
	//调度模块关闭
	virtual bool __cdecl StopService(IUnknownEx * pIUnknownEx);
	//事件处理接口
	virtual bool __cdecl OnAttemperEvent(WORD wIdentifier, void * pDataBuffer, WORD wDataSize, DWORD dwInsertTime);

	//状态接口
public:
	//关闭查询
	virtual bool __cdecl IsShallClose() { return m_bShallClose; }
	//私聊查询
	virtual bool __cdecl IsAllowWisper() { return m_bAllowWisper; }
	//聊天查询
	virtual bool __cdecl IsAllowRoomChat() { return m_bAllowRoomChat; }
	//聊天查询
	virtual bool __cdecl IsAllowGameChat() { return m_bAllowGameChat; }
	//进入查询
	virtual bool __cdecl IsAllowEnterRoom() { return m_bAllowEnterRoom; }
	//进入查询
	virtual bool __cdecl IsAllowEnterGame() { return m_bAllowEnterGame; }

	//是否为防作弊房间
	virtual bool __cdecl IsFZBRoom() { return m_bFZBRoom; }


	//状态通知
public:
	//发送状态
	virtual bool __cdecl SendTableStatus(WORD wTableID);
	//发送分数
	virtual bool __cdecl SendUserScore(IServerUserItem * pIServerUserItem);
	//发送状态
	virtual bool __cdecl SendUserStatus(IServerUserItem * pIServerUserItem);

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetTableTimer(WORD wTableID, WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam);
	//删除定时器
	virtual bool __cdecl KillTableTimer(WORD wTableID, WORD wTimerID);

	//管理接口
public:
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem);
	//输出信息
	virtual void __cdecl ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel);

	//配置函数 
public:
	//设置事件
	bool SetEventService(IUnknownEx * pIUnknownEx);
	//配置函数
	bool InitAttemperSink(tagAttemperSinkParameter * pAttemperSinkParameter, IUnknownEx * pIUnknownEx);

	//消息函数
public:
	//发送房间消息
	bool SendRoomMessage(WORD wIndex, WORD wRoundID, LPCTSTR lpszMessage, WORD wMessageType);
	//发送游戏消息
	bool SendGameMessage(WORD wIndex, WORD wRoundID, LPCTSTR lpszMessage, WORD wMessageType);
	//发送房间消息
	bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//发送游戏消息
	bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(WORD wTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool __cdecl OnEventDataBase(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent);
	//网络应答事件
	virtual bool __cdecl OnEventSocketAccept(NTY_SocketAcceptEvent * pSocketAcceptEvent);
	//网络读取事件
	virtual bool __cdecl OnEventSocketRead(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent);
	//网络关闭事件
	virtual bool __cdecl OnEventSocketClose(NTY_SocketCloseEvent * pSocketCloseEvent);

	//中心连接事件
public:
	//中心连接事件
	virtual bool __cdecl OnEventCenterSocketConnect(int iErrorCode);
	//中心读取事件
	virtual bool __cdecl OnEventCenterSocketRead(CMD_Command Command, void * pDataBuffer, WORD wDataSize);
	//中心关闭事件
	virtual bool __cdecl OnEventCenterSocketClose(bool bCloseByServer);

	//数据库函数
private:
	//用户登录成功
	bool OnDBLogonSuccess(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent);
	//用户登录失败
	bool OnDBLogonError(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent);

	//网络函数
private:
	//登录消息处理
	bool OnSocketMainLogon(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent);
	//用户消息处理
	bool OnSocketMainUser(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent);
	//管理消息处理
	bool OnSocketMainManager(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent);
	//框架消息处理
	bool OnSocketMainFrame(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent);
	//游戏消息处理
	bool OnSocketMainGame(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent);

	//辅助函数
private:
	//获取用户
	inline IServerUserItem * GetServerUserItem(WORD wIndex);
	//发送失败
	bool SendLogonFailed(WORD wIndex, WORD wRoundID, LPCTSTR pszMessage, LONG lErrorCode);
	//发送用户
	bool SendUserItem(IServerUserItem * pIServerUserItem, WORD wTargetIndex, WORD wTargetRoundID);
	//发送信息
	bool SendGameServerInfo(IServerUserItem * pIServerUserItem, WORD wIndex, WORD wRoundID);

	//中心网络
private:
	//列表消息处理
	bool OnCenterMainServerList(CMD_Command Command, void * pDataBuffer, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif