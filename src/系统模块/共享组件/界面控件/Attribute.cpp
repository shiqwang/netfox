#include "Stdafx.h"
#include "Attribute.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttributeManager::CAttributeManager()
{
}

//析构函数
CAttributeManager::~CAttributeManager()
{
}

//接口查询
void * __cdecl CAttributeManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttributeManager,Guid,dwQueryVer);
	QUERYINTERFACE(IAttributeService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttributeManager,Guid,dwQueryVer);
	return NULL;
}

//查询属性
IAttribute * CAttributeManager::QueryAttribute(LPCTSTR pszName)
{
	ASSERT(pszName);
	INT_PTR nAttributeCount=m_Attribute.GetCount();
	if (nAttributeCount>0)
	{
		CAttribute * pAttribute=NULL;
		DWORD dwHashValue=CAttribute::HashAttribute(pszName);
		for (INT_PTR i=0;i<nAttributeCount;i++)
		{
			pAttribute=&m_Attribute[i];
			if (pAttribute->GetHashValue()==dwHashValue)
			{
				LPCTSTR szAttributeName=pAttribute->GetAttributeName();
				bool bSame=(lstrcmp(pszName,szAttributeName)==0);
				if (bSame) return pAttribute;
			}
		}
	}
	return NULL;
}

//查询属性
IAttribute * CAttributeManager::QueryAttribute(WORD wIndex)
{
	if (wIndex>=m_Attribute.GetCount()) return NULL;
	return &m_Attribute[wIndex];
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, void * pPtr, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pPtr,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}


//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, INT * pInt, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pInt,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, BYTE * pByte, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pByte,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, WORD * pWord, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pWord,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, LONG * pLong, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pLong,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, DWORD * pDword, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pDword,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, DOUBLE * pDouble, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pDouble,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注册属性
bool CAttributeManager::RegisterAttribute(LPCTSTR pszName, CString * pString, bool bReadOnly)
{
	//查找属性项
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute) return false;

	//增加属性项
	CAttribute Attribute(pszName,pString,bReadOnly);
	m_Attribute.Add(Attribute);

	return true;
}

//注销属性
bool CAttributeManager::UnRegisterAttribute(LPCTSTR pszName)
{
	ASSERT(pszName);
	INT_PTR nAttributeCount=m_Attribute.GetCount();
	if (nAttributeCount>0)
	{
		CAttribute * pAttribute=NULL;
		DWORD dwHashValue=CAttribute::HashAttribute(pszName);
		for (int i=0;i<nAttributeCount;i++)
		{
			pAttribute=&m_Attribute[i];
			if (pAttribute->GetHashValue()==dwHashValue)
			{
				LPCTSTR szAttributeName=pAttribute->GetAttributeName();
				if (lstrcmp(pszName,szAttributeName)==0)
				{
					m_Attribute.RemoveAt(i);
					return true;
				}
			}
		}
	}
	return false;
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, VOID * * ppvValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(ppvValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, INT & iValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(iValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, BYTE & cbValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(cbValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, WORD & wValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(wValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, LONG & lValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(lValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, DWORD & dwValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(dwValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, DOUBLE & dValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(dValue);
}

//获取属性
bool CAttributeManager::GetAttributeValue(LPCTSTR pszName, CString & strValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetValue(strValue);
}

//获取属性
bool CAttributeManager::GetAttributeVarValue(LPCTSTR pszName, CString & strValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->GetVarValue(strValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, void * pValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(pValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, INT iValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(iValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, BYTE cbValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(cbValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, WORD wValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(wValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, LONG lValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(lValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, DWORD dwValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(dwValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, DOUBLE dValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(dValue);
}

//设置属性
bool CAttributeManager::SetAttributeValue(LPCTSTR pszName, LPCTSTR pszValue)
{
	IAttribute * pAttribute=QueryAttribute(pszName);
	if (pAttribute==NULL) return false;
	return pAttribute->SetValue(pszValue);
}

//////////////////////////////////////////////////////////////////////////
