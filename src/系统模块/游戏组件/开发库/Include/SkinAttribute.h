#ifndef SKIN_ATTRIBUTE_HEAD_FILE
#define SKIN_ATTRIBUTE_HEAD_FILE

#pragma once

#include "Attribute.h"
#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define CLASS_NAME					32									//名字长度
#define MAX_SKIN					64									//最大界面类数目

//属性名字
#define SKIN_CONTROL_CLASS_NAME_STRING		TEXT("类名字")				// TCHAR * 类型 （只读）

//////////////////////////////////////////////////////////////////////////

//属性记录类
class SKIN_CONTROL_CLASS CSkinRecordFile : public ISkinRecord
{
	//变量定义
protected:
	TCHAR							m_szIniFileName[MAX_PATH];			//配置文件
	TCHAR							m_szResDirectory[MAX_PATH];			//资源目录

	//函数定义
public:
	//构造函数
	CSkinRecordFile();
	//析构函数
	virtual ~CSkinRecordFile();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CSkinRecordFile))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//获取目录
	virtual LPCTSTR __cdecl GetResourceDirectory();
	//读取参数
	virtual int __cdecl ReadInt(LPCTSTR pszClassName, LPCTSTR pszItem, int iDefValue);
	//读取参数
	virtual COLORREF __cdecl ReadColor(LPCTSTR pszClassName, LPCTSTR pszItem, COLORREF crDefValue);
	//读取参数
	virtual RECT __cdecl ReadRect(LPCTSTR pszClassName, LPCTSTR pszItem, const RECT & rcDefValue);
	//读取参数
	virtual POINT __cdecl ReadPoint(LPCTSTR pszClassName, LPCTSTR pszItem, const POINT & ptDefValue);
	//读取参数
	virtual CString __cdecl ReadString(LPCTSTR pszClassName, LPCTSTR pszItem, LPCTSTR pszDefValue);
	//写入参数
	virtual bool __cdecl WriteInt(LPCTSTR pszClassName, LPCTSTR pszItem, int iValue) { return false; }
	//写入参数
	virtual bool __cdecl WriteColor(LPCTSTR pszClassName, LPCTSTR pszItem, COLORREF crValue) { return false; }
	//写入参数
	virtual bool __cdecl WriteRect(LPCTSTR pszClassName, LPCTSTR pszItem, const RECT & rcValue) { return false; }
	//写入参数
	virtual bool __cdecl WritePoint(LPCTSTR pszClassName, LPCTSTR pszItem, const POINT & ptValue) { return false; }
	//写入参数
	virtual bool __cdecl WriteString(LPCTSTR pszClassName, LPCTSTR pszItem, LPCTSTR pszValue) { return false; }

	//功能函数
public:
	//设置目录
	bool InitSkinRecordFile(LPCTSTR pszIniFileName, LPCTSTR pszResDirectory);
};

//////////////////////////////////////////////////////////////////////////

//界面属性
class SKIN_CONTROL_CLASS CSkinAttribute : public ISkinAttribute, public CAttributeManager
{
	//变量定义
protected:
	TCHAR							m_szClassName[CLASS_NAME];			//对象名字

	//资源变量
public:
	static CFont					m_DefaultFont;						//默认字体

	//内部变量
private:
	static bool						m_bFlags;							//内部标志
	static ISkinRecord				* m_pISkinRecord;					//配置接口
	static ISkinAttribute			* m_SkinPtrArray[MAX_SKIN];			//属性数组
	
	//函数定义
protected:
	//构造函数
	CSkinAttribute(LPCTSTR pszClassName);
	//析构函数
	virtual ~CSkinAttribute();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CSkinAttribute))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//获取名字
	virtual LPCTSTR __cdecl GetClassName() { return m_szClassName; }
	//获取字体
	virtual HFONT __cdecl GetDefaultFont() { return m_DefaultFont; }

	//配置函数
protected:
	//读取参数
	int ReadInt(LPCTSTR pszItem, int iDefValue);
	//读取参数
	COLORREF ReadColor(LPCTSTR pszItem, COLORREF crDefValue);
	//读取参数
	RECT ReadRect(LPCTSTR pszItem, const RECT & rcDefValue);
	//读取参数
	POINT ReadPoint(LPCTSTR pszItem, const POINT & ptDefValue);
	//读取参数
	CString ReadString(LPCTSTR pszItem, LPCTSTR pszDefValue);
	//写入参数
	bool WriteInt(LPCTSTR pszItem, int iValue);
	//写入参数
	bool WriteColor(LPCTSTR pszItem, COLORREF crValue);
	//写入参数
	bool WriteRect(LPCTSTR pszItem, const RECT & rcValue);
	//写入参数
	bool WritePoint(LPCTSTR pszItem, const POINT & ptValue);
	//写入参数
	bool WriteString(LPCTSTR pszItem, LPCTSTR pszValue);

	//功能函数
public:
	//加载界面信息
	static bool LoadSkinAttribute(IUnknownEx * pIUnknownEx);
	//保存界面信息
	static bool SaveSkinAttribute();
	//资源目录
	static LPCTSTR GetResourceDirectory();
	//枚举属性
	static void * GetAttributeManager(INT_PTR nIndex, const IID & Guid, DWORD dwQueryVer);

	//内部函数
private:
	//注册界面类
	static bool RegisterSkinClass(IUnknownEx * pIUnknownEx);
	//注销界面类
	static bool UnRegisterSkinClass(IUnknownEx * pIUnknownEx);
};

//////////////////////////////////////////////////////////////////////////

#endif
