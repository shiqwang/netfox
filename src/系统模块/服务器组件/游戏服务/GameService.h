#ifndef GAME_SERVICE_HEAD_FILE
#define GAME_SERVICE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//游戏服务类
class CGameService : public IGameService
{
	//变量定义
protected:
	CCenterSocket					m_CenterSocket;						//中心连接
	CDataBaseSink					m_DataBaseSink;						//数据库钩子
	CAttemperEngineSink				m_AttemperSink;						//调度钩子
	CServiceEngineHelper			m_ServiceEngine;					//服务引擎

	//配置变量
protected:
	TCHAR							m_szModule[32];						//模块名字
	tagDataBaseInfo					m_GameUserDBInfo;					//数据配置
	tagDataBaseInfo					m_GameScoreDBInfo;					//数据配置
	tagGameServiceAttrib			m_GameServiceAttrib;				//服务属性
	tagGameServiceOption			m_GameServiceOption;				//服务配置

	//组件接口
protected:
	HINSTANCE						m_hDllInstance;						//游戏实例
	IEventService					* m_pIEventService;					//事件服务
	IGameServiceManager				* m_pIGameServiceManager;			//管理接口

	//函数定义
public:
	//构造函数
	CGameService(void);
	//析构函数
	virtual ~CGameService(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CGameService))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//服务状态
	virtual bool __cdecl IsService();
	//设置事件
	virtual bool __cdecl SetEventService(IUnknownEx * pIEventService);
	//开始服务
	virtual bool __cdecl StartService(tagGameServiceParameter * pGameServiceParameter);
	//停止服务
	virtual bool __cdecl StopService();

	//内部函数
private:
	//释放模块
	bool UnLoadGameServiceModule();
	//加载模块
	bool LoadGameServiceModule(LPCTSTR pszModuleName);
	//显示错误
	void ShowErrorMessasge(LPCTSTR pszString, enTraceLevel TraceLevel);
};

//////////////////////////////////////////////////////////////////////////

#endif