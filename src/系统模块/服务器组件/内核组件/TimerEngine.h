#ifndef TIMERENGINE_HEAD_FILE
#define TIMERENGINE_HEAD_FILE

#pragma once

//系统头文件
#include "ServerKernel.h"

//类说明
class CTimerEngine;

//////////////////////////////////////////////////////////////////////////

//定时器线程
class CTimerThread : public CServiceThread
{
	//变量定义
protected:
	DWORD								m_dwTimerSpace;					//时间间隔
	CTimerEngine						* m_pTimerEngine;				//定时器引擎

	//函数定义
public:
	//构造函数
	CTimerThread(void);
	//析构函数
	virtual ~CTimerThread(void);

	//功能函数
public:
	//配置函数
	bool InitThread(CTimerEngine * pTimerEngine, DWORD dwTimerSpace);

	//重载函数
private:
	//运行函数
	virtual bool RepetitionRun();
};

//////////////////////////////////////////////////////////////////////////

//定时器子项
struct tagTimerItem
{
	WORD								wTimerID;						//定时器 ID
	DWORD								dwElapse;						//定时时间
	DWORD								dwTimeLeave;					//倒计时间
	DWORD								dwRepeatTimes;					//重复次数
	WPARAM								wBindParam;						//绑定参数
};

//定时器子项数组定义
typedef CArrayTemplate<tagTimerItem *> CTimerItemPtr;

//////////////////////////////////////////////////////////////////////////

//定时器引擎
class CTimerEngine : public ITimerEngine, public ITimerEngineManager
{
	friend class CTimerThread;

	//配置定义
protected:
	DWORD								m_dwTimerSpace;					//时间间隔

	//状态变量
protected:
	bool								m_bService;						//运行标志
	DWORD								m_dwTimePass;					//经过时间
	DWORD								m_dwTimeLeave;					//倒计时间
	CTimerItemPtr						m_TimerItemFree;				//空闲数组
	CTimerItemPtr						m_TimerItemActive;				//活动数组

	//组件变量
protected:
	CThreadLock							m_ThreadLock;					//线程锁
	CTimerThread						m_TimerThread;					//定时器线程
	CQueueServiceEvent					m_AttemperEvent;				//通知组件

	//函数定义
public:
	//构造函数
	CTimerEngine(void);
	//析构函数
	virtual ~CTimerEngine(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTimerEngine))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//设置定时器
	virtual bool __cdecl SetTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wParam);
	//删除定时器
	virtual bool __cdecl KillTimer(WORD wTimerID);
	//删除定时器
	virtual bool __cdecl KillAllTimer();

	//管理接口
public:
	//开始服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl StopService();
	//设置接口
	virtual bool __cdecl SetTimerEngineSink(IUnknownEx * pIUnknownEx);

	//内部函数
private:
	//定时器通知
	void OnTimerThreadSink();
};

//////////////////////////////////////////////////////////////////////////

#endif