#include "StdAfx.h"
#include "IniFile.h"

CIniFile::CIniFile(void)
{
	ZeroMemory( m_FileName, sizeof(m_FileName) );
}

CIniFile::~CIniFile(void)
{
}

void CIniFile::Load( LPCTSTR FileName )
{
	lstrcpy( m_FileName, FileName );
}

int CIniFile::GetInt(LPCTSTR pszSectionName, LPCTSTR pszKeyName)
{
	return GetPrivateProfileInt(pszSectionName, pszKeyName, 0, m_FileName);
}

int CIniFile::SetInt(LPCTSTR pszSectionName, LPCTSTR pszKeyName, int nValue)
{
    TCHAR szInt[10];
    wsprintf(szInt, _T("%d"), nValue);
    int err = WritePrivateProfileString(pszSectionName, pszKeyName, szInt, m_FileName);
	err = GetLastError();
	return 0;
}

int CIniFile::GetString(LPCTSTR pszSectionName, LPCTSTR pszKeyName, LPTSTR pszBuffer, int nLen)
{
    return GetPrivateProfileString(pszSectionName, pszKeyName, NULL, pszBuffer, nLen, m_FileName);
}

/*
int CIniFile::GetString(CString strSectionName, CString strKeyName, CString &strBuffer,int nLen)
{
    char *pszBuffer = new char[nLen];
    
    int n = GetPrivateProfileString(strSectionName, strKeyName, NULL, pszBuffer, nLen, m_FileName);

	if(n<=0)
		strBuffer = "";
	else
		strBuffer = pszBuffer;

    delete []pszBuffer;
	return n;
}
*/
int CIniFile::SetString(LPCTSTR pszSectionName, LPCTSTR pszKeyName, LPCTSTR pszBuffer)
{
    return WritePrivateProfileString(pszSectionName, pszKeyName, pszBuffer,m_FileName);
}