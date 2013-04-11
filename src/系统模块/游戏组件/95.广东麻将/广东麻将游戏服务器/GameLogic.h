/*-------------------------------------------------------------------------------------------
|
|								Mj		CGameLogic.cpp
|	
|------------------------------------------------------------------------------------------*/


#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once 
#include "..\消息定义\CMD_Mj.h"

//数值掩码
#define LOGIC_MASK_VALUE		0x0F							//麻将数值掩码
#define LOGIC_MASK_TYPE			0xF0							//麻将类型掩码

//圈风
static enum FENG
{
	FENG_EAST_EAST = 0,
	FENG_EAST_SOUTH,
	FENG_EAST_WEST,
	FENG_EAST_NORTH,
	FENG_SOUTH_EAST,
	FENG_SOUTH_SOUTH,
	FENG_SOUTH_WEST,
	FENG_SOUTH_NORTH,
	FENG_WEST_EAST,
	FENG_WEST_SOUTH,
	FENG_WEST_WEST,
	FENG_WEST_NORTH,
	FENG_NORTH_EAST,
	FENG_NORTH_SOUTH,
	FENG_NORTH_WEST,
	FENG_NORTH_NORTH,
};

//麻将类型结构
static enum BLOCKSTYLE
{	
	BLOCK_NULL	 = 0,				//无
	BLOCK_CHI,						//吃牌结构
	BLOCK_SUN,	 					//顺序结构
	BLOCK_PENG,	 					//碰子结构,3个相同	
	BLOCK_KE,						//刻子结构,3个相同,非碰
	BLOCK_GANG,						//杠子结构,4个相同
	BLOCK_AN_GANG ,					//暗杠
};

//麻将块结构
typedef struct BLOCK
{
	BYTE first;						//麻将块第一张牌值
	BLOCKSTYLE style;				//麻将块类型
}MJ_BLOCK,*PTR_MJ_BLOCK;

//胡牌结构
typedef struct HU
{
	BYTE jiangMj;					//将牌
	BLOCK block[(HAND_MJ_MAX-2)/3];				//麻将块
}MJ_HU, *PTR_MJ_HU;

//杠牌
typedef struct GANG
{
	BYTE GangMj;				//所杠的牌
	BYTE GangStyle;				//杠牌类型,4-暗杠,2-自摸明杠,1-普通杠,0-无杠
}MJ_GANGINFO, *PTR_MJ_GANGINFO;

class CGameLogic
{
	//变量定义
protected:
	//static const BYTE m_byMjs[ORG_MJ_MAX];								//麻将牌数据
	static const BYTE mjThirteen[13];									//十三幺牌
	static const BYTE mjNineLight[13];									//九莲灯

	//构造,析构
public:
	CGameLogic();
	~CGameLogic();

	//类型函数
public:
	//获取麻将类型
	BYTE GetMjType( BYTE byMjData )	{ return byMjData&LOGIC_MASK_TYPE; }
	//获取麻将数值
	BYTE GetMjValue( BYTE bMjData ) { return bMjData&LOGIC_MASK_VALUE; }

	//控制函数
public:
	//排列麻将
	void SortMj( BYTE byMjs[], BYTE byMjCount, BYTE byMjMax );
	//混乱麻将
	void RandMj( BYTE byMjs[], BYTE byMjCount );
	//删除麻将

	//逻辑函数
public:
	//是否为将牌
	bool IsJiang( BYTE byMj1, BYTE byMj2 );
	//是否可以吃牌, 返回吃牌信息，0-不成功,1-**@, 2-*@*, 3-@@*;
	int IsChi( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj );
	//是否可以碰牌	
	bool IsPeng( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj );
	//是否可以明杠
	MJ_GANGINFO IsGang( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj );
	//是否自摸明杠
	MJ_GANGINFO IsZMGang( BYTE byHandMj[], BYTE byHandMjCount, HU hu, BYTE jPos1=255, BYTE jPos2=255 );
	//是否暗杠
	MJ_GANGINFO IsAnGang( BYTE byHandMj[], BYTE byHandMjCount,BYTE jPos1=255, BYTE jPos2=255 );

	//是否可以胡牌
	int IsHu( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj, HU hu, HU *tempHu );
	//特殊胡牌
	bool IsSpecialHu( BYTE tempMj[] );
	bool IsThirteenOne( BYTE tempMj[] );
	bool IsSevenPairs( BYTE tempMj[] );
	bool IsNineLight( BYTE tempMj[], BYTE byMj );

	//辅助函数
public:
	//番计算
	long Pow( long x, long y);
	//投骰子
	int ActionDice();
	//左移麻将
	bool LMove( BYTE byMjs[], BYTE byRestCount );
	bool RMove( BYTE byMjs[] );
	//右移麻将
	bool RMove( BYTE byMjs[], BYTE byRestCount, BYTE MoveCount=1 );
	//插入麻将
	void InsertMjList( BYTE byMjList[], BYTE byRestCount );
	
	//麻将值转换
	//麻将值--图像校正
	bool TransMjDToI( BYTE srcMjData, BYTE *desMjData );
	bool TransMjDToI( BYTE srcMjData[], BYTE *desMjData[], BYTE desCount );

	//图像用值--麻将值
	bool TransMjIToD( BYTE srcMjData, BYTE *desMjData );

	//添加麻将块进胡牌结构
	void AddBlock( HU *hu, PTR_MJ_BLOCK block );
	//清理胡牌结构数据
	void CleanHu( HU *hu );
	//拷贝胡牌结构
	void CopyHu( HU *destHu, HU *srcHu );


	//门风计算
	BYTE TransIDToFeng(  WORD wChairID, WORD wBanker );

	//操作
public:
	//碰
	BLOCK Peng( BYTE byMjs[], BYTE byRestCount, BYTE byMj );
	//吃
	BLOCK Chi( BYTE byMjs[], BYTE byRestCount, BYTE byMj, int ChiMode );
	//杠
	BLOCK MGang( BYTE byMjs[], BYTE byRestCount, BYTE byMj );
	//自摸杠
	void ZMGang( BYTE byMjs[], BYTE byRestCount, HU *hu, BYTE GangMj );
	//暗杠
	BLOCK AnGang( BYTE byMjs[], BYTE byRestCount, BYTE GangMj );
	//分析胡牌，返回番值
	long CheckGDHu( HU hu, BYTE byFengWei, BYTE byQuanWei, char *strFang );

	//内部函数
private:
	//类型分析判断
	MJ_BLOCK CheckBlock( BYTE byMj1, BYTE byMj2, BYTE byMj3 );
};

#endif