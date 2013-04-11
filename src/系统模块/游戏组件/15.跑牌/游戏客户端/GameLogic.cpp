#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[52]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//类型判断
	if (IsTongHuaShun(cbCardData,cbCardCount)) return CT_TONG_HUA_SHUN;
	if (IsTieZhi(cbCardData,cbCardCount)) return CT_TIE_ZHI;
	if (IsHuLu(cbCardData,cbCardCount)) return CT_HU_LU;
	if (IsTongHua(cbCardData,cbCardCount)) return CT_TONG_HUA;
	if (IsShunZhi(cbCardData,cbCardCount)) return CT_SHUN_ZI;
	if (IsSanTiao(cbCardData,cbCardCount)) return CT_THREE_TIAO;
	if (IsDouble(cbCardData,cbCardCount)) return CT_DOUBLE;
	if (cbCardCount==1) return CT_SINGLE;

	return CT_ERROR;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[sizeof(m_bCardListData)];
	CopyMemory(cbCardData,m_bCardListData,sizeof(m_bCardListData));

	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(cbBufferCount-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[cbBufferCount-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(bRemoveCount<=cbCardCount);

	//定义变量
	BYTE bDeleteCount=0,bTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(bTempCardData)) return false;
	CopyMemory(bTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (bRemoveCard[i]==bTempCardData[j])
			{
				bDeleteCount++;
				bTempCardData[j]=0;
				break;
			}
		}
	}
	if (bDeleteCount!=bRemoveCount) return false;

	//清理扑克
	BYTE bCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (bTempCardData[i]!=0) cbCardData[bCardPos++]=bTempCardData[i];
	}

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//变量定义
	BYTE cbColor=GetCardColor(cbCardData);
	BYTE cbValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbColor<=0x30)&&(cbValue>=0x01)&&(cbValue<=0x0D)) return true;

	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//是否对子
bool CGameLogic::IsDouble(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=2) return false;
	
	//分析处理
	BYTE bNextValue=GetCardLogicValue(cbCardData[1]);
	BYTE bFirstValue=GetCardLogicValue(cbCardData[0]);

	return (bNextValue==bFirstValue);
}

//是否铁支
bool CGameLogic::IsTieZhi(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=5) return false;
	
	//变量定义
	BYTE bNoFixCount=0;
	BYTE bCenterValue=GetCardLogicValue(cbCardData[1]);

	//分析处理
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (GetCardLogicValue(cbCardData[i])!=bCenterValue)
		{
			bNoFixCount++;
			if (bNoFixCount>1) return false;
		}
	}

	return (bNoFixCount==1);
}

//是否葫芦
bool CGameLogic::IsHuLu(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=5) return false;

	//分析处理
	BYTE bMidValue=GetCardLogicValue(cbCardData[2]);
	BYTE bLastValue=GetCardLogicValue(cbCardData[4]);
	BYTE bFirstValue=GetCardLogicValue(cbCardData[0]);
	if (GetCardLogicValue(cbCardData[3])!=bLastValue) return false;
	if (GetCardLogicValue(cbCardData[1])!=bFirstValue) return false;
	if ((bMidValue!=bFirstValue)&&(bMidValue!=bLastValue)) return false;
	
	return true;
}

//是否同花
bool CGameLogic::IsTongHua(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=5) return false;
	
	//变量定义
	BYTE bFirstColor=GetCardColor(cbCardData[0]);

	//分析处理
	for (BYTE i=1;i<cbCardCount;i++)
	{
		if (GetCardColor(cbCardData[i])!=bFirstColor) return false;
	}

	return true;
}

//是否顺子
bool CGameLogic::IsShunZhi(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=5) return false;
	
	//变量定义
	BYTE bFirstValue=GetCardLogicValue(cbCardData[0]);

	//特殊处理
	if (bFirstValue==15)
	{
		for (BYTE i=1;i<cbCardCount;i++)
		{
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
			if ((bFirstValue!=(cbLogicValue+i))&&(bFirstValue!=(cbLogicValue+i+8))) return false;
		}

		return true;
	}

	//分析处理
	for (BYTE i=1;i<cbCardCount;i++)
	{
		if (bFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) return false;
	}

	return true;
}

//是否三条
bool CGameLogic::IsSanTiao(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=3) return false;
	
	//分析处理
	BYTE bFirstValue=GetCardLogicValue(cbCardData[0]);
	for (BYTE i=1;i<cbCardCount;i++)
	{
		if (GetCardLogicValue(cbCardData[i])!=bFirstValue) return false;
	}

	return true;
}

//是否同花顺
bool CGameLogic::IsTongHuaShun(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目判断
	if (cbCardCount!=5) return false;
	
	//变量定义
	BYTE bFirstColor=GetCardColor(cbCardData[0]);
	BYTE bFirstValue=GetCardLogicValue(cbCardData[0]);

	//特殊处理
	if (bFirstValue==15)
	{
		for (BYTE i=1;i<cbCardCount;i++)
		{
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
			if (GetCardColor(cbCardData[i])!=bFirstColor) return false;
			if ((bFirstValue!=(cbLogicValue+i))&&(bFirstValue!=(cbLogicValue+i+8))) return false;
		}

		return true;
	}

	//分析处理
	for (BYTE i=1;i<cbCardCount;i++)
	{
		if (GetCardColor(cbCardData[i])!=bFirstColor) return false;
		if (bFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) return false;
	}

	return true;
}

//对比扑克
bool CGameLogic::CompareCard(BYTE cbFirstCardData, BYTE cbNextCardData)
{
	//数值判断
	BYTE cbNextValue=GetCardLogicValue(cbNextCardData);
	BYTE cbFirstValue=GetCardLogicValue(cbFirstCardData);
	if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue);

	//花色判断
	BYTE cbNextColor=GetCardColor(cbNextCardData);
	BYTE cbFirstColor=GetCardColor(cbFirstCardData);

	return (cbFirstColor>cbNextColor);
}

//对比扑克
bool CGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//数目判断
	if (cbFirstCount!=cbNextCount) return false;

	//获取类型
	BYTE bNextType=GetCardType(bNextList,cbNextCount);
	BYTE bFirstType=GetCardType(bFirstList,cbFirstCount);

	//类型过滤
	ASSERT(bFirstType!=CT_ERROR);
	if ((bFirstType==CT_ERROR)||(bNextType==CT_ERROR)) return false;
	
	//简单类型
	switch(bFirstType)
	{
	case CT_SINGLE:			//单牌
	case CT_DOUBLE:			//对子
	case CT_THREE_TIAO:		//三条
		{
			if (bNextType!=bFirstType) return false;
			return CompareCard(bFirstList[0],bNextList[0]);
		}
	}

	//类型判断
	if (bFirstType!=bNextType) return (bFirstType>bNextType);

	//五条类型
	switch(bFirstType)
	{
	case CT_SHUN_ZI:		//顺子
	case CT_TONG_HUA:		//同花
	case CT_TONG_HUA_SHUN:	//同花顺
		{
			return CompareCard(bFirstList[0],bNextList[0]);
		}
	case CT_HU_LU:			//葫芦
		{
			return CompareCard(bFirstList[2],bNextList[2]);
		}
	case CT_TIE_ZHI:		//铁支
		{
			return CompareCard(bFirstList[1],bNextList[1]);
		}
	}
	
	return false;
}

//出牌判断
bool CGameLogic::SearchOutCard(BYTE cbCardData[], BYTE cbCardCount, BYTE bTurnCardData[], BYTE cbTurnCardCount, BYTE cbTurnOutType, BYTE cbOutCard[MAX_COUNT])
{
	//分析过滤
	if (cbCardCount<cbTurnCardCount) return false;

	return false;
}

//////////////////////////////////////////////////////////////////////////