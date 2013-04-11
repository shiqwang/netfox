#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define CT_THREE_TIAO				3									//三张类型
#define CT_FOUR_TIAO				4									//四张类型	
#define CT_FOUR_LINE				5									//四连类型
#define CT_ALL_BIG					6									//全大
#define CT_ALL_SMALL				7									//全小

//宏定义
#define MAX_COUNT					13									//最大数目

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//分析结构
struct tagAnalyseResult
{
	BYTE 							bFourCount;							//四张数目
	BYTE 							bThreeCount;						//三张数目
	BYTE 							bDoubleCount;						//两张数目
	BYTE							bSignedCount;						//单张数目
	BYTE 							bFourLogicVolue[3];					//四张列表
	BYTE 							bThreeLogicVolue[4];				//三张列表
	BYTE 							bDoubleLogicVolue[6];				//两张列表
	BYTE							m_bSCardData[13];					//手上扑克
	BYTE							m_bDCardData[13];					//手上扑克
	BYTE							m_bTCardData[13];					//手上扑克
	BYTE							m_bFCardData[13];					//手上扑克
};
////////////////////////////////////////////////////////////////////////
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

	bool MaxValue(BYTE FirstData[],BYTE NextData[],BYTE Count);

	//类型函数
public:
	//是否对子
	bool IsDouble(BYTE bCardData[], BYTE bCardCount);
	//是否三张
	bool IsSanTiao(BYTE bCardData[], BYTE bCardCount);
	//是否四张
	bool IsFourTiao(BYTE bCardData[], BYTE bCardCount);
	//是否四连
	bool IsFourLine(BYTE bCardData[], BYTE bCardCount);

	//是否4条2
	bool IsAllBig(BYTE bCardData[],BYTE bCardCount);
	//是否全小
	bool IsAllSmall(BYTE bCardData[],BYTE bCardCount);

	//功能函数
public:
	//对比扑克
	bool CompareCard(BYTE bFirstCardData, BYTE bNextCardData);
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount);
	//分析扑克
	void AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult);
	//出牌判断
	bool SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount, BYTE bTurnOutType, BYTE cbOutCard[MAX_COUNT]);
};

//////////////////////////////////////////////////////////////////////////

#endif