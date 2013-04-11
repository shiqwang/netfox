#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//常量定义

#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					42									//最大索引
#define MAX_COUNT					17									//最大数目
#define MAX_REPERTORY				144									//最大库存

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER					0x02								//中吃类型
#define WIK_RIGHT					0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG					0x20								//杠牌类型
#define WIK_CHI_HU					0x40								//吃胡类型

//////////////////////////////////////////////////////////////////////////
//胡牌定义

//小胡牌型
#define CHK_NULL					0x0000								//非胡类型
#define CHK_JI_HU					0x0002								//鸡胡类型
#define CHK_PING_HU					0x0001								//平胡类型

//大胡牌型
#define CHK_PENG_PENG				0x0004								//碰碰胡牌
#define	CHR_GANG_HU					0x0008								//杠后胡权位

//特殊牌型
#define CHK_SKY_SWIM				0x1000								//天游胡牌
#define CHK_SINGLE_SWIM				0x0100								//单游胡牌
#define CHK_DOUBLE_SWIM				0x0200								//双游胡牌
#define CHK_THREE_SWIM				0x0400								//三游胡牌

//特殊权位
#define CHR_SINGLE_SWIM				0x2000								//游金权位
#define CHR_DOUBLE_SWIM				0x4000								//双游权位
#define CHR_THREE_SWIM				0x8000								//三游权位

//////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心麻将
	BYTE							cbCardIndex[3];						//麻将索引
};

//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心麻将
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//胡牌结果
struct tagChiHuResult
{
	WORD							wChiHuKind;							//吃胡类型
	WORD							wChiHuRight;						//胡牌权位
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//麻将数目
	BYTE							cbCardData[4];						//麻将数据
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼麻将
	BYTE							cbWeaveKind[4];						//组合类型
	BYTE							cbCenterCard[4];					//中心麻将
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CArrayTemplate<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//麻将数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//混乱麻将
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//删除麻将
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除麻将
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除麻将
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount);

	//辅助函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//麻将数目
	BYTE GetCardCount(BYTE cbCardIndex[MAX_INDEX]);
	//组合麻将
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]); 


	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);
	//胡牌等级
	BYTE GetChiHuActionRank(tagChiHuResult & ChiHuResult);

	//动作判断
public:
	//吃牌判断
	BYTE EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	BYTE EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, tagGangCardResult & GangCardResult);
	//吃胡分析
	BYTE AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult,BYTE cbGoldCard[],BYTE cbGoldCount,bool &bGoldCardEye);
	//吃胡分析
	BYTE AnalyseChiHuLevity(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeamItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult &ChiHuResult,BYTE cbGoldCard[],BYTE cbGoldCount,BYTE cbReCardIndex[MAX_INDEX]);
	//吃胡分析
	BYTE AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult);
	
public:	
	//金牌数目
	BYTE GetGoldCardCount(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[],BYTE cbItemCount,BYTE bGoldCard[],BYTE bGoldCount);
	

	//转换函数
public:
	//麻将转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//麻将转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//麻将转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//麻将转换
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);

	//内部函数
private:	
	//分析麻将
	bool AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	
public:
	

	//游金状态
	//bool IsGoldSwimSingleStatus(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE bGoldCard[],BYTE bGoldCount);
	// 有无花牌
	bool IsHaveFlowerCard(BYTE cbCardIndex[MAX_INDEX],BYTE bFlowerCard[],BYTE bFlowerCount);
	//是否是花牌
	bool IsFlowerCard(BYTE cbCardData,BYTE bFlowerCard[],BYTE bFlowerCount);
	//获取花牌
	BYTE GetFlowerCard(BYTE cbCardIndex[MAX_INDEX],BYTE bFlowerCard[],BYTE bFlowerCount);
	//金牌判断
	bool IsGoldCard(BYTE cbCardData,BYTE bGoldCard[],BYTE bGoldCount);
	//东西南北
	bool IsExistAllSidesCard(BYTE cbCardIndex[MAX_INDEX]);
	//梅兰竹菊
	bool IsExistAllPlantCard(BYTE cbCardIndex[MAX_INDEX]);
	//春夏秋冬
	bool IsExistAllSeasonCard(BYTE cbCardIndex[MAX_INDEX]);
};

//////////////////////////////////////////////////////////////////////////

#endif