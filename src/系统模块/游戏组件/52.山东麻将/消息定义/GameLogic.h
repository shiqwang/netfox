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

//麻将类型结构
static enum BLOCKSTYLE
{	
	BLOCK_NULL	= 0,				//无
	BLOCK_SUN	= 1,				//顺序结构
	BLOCK_PENG	= 2,				//碰子结构,3个相同	
	BLOCK_KE	= 3,				//刻子结构,3个相同,非碰
	BLOCK_GANG	= 4,				//杠子结构,4个相同

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
	MJ_BLOCK block[4];				//麻将块
}MJ_HU, *PTR_MJ_HU;

//杠牌
typedef struct GANG
{
	BYTE GangPos;				//杠牌位置0~16
	BYTE GangStyle;				//杠牌类型,3-暗杠,2-自摸明杠,1-普通杠,0-无杠
}MJ_GANGINFO, *PTR_MJ_GANGINFO;

class CGameLogic
{
	//变量定义
protected:
	static const BYTE m_byMjs[ORG_MJ_MAX];						//麻将牌数据
	static const BYTE mjThirteen[13];									//十三幺牌

	//构造,析构
public:
	CGameLogic();
	~CGameLogic();

	//类型函数
public:
	//获取麻将类型
	BYTE GetMjType( BYTE bMjData[] );
	//获取麻将数值
	BYTE GetMjValue( BYTE bMjData ) { return bMjData&LOGIC_MASK_VALUE; }

	//控制函数
public:
	//排列麻将
	void SortMj( BYTE byMjs[], BYTE byMjCount );
	//混乱麻将
	void RandMj( BYTE byMjs[], BYTE byMjCount );
	//删除麻将

	//逻辑函数
public:
	//是否为将牌
	bool IsJiang( BYTE byMj1, BYTE byMj2 );
	//是否可以吃牌
	bool IsChi();
	//是否可以碰牌
	bool IsPeng();
	//是否可以胡牌
	bool IsHu( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj, HU hu, HU tempHu );
	//是否可以杠牌
	MJ_GANGINFO CheckGang();

	//辅助函数
public:
	//左移麻将
	bool LMove( BYTE byMjs[], BYTE byMaxCount, BYTE byRestCount );
	bool RMove( BYTE byMjs[] );
	//右移麻将
	bool RMove( BYTE byMjs[], BYTE byMaxCount, BYTE byRestCount, BYTE MoveCount=1 );
	//插入麻将
	void InsertMjList( BYTE byMjList[], BYTE byMaxCount, BYTE byRestCount );
	
	//麻将值转换
	//麻将值--图像校正
	bool TransMjDToI( BYTE srcMjData, BYTE *desMjData );
	bool TransMjDToI( BYTE srcMjData[], BYTE srcCount, BYTE *desMjData[], BYTE desCount );

	//图像用值--麻将值
	bool TransMjIToD( BYTE srcMjData, BYTE *desMjData );

	//添加麻将块进胡牌结构
	void AddBlock( HU *hu, PTR_MJ_BLOCK block );
	//清理胡牌结构数据
	void CleanHu( HU *hu );
	//拷贝胡牌结构
	void CopyHu( HU *destHu, HU *srcHu );

	//内部函数
private:
	//类型分析判断
	MJ_BLOCK CheckBlock( BYTE byMj1, BYTE byMj2, BYTE byMj3 );
};

#endif