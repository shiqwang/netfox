#ifndef ATTRIBUTE_HEAD_FILE
#define ATTRIBUTE_HEAD_FILE

#pragma once

#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////

//属性项类
class SKIN_CONTROL_CLASS CAttribute : public IAttribute
{
	//数据指针联合体
	union Attribute_Data
	{
		VOID							* m_Void;						//VOID 指针
		INT								* m_Int;						//INT 指针
		BYTE							* m_Byte;						//BYTE 指针
		WORD							* m_Word;						//WORD 指针
		LONG							* m_Long;						//LONG 指针
		DWORD							* m_Dword;						//DWORD 指针
		DOUBLE							* m_Double;						//DOUBLE 指针
		CString							* m_String;						//CString 指针
	};

	//变量定义
protected:
	bool								m_bReadOnly;					//是否只读
	DWORD								m_dwHash;						//哈希数值
	CString								m_strName;						//属性名字
	Attribute_Data						m_Data;							//属性数据
	enAttributeType						m_Type;							//属性类型
	
	//函数定义
public:
	//构造函数
	CAttribute()
	{
		m_dwHash=0L;
		m_bReadOnly=false;
		m_Type=Type_EMPTY;
		m_Data.m_Int=NULL;
	}
	//构造函数
	CAttribute(LPCTSTR pszName, VOID * pVoid, bool bReadOnly)
	{
		m_Type=Type_VOID;
		m_Data.m_Void=pVoid;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, INT * pInt, bool bReadOnly)
	{
		m_Type=Type_INT;
		m_Data.m_Int=pInt;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, BYTE * pByte, bool bReadOnly)
	{
		m_Type=Type_BYTE;
		m_Data.m_Byte=pByte;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, WORD * pWord, bool bReadOnly)
	{
		m_Type=Type_WORD;
		m_Data.m_Word=pWord;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, LONG * pLong, bool bReadOnly)
	{
		m_Type=Type_LONG;
		m_Data.m_Long=pLong;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, DWORD * pDword, bool bReadOnly)
	{
		m_Type=Type_DWORD;
		m_Data.m_Dword=pDword;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, DOUBLE * pDouble, bool bReadOnly)
	{
		m_Type=Type_DOUBLE;
		m_Data.m_Double=pDouble;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//构造函数
	CAttribute(LPCTSTR pszName, CString * pString, bool bReadOnly)
	{
		m_Type=Type_STRING;
		m_Data.m_String=pString;
		m_strName=pszName;
		m_bReadOnly=bReadOnly;
		m_dwHash=HashAttribute(pszName);
	}
	//析构函数
	virtual ~CAttribute() { }

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CAttribute))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer)
	{
		QUERYINTERFACE(IAttribute,Guid,dwQueryVer);
		QUERYINTERFACE_IUNKNOWNEX(IAttribute,Guid,dwQueryVer);
		return NULL;
	}

	//接口函数
public:
	//获取哈希值
	virtual DWORD __cdecl GetHashValue() { return m_dwHash; }
	//是否只读
	virtual bool __cdecl IsReadOnly() { return m_bReadOnly; }
	//获取名字
	virtual LPCTSTR __cdecl GetAttributeName() { return m_strName; }
	//获取类型
	virtual enAttributeType __cdecl GetAttributeType() { return m_Type; }
	//获取 PTR
	virtual bool __cdecl GetValue(VOID * * ppvValue)
	{
		ASSERT(m_Type==Type_VOID);
		if (m_Type!=Type_VOID) 
		{
			*ppvValue=NULL;
			return false;
		}
		*ppvValue=m_Data.m_Void;
		return true;
	};
	//获取 INT
	virtual bool __cdecl GetValue(INT & iValue)
	{
		ASSERT(m_Type==Type_INT);
		if (m_Type!=Type_INT) 
		{
			iValue=0;
			return false;
		}
		iValue=*m_Data.m_Int;
		return true;
	};
	//获取 BYTE
	virtual bool __cdecl GetValue(BYTE & cbValue)
	{
		ASSERT(m_Type==Type_BYTE);
		if (m_Type!=Type_BYTE) 
		{
			cbValue=0;
			return false;
		}
		cbValue=*m_Data.m_Byte;
		return true;
	};
	//获取 WORD
	virtual bool __cdecl GetValue(WORD & wValue)
	{
		ASSERT(m_Type==Type_WORD);
		if (m_Type!=Type_WORD) 
		{
			wValue=0;
			return false;
		}
		wValue=*m_Data.m_Word;
		return true;
	};
	//获取 LONG
	virtual bool __cdecl GetValue(LONG & lValue)
	{
		ASSERT(m_Type==Type_LONG);
		if (m_Type!=Type_LONG) 
		{
			lValue=0;
			return false;
		}
		lValue=*m_Data.m_Long;
		return true;
	};
	//获取 DWORD
	virtual bool __cdecl GetValue(DWORD & dwValue)
	{
		ASSERT(m_Type==Type_DWORD);
		if (m_Type!=Type_DWORD) 
		{
			dwValue=0;
			return false;
		}
		dwValue=*m_Data.m_Dword;
		return true;
	};
	//获取 DOUBLE
	virtual bool __cdecl GetValue(DOUBLE & dValue)
	{
		ASSERT(m_Type==Type_DOUBLE);
		if (m_Type!=Type_DOUBLE) 
		{
			dValue=0.0;
			return false;
		}
		dValue=*m_Data.m_Double;
		return true;
	};
	//获取字符串
	virtual bool __cdecl GetValue(CString & strValue)
	{
		ASSERT(m_Type==Type_STRING);
		if (m_Type!=Type_STRING) 
		{
			strValue.Empty();
			return false;
		}
		strValue=*m_Data.m_String;
		return true;
	}
	//获取变量
	virtual bool __cdecl GetVarValue(CString & strValue)
	{
		switch (m_Type)
		{
		case Type_INT:
			{
				strValue.Format(TEXT("%d"),*m_Data.m_Int);
				return true;
			}
		case Type_BYTE:
			{
				strValue.Format(TEXT("%d"),*m_Data.m_Byte);
				return true;
			}
		case Type_WORD:
			{
				strValue.Format(TEXT("%u"),*m_Data.m_Word);
				return true;
			}
		case Type_LONG:
			{
				strValue.Format(TEXT("%ld"),*m_Data.m_Long);
				return true;
			}
		case Type_DWORD:
			{
				strValue.Format(TEXT("%lu"),*m_Data.m_Dword);
				return true;
			}
		case Type_DOUBLE:
			{
				strValue.Format(TEXT("%f"),*m_Data.m_Double);
				return true;
			}
		case Type_STRING:
			{
				strValue=*m_Data.m_String;
				return true;
			}
		default:
			{
				ASSERT(0);
				break;
			}
		}
		return false;
	}
	//设置 PTR
	virtual bool __cdecl SetValue(void * pValue)
	{
		ASSERT(m_Type==Type_VOID);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_VOID) return false;
		m_Data.m_Void=pValue;
		return true;
	};
	//设置 INT
	virtual bool __cdecl SetValue(INT iValue)
	{
		ASSERT(m_Type==Type_INT);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_INT) return false;
		*m_Data.m_Int=iValue;
		return true;
	};
	//设置 BYTE
	virtual bool __cdecl SetValue(BYTE cbValue)
	{
		ASSERT(m_Type==Type_BYTE);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_BYTE) return false;
		*m_Data.m_Byte=cbValue;
		return true;
	};
	//设置 WORD
	virtual bool __cdecl SetValue(WORD wValue)
	{
		ASSERT(m_Type==Type_WORD);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_WORD) return false;
		*m_Data.m_Word=wValue;
		return true;
	};
	//设置 LONG
	virtual bool __cdecl SetValue(LONG lValue)
	{
		ASSERT(m_Type==Type_LONG);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_LONG) return false;
		*m_Data.m_Long=lValue;
		return true;
	};
	//设置 DWORD
	virtual bool __cdecl SetValue(DWORD dwValue)
	{
		ASSERT(m_Type==Type_DWORD);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_DWORD) return false;
		*m_Data.m_Dword=dwValue;
		return true;
	};
	//设置 DOUBLE
	virtual bool __cdecl SetValue(DOUBLE dValue)
	{
		ASSERT(m_Type==Type_DOUBLE);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_DOUBLE) return false;
		*m_Data.m_Double=dValue;
		return true;
	};
	//设置 STRING
	virtual bool __cdecl SetValue(LPCTSTR pszValue)
	{
		ASSERT(m_Type==Type_STRING);
		ASSERT(m_bReadOnly==false);
		if (m_bReadOnly) return false;
		if (m_Type!=Type_STRING) return false;
		*m_Data.m_String=pszValue;
		return true;
	};

	//哈希函数
public:
	//哈希转换
	static DWORD HashAttribute(LPCTSTR pszString)
	{
		DWORD dwHash=0L;
		CString strString=pszString;
		strString.MakeLower();
		int nLen=strString.GetLength();
		for (int i=0;i<nLen;i++)
		{
			dwHash^=strString[i];
			dwHash<<=1;
		}
		return dwHash;
	};
};

//////////////////////////////////////////////////////////////////////////

//属性管理类
class SKIN_CONTROL_CLASS CAttributeManager : public IAttributeManager, public IAttributeService
{
	//变量定义
protected:
	CArrayTemplate<CAttribute>			m_Attribute;				//属性数组

	//函数定义
public:
	//构造函数
	CAttributeManager();
	//析构函数
	virtual ~CAttributeManager();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CAttributeManager))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//查询属性
	virtual IAttribute * __cdecl QueryAttribute(WORD wIndex);
	//查询属性
	virtual IAttribute * __cdecl QueryAttribute(LPCTSTR pszName);
	//注销属性
	virtual bool __cdecl UnRegisterAttribute(LPCTSTR pszName);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, void * pPtr, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, INT * pInt, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, BYTE * pByte, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, WORD * pWord, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, LONG * pLong, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, DWORD * pDword, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, DOUBLE * pDouble, bool bReadOnly);
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, CString * pString, bool bReadOnly);

	//接口函数
public:
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, VOID * * ppvValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, INT & iValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, BYTE & cbValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, WORD & wValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, LONG & lValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, DWORD & dwValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, DOUBLE & dValue);
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, CString & strValue);
	//获取属性
	virtual bool __cdecl GetAttributeVarValue(LPCTSTR pszName, CString & strValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, void * pValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, INT iValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, BYTE cbValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, WORD wValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, LONG lValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, DWORD dwValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, DOUBLE dValue);
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, LPCTSTR pszValue);
};

//////////////////////////////////////////////////////////////////////////

#endif
