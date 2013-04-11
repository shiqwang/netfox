#ifndef DATA_BASE_SINK_HEAD_FILE
#define DATA_BASE_SINK_HEAD_FILE

#pragma once

#include "Afxinet.h"
#include "Commonality.h"
#include "InitParamter.h"

//////////////////////////////////////////////////////////////////////////

//数据库引擎钩子
class CDataBaseSink : public IDataBaseSink
{
	//变量定义
protected:
	CInitParamter					* m_pInitParamter;					//配置参数
	IEventService					* m_pIEventService;					//事件服务

	//组件变量
protected:
	CDataBaseHelper					m_UserDataBase;						//用户数据库
	CQueueServiceEvent				m_AttemperEvent;					//调度通知

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
	//模块启动
	virtual bool __cdecl StartService(IUnknownEx * pIUnknownEx);
	//模块关闭
	virtual bool __cdecl StopService(IUnknownEx * pIUnknownEx);
	//数据操作处理
	virtual bool __cdecl OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);

	//配置函数 
public:
	//设置事件
	bool SetEventService(IUnknownEx * pIUnknownEx);
	//配置函数
	bool InitDataBaseSink(CInitParamter * pInitParamter, IUnknownEx * pIUnknownEx);

	//处理函数
private:
	//登录请求处理
	bool OnRequestLogon(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);

	//存储过程
private:
	//I D 存储过程
	LONG SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP);
	//帐号存储过程
	LONG SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP);
	//注册存储过程
	LONG SPRegisterAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, WORD wFaceID, DWORD dwClientIP);
};

//////////////////////////////////////////////////////////////////////////

#endif