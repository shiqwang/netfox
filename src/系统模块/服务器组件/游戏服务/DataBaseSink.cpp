#include "StdAfx.h"
#include "Afxinet.h"
#include "DataBaseSink.h"

//////////////////////////////////////////////////////////////////////////
extern void WriteLog(CString strFileName, CString strText);
//构造函数
CDataBaseSink::CDataBaseSink()
{
	//设置变量
	m_pIEventService=NULL;
	m_pGameUserDBInfo=NULL;
	m_pGameScoreDBInfo=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CDataBaseSink::~CDataBaseSink()
{
}

//接口查询
void * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CDataBaseSink::StartService(IUnknownEx * pIUnknownEx)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into StartService");
	WriteLog(strFile, strTemp);

	//效验参数
	ASSERT(m_pGameUserDBInfo!=NULL);
	ASSERT(m_pGameScoreDBInfo!=NULL);
	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	//创建实例
	if ((m_GameUserDB.GetInterface()==NULL)&&(m_GameUserDB.CreateInstance()==false))
	{
		m_pIEventService->ShowEventNotify(TEXT("用户数据库对象创建失败"),Level_Exception);
		return false;
	}

	//创建实例
	if ((m_GameScoreDB.GetInterface()==NULL)&&(m_GameScoreDB.CreateInstance()==false))
	{
		m_pIEventService->ShowEventNotify(TEXT("游戏数据库对象创建失败"),Level_Exception);
		return false;
	}

	try
	{
		//变量定义
		BYTE * pcbAddr=NULL;
		TCHAR szDataBaseAddr[16]=TEXT("");

		//连接用户数据库
		pcbAddr=(BYTE *)&m_pGameUserDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);

	strTemp.Format("m_pGameUserDBInfo %s	%d	%s	%s	%s",
		szDataBaseAddr,m_pGameUserDBInfo->wDataBasePort,m_pGameUserDBInfo->szDataBaseName,
			m_pGameUserDBInfo->szDataBaseUser,m_pGameUserDBInfo->szDataBasePass);
	WriteLog(strFile, strTemp);

		m_GameUserDB->SetConnectionInfo(szDataBaseAddr,m_pGameUserDBInfo->wDataBasePort,m_pGameUserDBInfo->szDataBaseName,
			m_pGameUserDBInfo->szDataBaseUser,m_pGameUserDBInfo->szDataBasePass);
		m_GameUserDB->OpenConnection();

		//连接游戏数据库
		pcbAddr=(BYTE *)&m_pGameScoreDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);

	strTemp.Format("m_pGameScoreDBInfo %s	%d	%s	%s	%s",
		szDataBaseAddr,m_pGameScoreDBInfo->wDataBasePort,m_pGameScoreDBInfo->szDataBaseName,
			m_pGameScoreDBInfo->szDataBaseUser,m_pGameScoreDBInfo->szDataBasePass
		);
	WriteLog(strFile, strTemp);
		m_GameScoreDB->SetConnectionInfo(szDataBaseAddr,m_pGameScoreDBInfo->wDataBasePort,m_pGameScoreDBInfo->szDataBaseName,
			m_pGameScoreDBInfo->szDataBaseUser,m_pGameScoreDBInfo->szDataBasePass);
		m_GameScoreDB->OpenConnection();

		return true;
	}
	catch (IADOError * pIADOError)
	{
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return false;
}

//调度模块关闭
bool __cdecl CDataBaseSink::StopService(IUnknownEx * pIUnknownEx)
{
	try
	{
		//关闭连接
		if (m_GameUserDB.GetInterface()) m_GameUserDB->CloseConnection();
		if (m_GameScoreDB.GetInterface()) m_GameScoreDB->CloseConnection();

		return true;
	}
	catch (IADOError * pIADOError)
	{
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return false;
}

//数据操作处理
bool __cdecl CDataBaseSink::OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	switch (DataBaseEvent.wRequestID)
	{
	case DBR_GR_LOGON_BY_ACCOUNTS:		//帐户登录
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_LOGON_BY_USERID:		//I D 登录
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_WRITE_SCORE:			//写分操作
		{
			return OnRequestWriteScore(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_LIMIT_ACCOUNTS:			//禁用帐号
		{
			return OnRequestLimitAccounts(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_SET_USER_RIGHT:			//设置权限
		{
			return OnRequestSetUserRight(DataBaseEvent,pDataBuffer,wDataSize);
		}
	}

	return false;
}

//设置事件
bool CDataBaseSink::SetEventService(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);
	return (m_pIEventService!=NULL);
}

//配置函数
bool CDataBaseSink::InitDataBaseSink(tagDataBaseSinkParameter * pDataBaseSinkParameter, IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pDataBaseSinkParameter!=NULL);

	//设置变量
	m_pGameUserDBInfo=pDataBaseSinkParameter->pGameUserDBInfo;
	m_pGameScoreDBInfo=pDataBaseSinkParameter->pGameScoreDBInfo;
	m_pGameServiceAttrib=pDataBaseSinkParameter->pGameServiceAttrib;
	m_pGameServiceOption=pDataBaseSinkParameter->pGameServiceOption;

	//查询接口
	IServiceEngine * pIServiceEngine=(IServiceEngine *)pIUnknownEx->QueryInterface(IID_IServiceEngine,VER_IServiceEngine);
	ASSERT(pIServiceEngine!=NULL);
	if (pIServiceEngine==NULL) throw TEXT("服务引擎接口查询失败");

	//获取逻辑引擎
	IAttemperEngine * pIAttemperEngine=(IAttemperEngine *)pIServiceEngine->GetAttemperEngine(IID_IAttemperEngine,VER_IAttemperEngine);
	ASSERT(pIAttemperEngine!=NULL);
	if (pIAttemperEngine==NULL) throw TEXT("调度引擎接口查询失败");

	//设置通知组件
	IUnknownEx * pIQueueService=(IUnknownEx *)pIAttemperEngine->GetQueueService(IID_IQueueService,VER_IQueueService);
	if (m_AttemperEvent.SetQueueService(pIQueueService)==false) throw TEXT("逻辑引擎通知接口设置失败");

	return true;
}

//登录请求处理
bool CDataBaseSink::OnRequestLogon(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into OnRequestLogon");
	WriteLog(strFile, strTemp);

	try
	{
		//登录处理
		LONG lReturnValue=-1;
		LPCTSTR pszPassword=NULL;
		switch (DataBaseEvent.wRequestID)
		{
		case DBR_GR_LOGON_BY_ACCOUNTS:		//帐户登录
			{
	strTemp.Format("into 帐户登录");
	WriteLog(strFile, strTemp);

				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GR_LogonByAccounts));
				if (wDataSize!=sizeof(DBR_GR_LogonByAccounts)) return false;

				//执行查询
				DBR_GR_LogonByAccounts * pLogonByAccounts=(DBR_GR_LogonByAccounts *)pDataBuffer;
				lReturnValue=SPLogonByAccounts(pLogonByAccounts->szAccounts,pLogonByAccounts->szPassWord,pLogonByAccounts->dwClientIP);
				pszPassword=pLogonByAccounts->szPassWord;

				break;
			}
		case DBR_GR_LOGON_BY_USERID:		//I D 登录
			{
	strTemp.Format("into I D 登录");
	WriteLog(strFile, strTemp);
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GR_LogonByUserID));
				if (wDataSize!=sizeof(DBR_GR_LogonByUserID)) return false;

				//执行查询
				DBR_GR_LogonByUserID * pLogonByUserID=(DBR_GR_LogonByUserID *)pDataBuffer;
				lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP);
				pszPassword=pLogonByUserID->szPassWord;

	strTemp.Format("pszPassword=%s", pszPassword);
	WriteLog(strFile, strTemp);

				break;
			}
		default :	//默认处理
			{
				ASSERT(FALSE); 
				return false;
			}
		}

		//登录失败
		if (lReturnValue!=0L)
		{
			DBR_GR_LogonError LogonError;
			LogonError.lErrorCode=lReturnValue;
			m_GameScoreDB->GetFieldValue(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,sizeof(LogonError.szErrorDescribe));
			m_AttemperEvent.PostDataBaseEvent(DBR_GR_LOGON_ERROR,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,&LogonError,sizeof(LogonError));
			m_GameScoreDB->CloseRecordset();
			return true;
		}

		//变量定义
		DBR_GR_LogonSuccess LogonSuccess;
		memset(&LogonSuccess,0,sizeof(LogonSuccess));
		CopyMemory(LogonSuccess.szPassWord,pszPassword,sizeof(LogonSuccess.szPassWord));

	strTemp.Format("aaaa");
	WriteLog(strFile, strTemp);

		//读取用户信息
		m_GameScoreDB->GetFieldValue(TEXT("UserID"),LogonSuccess.dwUserID);
		m_GameScoreDB->GetFieldValue(TEXT("Gender"),LogonSuccess.cbGender);
		m_GameScoreDB->GetFieldValue(TEXT("MemberOrder"),LogonSuccess.cbMember);
		m_GameScoreDB->GetFieldValue(TEXT("FaceID"),LogonSuccess.wFaceID);
		m_GameScoreDB->GetFieldValue(TEXT("GroupID"),LogonSuccess.dwGroupID);
		m_GameScoreDB->GetFieldValue(TEXT("Experience"),LogonSuccess.lExperience);
		m_GameScoreDB->GetFieldValue(TEXT("UserRight"),LogonSuccess.dwUserRight);
		m_GameScoreDB->GetFieldValue(TEXT("MasterRight"),LogonSuccess.dwMasterRight);
		m_GameScoreDB->GetFieldValue(TEXT("Accounts"),LogonSuccess.szAccounts,sizeof(LogonSuccess.szAccounts));
		m_GameScoreDB->GetFieldValue(TEXT("GroupName"),LogonSuccess.szGroupName,sizeof(LogonSuccess.szGroupName));

	strTemp.Format("bbbb");
	WriteLog(strFile, strTemp);
		//读取游戏信息
		m_GameScoreDB->GetFieldValue(TEXT("Score"),LogonSuccess.lScore);
		m_GameScoreDB->GetFieldValue(TEXT("WinCount"),LogonSuccess.lWinCount);
		m_GameScoreDB->GetFieldValue(TEXT("LostCount"),LogonSuccess.lLostCount);
		m_GameScoreDB->GetFieldValue(TEXT("DrawCount"),LogonSuccess.lDrawCount);
		m_GameScoreDB->GetFieldValue(TEXT("FleeCount"),LogonSuccess.lFleeCount);

	strTemp.Format("cccc");
	WriteLog(strFile, strTemp);

		//投递调度通知
		m_AttemperEvent.PostDataBaseEvent(DBR_GR_LOGON_SUCCESS,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
			&LogonSuccess,sizeof(LogonSuccess));
	}
	catch (IADOError * pIADOError)
	{
		//输出错误
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);

		//操作失败
		DBR_GR_LogonError LogonError;
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("由于数据库操作异常，请您稍后重试或选择另一游戏服务器！"),sizeof(LogonError.szErrorDescribe));
		m_AttemperEvent.PostDataBaseEvent(DBR_GR_LOGON_ERROR,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
			&LogonError,sizeof(LogonError));
	}

	//关闭记录集
	m_GameScoreDB->CloseRecordset();

	return true;
}

//写分请求
bool CDataBaseSink::OnRequestWriteScore(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteScore));
		if (wDataSize!=sizeof(DBR_GR_WriteScore)) return false;

		//执行查询
		DBR_GR_WriteScore * pWriteScore=(DBR_GR_WriteScore *)pDataBuffer;
		LONG lReturnValue=SPWriteUserScore(pWriteScore->dwUserID,pWriteScore->dwPlayTimeCount,pWriteScore->dwOnlineTimeCount,pWriteScore->dwClientIP,pWriteScore->ScoreModifyInfo);

		//结果处理
		if (lReturnValue!=0L)
		{
		}
	}
	catch (IADOError * pIADOError)
	{
		//输出错误
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return true;
}

//禁用帐户
bool CDataBaseSink::OnRequestLimitAccounts(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LimitAccounts));
		if (wDataSize!=sizeof(DBR_GR_LimitAccounts)) return false;

		//执行查询
		DBR_GR_LimitAccounts * pLimitAccounts=(DBR_GR_LimitAccounts *)pDataBuffer;
		LONG lReturnValue=SPCongealAccounts(pLimitAccounts->dwUserID,pLimitAccounts->dwMasterUserID,pLimitAccounts->dwMasterClientIP);

		return true;
	}
	catch (IADOError * pIADOError)
	{
		//输出错误
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return true;
}

//设置权限
bool CDataBaseSink::OnRequestSetUserRight(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_SetUserRight));
		if (wDataSize!=sizeof(DBR_GR_SetUserRight)) return false;

		//执行查询
		DBR_GR_SetUserRight * pSetUserRight=(DBR_GR_SetUserRight *)pDataBuffer;

		//游戏权限
		if (pSetUserRight->cbGame==TRUE) 
		{
			SPSetUserGameRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		//帐号权限
		if (pSetUserRight->cbAccounts==TRUE) 
		{
			SPSetUserAccountsRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		return true;
	}
	catch (IADOError * pIADOError)
	{
		//输出错误
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return true;
}

//I D 存储过程
LONG CDataBaseSink::SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into SPLogonByUserID %d	%s	%d",  dwUserID,  pszPassword,  dwClientIP);
	WriteLog(strFile, strTemp);

	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	// 123.123.123.123
	strTemp.Format("szClientIP=%s", szClientIP);
	WriteLog(strFile, strTemp);

	//执行存储过程
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_LogonByUserID");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameScoreDB->AddParamter(TEXT("@strPassword"),adParamInput,adChar,lstrlen(pszPassword),_variant_t(pszPassword));
	m_GameScoreDB->AddParamter(TEXT("@wKindID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceAttrib->wKindID));
	m_GameScoreDB->AddParamter(TEXT("@wServerID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceOption->wServerID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));

	strTemp.Format("aaa1 kindid=%d serverid=%d", m_pGameServiceAttrib->wKindID, m_pGameServiceOption->wServerID);
	WriteLog(strFile, strTemp);

	m_GameScoreDB->ExecuteCommand(true);

	strTemp.Format("aaa2");
	WriteLog(strFile, strTemp);

	return m_GameScoreDB->GetReturnValue();
}

//帐号存储过程
LONG CDataBaseSink::SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_LogonByAccounts");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@strAccounts"),adParamInput,adVarChar,lstrlen(pszAccounts),_variant_t(pszAccounts));
	m_GameScoreDB->AddParamter(TEXT("@strPassword"),adParamInput,adChar,lstrlen(pszPassword),_variant_t(pszPassword));
	m_GameScoreDB->AddParamter(TEXT("@wKindID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceAttrib->wKindID));
	m_GameScoreDB->AddParamter(TEXT("@wServerID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceOption->wServerID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(true);

	return 0;
}

//写分存储过程
LONG CDataBaseSink::SPWriteUserScore(DWORD dwUserID, DWORD dwPlayTimeCount, DWORD dwOnLineTimeCount, DWORD dwClientIP, tagUserScore & UserScore)
{

	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s游戏服务.log",strTime);

	strTemp.Format("into SPWriteUserScore %d",  dwUserID);
	WriteLog(strFile, strTemp);

	//效验参数
	ASSERT(dwUserID!=0L);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	strTemp.Format("szClientIP %s",  szClientIP);
	WriteLog(strFile, strTemp);

	//执行存储过程
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_WriteGameScore");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameScoreDB->AddParamter(TEXT("@lScore"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lScore));
	m_GameScoreDB->AddParamter(TEXT("@lWinCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lWinCount));
	m_GameScoreDB->AddParamter(TEXT("@lLostCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lLostCount));
	m_GameScoreDB->AddParamter(TEXT("@lDrawCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lDrawCount));
	m_GameScoreDB->AddParamter(TEXT("@lFleeCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lFleeCount));
	m_GameScoreDB->AddParamter(TEXT("@lExperience"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lExperience));
	m_GameScoreDB->AddParamter(TEXT("@dwPlayTimeCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwPlayTimeCount));
	m_GameScoreDB->AddParamter(TEXT("@dwOnLineTimeCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwOnLineTimeCount));
	m_GameScoreDB->AddParamter(TEXT("@wKindID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceAttrib->wKindID));
	m_GameScoreDB->AddParamter(TEXT("@wServerID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceOption->wServerID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(false);

	strTemp.Format("%d  %d  %d",  m_pGameServiceAttrib->wKindID, m_pGameServiceOption->wServerID, dwUserID);
	WriteLog(strFile, strTemp);

	return m_GameScoreDB->GetReturnValue();
}

//禁号存储过程
LONG CDataBaseSink::SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_GameUserDB->ClearAllParameters();
	m_GameUserDB->SetSPName("GSP_GR_CongealAccounts");
	m_GameUserDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameUserDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameUserDB->AddParamter(TEXT("@dwMasterUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwMasterUserID));
	m_GameUserDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameUserDB->ExecuteCommand(false);

	return m_GameUserDB->GetReturnValue();
}

//权限存储过程
LONG CDataBaseSink::SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_SetUserRight");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameScoreDB->AddParamter(TEXT("@dwUserRight"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserRight));
	m_GameScoreDB->AddParamter(TEXT("@dwMasterUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwMasterUserID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(false);

	return m_GameScoreDB->GetReturnValue();
}

//权限存储过程
LONG CDataBaseSink::SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_GameUserDB->ClearAllParameters();
	m_GameUserDB->SetSPName("GSP_GR_SetUserRight");
	m_GameUserDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameUserDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameUserDB->AddParamter(TEXT("@dwUserRight"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserRight));
	m_GameUserDB->AddParamter(TEXT("@dwMasterUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwMasterUserID));
	m_GameUserDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameUserDB->ExecuteCommand(false);

	return m_GameUserDB->GetReturnValue();
}

//////////////////////////////////////////////////////////////////////////
