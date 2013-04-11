#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_INVALID					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
//#define CT_ONE_LINE					4									//单连类型
//#define CT_DOUBLE_LINE				5									//对连类型
//#define CT_THREE_LINE				6									//三连类型
//#define CT_THREE_LINE_TAKE_ONE		7									//三带一单
//#define CT_THREE_LINE_TAKE_DOUBLE	8									//三带一对
//#define CT_FOUR_LINE_TAKE_ONE		9									//四带两单
//#define CT_FOUR_LINE_TAKE_DOUBLE	10									//四带两对
#define CT_LIANGZI					11									//亮子类型
//#define CT_MISSILE_CARD				12									//火箭类型
#define CT_DROGONANDPHONIX			13									//龙凤齐
#define CT_EIGHTDOUBLE				14									//八大对
#define CT_FISH						15									//鱼
#define CT_BAI						16									//摆
#define CT_XIBA0LAING				17									//喜包亮
#define CT_XI						18
#define CT_OX						19

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
	BYTE							m_bSCardData[20];					//手上扑克
	BYTE							m_bDCardData[20];					//手上扑克
	BYTE							m_bTCardData[20];					//手上扑克
	BYTE							m_bFCardData[20];					//手上扑克
};
//用户出牌信息
struct tagUserOutCardInfo
{
	int								IsWiner;							//是否赢家
	BYTE							IsCiPai;								//刺牌
	BYTE							bOxCard;							//牛牌
	BYTE							bXiCard;							//喜牌
	BYTE							bXianPai;							//掀牌
	BYTE							bBeiXian;							//被掀
	BYTE							bLiangPai;							//亮牌

};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_bCardListData[49];				//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
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
	//红黑搭配
	bool IsRedAndBlackDouble(const BYTE bCardData[],BYTE bCardCount);
	bool IsRedAndBlackDouble(BYTE bFirstCardData,BYTE bNextCardData);
	//抢刺
	bool IsQiangCi(const BYTE bCardData[],BYTE bCardCount);	
	//牛牌
	bool IsOx(const BYTE bCardData[],BYTE bCardCount);
	//喜牌
	bool IsXi(const BYTE bCardData[],BYTE bCardCount);
	//有摆
	bool IshavingBai(const BYTE bCardData[],BYTE bCardCount);
	//有鱼
	bool IsHavingFish(const BYTE bCardData[],BYTE bCardCount);
	//摆亮
	bool IsBaiLiang(const BYTE bCardData[],BYTE bCardCount);
	//鱼亮
	bool IsFishLiang(const BYTE bCardData[],BYTE bCardCount);
	//喜包亮
	bool IsXiBaoLiang(const BYTE bCardData[],BYTE bCardCount);
	
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