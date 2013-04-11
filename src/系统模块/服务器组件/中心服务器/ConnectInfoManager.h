#ifndef CONNECT_INFO_MANAGER_HEAD_FILE
#define CONNECT_INFO_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//连接类型
enum enConnectKind
{
	ConnectKind_Unknow,				//未知类型
	ConnectKind_GamePlaza,			//广场类型
	ConnectKind_GameServer,			//房间类型
};

//////////////////////////////////////////////////////////////////////////

//连接信息结构
class CConnectItem
{
	//变量定义
public:
	WORD							m_wRoundID;							//循环索引
	DWORD							m_dwClientIP;						//连接地址
	const enConnectKind				m_ConnectKind;						//连接类型

	//函数定义
public:
	CConnectItem(enConnectKind ConnectKind);
	virtual ~CConnectItem();
};

//////////////////////////////////////////////////////////////////////////

//广场连接信息
class CConnectItem_Plaza : public CConnectItem
{
	//函数定义
public:
	CConnectItem_Plaza();
	virtual ~CConnectItem_Plaza();
};

//////////////////////////////////////////////////////////////////////////

//房间连接信息
class CConnectItem_GameServer : public CConnectItem
{
	//变量定义
public:
	WORD							m_wKindID;							//类型标识
	WORD							m_wServerID;						//房间标识

	//函数定义
public:
	CConnectItem_GameServer();
	virtual ~CConnectItem_GameServer();
};

//////////////////////////////////////////////////////////////////////////

//连接信息管理
class CConnectInfoManager
{
	//函数定义
public:
	CConnectInfoManager();
	virtual ~CConnectInfoManager();
};

//////////////////////////////////////////////////////////////////////////

#endif
