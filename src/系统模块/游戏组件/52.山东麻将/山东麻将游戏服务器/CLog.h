//CLog.h

#pragma once
//#define DEBUG
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string>

#define SETLINE(Log) \
	Log.m_iLine = __LINE__;\
	strcpy( Log.m_fileName, __FILE__);

#define SETSIGNED( Log )\
	Log.bSigned = true;\
	Log.bTimeSigned = true;

#define CANCELTIME( Log )\
	Log.bTimeSigned = false;

#define CANCELSIGNED( Log )\
	Log.bSigned = false;\
	Log.bTimeSigned = false;

#define REPORT_E( Log, sEInfo,ufileID )\
	if( Log.bSigned )\
	{ Log.m_iLine = __LINE__;\
	strcpy( Log.m_fileName, __FILE__ );}\
	Log.ReportError( sEInfo, ufileID );

#define REPORT_N( Log, sNInfo,ufileID )\
	if( Log.bSigned )\
	{ Log.m_iLine = __LINE__;\
	strcpy( Log.m_fileName,__FILE__ );}\
	Log.ReportNormal( sNInfo, ufileID );

#define REPORT_V( Log, Vname,value, ufileId )\
	if( Log.bSigned )\
	{\
	Log.m_iLine = __LINE__;\
	strcpy( Log.m_fileName, __FILE__ );\
	}\
	Log.ReportVariable( Vname, value, ufileId );

typedef struct _FILELIST
{
	FILE *file;
	char *filename;
	int iEcount;
	_FILELIST *next;
	unsigned int uId;
}*PTR_FILELIST,FILELIST;

class CLog
{
private:

	PTR_FILELIST ptr_fileList_head, ptr_fileList;
	void GetLogTime();	
	char *strTime;
	bool bConsel;
	SYSTEMTIME time_start;
	SYSTEMTIME time_end;

public:	
	bool bSigned;
	bool bTimeSigned;
	int  m_iLine;
	char *m_fileName;
	CLog();
	~CLog();

	void OpenNewFile( char *filename, unsigned int ufileId );
	void DrawLine( int style, unsigned int fileId );
	void clear();
	void Reset();

	void Report( char *str, unsigned int fileId );
	void ReportError( char *EInfo,unsigned int fileId ); 
	void ReportNormal( char *Ninfo,unsigned int fileId ); 
	void ReportVariable( char *VariableName, char *value, unsigned int fileId );
	void ReportVariable( char *VariableName, int value, unsigned int fileId );
	void ReportVariable( char *VariabelName, unsigned value, unsigned int fileId );
	void ReportVariable( char *VariableName, float value, unsigned int fileId );
	void ReportVariable( char *VariableName, double value, unsigned int fileId );
	void ReportVariable( char *VariableName, std::string value, unsigned int fileID );
};