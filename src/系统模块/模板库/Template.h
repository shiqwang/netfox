#ifndef TEMPLATE_HEAD_FILE
#define TEMPLATE_HEAD_FILE

/////////////////////////////////////////////////////////////////////////////////////////

//计算数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//计算字符长度
#define CountString(String) ((UINT)((lstrlen(String)+1)*sizeof(TCHAR)))

//安全删除指针
#define  SafeDelete(pData)	{ try { delete pData; } catch (...) { } pData=NULL; } 

//安全删除指针
#define  SafeDeleteArray(pData)	{ try { delete [] pData; } catch (...) { } pData=NULL; } 

/////////////////////////////////////////////////////////////////////////////////////////

//模板头文件
#include "Array.h"
#include "Modelu.h"
#include "Factory.h"

/////////////////////////////////////////////////////////////////////////////////////////

#endif