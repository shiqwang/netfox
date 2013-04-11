#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

/*
单张：一张任意的牌
对子：两张同样大小的牌
三条：三张同样大小的牌
四条：四张同样大小的牌

连牌：三张以上顺序相连的牌（3、2、A不能做连牌）
对子连牌：三对以上顺序相连的对子（3、2、A不能做对子连牌）

6、牌型的比较
点数的大小： 3>2>A>K>Q>J>10>9>8>7>6>5>4
只有相同的牌型才可以比较大小。
单张、对子、三条、四条按照点数比较大小。
连牌、对子连牌比较最大的牌的点数。

*/
//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_INVALID					0									//错误类型

#define CT_SINGLE					1									//单牌类型 ok
#define CT_DOUBLE					2									//对牌类型	ok
#define CT_THREE					3									//三条类型	ok

#define CT_ONE_LINE					4									//单连类型	OK abc
#define CT_DOUBLE_LINE				5									//对连类型	OK aabbcc    6
#define CT_THREE_LINE				6									//三连类型	OK aaabbbccc 9

//#define CT_THREE_LINE_TAKE_ONE		7									//三带一单
//#define CT_THREE_LINE_TAKE_DOUBLE	8									//三带一对
//#define CT_FOUR_LINE_TAKE_ONE		9									//四带两单
//#define CT_FOUR_LINE_TAKE_DOUBLE	10									//四带两对

#define CT_BOMB_CARD				11									//炸弹类型 4个一样的 ok
//#define CT_MISSILE_CARD				12									//火箭类型 双王



//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//分析结构
struct tagAnalyseResult
{
	BYTE 							bFourCount;							//四张数目
	BYTE 							bThreeCount;						//三张数目
	BYTE 							bDoubleCount;						//两张数目
	BYTE							bSignedCount;						//单张数目
	BYTE 							bFourLogicVolue[5];					//四张列表
	BYTE 							bThreeLogicVolue[6];				//三张列表
	BYTE 							bDoubleLogicVolue[10];				//两张列表
	BYTE							m_bSCardData[ZHUANG_CARD_NUM /*20*/];					//手上扑克
	BYTE							m_bDCardData[ZHUANG_CARD_NUM /*20*/];					//手上扑克
	BYTE							m_bTCardData[ZHUANG_CARD_NUM /*20*/];					//手上扑克
	BYTE							m_bFCardData[ZHUANG_CARD_NUM /*20*/];					//手上扑克
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_bCardListData[ALL_CARD_NUM];				//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//是否有对牌
	//条件：>= 2
	bool IsHaveDuiCard(const BYTE bCardData[], BYTE bCardCount);

	//是否为单联
	//条件：>= 3
	bool IsDanLianCardType(const BYTE bCardData[], BYTE bCardCount);

	//是否为双联
	//条件：>= 6
	bool IsShuangLianCardType(const BYTE bCardData[], BYTE bCardCount);

	//是否为三联
	//条件：>= 9
	bool IsThreeLianCardType(const BYTE bCardData[], BYTE bCardCount);


	//获取类型
	BYTE GetCardType(const BYTE bCardData[], BYTE bCardCount);
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
	//逻辑数值
	BYTE GetCardLogicValue(BYTE bCardData);
	//对比扑克
	bool CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount);

	//内部函数
public:
	//分析扑克
	void AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult);
};

//////////////////////////////////////////////////////////////////////////

#endif