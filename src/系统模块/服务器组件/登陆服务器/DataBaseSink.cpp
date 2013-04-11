#include "StdAfx.h"
#include "Commonality.h"
#include "DataBaseSink.h"

//判断是否过期
extern int IsTimeOut();
//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseSink::CDataBaseSink()
{
	m_pInitParamter=NULL;
	m_pIEventService=NULL;
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

//模块启动
bool __cdecl CDataBaseSink::StartService(IUnknownEx * pIUnknownEx)
{
	//如果超时，退出
if ( IsTimeOut())
{
return 0;
}

	//效验参数
	ASSERT(m_pInitParamter!=NULL);
	ASSERT(m_pIEventService!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	//创建实例
	if (m_UserDataBase.GetInterface()!=NULL==false)
	{
		if (m_UserDataBase.CreateInstance()==false)
		{
			m_pIEventService->ShowEventNotify(TEXT("用户数据库对象创建失败"),Level_Exception);
			return false;
		}
	}

	try
	{
		//连接用户数据库
		m_UserDataBase->SetConnectionInfo(m_pInitParamter->m_szUserDataBaseAddr,m_pInitParamter->m_wUserDataBasePort,
			m_pInitParamter->m_szUserDataBaseName,m_pInitParamter->m_szUserDataBaseUser,m_pInitParamter->m_szUserDataBasePass);
		m_UserDataBase->OpenConnection();

		return true;
	}
	catch (IADOError * pIADOError)
	{
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return false;
}

//模块关闭
bool __cdecl CDataBaseSink::StopService(IUnknownEx * pIUnknownEx)
{
	try
	{
		if (m_UserDataBase.GetInterface()!=NULL) m_UserDataBase->CloseConnection();

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
	case DBR_GP_LOGON_BY_ACCOUNTS:		//帐号登录
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GP_LOGON_BY_USERID:		//I D 登录
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
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
bool CDataBaseSink::InitDataBaseSink(CInitParamter * pInitParamter, IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pInitParamter!=NULL);

	//设置变量
	m_pInitParamter=pInitParamter;

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
	//如果超时，退出
if ( IsTimeOut())
{
return 0;
}
	try
	{
		//登录处理
		LONG lReturnValue=-1;
		LPCTSTR pszPassword=NULL;
		switch (DataBaseEvent.wRequestID)
		{
		case DBR_GP_LOGON_BY_ACCOUNTS:		//帐号登录
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_LogonByAccounts));
				if (wDataSize!=sizeof(DBR_GP_LogonByAccounts)) return false;

				//执行查询
				DBR_GP_LogonByAccounts * pLogonByAccounts=(DBR_GP_LogonByAccounts *)pDataBuffer;
				lReturnValue=SPLogonByAccounts(pLogonByAccounts->szAccounts,pLogonByAccounts->szPassWord,pLogonByAccounts->dwClientIP);
				pszPassword=pLogonByAccounts->szPassWord;

				break;
			}
		case DBR_GP_LOGON_BY_USERID:		//I D 登录
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_LogonByUserID));
				if (wDataSize!=sizeof(DBR_GP_LogonByUserID)) return false;

				//执行查询
				DBR_GP_LogonByUserID * pLogonByUserID=(DBR_GP_LogonByUserID *)pDataBuffer;
				lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP);
				pszPassword=pLogonByUserID->szPassWord;

				break;
			}
		case DBR_GP_REGISTER_ACCOUNTS:
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
				if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

				//执行查询
				DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pDataBuffer;
				lReturnValue=SPRegisterAccounts(pRegisterAccounts->szAccounts,pRegisterAccounts->szPassWord,
					pRegisterAccounts->wFaceID,pRegisterAccounts->dwClientIP);
				pszPassword=pRegisterAccounts->szPassWord;

				break;
			}
		default :	//默认处理
			{
				ASSERT(FALSE); 
				return false;
			}
		}

		//结果判断
		if (lReturnValue==0L)
		{
			//登录成功
			DBR_GP_LogonSuccess LogonSuccess;
			memset(&LogonSuccess,0,sizeof(LogonSuccess));
			m_UserDataBase->GetFieldValue(TEXT("UserID"),LogonSuccess.dwUserID);
			m_UserDataBase->GetFieldValue(TEXT("Gender"),LogonSuccess.cbGender);
			m_UserDataBase->GetFieldValue(TEXT("MemberOrder"),LogonSuccess.cbMember);
			m_UserDataBase->GetFieldValue(TEXT("FaceID"),LogonSuccess.wFaceID);
			m_UserDataBase->GetFieldValue(TEXT("GroupID"),LogonSuccess.dwGroupID);
			m_UserDataBase->GetFieldValue(TEXT("Experience"),LogonSuccess.dwExperience);
			m_UserDataBase->GetFieldValue(TEXT("UserRight"),LogonSuccess.dwUserRight);
			m_UserDataBase->GetFieldValue(TEXT("MasterRight"),LogonSuccess.dwMasterRight);
			m_UserDataBase->GetFieldValue(TEXT("Accounts"),LogonSuccess.szAccounts,sizeof(LogonSuccess.szAccounts));
			m_UserDataBase->GetFieldValue(TEXT("GroupName"),LogonSuccess.szGroupName,sizeof(LogonSuccess.szGroupName));
			CopyMemory(LogonSuccess.szPassWord,pszPassword,sizeof(LogonSuccess.szPassWord));
			m_AttemperEvent.PostDataBaseEvent(DBR_GP_LOGON_SUCCESS,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
				&LogonSuccess,sizeof(LogonSuccess));
		}
		else
		{
			//登录失败
			DBR_GP_LogonError LogonError;
			LogonError.lErrorCode=lReturnValue;
			m_UserDataBase->GetFieldValue(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,sizeof(LogonError.szErrorDescribe));
			m_AttemperEvent.PostDataBaseEvent(DBR_GP_LOGON_ERROR,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
				&LogonError,sizeof(LogonError));
		}
	}
	catch (IADOError * pIADOError)
	{
		//输出错误
		UNREFERENCED_PARAMETER(pIADOError);
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);

		//操作失败
		DBR_GP_LogonError LogonError;
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),
			sizeof(LogonError.szErrorDescribe));
		m_AttemperEvent.PostDataBaseEvent(DBR_GP_LOGON_ERROR,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
			&LogonError,sizeof(LogonError));
	}

	//关闭记录集
	m_UserDataBase->CloseRecordset();

	return true;
}

//帐号存储过程
LONG CDataBaseSink::SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP)
{
	//如果超时，退出
if ( IsTimeOut())
{
return 0;
}
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_UserDataBase->ClearAllParameters();
	m_UserDataBase->SetSPName("GSP_GP_LogonByAccounts");
	m_UserDataBase->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,4,_variant_t((long)0));
	m_UserDataBase->AddParamter(TEXT("@strAccounts"),adParamInput,adVarChar,CountString(pszAccounts),_variant_t(pszAccounts));
	m_UserDataBase->AddParamter(TEXT("@strPassword"),adParamInput,adChar,CountString(pszPassword),_variant_t(pszPassword));
	m_UserDataBase->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,CountString(szClientIP),_variant_t(szClientIP));
	m_UserDataBase->ExecuteCommand(true);

	return m_UserDataBase->GetReturnValue();
}

//更新存储过程
LONG CDataBaseSink::SPRegisterAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, WORD wFaceID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_UserDataBase->ClearAllParameters();
	m_UserDataBase->SetSPName("GSP_GP_RegisterUser");
	m_UserDataBase->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,4,_variant_t((long)0));
	m_UserDataBase->AddParamter(TEXT("@strAccounts"),adParamInput,adVarChar,CountString(pszAccounts),_variant_t(pszAccounts));
	m_UserDataBase->AddParamter(TEXT("@strPassword"),adParamInput,adChar,CountString(pszPassword),_variant_t(pszPassword));
	m_UserDataBase->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,CountString(szClientIP),_variant_t(szClientIP));
	m_UserDataBase->AddParamter(TEXT("@cbGender"),adParamInput,adInteger,sizeof(long),_variant_t((long)0L));
	m_UserDataBase->AddParamter(TEXT("@nFaceID"),adParamInput,adInteger,sizeof(long),_variant_t((long)wFaceID));
	m_UserDataBase->ExecuteCommand(true);

	return m_UserDataBase->GetReturnValue();
}

//I D 存储过程
LONG CDataBaseSink::SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP)
{
	//如果超时，退出
if ( IsTimeOut())
{
return 0;
}

	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_UserDataBase->ClearAllParameters();
	m_UserDataBase->SetSPName("GSP_GP_LogonByUserID");
	m_UserDataBase->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_UserDataBase->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_UserDataBase->AddParamter(TEXT("@strPassword"),adParamInput,adChar,CountString(pszPassword),_variant_t(pszPassword));
	m_UserDataBase->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,CountString(szClientIP),_variant_t(szClientIP));
	m_UserDataBase->ExecuteCommand(true);

	return m_UserDataBase->GetReturnValue();
}

//////////////////////////////////////////////////////////////////////////
