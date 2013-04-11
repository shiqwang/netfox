#include "StdAfx.h"
#include "EventService.h"

//////////////////////////////////////////////////////////////////////////

//颜色定义
#define COLOR_TIME						RGB(0,0,0)
#define COLOR_NORMAL					RGB(125,125,125)
#define COLOR_WARN						RGB(255,128,0)
#define COLOR_EXCEPTION					RGB(200,0,0)
#define COLOR_DEBUG						RGB(0,128,128)

//静态变量
IEventService		* CEventTrace::m_pIEventService=NULL;				//事件接口

//////////////////////////////////////////////////////////////////////////

//构造函数
CEventService::CEventService(void)
{
	//设置变量
	m_hRichEdit=NULL;
	m_nMaxLineCount=300;
	m_nReserveLineCount=20;
	for (int i=0;i<CountArray(m_bEventService);i++) m_bEventService[i]=true;
}

//析构函数
CEventService::~CEventService(void)
{
}

//接口查询
void * __cdecl CEventService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IEventService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IEventService,Guid,dwQueryVer);
	return NULL;
}

//设置句柄
bool CEventService::SetRichEditHwnd(HWND hRichEdit)
{
	ASSERT(IsWindow(hRichEdit));
	if (IsWindow(hRichEdit))
	{
		m_hRichEdit=hRichEdit;
		return true;
	}
	return false;
}

//设置级别
void __cdecl CEventService::ConfigEventService(enTraceLevel TraceLevel, bool bShow)
{
	ASSERT(TraceLevel<EVENT_LEVEL_COUNT);
	if (TraceLevel<EVENT_LEVEL_COUNT) m_bEventService[TraceLevel]=bShow;
	return;
}

//插入字符
void __cdecl CEventService::ShowEventNotify(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//判断显示
	ASSERT(TraceLevel<EVENT_LEVEL_COUNT);
	if ((TraceLevel<EVENT_LEVEL_COUNT)&&(m_bEventService[TraceLevel]==false)) return;

	//设置格式
	CHARFORMAT2 CharFormat;
	memset(&CharFormat,0,sizeof(CharFormat));
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR|CFM_FACE|CFM_SIZE;
	CharFormat.yHeight=180;
	lstrcpyn(CharFormat.szFaceName,TEXT("宋体"),sizeof(CharFormat.szFaceName));

	//获取颜色
	COLORREF crColor=COLOR_NORMAL;
	switch (TraceLevel)
	{
	case Level_Normal: { crColor=COLOR_NORMAL; break; }
	case Level_Warning: { crColor=COLOR_WARN; break; }
	case Level_Exception: { crColor=COLOR_EXCEPTION; break; }
	case Level_Debug: { crColor=COLOR_DEBUG; break; }
	}

	//锁定资源
	CThreadLockHandle LockHandle(&m_ThreadLock);

	//获取指针
	if (IsWindow(m_hRichEdit)==FALSE) return;
	CRichEditCtrl * pRichEdit=(CRichEditCtrl *)CRichEditCtrl::FromHandle(m_hRichEdit);
	if (pRichEdit==NULL) return;

	//清理信息
	int nLineCount=pRichEdit->GetLineCount();
	if (nLineCount>=m_nMaxLineCount)
	{
		ASSERT(m_nMaxLineCount>m_nReserveLineCount);
		pRichEdit->SetSel(0,pRichEdit->LineIndex(nLineCount-m_nReserveLineCount));
		pRichEdit->ReplaceSel(TEXT(""));
	}

	//获取时间
	SYSTEMTIME SystemTime;
	TCHAR szTimeBuffer[64];
	GetLocalTime(&SystemTime);
	_snprintf(szTimeBuffer,sizeof(szTimeBuffer),TEXT("【 %02d:%02d:%02d 】"),SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

	//插入消息
	CharFormat.crTextColor=COLOR_TIME;
	long lTextLength=pRichEdit->GetWindowTextLength();
	pRichEdit->SetSel(lTextLength,lTextLength);
	pRichEdit->SetSelectionCharFormat(CharFormat);
	pRichEdit->ReplaceSel(szTimeBuffer);

	//插入消息
	CharFormat.crTextColor=crColor;
	lTextLength=pRichEdit->GetWindowTextLength();
	pRichEdit->SetSel(lTextLength,lTextLength);
	pRichEdit->SetSelectionCharFormat(CharFormat);
	pRichEdit->ReplaceSel(pszString);

	//插入消息
	lTextLength=pRichEdit->GetWindowTextLength();
	pRichEdit->SetSel(lTextLength,lTextLength);
	pRichEdit->SetSelectionCharFormat(CharFormat);
	pRichEdit->ReplaceSel(TEXT("\n"));

	return;
}

//////////////////////////////////////////////////////////////////////////

//设置接口
bool CEventTrace::SetEventService(IUnknownEx * pIUnknownEx)
{
	if (pIUnknownEx!=NULL)
	{
		IEventService * pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
		ASSERT(pIEventService!=NULL);
		if (pIEventService==NULL) return false;
		m_pIEventService=pIEventService;
	}
	else m_pIEventService=NULL;

	return true;
}

//获取接口
void * CEventTrace::GetEventService(const IID & Guid, DWORD dwQueryVer)
{
	if (m_pIEventService==NULL) return NULL;
	return m_pIEventService->QueryInterface(Guid,dwQueryVer);
}

//设置级别
void CEventTrace::ConfigEventService(enTraceLevel TraceLevel, bool bShow)
{
	if (m_pIEventService!=NULL) m_pIEventService->ConfigEventService(TraceLevel,bShow);
}

//事件通知
void CEventTrace::ShowEventNotify(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pszString,TraceLevel);
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateEventService(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CEventService * pEventService=NULL;
	try
	{
		pEventService=new CEventService();
		if (pEventService==NULL) throw TEXT("创建失败");
		void * pObject=pEventService->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pEventService);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
