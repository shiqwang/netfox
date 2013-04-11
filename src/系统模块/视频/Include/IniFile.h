#pragma once

#include <windows.h>

//读ini文件
class CIniFile
{
protected:
	TCHAR m_FileName[MAX_PATH]; //文件名
public:
	CIniFile(void);
	~CIniFile(void);

	void Load( LPCTSTR FileName ); //取文件

	int GetInt(LPCTSTR pszSectionName, LPCTSTR pszKeyName); //读int类型值
	int SetInt(LPCTSTR pszSectionName, LPCTSTR pszKeyName, int nValue); //设置int类型值

	int GetString(LPCTSTR pszSectionName, LPCTSTR pszKeyName, LPTSTR pszBuffer, int nLen); //得到字符串类型值
	//int GetString(CString strSectionName, CString strKeyName, CString &strBuffer,int nLen);
	int SetString(LPCTSTR pszSectionName, LPCTSTR pszKeyName, LPCTSTR pszBuffer); //设置字符串类型值
};
