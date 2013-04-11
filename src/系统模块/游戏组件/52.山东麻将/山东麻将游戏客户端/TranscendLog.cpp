//TranscendLog.cpp
#include "stdafx.h"
#include "TranscendLog.h"

TranscendLog::TranscendLog()
{
	bSigned = false;
	bTimeSigned = false;
	bConsel = false;
	m_iLine = 0;
	GetLocalTime( &time_start );
	strTime = new char[256];
	sprintf( strTime, "%d年%d月%d日，%d时%d分%d秒", time_start.wYear, time_start.wMonth, time_start.wDay, time_start.wHour, time_start.wMinute, time_start.wSecond );
	char *fileName;
	m_fileName = new char[256];
	fileName = new char[256];

	//默认创建Debug.log文件
	sprintf( fileName, ".\\Debug.log" );
	ptr_fileList_head = new FILELIST;
	if( (ptr_fileList_head->file = fopen( fileName, "w" ) ) == NULL )
		MessageBox( 0, "打开文件失败!", 0, 0 );
	ptr_fileList_head->uId = 1;
	ptr_fileList_head->filename = new char[256];
	ptr_fileList_head->iEcount = 0;
	strcpy( ptr_fileList_head->filename, fileName );
	ptr_fileList_head->next = NULL;
	ptr_fileList = ptr_fileList_head;
	delete fileName;
}

TranscendLog::~TranscendLog()
{
	clear();
	if( m_fileName )
	{
		delete []m_fileName;
	}
	if( strTime )
	{
		delete []strTime;
	}
}

void TranscendLog::clear()
{
	char *str = new char[256];
	//关闭已打开文件
	ptr_fileList = ptr_fileList_head;
	while( ptr_fileList->next != NULL )
	{
		DrawLine( 1, ptr_fileList->uId );
		sprintf( str, "错误数：	%d", ptr_fileList->iEcount );
		Report( str,ptr_fileList->uId ); 
		fclose( ptr_fileList->file );
		ptr_fileList = ptr_fileList->next;
	}
	DrawLine( 1, ptr_fileList->uId );
	sprintf( str, "错误数：	%d", ptr_fileList->iEcount );
	Report( str,ptr_fileList->uId );
	fclose( ptr_fileList->file );	
	//删除指针链表
	ptr_fileList = ptr_fileList_head;
	while( ptr_fileList->next != NULL )
	{
		PTR_FILELIST ptr_tmp;
		ptr_tmp = ptr_fileList;
		delete []ptr_tmp->filename;
		ptr_fileList = ptr_fileList->next;
		delete ptr_tmp;
	}
	delete []ptr_fileList->filename;

	delete ptr_fileList;
}

void TranscendLog::GetLogTime()
{
	GetLocalTime( &time_end );
	sprintf( strTime, 	"%d年%d月%d日，%d时%d分%d秒", time_end.wYear, time_end.wMonth, time_end.wDay, time_end.wHour, time_end.wMinute, time_end.wSecond );
}
/*************************************************************
*				函数名: OpenNewFile(……)
*				功能:	打开文件，没有则创建
*				参数:	filename-文件名
						uId-要附于的文件名ID号
*************************************************************/
void TranscendLog::OpenNewFile( char *filename, unsigned int ufileId )
{
	//char *str = new char[256];
	//strcpy( str,filename );
	//指向表尾
	ptr_fileList = ptr_fileList_head;
	while( ptr_fileList->next != NULL )
	{
		if( ptr_fileList->uId == ufileId )
		{
			MessageBox( 0, "文件ID已定义", 0, 0 );
			return;
		}
		ptr_fileList = ptr_fileList->next;
	}
	ptr_fileList->next = new FILELIST;
	ptr_fileList->next->file = fopen( filename, "w" );
	ptr_fileList->next->filename = new char[256];
	strcpy( ptr_fileList->next->filename, filename );
	ptr_fileList->next->uId = ufileId;
	ptr_fileList->next->iEcount = 0;
	ptr_fileList->next->next = NULL;
	ptr_fileList = ptr_fileList->next;

	//delete []str;

}
/*************************************************************
*				函数名: DrawLine(……)
*				功能:	标志
*				参数:	style-画线类型
*************************************************************/
void TranscendLog::DrawLine( int style, unsigned int fileId )
{
	ptr_fileList = ptr_fileList_head;
	while( ptr_fileList->uId != fileId && ptr_fileList->next != NULL )
	{
		ptr_fileList = ptr_fileList->next;
	}
	if( ptr_fileList->uId != fileId )
	{
		MessageBox( 0, "OpenFile,Error!", 0, 0 );
		return ;
	}
	FILE *file;
	file = ptr_fileList->file;
	char *fileName = ptr_fileList->filename;
	file = ptr_fileList->file;

#ifdef DEBUG
	{
		if( file )
		{
			fclose( file );
		}
		if( (file = fopen( fileName, "a"))==NULL )
		{
			MessageBox( 0, "打开文件失败!", 0, 0 );
		}
	}
#endif
	if( style == 1 )
	{
		fprintf( file , "*******************************************************************************\n" );
		if( bConsel )
		{
			printf( "********************************************************************************\n" );
		}
	}
	else if( style == 2 )
	{
		fprintf( file, "................................................................................\n" );
		if( bConsel )
		{
			printf( "................................................................................\n" );
		}
	}
	else if( style == 3 )
	{
		fprintf( file, "--------------------------------------------------------------------------------\n" );
		if( bConsel )
		{
			printf( "--------------------------------------------------------------------------------\n" );
		}
	}
	else
	{
		fprintf( file, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
		if( bConsel )
		{
			printf( "--------------------------------------------------------------------------------\n" );
		}
	}
}

/*************************************************************
*				函数名: Report(……)
*				功能:	输出报告
*				参数:	str-输出信息
*						fileId-文件ID
*************************************************************/
void TranscendLog::Report( char *str, unsigned int fileId )
{
	//寻找文件
	ptr_fileList = ptr_fileList_head;
	while( ptr_fileList->uId != fileId && ptr_fileList->next != NULL )
	{
		ptr_fileList = ptr_fileList->next;
	}
	if( ptr_fileList->uId != fileId )
	{
		MessageBox( 0, "OpenFile,Error!", 0, 0 );
		return ;
	}
	FILE *file;
	char *fileName = ptr_fileList->filename;
	file = ptr_fileList->file;
#ifdef DEBUG
	{
		if( file )
		{
			fclose( file );
		}
		if( (file = fopen( fileName, "a"))==NULL )
		{
			MessageBox( 0, "打开文件失败!", 0, 0 );
		}
	}
#endif
	//输出到文件

	if( bSigned && !bTimeSigned)
	{
		fprintf( file, "%s", str );
		fprintf( file, "  FILE：%s,LINE：%d\n",m_fileName, m_iLine);
		if( bConsel )
		{
			printf( "%s", str );
			printf( " FILE：%s,LINE：%d\n",m_fileName, m_iLine );
		}
	}
	else if( bTimeSigned && !bSigned )
	{
		GetLogTime();
		fprintf( file, "%s", str );
		fprintf( file, "  TIME：%s\n", strTime );
		if( bConsel )
		{
			printf( "%s", str );
			printf(  " TIME：%s\n", strTime  );
		}
	}
	else if( bSigned && bTimeSigned )
	{
		fprintf( file, "%s", str );
		fprintf( file, "  FILE：%s,LINE：%d",m_fileName, m_iLine);
		fprintf( file, "  TIME：%s\n", strTime );
		if( bConsel )
		{
			printf( "%s", str );
			printf( " FILE：%s,LINE：%d\n",m_fileName, m_iLine );
		}
	}
	else
	{
		fprintf( file, "%s\n", str );
		//consel窗口
		if( bConsel )
		{
			printf( "%s\n", str );
		}
	}
}
/*************************************************************
*				函数名: ReportError(……)
*				功能:	报告异常
*				参数:	EInfo-异常信息
*						fileId-文件ID
*************************************************************/
void TranscendLog::ReportError( char *EInfo,unsigned int fileId )
{
	ptr_fileList = ptr_fileList_head;
	while( ptr_fileList->uId != fileId && ptr_fileList->next != NULL )
	{
		ptr_fileList = ptr_fileList->next;
	}
	if( ptr_fileList->uId != fileId )
	{
		MessageBox( 0, "OpenFile,Error!", 0, 0 );
		return ;
	}
	ptr_fileList->iEcount++;
	char *Error;
	Error = new char[256];
	strcpy( Error, "ERROR：" );
	strcat( Error, EInfo );
	Report( Error, fileId );
	delete []Error;
}

/*************************************************************
*				函数名: ReportNormal(……)
*				功能:	报告正常
*				参数:	NInfo-正常信息
*						fielID-文件ID
*************************************************************/
void TranscendLog::ReportNormal( char *NInfo,unsigned int fileId )
{
	char *Normal;
	Normal = new char[256];
	strcpy( Normal, "NORMAL：" );
	strcat( Normal, NInfo );
	Report( Normal, fileId );
	delete []Normal;
}
/*************************************************************
*				函数名: ReportVariable(……)
*				功能:	输出变量
*				参数:	VariableName-变量名
*						value-变量值
*						fileId-文件ID
*************************************************************/
void TranscendLog::ReportVariable( char *VariableName, char *value, unsigned int fileId )
{
	char *VariableInfo;
	VariableInfo = new char[100];
	strcpy( VariableInfo, "VARIABLE：" );
	strcat( VariableInfo, VariableName );
	sprintf( VariableInfo, "%s=%s", VariableInfo, value );
	Report( VariableInfo, fileId );
	delete []VariableInfo;
}

void TranscendLog::ReportVariable( char *VariableName, int value, unsigned int fileId )
{
	char *VariableInfo;
	VariableInfo = new char[100];
	strcpy( VariableInfo, "VARIABLE：" );
	strcat( VariableInfo, VariableName );
	sprintf( VariableInfo, "%s=%d", VariableInfo, value );
	Report( VariableInfo, fileId );
	delete []VariableInfo;
}
 
void TranscendLog::ReportVariable( char *VariableName, unsigned int value, unsigned int fileId )
{
	char *VariableInfo;
	VariableInfo = new char[100];
	strcpy( VariableInfo, "VARIABLE：" );
	strcat( VariableInfo, VariableName );
	sprintf( VariableInfo, "%s=%d", VariableInfo, value );
	Report( VariableInfo, fileId );
}

void TranscendLog::ReportVariable( char *VariableName, double value, unsigned int fileId )
{
	char *VariableInfo;
	VariableInfo = new char[100];
	strcpy( VariableInfo, "VARIABLE：" );
	strcat( VariableInfo, VariableName );
	sprintf( VariableInfo, "%s=%l", VariableInfo, value );
	Report( VariableInfo, fileId );
	delete []VariableInfo;
}

void TranscendLog::ReportVariable( char *VariableName, float value, unsigned int fileId )
{
	char *VariableInfo;
	VariableInfo = new char[100];
	strcpy( VariableInfo, "VARIABLE：" );
	strcat( VariableInfo, VariableName );
	sprintf( VariableInfo, "%s=%f", VariableInfo, value );
	Report( VariableInfo, fileId );
	delete []VariableInfo;
}

void TranscendLog::ReportVariable( char *VariableName, std::string value, unsigned int fileId )
{
	char *VariableInfo;
	VariableInfo = new char[100];
	strcpy( VariableInfo, "VARIABLE：" );
	strcat( VariableInfo, VariableName );
	sprintf( VariableInfo, "%s=%s", VariableInfo, value.c_str() );
	Report( VariableInfo, fileId );
	delete []VariableInfo;
}