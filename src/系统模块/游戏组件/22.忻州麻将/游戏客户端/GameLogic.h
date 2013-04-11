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
#define WIK_CHI_HU					0x40								//吃胡类型

//////////////////////////////////////////////////////////////////////////
//胡牌定义

//小胡牌型
#define CHK_NULL					0x0000								//非胡类型
#define CHK_JI_HU					0x0001								//鸡胡类型
#define CHK_PING_HU					0x0002								//平胡类型

//特殊胡牌
/*
#define CHK_SIXI_HU					0x0010								//四喜胡牌
#define CHK_BANBAN_HU				0x0020								//板板胡牌
#define CHK_LIULIU_HU				0x0040								//六六顺牌
#define CHK_QUEYISE_HU				0x0080								//缺一色牌
*/

//大胡牌型
#define CHK_PENG_PENG				0x0100								//碰碰胡牌
//#define CHK_JIANG_JIANG				0x0200								//将将胡牌
#define CHK_QI_XIAO_DUI				0x0400								//七小对牌
#define CHK_HAO_HUA_DUI				0x0400								//豪华对牌

//需将权位
#define CHR_DI						0x0001								//地胡权位
#define CHR_TIAN					0x0002								//天胡权位
#define CHR_HAI_DI					0x0004								//海底权位
#define CHR_QIANG_GANG				0x0008								//抢杆权位

//乱将权位
#define CHR_QING_YI_SE				0x0100								//清色权位
#define CHR_QUAN_QIU_REN			0x0200								//全求权位

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

#define ONE_TYPE_NUM	9
#define MJ_KIND_MAX	4 //TONG,TIAO,WAN,ZI
#define MJ_NUM_MAX 4 //1,2,3,4
//单牌列表
//双牌列表
//三牌列表
//4牌列表
//1----->9  0=无
typedef struct _MJ_LIST_ITEM
{
	BYTE useMjInt;//
	BYTE	mjList[ONE_TYPE_NUM];//
}
MJ_LIST_ITEM;

typedef struct _CARD_LIST
{
	int cardType;//1,2,3,4
	MJ_LIST_ITEM    mjCardListAll;//所有的排列
	MJ_LIST_ITEM	mjCardList1;//
	MJ_LIST_ITEM	mjCardList2;//
	MJ_LIST_ITEM	mjCardList3;//
	MJ_LIST_ITEM	mjCardList4;//
}
CARD_LIST;
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
	//吃胡判断
//	BYTE EstimateChiHu(BYTE cbCardIndex[MAX_INDEX]);
	//吃牌判断
//	BYTE EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	BYTE EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, tagGangCardResult & GangCardResult);
	//吃胡分析
//	BYTE AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult);

	//特殊胡牌
public:
	//四喜胡牌
//	bool IsSiXi(BYTE cbCardIndex[MAX_INDEX]);
	//缺一色牌
	bool IsQueYiSe(BYTE cbCardIndex[MAX_INDEX]);
	//板板胡牌
//	bool IsBanBanHu(BYTE cbCardIndex[MAX_INDEX]);
	//六六顺牌
//	bool IsLiuLiuShun(BYTE cbCardIndex[MAX_INDEX]);

	//特殊胡牌
public:
	//清一色牌
	bool IsQingYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);
	//七小对牌
	bool IsQiXiaoDui(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);
	//豪华对牌
	bool IsHaoHuaDui(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);
	//将将胡牌
	bool IsJiangJiangHu(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);

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
};

//////////////////////////////////////////////////////////////////////////

#endif