#pragma once
#include "..\Include\StlMap.h"
#include "windows.h"

//字节含义
typedef enum
{
	T_BYTE = 0x01,
	T_WORD,
	T_DWORD,
	T_DATA,
	T_LEN_DATA
};

typedef CSTLMap<WORD,string> MAP_PACKET_CMD; //包结构类型

//封包结构类
class CPacket
{
protected:
	MAP_PACKET_CMD m_PacketCmd; //结构集合
public:
	CPacket(void);
	virtual ~CPacket(void);

	virtual void AddPacket( WORD Packet, ... ); //添加封包
	virtual int GetPacket( char* pBuffer, int length, WORD Packet, ... ); //得到封包
	virtual	int GetValue( const char* pBuffer, int length, WORD Packet, ... ); //得到值
};
