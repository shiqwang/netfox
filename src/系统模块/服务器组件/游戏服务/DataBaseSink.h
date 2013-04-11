#ifndef DATA_BASE_SINK_HEAD_FILE
#define DATA_BASE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//数据库参数
struct tagDataBaseSinkParameter
{
	tagDataBaseInfo						* pGameUserDBInfo;				//连接信息
	tagDataBaseInfo						* pGameScoreDBInfo;				//连接信息
	tagGameServiceAttrib				* pGameServiceAttrib;			//服务属性
	tagGameServiceOption				* pGameServiceOption;			//配置信息
};

//////////////////////////////////////////////////////////////////////////
//数据库请求数据包

//数据库请求标识
#define	DBR_GR_LOGON_BY_ACCOUNTS		1								//帐户登陆
#define	DBR_GR_LOGON_BY_USERID			2								//I D 登陆
#define DBR_GR_WRITE_SCORE				3								//修改积分
#define DBR_GR_LIMIT_ACCOUNTS			4								//禁用帐户
#define DBR_GR_SET_USER_RIGHT			5								//设置权限

//数据库输出标识
#define DBR_GR_LOGON_SUCCESS			100								//登陆成功
#define DBR_GR_LOGON_ERROR				101								//登陆失败

//帐户登陆
struct DBR_GR_LogonByAccounts
{
	DWORD								dwClientIP;						//连接地址
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//ID 登录
struct DBR_GR_LogonByUserID
{
	DWORD								dwClientIP;						//连接地址
	DWORD								dwUserID;						//用户 I D
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//禁用帐户
struct DBR_GR_LimitAccounts
{
	DWORD								dwUserID;						//用户 I D
	DWORD								dwMasterUserID;					//管理 I D
	DWORD								dwMasterClientIP;				//连接地址
};

//设置权限
struct DBR_GR_SetUserRight
{
	//权限范围
	BYTE								cbGame;							//设置游戏
	BYTE								cbAccounts;						//设置帐号

	//权限信息
	DWORD								dwUserID;						//用户 I D
	DWORD								dwUserRight;					//用户权限
	DWORD								dwMasterUserID;					//管理 I D
	DWORD								dwMasterClientIP;				//连接地址
};

//登陆成功
struct DBR_GR_LogonSuccess
{
	//基本信息
	BYTE								cbGender;						//用户性别
	BYTE								cbMember;						//会员等级
	WORD								wFaceID;						//头像索引
	DWORD								dwUserID;						//用户 I D
	DWORD								dwGroupID;						//社团索引
	DWORD								dwUserRight;					//用户等级
	DWORD								dwMasterRight;					//管理权限
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	TCHAR								szGroupName[GROUP_LEN];			//社团名字

	//积分信息
	LONG								lGold;							//用户金币
	LONG								lScore;							//用户分数
	LONG								lWinCount;						//胜利盘数
	LONG								lLostCount;						//失败盘数
	LONG								lDrawCount;						//和局盘数
	LONG								lFleeCount;						//断线数目
	LONG								lExperience;					//用户经验
};

//登陆失败
struct DBR_GR_LogonError
{
	LONG								lErrorCode;						//错误代码
	TCHAR								szErrorDescribe[128];			//错误消息
};

//修改积分
struct DBR_GR_WriteScore
{
	DWORD								dwUserID;						//用户地址
	DWORD								dwClientIP;						//连接地址
	DWORD								dwPlayTimeCount;				//游戏时间
	DWORD								dwOnlineTimeCount;				//在线时间
	TCHAR								szAccounts[NAME_LEN];			//帐户名字
	tagUserScore						ScoreBornInfo;					//原本积分
	tagUserScore						ScoreModifyInfo;				//修改积分
	tagUserScore						ScoreResultInfo;				//用户分数
};

//////////////////////////////////////////////////////////////////////////

//数据库引擎钩子
class CDataBaseSink : public IDataBaseSink
{
	//配置变量
protected:
	tagDataBaseInfo						* m_pGameUserDBInfo;			//连接信息
	tagDataBaseInfo						* m_pGameScoreDBInfo;			//连接信息
	tagGameServiceAttrib				* m_pGameServiceAttrib;			//服务属性
	tagGameServiceOption				* m_pGameServiceOption;			//配置信息

	//接口变量
protected:
	IEventService						* m_pIEventService;				//事件服务

	//组件变量
protected:
	CDataBaseHelper						m_GameUserDB;					//用户数据库
	CDataBaseHelper						m_GameScoreDB;					//游戏数据库
	CQueueServiceEvent					m_AttemperEvent;				//调度通知

	//函数定义
public:
	//构造函数
	CDataBaseSink();
	//析构函数
	virtual ~CDataBaseSink();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CDataBaseSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//调度接口
public:
	//调度模块启动
	virtual bool __cdecl StartService(IUnknownEx * pIUnknownEx);
	//调度模块关闭
	virtual bool __cdecl StopService(IUnknownEx * pIUnknownEx);
	//数据操作处理
	virtual bool __cdecl OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);

	//配置函数 
public:
	//设置事件
	bool SetEventService(IUnknownEx * pIUnknownEx);
	//配置函数
	bool InitDataBaseSink(tagDataBaseSinkParameter * pDataBaseSinkParameter, IUnknownEx * pIUnknownEx);

	//处理函数
private:
	//登录请求
	bool OnRequestLogon(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);
	//写分请求
	bool OnRequestWriteScore(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);
	//禁用帐户
	bool OnRequestLimitAccounts(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);
	//设置权限
	bool OnRequestSetUserRight(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);

	//存储过程
protected:
	//I D 存储过程
	LONG SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP);
	//帐号存储过程
	LONG SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP);
	//写分存储过程
	LONG SPWriteUserScore(DWORD dwUserID, DWORD dwPlayTimeCount, DWORD dwOnLineTimeCount, DWORD dwClientIP, tagUserScore & UserScore);
	//禁号存储过程
	LONG SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP);
	//权限存储过程
	LONG SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP);
	//权限存储过程
	LONG SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP);
};

//////////////////////////////////////////////////////////////////////////

#endif