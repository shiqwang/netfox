#include "StdAfx.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"

CAttemperEngineSink::CAttemperEngineSink()
{
	m_pInitParamter=NULL;
	m_pITimerEngine=NULL;
	m_pIEventService=NULL;
	m_pITCPSocketEngine=NULL;
}

CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
void * __cdecl CAttemperEngineSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CAttemperEngineSink::StartService(IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(m_pInitParamter!=NULL);
	ASSERT(m_pIEventService!=NULL);

	//创建组件
	if (m_ServerListCenter.GetInterface()==NULL)
	{
		if (m_ServerListCenter.CreateInstance()==false)
		{
			m_pIEventService->ShowEventNotify(TEXT("中心服务器列表组件创建失败"),
											Level_Exception);
			return false;
		}
		m_ServerListCenter->SetEventService(m_pIEventService);
	}

	//加载列表
	tagDataBaseInfo DataBaseInfo;
	DataBaseInfo.wDataBasePort=m_pInitParamter->m_wServerDataBasePort;
	DataBaseInfo.dwDataBaseAddr=inet_addr(m_pInitParamter->m_szServerDataBaseAddr);
	lstrcpyn(DataBaseInfo.szDataBaseUser,m_pInitParamter->m_szServerDataBaseUser,CountArray(DataBaseInfo.szDataBaseUser));
	lstrcpyn(DataBaseInfo.szDataBasePass,m_pInitParamter->m_szServerDataBasePass,CountArray(DataBaseInfo.szDataBasePass));
	lstrcpyn(DataBaseInfo.szDataBaseName,m_pInitParamter->m_szServerDataBaseName,CountArray(DataBaseInfo.szDataBaseName));
	if (m_ServerListCenter->LoadServerList(DataBaseInfo)==false) 
	{
		m_pIEventService->ShowEventNotify(TEXT("游戏列表加载失败"),Level_Exception);
		return false;
	}

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::StopService(IUnknownEx * pIUnknownEx)
{
	//重置列表
	if (m_ServerListCenter.GetInterface()!=NULL) 
	{
		m_ServerListCenter->ResetServerList();
	}

	return true;
}

//事件处理接口
bool __cdecl CAttemperEngineSink::OnAttemperEvent(WORD wIdentifier, 
												  void * pDataBuffer, 
												  WORD wDataSize, 
												  DWORD dwInsertTime)
{
	return false;
}

//设置事件
bool CAttemperEngineSink::SetEventService(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);

	//设置组件
	if (m_ServerListCenter.GetInterface()!=NULL)
	{
		m_ServerListCenter->SetEventService(m_pIEventService);
	}

	return (m_pIEventService!=NULL);
}

//配置函数
bool CAttemperEngineSink::InitAttemperSink(CInitParamter * pInitParamter, 
										   IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pInitParamter!=NULL);

	//设置变量
	m_pInitParamter=pInitParamter;

	//服务引擎
	IServiceEngine * pIServiceEngine=(IServiceEngine *)pIUnknownEx->QueryInterface(IID_IServiceEngine,VER_IServiceEngine);
	if (pIServiceEngine==NULL)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取服务引擎服务失败"),
										Level_Exception);
		return false;
	}

	//定时器引擎
	m_pITimerEngine=(ITimerEngine *)pIServiceEngine->GetTimerEngine(IID_ITimerEngine,VER_ITimerEngine);
	if (m_pITimerEngine==NULL)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取定时器引擎服务失败"),Level_Exception);
		return false;
	}

	//网络引擎
	m_pITCPSocketEngine=(ITCPSocketEngine *)pIServiceEngine->GetTCPSocketEngine(IID_ITCPSocketEngine,VER_ITCPSocketEngine);
	if (m_pITCPSocketEngine==NULL)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取网络引擎服务失败"),
										Level_Exception);
		return false;
	}

	//数据库引擎
	IDataBaseEngine * pIDataBaseEngine=(IDataBaseEngine *)pIServiceEngine->GetDataBaseEngine(IID_IDataBaseEngine,VER_IDataBaseEngine);
	if (pIDataBaseEngine==NULL)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取数据库引擎服务失败"),
										Level_Exception);
		return false;
	}

	//数据库触发
	IUnknownEx * pIQueueService=(IUnknownEx *)pIDataBaseEngine->GetQueueService(IID_IUnknownEx,VER_IUnknownEx);
	if (m_DataBaseEvent.SetQueueService(pIQueueService)==false)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取数据库触发服务失败"),
											Level_Exception);
		return false;
	}

	return true;
}

//定时器事件
bool __cdecl CAttemperEngineSink::OnEventTimer(WORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//数据库事件
bool __cdecl CAttemperEngineSink::OnEventDataBase(void * pDataBuffer, 
												  WORD wDataSize, 
												  NTY_DataBaseEvent * pDataBaseEvent)
{
	return false;
}

//网络应答事件
bool __cdecl CAttemperEngineSink::OnEventSocketAccept(NTY_SocketAcceptEvent * pSocketAcceptEvent)
{
	return true;
}

//网络读取事件
bool __cdecl CAttemperEngineSink::OnEventSocketRead(CMD_Command Command, 
													void * pDataBuffer, 
													WORD wDataSize, 
													NTY_SocketReadEvent * pSocketReadEvent)
{
	switch (Command.wMainCmdID)
	{
	//列表消息
	case MDM_CS_SERVER_LIST:			
		{
			return OnSocketMainServerList(Command,
									pDataBuffer,
									wDataSize,
									pSocketReadEvent);
		}

	//房间管理
	case MDM_CS_SERVER_MANAGER:			
		{
			return OnSocketMainServerManager(Command,
									pDataBuffer,
									wDataSize,
									pSocketReadEvent);
		}
	}

	return false;
}

//网络关闭事件
bool __cdecl CAttemperEngineSink::OnEventSocketClose(NTY_SocketCloseEvent * pSocketCloseEvent)
{
	return true;
}

//列表消息处理
bool CAttemperEngineSink::OnSocketMainServerList(CMD_Command Command, 
												 void * pDataBuffer, 
												 WORD wDataSize, 
												 NTY_SocketReadEvent * pSocketReadEvent)
{
	ASSERT(Command.wMainCmdID==MDM_CS_SERVER_LIST);
	switch (Command.wSubCmdID)
	{
	//获取列表
	case SUB_CS_GET_SERVER_LIST:	
		{
			//变量定义
			WORD wIndex=pSocketReadEvent->wIndex;
			WORD wRountID=pSocketReadEvent->wRoundID;

			//列表信息
			CMD_CS_ListInfo ListInfo;
			memset(&ListInfo,0,sizeof(ListInfo));
			ListInfo.dwTypeCount=m_ServerListCenter->GetGameTypeCount();
			ListInfo.dwKindCount=m_ServerListCenter->GetGameKindCount();
			ListInfo.dwServerCount=m_ServerListCenter->GetGameServerCount();
			ListInfo.dwStationCount=m_ServerListCenter->GetGameStationCount();
			m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_INFO,&ListInfo,sizeof(ListInfo));

			//类型信息
			POSITION Pos=NULL;
			WORD wSendSize=0L;
			BYTE cbBuffer[SOCKET_PACKAGE];
			for (DWORD i=0;i<ListInfo.dwTypeCount;i++)
			{
				if ((wSendSize+sizeof(tagGameType))>sizeof(cbBuffer))
				{
					m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_TYPE,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameTypeItem * pGameTypeItem=m_ServerListCenter->EmunGameTypeItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameTypeItem->GameType,sizeof(tagGameType));
				wSendSize+=sizeof(tagGameType);
			}
			if (wSendSize>0) m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_TYPE,cbBuffer,wSendSize);

			//种类信息
			Pos=NULL;
			wSendSize=0L;
			for (DWORD i=0;i<ListInfo.dwKindCount;i++)
			{
				if ((wSendSize+sizeof(tagGameKind))>sizeof(cbBuffer))
				{
					m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_KIND,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameKindItem * pGameKindItem=m_ServerListCenter->EmunGameKindItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameKindItem->GameKind,sizeof(tagGameKind));
				wSendSize+=sizeof(tagGameKind);
			}
			if (wSendSize>0) m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_KIND,cbBuffer,wSendSize);

			//站点信息
			Pos=NULL;
			wSendSize=0L;
			for (DWORD i=0;i<ListInfo.dwStationCount;i++)
			{
				if ((wSendSize+sizeof(tagGameStation))>sizeof(cbBuffer))
				{
					m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_STATION,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameStationItem * pGameStationItem=m_ServerListCenter->EmunGameStationItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameStationItem->GameStation,sizeof(tagGameStation));
				wSendSize+=sizeof(tagGameStation);
			}
			if (wSendSize>0) m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_STATION,cbBuffer,wSendSize);

			//房间信息
			Pos=NULL;
			wSendSize=0L;
			for (DWORD i=0;i<ListInfo.dwServerCount;i++)
			{
				if ((wSendSize+sizeof(tagGameServer))>sizeof(cbBuffer))
				{
					m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_SERVER,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameServerItem * pGameServerItem=m_ServerListCenter->EmunGameServerItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameServerItem->GameServer,sizeof(tagGameServer));
				wSendSize+=sizeof(tagGameServer);
			}
			if (wSendSize>0) m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_SERVER,cbBuffer,wSendSize);

			//发送完成
			m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_CS_SERVER_LIST,SUB_CS_LIST_FINISH);

			return true;	
		}
	}

	return false;
}

//房间消息处理
bool CAttemperEngineSink::OnSocketMainServerManager(CMD_Command Command, 
													void * pDataBuffer, 
													WORD wDataSize, 
													NTY_SocketReadEvent * pSocketReadEvent)
{
	ASSERT(Command.wMainCmdID==MDM_CS_SERVER_MANAGER);
	switch (Command.wSubCmdID)
	{
	//房间注册
	case SUB_CS_REG_GAME_SERVER:		
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_RegGameServer));
			if (wDataSize!=sizeof(CMD_CS_RegGameServer)) return false;

			//消息处理
			CMD_CS_RegGameServer * pRegGameServer=(CMD_CS_RegGameServer *)pDataBuffer;
			tagGameServer * pGameServer=&pRegGameServer->GameServer;
			tagGameServerItem * pGameServerItem=m_ServerListCenter->SearchGameServer(pGameServer->wKindID,pGameServer->wServerID);
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->dwUpdateTime=(DWORD)time(NULL);
				CopyMemory(&pGameServerItem->GameServer,pGameServer,sizeof(tagGameServer));
			}
			else m_ServerListCenter->InsertGameServer(pGameServer);

			return true;
		}

	//注销房间
	case SUB_CS_UNREG_GAME_SERVER:		
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_UnRegGameServer));
			if (wDataSize!=sizeof(CMD_CS_UnRegGameServer)) return false;

			//消息处理
			CMD_CS_UnRegGameServer * pUnRegGameServer=(CMD_CS_UnRegGameServer *)pDataBuffer;
			m_ServerListCenter->DeleteGameServer(pUnRegGameServer->wKindID,pUnRegGameServer->wServerID);

			return true;
		}

	//更新人数
	case SUB_CS_SERVER_ONLINE_COUNT:	
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ServerOnLineCount));
			if (wDataSize!=sizeof(CMD_CS_ServerOnLineCount)) return false;

			//消息处理
			CMD_CS_ServerOnLineCount * pServerOnLineCount=(CMD_CS_ServerOnLineCount *)pDataBuffer;
			WORD wKindID=pServerOnLineCount->wKindID;
			WORD wServerID=pServerOnLineCount->wServerID;
			m_ServerListCenter->UpdateServerOnLineCount(wKindID,wServerID,pServerOnLineCount->dwOnLineCount);

			return true;
		}
	}

	return false;
}


//-----------------------------------------------
//					the end
//-----------------------------------------------
