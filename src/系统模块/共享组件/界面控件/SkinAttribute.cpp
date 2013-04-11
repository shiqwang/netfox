#include "Stdafx.h"
#include "SkinButton.h"
#include "SkinAttribute.h"

//最大记录缓冲
#define MAX_RECORD_BUF				256

//////////////////////////////////////////////////////////////////////////

//静态变量
bool					CSkinAttribute::m_bFlags=false;							//内部标志
CFont					CSkinAttribute::m_DefaultFont;							//默认字体
ISkinRecord	*			CSkinAttribute::m_pISkinRecord=NULL;					//配置接口
ISkinAttribute *		CSkinAttribute::m_SkinPtrArray[MAX_SKIN];				//属性数组

//////////////////////////////////////////////////////////////////////

//构造函数
CSkinRecordFile::CSkinRecordFile()
{
	m_szIniFileName[0]=0;
	m_szResDirectory[0]=0;
}

//析构函数
CSkinRecordFile::~CSkinRecordFile()
{
}

//接口查询
void * __cdecl CSkinRecordFile::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ISkinRecord,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ISkinRecord,Guid,dwQueryVer);
	return NULL;
}

//获取目录
LPCTSTR __cdecl CSkinRecordFile::GetResourceDirectory()
{
	return m_szResDirectory;
}

//读取参数
int __cdecl CSkinRecordFile::ReadInt(LPCTSTR pszClassName, LPCTSTR pszItem, int iDefValue)
{
	return GetPrivateProfileInt(pszClassName,pszItem,iDefValue,m_szIniFileName);
}

//读取参数
COLORREF __cdecl CSkinRecordFile::ReadColor(LPCTSTR pszClassName, LPCTSTR pszItem, COLORREF crDefValue)
{
	TCHAR szBuffer[MAX_RECORD_BUF]=TEXT("");
	GetPrivateProfileString(pszClassName,pszItem,TEXT(""),szBuffer,sizeof(szBuffer),m_szIniFileName);
	if (szBuffer[0]!=0)
	{
		int nColorRed=GetRValue(crDefValue);
		int nColorGreen=GetGValue(crDefValue);
		int nColorBule=GetBValue(crDefValue);
		sscanf(szBuffer,TEXT("RGB(%d,%d,%d)"),&nColorRed,&nColorGreen,&nColorBule);
		return RGB(nColorRed,nColorGreen,nColorBule);
	}
	return crDefValue;
}

//读取参数
RECT __cdecl CSkinRecordFile::ReadRect(LPCTSTR pszClassName, LPCTSTR pszItem, const RECT & rcDefValue)
{
	TCHAR szBuffer[MAX_RECORD_BUF]=TEXT("");
	GetPrivateProfileString(pszClassName,pszItem,TEXT(""),szBuffer,sizeof(szBuffer),m_szIniFileName);
	if (szBuffer[0]!=0)
	{
		RECT Rect=rcDefValue;
		sscanf(szBuffer,TEXT("%d,%d,%d,%d"),&Rect.left,&Rect.top,&Rect.right,&Rect.bottom);
		return Rect;
	}
	return rcDefValue;
}

//读取参数
POINT __cdecl CSkinRecordFile::ReadPoint(LPCTSTR pszClassName, LPCTSTR pszItem, const POINT & ptDefValue)
{
	TCHAR szBuffer[MAX_RECORD_BUF]=TEXT("");
	GetPrivateProfileString(pszClassName,pszItem,TEXT(""),szBuffer,sizeof(szBuffer),m_szIniFileName);
	if (szBuffer[0]!=0)
	{
		POINT Point=ptDefValue;
		sscanf(szBuffer,TEXT("%d,%d"),&Point.x,&Point.y);
		return Point;
	}
	return ptDefValue;
}

//读取参数
CString __cdecl CSkinRecordFile::ReadString(LPCTSTR pszClassName, LPCTSTR pszItem, LPCTSTR pszDefValue)
{
	TCHAR szBuffer[MAX_RECORD_BUF]=TEXT("");
	GetPrivateProfileString(pszClassName,pszItem,TEXT(""),szBuffer,sizeof(szBuffer),m_szIniFileName);
	if (szBuffer[0]!=0) return szBuffer;
	return pszDefValue;
}

//设置目录
bool CSkinRecordFile::InitSkinRecordFile(LPCTSTR pszIniFileName, LPCTSTR pszResDirectory)
{
	m_szIniFileName[0]=0;
	m_szResDirectory[0]=0;
	if (pszIniFileName!=NULL) lstrcpyn(m_szIniFileName,pszIniFileName,sizeof(m_szIniFileName));
	if (pszResDirectory!=NULL)  lstrcpyn(m_szResDirectory,pszResDirectory,sizeof(m_szResDirectory));

	return true;
}

//////////////////////////////////////////////////////////////////////

//构造函数
CSkinAttribute::CSkinAttribute(LPCTSTR pszClassName) 
{ 
	//设置变量
	ASSERT(pszClassName);
	lstrcpyn(m_szClassName,pszClassName,sizeof(m_szClassName));
	RegisterAttribute(SKIN_CONTROL_CLASS_NAME_STRING,m_szClassName,true);

	//创建对象
	if (m_DefaultFont.GetSafeHandle()==NULL) 
		m_DefaultFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));

	//注册组件
	if (m_bFlags==false)
	{
		m_bFlags=true;
		memset(m_SkinPtrArray,0,sizeof(m_SkinPtrArray));
	}
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	ASSERT(pIUnknownEx!=NULL);
	RegisterSkinClass(pIUnknownEx);

	return;
}

//析构函数
CSkinAttribute::~CSkinAttribute()
{
	//注销组件
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	ASSERT(pIUnknownEx!=NULL);
	UnRegisterSkinClass(pIUnknownEx);

	return;
}

//接口查询
void * CSkinAttribute::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ISkinAttribute,Guid,dwQueryVer);
	return CAttributeManager::QueryInterface(Guid,dwQueryVer);
}

//读取参数
int CSkinAttribute::ReadInt(LPCTSTR pszItem, int iDefValue)
{
	if (m_pISkinRecord==NULL) return iDefValue;
	return m_pISkinRecord->ReadInt(GetClassName(),pszItem,iDefValue);
}

//读取参数
COLORREF CSkinAttribute::ReadColor(LPCTSTR pszItem, COLORREF crDefValue)
{
	if (m_pISkinRecord==NULL) return crDefValue;
	return m_pISkinRecord->ReadColor(GetClassName(),pszItem,crDefValue);
}

//读取参数
RECT CSkinAttribute::ReadRect(LPCTSTR pszItem, const RECT & rcDefValue)
{
	if (m_pISkinRecord==NULL) return rcDefValue;
	return m_pISkinRecord->ReadRect(GetClassName(),pszItem,rcDefValue);
}

//读取参数
POINT CSkinAttribute::ReadPoint(LPCTSTR pszItem, const POINT & ptDefValue)
{
	if (m_pISkinRecord==NULL) return ptDefValue;
	return m_pISkinRecord->ReadPoint(GetClassName(),pszItem,ptDefValue);
}

//读取参数
CString CSkinAttribute::ReadString(LPCTSTR pszItem, LPCTSTR pszDefValue)
{
	if (m_pISkinRecord==NULL) return pszDefValue;
	return m_pISkinRecord->ReadString(GetClassName(),pszItem,pszDefValue);
}

//写入参数
bool CSkinAttribute::WriteInt(LPCTSTR pszItem, int iValue)
{
	if (m_pISkinRecord==NULL) return false;
	return m_pISkinRecord->WriteInt(GetClassName(),pszItem,iValue);
}

//写入参数
bool CSkinAttribute::WriteColor(LPCTSTR pszItem, COLORREF crValue)
{
	if (m_pISkinRecord==NULL) return false;
	return m_pISkinRecord->WriteColor(GetClassName(),pszItem,crValue);
}

//写入参数
bool CSkinAttribute::WriteRect(LPCTSTR pszItem, const RECT & rcValue)
{
	if (m_pISkinRecord==NULL) return false;
	return m_pISkinRecord->WriteRect(GetClassName(),pszItem,rcValue);
}

//写入参数
bool CSkinAttribute::WritePoint(LPCTSTR pszItem, const POINT & ptValue)
{
	if (m_pISkinRecord==NULL) return false;
	return m_pISkinRecord->WritePoint(GetClassName(),pszItem,ptValue);
}

//写入参数
bool CSkinAttribute::WriteString(LPCTSTR pszItem, LPCTSTR pszValue)
{
	if (m_pISkinRecord==NULL) return false;
	return m_pISkinRecord->WriteString(GetClassName(),pszItem,pszValue);
}

//加载界面信息
bool CSkinAttribute::LoadSkinAttribute(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	ISkinRecord * pISkinRecord=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ISkinRecord);
	ASSERT(pISkinRecord!=NULL);
	if (pISkinRecord==NULL) return false;

	//设置接口
	m_pISkinRecord=pISkinRecord;

	//改变界面
	bool bSuccess=true;
	for (int i=0;i<MAX_SKIN;i++)
	{
		if (m_SkinPtrArray[i]==NULL) break;
		if (!m_SkinPtrArray[i]->LoadSkinOption()) bSuccess=false;
	}

	return bSuccess;
}

//保存界面信息
bool CSkinAttribute::SaveSkinAttribute()
{
	bool bSuccess=true;
	for (int i=0;i<MAX_SKIN;i++)
	{
		if (m_SkinPtrArray[i]==NULL) break;
		if (!m_SkinPtrArray[i]->SaveSkinOption()) bSuccess=false;
	}
	return bSuccess;
}

//资源目录
LPCTSTR CSkinAttribute::GetResourceDirectory()
{
	ASSERT(m_pISkinRecord!=NULL);
	if (m_pISkinRecord==NULL) return NULL;
	return m_pISkinRecord->GetResourceDirectory();
}

//枚举属性
void * CSkinAttribute::GetAttributeManager(INT_PTR nIndex, const IID & Guid, DWORD dwQueryVer)
{
	if (nIndex>=MAX_SKIN) return NULL;
	ISkinAttribute * pISkinAttribute=m_SkinPtrArray[nIndex];
	if (pISkinAttribute==NULL) return NULL;
	return pISkinAttribute->QueryInterface(Guid,dwQueryVer);
}

//注册界面类
bool CSkinAttribute::RegisterSkinClass(IUnknownEx * pIUnknownEx)
{
	int i=0;
	//查询参数
	ASSERT(pIUnknownEx!=NULL);
	ISkinAttribute * pISkinAttribute=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ISkinAttribute);
	ASSERT(pISkinAttribute!=NULL);
	if (pISkinAttribute==NULL) return false;

	//查找组件
	for (i=0;i<MAX_SKIN;i++)	
	{
		if (m_SkinPtrArray[i]==NULL) break;
		if (pISkinAttribute==m_SkinPtrArray[i])
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//注册组件
	ASSERT(i<MAX_SKIN);
	if (i>=MAX_SKIN) return false;
	m_SkinPtrArray[i]=pISkinAttribute;

	return true;
}

//注销界面类
bool CSkinAttribute::UnRegisterSkinClass(IUnknownEx * pIUnknownEx)
{
	int j = 0;
	//查询参数
	ASSERT(pIUnknownEx!=NULL);
	ISkinAttribute * pISkinAttribute=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ISkinAttribute);
	ASSERT(pISkinAttribute!=NULL);
	if (pISkinAttribute==NULL) return false;

	//查找组件
	for (int i=0;i<MAX_SKIN;i++)
	{
		if (m_SkinPtrArray[i]==NULL) break;
		if (pISkinAttribute==m_SkinPtrArray[i])
		{
			for (j=i;j<MAX_SKIN-1;j++) 
			{
				if (m_SkinPtrArray[j+1]==NULL) break;
				m_SkinPtrArray[j]=m_SkinPtrArray[j+1];
			}
			m_SkinPtrArray[j]=NULL;
			return true;
		}
	}

	ASSERT(FALSE);
	return false;
}

//////////////////////////////////////////////////////////////////////////
