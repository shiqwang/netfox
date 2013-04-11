#ifndef DATA_STORAGE_HEAD_FILE
#define DATA_STORAGE_HEAD_FILE

#include "ComService.h"

#pragma once

//////////////////////////////////////////////////////////////////////////
//结构体定义

//数据队列头
struct tagDataHead
{
	WORD							wDataSize;							//数据大小
	WORD							wIdentifier;						//类型标识
	DWORD							dwInsertTime;						//插入时间
};

//负荷信息
struct tagBurthenInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwBufferSize;						//缓冲区长度
	DWORD							dwDataPacketCount;					//数据包数目
};

//////////////////////////////////////////////////////////////////////////

//数据存储类
class COM_SERVICE_CLASS CDataStorage
{
	//数据变量
protected:
	DWORD							m_dwDataSize;						//数据大小
	DWORD							m_dwBufferSize;						//缓冲区长度
	DWORD							m_dwInsertPos;						//数据插入位
	DWORD							m_dwTerminalPos;					//数据结束位
	DWORD							m_dwDataQueryPos;					//数据查询位
	DWORD							m_dwDataPacketCount;				//数据包数目
	BYTE							* m_pDataStorageBuffer;				//数据指针

	//函数定义
public:
	//构造函数
	CDataStorage(void);
	//析构函数
	virtual ~CDataStorage(void);

	//功能函数
public:
	//负荷信息
	bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);
	//插入数据
	bool AddData(WORD wIdentifier, void * const pBuffer, WORD wDataSize);
	//获取数据
	bool GetData(tagDataHead & DataHead, void * pBuffer, WORD wBufferSize);
	//删除数据
	void RemoveData(bool bFreeMemroy);
};

//////////////////////////////////////////////////////////////////////////

#endif