/*------------------------------------------------------
|
|			麻将类:		tMj.h
|
--------------------------------------------------------*/
#pragma once
#include "stdafx.h"
#define ORG_MJ_MAX				136    

#define MJ_WIDTH 45
#define MJ_HEIGHT 69


static enum MjStyle
{
	MJ_NULL   = 0,
	MJ_WAN ,
	MJ_BING,
	MJ_TIAO,
	MJ_FEN,
	MJ_ZI,
	MJ_HUA,
};
static enum MjToward
{
	MJT_BACK = 0,
	MJT_EAST,			//以自已方向为东
	MJT_SOUTH,
	MJT_WEST,
	MJT_NORTH,
};
static enum MjState
{
	MJS_TABLE = 0,			//桌面
	MJS_IN ,
	MJS_OUT,
	MJS_HU,
};
class tMj
{
private:
	BYTE m_value;			//牌值1~9	

public:
	tMj();
	~tMj();
	bool operator == ( tMj& ptdc );
	void operator =  ( tMj& tdc );
	tMj operator +( int value );

	void newtMj();
	bool isUpperthan( tMj* ptdc );
	bool isUpperthan( tMj& tdc );
	void SetValue( BYTE value );
	BYTE GetValue();
};