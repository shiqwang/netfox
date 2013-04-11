#pragma once
#include "PacketDefine.h"
#include "packet.h"

class CMyPacket :
	public CPacket
{
public:
	CMyPacket(void);
	virtual ~CMyPacket(void);
};
