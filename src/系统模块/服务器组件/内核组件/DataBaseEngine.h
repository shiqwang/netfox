#ifndef DATA_BASE_ENGINE_HEAD_FILE
#define DATA_BASE_ENGINE_HEAD_FILE

#pragma once

//组件头文件
#include "ServerKernel.h"
#include "QueueService.h"

//////////////////////////////////////////////////////////////////////////

//ADO 错误类
class CADOError : public IADOError
{
	//变量定义
protected:
	enADOErrorType					m_enErrorType;						//错误代号
	CString							m_strErrorDescribe;					//错误信息

	//函数定义
public:
	//构造函数
	CADOError();
	//析构函数
	virtual ~CADOError();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CADOError))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//功能接口
public:
	//错误类型
	virtual enADOErrorType __cdecl GetErrorType() { return m_enErrorType; }
	//错误描述
	virtual LPCTSTR __cdecl GetErrorDescribe() { return m_strErrorDescribe; }

	//功能函数
public:
	//设置错误
	void SetErrorInfo(enADOErrorType enErrorType, LPCTSTR pszDescribe);
};

//////////////////////////////////////////////////////////////////////////

//数据库对象
class CDataBase : public IDataBase
{
	//信息变量
protected:
	CADOError						m_ADOError;							//错误对象
	CString							m_strConnect;						//连接字符串
	CString							m_strErrorDescribe;					//错误信息

	//状态变量
protected:
	DWORD							m_dwConnectCount;					//重试次数
	DWORD							m_dwConnectErrorTime;				//错误时间
	const DWORD						m_dwResumeConnectCount;				//恢复次数
	const DWORD						m_dwResumeConnectTime;				//恢复时间

	//内核变量
protected:
	_CommandPtr						m_DBCommand;						//命令对象
	_RecordsetPtr					m_DBRecordset;						//记录集对象
	_ConnectionPtr					m_DBConnection;						//数据库对象

	//函数定义
public:
	//构造函数
	CDataBase();
	//析构函数
	virtual ~CDataBase();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CDataBase))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//打开连接
	virtual bool __cdecl OpenConnection();
	//关闭记录
	virtual bool __cdecl CloseRecordset();
	//关闭连接
	virtual bool __cdecl CloseConnection();
	//重新连接
	virtual bool __cdecl TryConnectAgain(bool bFocusConnect, CComError * pComError);
	//设置信息
	virtual bool __cdecl SetConnectionInfo(LPCTSTR szIP, WORD wPort, LPCTSTR szData, LPCTSTR szName, LPCTSTR szPass);

	//状态接口
public:
	//是否连接错误
	virtual bool __cdecl IsConnectError();
	//是否打开
	virtual bool __cdecl IsRecordsetOpened();

	//记录集接口
public:
	//往下移动
	virtual void __cdecl MoveToNext();
	//移到开头
	virtual void __cdecl MoveToFirst();
	//是否结束
	virtual bool __cdecl IsEndRecordset();
	//获取数目
	virtual long __cdecl GetRecordCount();
	//获取大小
	virtual long __cdecl GetActualSize(LPCTSTR pszParamName);
	//绑定对象
	virtual bool __cdecl BindToRecordset(CADORecordBinding * pBind);

	//字段接口
public:
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, WORD & wValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, INT & nValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, LONG & lValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DWORD & ulValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DOUBLE & dbValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, TCHAR szBuffer[], UINT uSize);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time);
	//获取参数
	virtual bool __cdecl GetFieldValue(LPCTSTR lpFieldName, bool & bValue);

	//命令对象接口
public:
	//设置存储过程
	virtual void __cdecl SetSPName(LPCTSTR pszSpName);
	//插入参数
	virtual void __cdecl AddParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, ADOCG::DataTypeEnum Type, long lSize, _variant_t & vtValue);
	//删除参数
	virtual void __cdecl ClearAllParameters();
	//获得参数
	virtual void __cdecl GetParameterValue(LPCTSTR pszParamName, _variant_t & vtValue);
	//获取返回数值
	virtual long __cdecl GetReturnValue();

	//执行接口
public:
	//执行语句
	virtual bool __cdecl Execute(LPCTSTR pszCommand);
	//执行命令
	virtual bool __cdecl ExecuteCommand(bool bRecordset);

	//内部函数
private:
	//获取错误
	LPCTSTR GetComErrorDescribe(CComError & ComError);
	//设置错误
	void SetErrorInfo(enADOErrorType enErrorType, LPCTSTR pszDescribe);
};

//////////////////////////////////////////////////////////////////////////

//数据库管理类
class CDataBaseEngine : public IDataBaseEngine, public IQueueServiceSink
{
	//内核变量
protected:
	bool							m_bService;							//运行标志
	CQueueService					m_RequestQueueService;				//队列对象
	IDataBaseSink					* m_pIDataBaseSink;					//通知钩子

	//函数定义
public:
	//构造函数
	CDataBaseEngine(void);
	//析构函数
	virtual ~CDataBaseEngine(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CDataBaseEngine))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool __cdecl StartService();
	//停止服务
	virtual bool __cdecl StopService();
	//注册钩子
	virtual bool __cdecl SetDataBaseSink(IUnknownEx * pIUnknownEx);
	//获取接口
	virtual void * __cdecl GetQueueService(const IID & Guid, DWORD dwQueryVer);

	//队列接口
public:
	//队列接口
	virtual void __cdecl OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize, DWORD dwInsertTime);
};

//////////////////////////////////////////////////////////////////////////

#endif