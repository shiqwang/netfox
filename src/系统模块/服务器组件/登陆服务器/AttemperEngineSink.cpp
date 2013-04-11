#include "StdAfx.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define TIME_RECONNECT						15000L						//重连时间
#define TIME_UPDATE_LIST					30000L						//更新时间

//定时器 ID
#define IDI_CONNECT_CENTER_SERVER			1							//连接服务器
#define IDI_UPDATE_SERVER_LIST				2							//更新列表

//////////////////////////////////////////////////////////////////////////

void WriteLog(CString strFileName, CString strText)
{
	try
	{
        CTime tm = CTime::GetCurrentTime();
        CString strTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
		//BOOL bFull = FALSE;
		CStdioFile file;
		if( file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite) != 0)
		{
			file.SeekToEnd();
			file.WriteString(strTime);
			file.WriteString(strText);
			file.WriteString(_T("\n\n"));
			//if(file.GetLength() > 2000000)
			//	bFull = TRUE;
			file.Close();
		}
		/*
		if(!bFull) return;
		if( file.Open(strFileName, CFile::modeCreate|CFile::modeReadWrite) != 0)
		{
		file.SeekToEnd();
		file.WriteString(strTime);
		file.WriteString(strText);
		file.WriteString(_T("\n"));
		file.Close();
		}
		*/
	}
	catch(...)
	{
	}
}

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量
	m_pInitParamter=NULL;
	m_pConnectItemInfo=NULL;

	//中心服务器
	m_pCenterSocket=NULL;
	m_SocketState=SocketState_NoConnect;

	//接口变量
	m_pITimerEngine=NULL;
	m_pIEventService=NULL;
	m_pITCPSocketEngine=NULL;

	return;
}

//析构函数
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

	//创建辅助变量
	m_pConnectItemInfo=new tagConnectItemInfo [m_pInitParamter->m_wMaxConnect];
	if (m_pConnectItemInfo==NULL) return false;
	memset(m_pConnectItemInfo,0,sizeof(tagConnectItemInfo)*m_pInitParamter->m_wMaxConnect);

	//连接中心服务器
	DWORD dwServerIP=inet_addr(m_pInitParamter->m_szCenterServerAddr);
	m_pCenterSocket->Connect(dwServerIP,PORT_CENTER_SERVER);

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::StopService(IUnknownEx * pIUnknownEx)
{
	//删除数据
	SafeDeleteArray(m_pConnectItemInfo);

	return true;
}

//事件处理接口
bool __cdecl CAttemperEngineSink::OnAttemperEvent(WORD wIdentifier, void * pDataBuffer, WORD wDataSize, DWORD dwInsertTime)
{
	switch (wIdentifier)
	{
	case EVENT_CENTER_CONNECT:	//中心连接事件
		{
			//效验大小
			ASSERT(wDataSize==sizeof(NTY_CenterConnect));
			if (wDataSize!=sizeof(NTY_CenterConnect)) return false;

			//消息处理
			NTY_CenterConnect * pCenterSocketConnect=(NTY_CenterConnect *)pDataBuffer;

			return OnEventCenterSocketConnect(pCenterSocketConnect->iErrorCode);
		}
	case EVENT_CENTER_READ:		//中心读取事件
		{
			//效验大小
			NTY_CenterRead * pCenterSocketRead=(NTY_CenterRead *)pDataBuffer;
			ASSERT(wDataSize>=((sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer))));
			ASSERT(wDataSize==(sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer)+pCenterSocketRead->wDataSize));
			if (wDataSize<(sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer))) return false;
			if (wDataSize!=sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer)+pCenterSocketRead->wDataSize) return false;

			//处理消息
			bool bSuccess=false;
			try 
			{ 
				bSuccess=OnEventCenterSocketRead(pCenterSocketRead->Command,pCenterSocketRead->cbReadBuffer,pCenterSocketRead->wDataSize);
			}
			catch (...)	{ }
			if (bSuccess==false) m_pCenterSocket->CloseSocket(true);

			return true;
		}
	case EVENT_CENTER_CLOSE:		//中心关闭事件
		{
			//效验大小
			ASSERT(wDataSize==sizeof(NTY_CenterClose));
			if (wDataSize!=sizeof(NTY_CenterClose)) return false;

			//消息处理
			NTY_CenterClose * pCenterSocketClose=(NTY_CenterClose *)pDataBuffer;
			OnEventCenterSocketClose(pCenterSocketClose->bCloseByServer);

			return true;
		}
	}

	return false;
}

//设置事件
bool CAttemperEngineSink::SetEventService(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);

	//设置接口
	m_ServerList.SetEventService(m_pIEventService);

	return (m_pIEventService!=NULL);
}

//配置函数
bool CAttemperEngineSink::InitAttemperSink(CInitParamter * pInitParamter, CCenterSocket * pCenterSocket, IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pInitParamter!=NULL);
	ASSERT(pCenterSocket!=NULL);

	//设置变量
	m_pInitParamter=pInitParamter;
	m_pCenterSocket=pCenterSocket;

	//服务引擎
	IServiceEngine * pIServiceEngine=(IServiceEngine *)pIUnknownEx->QueryInterface(IID_IServiceEngine,VER_IServiceEngine);
	if (pIServiceEngine==NULL)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取服务引擎服务失败"),Level_Exception);
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
		m_pIEventService->ShowEventNotify(TEXT("获取网络引擎服务失败"),Level_Exception);
		return false;
	}

	//数据库引擎
	IDataBaseEngine * pIDataBaseEngine=(IDataBaseEngine *)pIServiceEngine->GetDataBaseEngine(IID_IDataBaseEngine,VER_IDataBaseEngine);
	if (pIDataBaseEngine==NULL)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取数据库引擎服务失败"),Level_Exception);
		return false;
	}

	//数据库触发
	IUnknownEx * pIQueueService=(IUnknownEx *)pIDataBaseEngine->GetQueueService(IID_IUnknownEx,VER_IUnknownEx);
	if (m_DataBaseEvent.SetQueueService(pIQueueService)==false)
	{
		m_pIEventService->ShowEventNotify(TEXT("获取数据库触发服务失败"),Level_Exception);
		return false;
	}

	//列表组件
	if (m_ServerList.SetSocketEngine(m_pITCPSocketEngine)==false)
	{
		m_pIEventService->ShowEventNotify(TEXT("列表组件与网络引擎绑定失败"),Level_Exception);
		return false;
	}

	return true;
}

//定时器事件
bool CAttemperEngineSink::OnEventTimer(WORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_CONNECT_CENTER_SERVER:		//连接中心服务器
		{
			DWORD dwServerIP=inet_addr(m_pInitParamter->m_szCenterServerAddr);
			m_pCenterSocket->Connect(dwServerIP,PORT_CENTER_SERVER);
			return true;
		}
	case IDI_UPDATE_SERVER_LIST:		//更新服务器列表
		{
			if (m_SocketState==SocketState_Connected)
			{
				m_pCenterSocket->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);
			}
			return true;
		}
	}
	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)
{
	switch (pDataBaseEvent->wRequestID)
	{
	case DBR_GP_LOGON_SUCCESS:		//登录成功
		{
			return OnDBLogonSuccess(pDataBuffer,wDataSize,pDataBaseEvent);
		}
	case DBR_GP_LOGON_ERROR:		//登录失败
		{
			return OnDBLogonError(pDataBuffer,wDataSize,pDataBaseEvent);
		}
	}

	return false;
}

//用户登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)
{
	//效验参数
	ASSERT(pDataBaseEvent->wRequestID==DBR_GP_LOGON_SUCCESS);
	ASSERT(wDataSize==sizeof(DBR_GP_LogonSuccess));
	if (wDataSize!=sizeof(DBR_GP_LogonSuccess)) return false;

	//判断在线
	ASSERT(pDataBaseEvent->wIndex<m_pInitParamter->m_wMaxConnect);
	tagConnectItemInfo * pConnectItemInfo=(m_pConnectItemInfo+pDataBaseEvent->wIndex);
	if (pConnectItemInfo->bConnect==FALSE) return true;
	if (pConnectItemInfo->wRoundID!=pDataBaseEvent->wRoundID) return true;

	//构造数据
	BYTE cbBuffer[SOCKET_PACKAGE];
	CMD_GP_LogonSuccess * pCMDLogonSuccess=(CMD_GP_LogonSuccess *)cbBuffer;
	DBR_GP_LogonSuccess * pDBRLogonSuccess=(DBR_GP_LogonSuccess *)pDataBuffer;
	pCMDLogonSuccess->cbGender=pDBRLogonSuccess->cbGender;
	pCMDLogonSuccess->cbMember=pDBRLogonSuccess->cbMember;
	pCMDLogonSuccess->wFaceID=pDBRLogonSuccess->wFaceID;
	pCMDLogonSuccess->dwUserID=pDBRLogonSuccess->dwUserID;
	pCMDLogonSuccess->dwGroupID=pDBRLogonSuccess->dwGroupID;
	pCMDLogonSuccess->dwExperience=pDBRLogonSuccess->dwExperience;
	pCMDLogonSuccess->dwUserRight=pDBRLogonSuccess->dwUserRight;
	pCMDLogonSuccess->dwMasterRight=pDBRLogonSuccess->dwMasterRight;

	//叠加数据
	CSendPacketHelper SendPacket(cbBuffer+sizeof(CMD_GP_LogonSuccess),sizeof(cbBuffer)-sizeof(CMD_GP_LogonSuccess));
	SendPacket.AddPacket(pDBRLogonSuccess->szAccounts,CountString(pDBRLogonSuccess->szAccounts),DTP_USER_ACCOUNTS);
	SendPacket.AddPacket(pDBRLogonSuccess->szPassWord,CountString(pDBRLogonSuccess->szPassWord),DTP_USER_PASS);

	//社团信息
	if (pDBRLogonSuccess->dwGroupID!=0L)
	{
		ASSERT(pDBRLogonSuccess->szGroupName[0]!=0);
		SendPacket.AddPacket(pDBRLogonSuccess->szGroupName,CountString(pDBRLogonSuccess->szGroupName),DTP_USER_GROUP_NAME);
	}
	
	//站点主页
	if (m_pInitParamter->m_szMainPage[0]!=0)
	{
		SendPacket.AddPacket(m_pInitParamter->m_szMainPage,CountString(m_pInitParamter->m_szMainPage),DTP_STATION_PAGE);
	}

	//发送登录结果
	WORD wIndex=pDataBaseEvent->wIndex;
	WORD wRoundID=pDataBaseEvent->wRoundID;
	WORD wSendSize=sizeof(CMD_GP_LogonSuccess)+SendPacket.GetDataSize();
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,cbBuffer,wSendSize);

	//列表配置
	CMD_GP_ListConfig ListConfig;
	ListConfig.bShowOnLineCount=TRUE;
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GP_SERVER_LIST,SUB_GP_LIST_CONFIG,&ListConfig,sizeof(ListConfig));

	//发送游戏列表
	m_ServerList.SendGameTypeList(wIndex,wRoundID);
	m_ServerList.SendGameKindList(wIndex,wRoundID);
	m_ServerList.SendGameStationList(wIndex,wRoundID);
	m_ServerList.SendGameServerList(wIndex,wRoundID);

	//发送登录完成
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GP_LOGON,SUB_GP_LOGON_FINISH);

	//关闭连接
	m_pITCPSocketEngine->ShutDownSocket(wIndex,wRoundID);

	return true;
}

//用户登录失败
bool CAttemperEngineSink::OnDBLogonError(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)
{
	//效验参数
	ASSERT(pDataBaseEvent->wRequestID==DBR_GP_LOGON_ERROR);
	ASSERT(wDataSize==sizeof(DBR_GP_LogonError));

	//判断在线
	ASSERT(pDataBaseEvent->wIndex<m_pInitParamter->m_wMaxConnect);
	tagConnectItemInfo * pConnectItemInfo=(m_pConnectItemInfo+pDataBaseEvent->wIndex);
	if (pConnectItemInfo->bConnect==FALSE) return true;
	if (pConnectItemInfo->wRoundID!=pDataBaseEvent->wRoundID) return true;

	//变量定义
	CMD_GP_LogonError LogonError;
	DBR_GP_LogonError * pLogonError=(DBR_GP_LogonError *)pDataBuffer;
	pLogonError->szErrorDescribe[CountString(pLogonError->szErrorDescribe)-1]=0;

	//构造数据
	LogonError.lErrorCode=pLogonError->lErrorCode;
	lstrcpyn(LogonError.szErrorDescribe,pLogonError->szErrorDescribe,sizeof(LogonError.szErrorDescribe));
	WORD wDescribeSize=CountString(LogonError.szErrorDescribe);

	//发送数据
	WORD wSendSize=sizeof(LogonError)-sizeof(LogonError.szErrorDescribe)+wDescribeSize;
	m_pITCPSocketEngine->SendData(pDataBaseEvent->wIndex,pDataBaseEvent->wRoundID,MDM_GP_LOGON,
		SUB_GP_LOGON_ERROR,&LogonError,wSendSize);

	//关闭连接
	m_pITCPSocketEngine->ShutDownSocket(pDataBaseEvent->wIndex,pDataBaseEvent->wRoundID);

	return true;
}

//网络应答事件
bool __cdecl CAttemperEngineSink::OnEventSocketAccept(NTY_SocketAcceptEvent * pSocketAcceptEvent)
{
	//效验参数
	ASSERT(pSocketAcceptEvent->wIndex<m_pInitParamter->m_wMaxConnect);

	//设置数据
	tagConnectItemInfo * pConnectItemInfo=m_pConnectItemInfo+pSocketAcceptEvent->wIndex;
	pConnectItemInfo->bConnect=TRUE;
	pConnectItemInfo->wRoundID=pSocketAcceptEvent->wRoundID;
	pConnectItemInfo->dwClientIP=pSocketAcceptEvent->dwClientIP;

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
	//登录消息
	case MDM_GP_LOGON:				
		{
			return OnSocketMainLogon(Command,pDataBuffer,wDataSize,pSocketReadEvent);
		}
	}

	return false;
}

//网络关闭事件
bool __cdecl CAttemperEngineSink::OnEventSocketClose(NTY_SocketCloseEvent * pSocketCloseEvent)
{
	//效验参数
	ASSERT(pSocketCloseEvent->wIndex<m_pInitParamter->m_wMaxConnect);

	//设置数据
	tagConnectItemInfo * pConnectItemInfo=m_pConnectItemInfo+pSocketCloseEvent->wIndex;
	pConnectItemInfo->wRoundID++;
	pConnectItemInfo->dwClientIP=0;
	pConnectItemInfo->bConnect=FALSE;

	return true;
}

//中心连接事件
bool __cdecl CAttemperEngineSink::OnEventCenterSocketConnect(int iErrorCode)
{
	//错误判断
	if (iErrorCode!=0)
	{
		m_SocketState=SocketState_NoConnect;
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);
		return true;
	}

	//设置变量
	m_SocketState=SocketState_Connected;

	//获取列表
	m_pCenterSocket->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST,TIME_UPDATE_LIST,TIMER_REPEAT_TIMER,NULL);

	return true;
}

//中心读取事件
bool __cdecl CAttemperEngineSink::OnEventCenterSocketRead(CMD_Command Command, void * pDataBuffer, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:	//列表消息
		{
			return OnCenterMainServerList(Command,pDataBuffer,wDataSize);
		}
	}
	return true;
}

//中心关闭事件
bool __cdecl CAttemperEngineSink::OnEventCenterSocketClose(bool bCloseByServer)
{
	//设置变量
	m_SocketState=SocketState_NoConnect;
	m_pITimerEngine->KillTimer(IDI_UPDATE_SERVER_LIST);

	//重新连接
	if (bCloseByServer==true) m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

	return true;
}

//登录消息处理
bool CAttemperEngineSink::OnSocketMainLogon(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnSocketMainLogon.log",strTime);

	strTemp.Format("into OnSocketMainLogon");
	WriteLog(strFile, strTemp);

	ASSERT(Command.wMainCmdID==MDM_GP_LOGON);
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LOGON_ACCOUNTS:		//帐号登录
		{

	strTemp.Format("into 帐号登录");
	WriteLog(strFile, strTemp);

			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonByAccounts));
			if (wDataSize<sizeof(CMD_GP_LogonByAccounts)) return false;

			//处理消息
			CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)pDataBuffer;
			pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
			pLogonByAccounts->szPassWord[CountArray(pLogonByAccounts->szPassWord)-1]=0;
			pLogonByAccounts->szRealityPass[CountArray(pLogonByAccounts->szRealityPass)-1]=0;

			//连接信息
			ASSERT(pSocketReadEvent->wIndex<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pConnectItemInfo+pSocketReadEvent->wIndex)->dwClientIP;

	strTemp.Format("dwPlazaVersion=%d,%d", pLogonByAccounts->dwPlazaVersion, VER_PLAZA_FRAME);
	WriteLog(strFile, strTemp);

			//效验版本
			if (pLogonByAccounts->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pLogonByAccounts->dwPlazaVersion);
				WORD wHigh=HIWORD(pLogonByAccounts->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				WORD wIndex=pSocketReadEvent->wIndex;
				WORD wRountID=pSocketReadEvent->wRoundID;
				m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					//m_pITCPSocketEngine->ShutDownSocket(wIndex,wRountID);
					return true;
				}
			}

			//构造数据
			DBR_GP_LogonByAccounts LogonByAccounts;
			memset(&LogonByAccounts,0,sizeof(LogonByAccounts));
			LogonByAccounts.dwClientIP=dwClientAddr;
			lstrcpyn(LogonByAccounts.szAccounts,pLogonByAccounts->szAccounts,sizeof(LogonByAccounts.szAccounts));
			lstrcpyn(LogonByAccounts.szPassWord,pLogonByAccounts->szPassWord,sizeof(LogonByAccounts.szPassWord));
			lstrcpyn(LogonByAccounts.szRealityPass,pLogonByAccounts->szRealityPass,sizeof(LogonByAccounts.szRealityPass));

			//投递数据库
			m_DataBaseEvent.PostDataBaseEvent(DBR_GP_LOGON_BY_ACCOUNTS,pSocketReadEvent->wIndex,
				pSocketReadEvent->wRoundID,&LogonByAccounts,sizeof(LogonByAccounts));

			return true;
		}
	case SUB_GP_LOGON_USERID:		//ID 登录
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonByUserID));
			if (wDataSize<sizeof(CMD_GP_LogonByUserID)) return false;

			//处理消息
			CMD_GP_LogonByUserID * pLogonByUserID=(CMD_GP_LogonByUserID *)pDataBuffer;
			pLogonByUserID->szPassWord[CountArray(pLogonByUserID->szPassWord)-1]=0;

			//连接信息
			ASSERT(pSocketReadEvent->wIndex<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pConnectItemInfo+pSocketReadEvent->wIndex)->dwClientIP;

	strTemp.Format("2 dwPlazaVersion=%d,%d", pLogonByUserID->dwPlazaVersion, VER_PLAZA_FRAME);
	WriteLog(strFile, strTemp);

			//效验版本
			if (pLogonByUserID->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pLogonByUserID->dwPlazaVersion);
				WORD wHigh=HIWORD(pLogonByUserID->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				WORD wIndex=pSocketReadEvent->wIndex;
				WORD wRountID=pSocketReadEvent->wRoundID;
				m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					//m_pITCPSocketEngine->ShutDownSocket(wIndex,wRountID);
					return true;
				}
			}

			//构造数据
			DBR_GP_LogonByUserID LogonByUserID;
			memset(&LogonByUserID,0,sizeof(LogonByUserID));
			LogonByUserID.dwClientIP=dwClientAddr;
			LogonByUserID.dwUserID=pLogonByUserID->dwUserID;
			lstrcpyn(LogonByUserID.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonByUserID.szPassWord));

			//投递数据库
			m_DataBaseEvent.PostDataBaseEvent(DBR_GP_LOGON_BY_USERID,pSocketReadEvent->wIndex,
				pSocketReadEvent->wRoundID,&LogonByUserID,sizeof(LogonByUserID));

			return true;
		}
	case SUB_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_RegisterAccounts));
			if (wDataSize<sizeof(CMD_GP_RegisterAccounts)) return false;

			//处理消息
			CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)pDataBuffer;
			pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
			pRegisterAccounts->szPassWord[CountArray(pRegisterAccounts->szPassWord)-1]=0;

			//连接信息
			ASSERT(pSocketReadEvent->wIndex<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pConnectItemInfo+pSocketReadEvent->wIndex)->dwClientIP;

			//效验版本
			if (pRegisterAccounts->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pRegisterAccounts->dwPlazaVersion);
				WORD wHigh=HIWORD(pRegisterAccounts->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				WORD wIndex=pSocketReadEvent->wIndex;
				WORD wRountID=pSocketReadEvent->wRoundID;
				m_pITCPSocketEngine->SendData(wIndex,wRountID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					m_pITCPSocketEngine->ShutDownSocket(wIndex,wRountID);
					return true;
				}
			}

			//构造数据
			DBR_GP_RegisterAccounts RegisterAccounts;
			memset(&RegisterAccounts,0,sizeof(RegisterAccounts));
			RegisterAccounts.dwClientIP=dwClientAddr;
			RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
			lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,sizeof(RegisterAccounts.szAccounts));
			lstrcpyn(RegisterAccounts.szPassWord,pRegisterAccounts->szPassWord,sizeof(RegisterAccounts.szPassWord));

			//投递数据库
			m_DataBaseEvent.PostDataBaseEvent(DBR_GP_REGISTER_ACCOUNTS,pSocketReadEvent->wIndex,
				pSocketReadEvent->wRoundID,&RegisterAccounts,sizeof(RegisterAccounts));

			return true;
		}
	}
	return false;
}

//列表消息处理
bool CAttemperEngineSink::OnCenterMainServerList(CMD_Command Command, void * pDataBuffer, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_CS_SERVER_LIST);
	switch (Command.wSubCmdID)
	{
	case SUB_CS_LIST_INFO:		//列表信息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ListInfo));
			if (wDataSize!=sizeof(CMD_CS_ListInfo)) return false;

			//消息处理
			m_ServerList.ResetServerListBuffer();

			return true;
		}
	case SUB_CS_LIST_TYPE:		//种类信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameType));
			ASSERT(wDataSize%sizeof(tagGameType)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameType);
			tagGameType * pGameType=(tagGameType *)pDataBuffer;
			m_ServerList.AppendGameTypeBuffer(pGameType,dwCount);

			return true;
		}
	case SUB_CS_LIST_KIND:		//类型信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameKind));
			ASSERT(wDataSize%sizeof(tagGameKind)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameKind);
			tagGameKind * pGameKind=(tagGameKind *)pDataBuffer;
			m_ServerList.AppendGameKindBuffer(pGameKind,dwCount);

			return true;
		}
	case SUB_CS_LIST_STATION:	//站点信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameStation));
			ASSERT(wDataSize%sizeof(tagGameStation)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameStation);
			tagGameStation * pGameStation=(tagGameStation *)pDataBuffer;
			m_ServerList.AppendGameStationBuffer(pGameStation,dwCount);

			return true;
		}
	case SUB_CS_LIST_SERVER:	//房间信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameServer));
			ASSERT(wDataSize%sizeof(tagGameServer)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pDataBuffer;
			m_ServerList.AppendGameServerBuffer(pGameServer,dwCount);

			return true;
		}
	case SUB_CS_LIST_FINISH:	//更新完成
		{
			//消息处理
			m_ServerList.ActiveServerListBuffer();

			return true;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
