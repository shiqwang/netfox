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
BYTE CGameLogic::GetCardType(BYTE bCardData[], BYTE bCardCount)
{
	//类型判断

	if (bCardCount==1)						return CT_SINGLE;
	if(IsDouble(bCardData,bCardCount))		return CT_DOUBLE;
	if(IsSanTiao(bCardData,bCardCount))		return CT_THREE_TIAO;
	if(IsFourTiao(bCardData,bCardCount))	return CT_FOUR_TIAO;
	if(IsFourLine(bCardData,bCardCount))	return CT_FOUR_LINE;
	if(IsAllBig(bCardData,bCardCount))		return CT_ALL_BIG;
	if(IsAllSmall(bCardData,bCardCount))	return CT_ALL_SMALL;
	return CT_ERROR;
}

//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicValue[MAX_COUNT];
	for (BYTE i=0;i<bCardCount;i++)	bLogicValue[i]=GetCardLogicValue(bCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE bTempData,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicValue[i]<bLogicValue[i+1])||
				((bLogicValue[i]==bLogicValue[i+1])&&(bCardData[i]<bCardData[i+1])))
			{
				//交换位置
				bTempData=bCardData[i];
				bCardData[i]=bCardData[i+1];
				bCardData[i+1]=bTempData;
				bTempData=bLogicValue[i];
				bLogicValue[i]=bLogicValue[i+1];
				bLogicValue[i+1]=bTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	//混乱准备
	BYTE bCardData[sizeof(m_bCardListData)];
	CopyMemory(bCardData,m_bCardListData,sizeof(m_bCardListData));

	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(bBufferCount-bRandCount);
		bCardBuffer[bRandCount++]=bCardData[bPosition];
		bCardData[bPosition]=bCardData[bBufferCount-bRandCount];
	} while (bRandCount<bBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount)
{
	//检验数据
	ASSERT(bRemoveCount<=bCardCount);

	//定义变量
	BYTE bDeleteCount=0,bTempCardData[MAX_COUNT];
	if (bCardCount>CountArray(bTempCardData)) return false;
	CopyMemory(bTempCardData,bCardData,bCardCount*sizeof(bCardData[0]));

	//置零扑克
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<bCardCount;j++)
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
	for (BYTE i=0;i<bCardCount;i++)
	{
		if (bTempCardData[i]!=0) bCardData[bCardPos++]=bTempCardData[i];
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
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);

	//转换数值
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//是否对子
bool CGameLogic::IsDouble(BYTE bCardData[], BYTE bCardCount)
{
	//数目判断
	if (bCardCount!=2) return false;
	
	//分析处理
	BYTE bNextValue=GetCardLogicValue(bCardData[1]);
	BYTE bFirstValue=GetCardLogicValue(bCardData[0]);

	return (bNextValue==bFirstValue);
}
//是否三条
bool CGameLogic::IsSanTiao(BYTE bCardData[], BYTE bCardCount)
{
	//数目判断
	if (bCardCount!=3) return false;

	//分析处理
	BYTE bFirstValue=GetCardLogicValue(bCardData[0]);
	for (BYTE i=1;i<bCardCount;i++)
	{
		if (GetCardLogicValue(bCardData[i])!=bFirstValue) return false;
	}

	return true;
}
//是否四条
bool CGameLogic::IsFourTiao(BYTE bCardData[], BYTE bCardCount)
{
	//数目判断
	if (bCardCount!=4) return false;

	//分析处理
	BYTE bFirstValue=GetCardLogicValue(bCardData[0]);
	for (BYTE i=1;i<bCardCount;i++)
	{
		if (GetCardLogicValue(bCardData[i])!=bFirstValue) return false;
	}

	return true;
}
//是否四连
bool CGameLogic::IsFourLine(BYTE bCardData[], BYTE bCardCount)
{
	//数目判断
	if (bCardCount!=4) return false;

	//变量定义
	BYTE bFirstValue=GetCardLogicValue(bCardData[0]);
///////////////////////////////////////////////////////////////
	BYTE value[4];
	value[0]=GetCardValue(bCardData[0]);
	value[1]=GetCardValue(bCardData[1]);
	value[2]=GetCardValue(bCardData[2]);
	value[3]=GetCardValue(bCardData[3]);

	if(value[0]==2)
	{
		if(  value[1]==1 && value[2]==4  &&value[3]==3   ) return true;
		if(  value[1]==5 && value[2]==4  &&value[3]==3   ) return true;
		if(  value[1]==1 && value[2]==13 &&value[3]==12  ) return false;
	}
///////////////////////////////////////////////////////////////
	//特殊处理
	if (bFirstValue==15)
	{
		for (BYTE i=1;i<bCardCount;i++)
		{
			BYTE cbLogicValue=GetCardLogicValue(bCardData[i]);
			if ((bFirstValue!=(cbLogicValue+i))&&(bFirstValue!=(cbLogicValue+i+10))) return false;
		}
		return true;
	}
	//if (bFirstValue==14)
	//{
	//	if( (GetCardValue(bCardData[1])==2) && (GetCardValue(bCardData[2])==3) && (GetCardValue(bCardData[3])==4) )
	//		return true;
	//	else
	//		return false;
	//}
	//分析处理
	for (BYTE i=1;i<bCardCount;i++)
	{
		if (bFirstValue!=(GetCardLogicValue(bCardData[i])+i)) return false;
	}

	return true;
}

//是否4条2
bool CGameLogic::IsAllBig(BYTE bCardData[],BYTE bCardCount)
{
	for (BYTE i=0;i<bCardCount;i++) 
	{
		if(GetCardValue(bCardData[i])!=2)
			return false;
	}
	return true;
}
//是否全小
bool CGameLogic::IsAllSmall(BYTE bCardData[],BYTE bCardCount)
{
	for (BYTE i=0;i<bCardCount;i++) 
	{
		if(GetCardLogicValue(bCardData[i])>10)
			return false;
	}
    return true;
}

//对比扑克
bool CGameLogic::CompareCard(BYTE bFirstCardData, BYTE bNextCardData)
{
	//数值判断
	BYTE cbNextValue=GetCardLogicValue(bNextCardData);
	BYTE cbFirstValue=GetCardLogicValue(bFirstCardData);
	if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue);
	if (cbFirstValue==cbNextValue) return false;

	////花色判断
	//BYTE cbNextColor=GetCardColor(bNextCardData);
	//BYTE cbFirstColor=GetCardColor(bFirstCardData);

	//return (cbFirstColor>cbNextColor);
	return false;
}
bool CGameLogic::MaxValue(BYTE FirstData[],BYTE NextData[],BYTE Count)
{
	BYTE FirstBig,NextBig;
	for(BYTE i=0;i<Count;i++)
	{
		if(GetCardValue(FirstData[i])>GetCardValue(FirstData[i+1]))
			FirstBig=GetCardValue(FirstData[i]);
		if(GetCardValue(NextData[i])>GetCardValue(NextData[i+1]))
			NextBig=GetCardValue(NextData[i]);
	}
	return (FirstBig>NextBig);
}

//对比扑克
bool CGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//数目判断
	if (bFirstCount!=bNextCount) return false;

	//获取类型
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	//类型过滤
	ASSERT(bFirstType!=CT_ERROR);
	if ((bFirstType==CT_ERROR)||(bNextType==CT_ERROR)) return false;
	
	//简单类型
	switch(bFirstType)
	{
	case CT_SINGLE:			//单牌
	case CT_DOUBLE:			//对子
	case CT_THREE_TIAO:		//三条
	case CT_FOUR_TIAO:
		{
			if (bNextType!=bFirstType) return false;
			return CompareCard(bFirstList[0],bNextList[0]);
		}
	case CT_FOUR_LINE:
		{
			if (bNextType!=bFirstType) return false;
			if(GetCardValue(bNextList[0])==2) return MaxValue(bFirstList,bNextList,bFirstCount);
			return CompareCard(bFirstList[0],bNextList[0]);
		}
	}

	////类型判断
	//if (bFirstType!=bNextType) return (bFirstType>bNextType);

	////五条类型
	//switch(bFirstType)
	//{
	//case CT_SHUN_ZI:		//顺子
	//case CT_TONG_HUA:		//同花
	//case CT_TONG_HUA_SHUN:	//同花顺
	//	{
	//		return CompareCard(bFirstList[0],bNextList[0]);
	//	}
	//case CT_HU_LU:			//葫芦
	//	{
	//		return CompareCard(bFirstList[2],bNextList[2]);
	//	}
	//case CT_TIE_ZHI:		//铁支
	//	{
	//		return CompareCard(bFirstList[1],bNextList[1]);
	//	}
	//}
	
	return false;
}

//出牌判断
bool CGameLogic::SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount, BYTE bTurnOutType, BYTE cbOutCard[MAX_COUNT])
{
	//分析过滤
	if (bCardCount<bTurnCardCount) return false;

	return false;
}

//////////////////////////////////////////////////////////////////////////
//分析扑克
void CGameLogic::AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult)
{
	//变量定义
	BYTE bSameCount=1,bCardValueTemp=0;
	BYTE bLogicValue=GetCardLogicValue(bCardData[0]);

	//设置结果
	memset(&AnalyseResult,0,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=1;i<bCardCount;i++)
	{
		//获取扑克
		bCardValueTemp=GetCardLogicValue(bCardData[i]);
		if (bCardValueTemp==bLogicValue) bSameCount++;

		//保存结果
		if ((bCardValueTemp!=bLogicValue)||(i==(bCardCount-1)))
		{
			switch (bSameCount)
			{
			case 2:		//两张
				{
					CopyMemory(&AnalyseResult.m_bDCardData[AnalyseResult.bDoubleCount*2],&bCardData[i-2+((i==bCardCount-1&&bCardValueTemp==bLogicValue)?1:0)],2);
					AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount++]=bLogicValue;
					break;
				}
			case 3:		//三张
				{
					CopyMemory(&AnalyseResult.m_bTCardData[AnalyseResult.bThreeCount*3],&bCardData[i-3+((i==bCardCount-1&&bCardValueTemp==bLogicValue)?1:0)],3);
					AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount++]=bLogicValue;
					break;
				}
			case 4:		//四张
				{
					CopyMemory(&AnalyseResult.m_bFCardData[AnalyseResult.bFourCount*4],&bCardData[i-4+((i==bCardCount-1&&bCardValueTemp==bLogicValue)?1:0)],4);
					AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount++]=bLogicValue;
					break;
				}
			}
		}

		//设置变量
		if (bCardValueTemp!=bLogicValue)
		{
			if(bSameCount==1)
			{
				if(i!=bCardCount-1)
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i-1];
				else
				{
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i-1];
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i];
				}
			}
			else
			{
				if(i==bCardCount-1)
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i];
			}
			bSameCount=1;
			bLogicValue=bCardValueTemp;

		}
	}

	//单牌数目
	BYTE bOtherCount=AnalyseResult.bDoubleCount*2+AnalyseResult.bThreeCount*3+AnalyseResult.bFourCount*4;
	return;
}