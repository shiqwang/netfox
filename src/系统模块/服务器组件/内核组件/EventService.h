#ifndef EVENT_SERVICE_HEAD_FILE
#define EVENT_SERVICE_HEAD_FILE

#pragma once

#include "ServerKernel.h"

//////////////////////////////////////////////////////////////////////////

//事件服务类
class CEventService : public IEventService
{
	//变量定义
protected:
	HWND							m_hRichEdit;						//控件句柄
	CThreadLock						m_ThreadLock;						//线程锁
	bool							m_bEventService[EVENT_LEVEL_COUNT];	//显示标志

	//状态变量
protected:
	int								m_nMaxLineCount;					//最大行数
	int								m_nReserveLineCount;				//保留行数

	//函数定义
public:
	//构造函数
	CEventService(void);
	//析构函数
	virtual ~CEventService(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CEventService))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//功能接口
public:
	//设置句柄
	virtual bool __cdecl SetRichEditHwnd(HWND hRichEdit);
	//设置级别
	virtual void __cdecl ConfigEventService(enTraceLevel TraceLevel, bool bShow);
	//事件通知
	virtual void __cdecl ShowEventNotify(LPCTSTR pszString, enTraceLevel TraceLevel);
};

//////////////////////////////////////////////////////////////////////////

//事件输出类
class CEventTrace
{
	//变量定义
private:
	static IEventService					* m_pIEventService;			//事件接口

	//函数定义
private:
	//构造函数
	CEventTrace() {}

	//功能函数
public:
	//设置接口
	static bool SetEventService(IUnknownEx * pIUnknownEx);
	//获取接口
	static void * GetEventService(const IID & Guid, DWORD dwQueryVer);
	//设置级别
	static void ConfigEventService(enTraceLevel TraceLevel, bool bShow);
	//事件通知
	static void ShowEventNotify(LPCTSTR pszString, enTraceLevel TraceLevel);
};

//////////////////////////////////////////////////////////////////////////

#endif