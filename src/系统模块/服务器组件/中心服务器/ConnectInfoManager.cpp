#include "StdAfx.h"
#include "ConnectInfoManager.h"


//
//CConnectItem
//
CConnectItem::CConnectItem(enConnectKind ConnectKind) : m_ConnectKind(ConnectKind)
{
	m_wRoundID=0;
	m_dwClientIP=0;
}

CConnectItem::~CConnectItem()
{
}

//
//CConnectItem_Plaza
//
CConnectItem_Plaza::CConnectItem_Plaza() : CConnectItem(ConnectKind_GamePlaza)
{
}

CConnectItem_Plaza::~CConnectItem_Plaza()
{
}

//
//CConnectItem_GameServer
//
CConnectItem_GameServer::CConnectItem_GameServer() : CConnectItem(ConnectKind_GameServer)
{
}

CConnectItem_GameServer::~CConnectItem_GameServer()
{
}

//
//CConnectInfoManager
//
CConnectInfoManager::CConnectInfoManager()
{
}

CConnectInfoManager::~CConnectInfoManager()
{
}


//-----------------------------------------------
//					the end
//-----------------------------------------------
