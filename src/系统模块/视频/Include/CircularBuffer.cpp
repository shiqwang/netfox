#include "StdAfx.h"
#include "CircularBuffer.h"

CCircularBuffer::CCircularBuffer(int size)
: m_iBufSize(0) //初始化
, m_pBuffer(NULL)
, m_iHeadPos(0)
, m_iTailPos(0)
{
	m_iBufSize=size; //缓存大小
	m_pBuffer = new char[m_iBufSize]; //创建缓存
}

CCircularBuffer::~CCircularBuffer(void)
{
	delete [] m_pBuffer; //删除缓存
	m_pBuffer = NULL; //设置为空
}
