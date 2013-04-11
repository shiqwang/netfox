#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define CT_THREE_TIAO				3									//三条类型
#define	CT_SHUN_ZI					4									//顺子类型
#define CT_TONG_HUA					5									//同花类型
#define CT_HU_LU					6									//葫芦类型
#define CT_TIE_ZHI					7									//铁支类型
#define CT_TONG_HUA_SHUN			8									//同花顺型

//宏定义
#define MAX_COUNT					13									//最大数目

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_bCardListData[52];				//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE bCardData[], BYTE bCardCount);
	//获取数值
	BYTE GetCardValue(BYTE bCardData) { return bCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE bCardData) { return bCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE bCardData[], BYTE bCardCount);
	//混乱扑克
	void RandCardList(BYTE bCardBuffer[], BYTE bBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount);

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);

	//类型函数
public:
	//是否对子
	bool IsDouble(BYTE bCardData[], BYTE bCardCount);
	//是否铁支
	bool IsTieZhi(BYTE bCardData[], BYTE bCardCount);
	//是否葫芦
	bool IsHuLu(BYTE bCardData[], BYTE bCardCount);
	//是否同花
	bool IsTongHua(BYTE bCardData[], BYTE bCardCount);
	//是否顺子
	bool IsShunZhi(BYTE bCardData[], BYTE bCardCount);
	//是否三条
	bool IsSanTiao(BYTE bCardData[], BYTE bCardCount);
	//是否同花顺
	bool IsTongHuaShun(BYTE bCardData[], BYTE bCardCount);

	//功能函数
public:
	//对比扑克
	bool CompareCard(BYTE bFirstCardData, BYTE bNextCardData);
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount);
	//出牌判断
	bool SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount, BYTE bTurnOutType, BYTE cbOutCard[MAX_COUNT]);
};

//////////////////////////////////////////////////////////////////////////

#endif