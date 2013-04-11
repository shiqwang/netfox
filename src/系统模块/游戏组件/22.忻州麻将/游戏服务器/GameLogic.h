#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//常量定义

#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					34									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				136  //108									//最大库存

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
#define WIK_FILL					0x10								//补牌类型
#define WIK_GANG					0x20								//杠牌类型

#define WIK_TING_CARD					0x30								//听牌类型

#define WIK_CHI_HU					0x40								//吃胡类型




//////////////////////////////////////////////////////////////////////////
//胡牌定义

//非胡类型
#define CHK_NULL					0x0000								//非胡类型

//小胡牌型
#define CHK_JI_HU					0x0001								//鸡胡类型
#define CHK_PING_HU					0x0002								//平胡类型

//大胡牌型
#define CHR_DI						0x0100								//地胡权位
#define CHR_TIAN					0x0200								//天胡权位
#define CHK_PENG_PENG				0x0300								//碰碰胡牌
#define CHK_QING_YI_SE				0x0800								//清色权位
#define CHR_QIANG_GANG				0x2000								//抢杆权位
#define CHK_QUAN_QIU_REN			0x1000								//全求权位

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
public:
	//计算MJ的分数  番数  赢家
	int GetWinMJNum(BYTE cbCardIndex[MAX_INDEX], BYTE cbItemCount);
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
//输入：BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult
//返回：
	BYTE AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult);

	//特殊胡牌
public:
	//是否为砍和
	//砍  和：例如：12和3，89和7，46和5，2和2。（七小对不算）
	bool IsKanMJ(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);

	//清一色牌 是否为清一色
	bool IsQingYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);

	//是否为一条龙
	//牌里出现万，条，饼任意一门的123456789即可
	bool IsOneLongMJ(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);

	//七小对牌
	bool IsQiXiaoDui(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);

	//缺一色牌
	//缺一门：和牌时牌里没有万，饼，条任意一门，缺少2门的不算却和
	bool IsQueYiSe(BYTE cbCardIndex[MAX_INDEX]);

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
	//输入：BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount
	//返回：CAnalyseItemArray & AnalyseItemArray
	bool AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
};

//////////////////////////////////////////////////////////////////////////

#endif