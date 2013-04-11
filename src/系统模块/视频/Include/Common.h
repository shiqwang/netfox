#pragma once

#include <windows.h>

//得到程序路径
inline void GetProgPath( TCHAR *pPath=NULL, TCHAR* ExtFileName=NULL )
{
	char Buf[MAX_PATH];
	char drive[_MAX_DRIVE], dir[_MAX_DIR];

	GetModuleFileNameA( NULL, Buf, MAX_PATH );
	
#pragma warning(disable : 4996)
	_splitpath(Buf,drive,dir,NULL,NULL);
#pragma warning(disable:  )

	if( pPath )
	{
#ifdef _UNICODE
		USES_CONVERSION;
		wsprintf( pPath, _T("%s%s%s"), A2W(drive), A2W(dir), ExtFileName );
#else
		wsprintf( pPath, _T("%s%s%s"), drive, dir, ExtFileName );
#endif
	}
//	return (CString)drive + dir + ExtFileName;
};

//检测实例
inline BOOL CheckInstance( LPCTSTR InstanceName )
{
	// 保证只有一个GameHub进程	
	BYTE sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &sd, FALSE}, *psa = NULL;
	if( InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) )
        if( SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE) )
			psa = &sa;

	HANDLE hSem = CreateSemaphore(psa,1,1,InstanceName);
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		return TRUE;
	}
	return FALSE;
}

inline void OutInfo( const char* s )
{
	printf( s );
}