#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[49]=
{
	0x01,0x02,0x03,/*0x04,*/0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,/*0x14,*/0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,/*0x24,*/0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,/*0x34,*/0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x41,/*,0x42,*/
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
BYTE CGameLogic::GetCardType(const BYTE bCardData[], BYTE bCardCount)
{
	//开始分析
	switch (bCardCount)
	{
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌和火箭
		{
			//if ((bCardData[0]==0x42)&&(bCardData[1]==0x41)) return CT_MISSILE_CARD;
			
			return (GetCardValue(bCardData[0])==GetCardValue(bCardData[1]))?CT_DOUBLE:CT_INVALID;
		}
	case 3:
		{
			if(IsOx(bCardData,bCardCount))return CT_OX;
			if(IsXi(bCardData,bCardCount))return CT_XI;
			if(GetCardLogicValue(bCardData[0])==0x0D&&GetCardLogicValue(bCardData[1])==0x0A&&GetCardLogicValue(bCardData[2])==0x08)
				return CT_BAI;
			if(GetCardLogicValue(bCardData[0])==0x03&&GetCardLogicValue(bCardData[1])==0x02&&GetCardLogicValue(bCardData[2])==0x01)
				return CT_FISH;
		}
	case 4:	//连牌和炸弹
		{
			BYTE bLogicValue=GetCardValue(bCardData[0]);
			for (BYTE i=1;i<bCardCount;i++)
			{
				if (bLogicValue!=GetCardValue(bCardData[i])) break;
			}
			if (i==bCardCount) return (bCardCount==3)?CT_THREE:CT_LIANGZI;
			if (bCardCount==3) return CT_INVALID;
			if(i==bCardCount)return CT_LIANGZI;
			break;
		}
	case 6:
	case 7:
		{
			if(IsXiBaoLiang(bCardData,bCardCount))
				return CT_XIBA0LAING;
		}

	case 16://龙凤齐，八大对
		{
			BYTE bTempCardData[16];
			ZeroMemory(bTempCardData,sizeof(bTempCardData));
			CopyMemory(bTempCardData,bCardData,sizeof(bTempCardData));
			SortCardList(bTempCardData,sizeof(bTempCardData));
			BYTE bTempDoubleCount=0;
	
			for(BYTE i=0;i+1<bCardCount;i++)
			{
				if(GetCardValue(bCardData[i])==GetCardValue(bCardData[i+1]))
					bTempDoubleCount++;
				else break;
			}
			
			if(bTempDoubleCount==8)return CT_EIGHTDOUBLE;
			BYTE bK=0;
			BYTE b10=0;
			BYTE b8=0;
			BYTE b3=0;
			BYTE b2=0;
			BYTE b1=0;
			bool bDragonaAndPhoenix=false;
			for(BYTE i=0;i<bCardCount;i++)
			{
				switch(GetCardValue(bCardData[i]))
				{
				case 0x0D:{bK++;break;}
				case 0x08:{b8++;break;}
				case 0x0A:{b10++;break;}
				case 0x03:{b3++;break;}
				case 0x02:{b2++;break;}
				case 0x01:{b1++;break;}

				}

			}
			if(bK==b10&&b8==b3&&b2==b1&&b1==2&&b10==b8&&b3==b2)
				bDragonaAndPhoenix=true;
			if(bDragonaAndPhoenix)
				return CT_DROGONANDPHONIX;


		}
	}

	////其他牌型
	//if (bCardCount>=4)
	//{
	//	//分析扑克
	//	tagAnalyseResult AnalyseResult;
	//	AnalysebCardData(bCardData,bCardCount,AnalyseResult);

	//	//四牌判断
	//	if (AnalyseResult.bFourCount>0)
	//	{
	//		if ((AnalyseResult.bFourCount==1)&&(bCardCount==5)) return CT_FOUR_LINE_TAKE_ONE;
	//		if ((AnalyseResult.bFourCount==1)&&(AnalyseResult.bDoubleCount==1)&&(bCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
	//		if ((AnalyseResult.bFourCount==1)&&(AnalyseResult.bDoubleCount==2)&&(bCardCount==8)) return CT_FOUR_LINE_TAKE_DOUBLE;
	//		if ((AnalyseResult.bFourCount==1)&&(bCardCount==6)) return CT_FOUR_LINE_TAKE_DOUBLE;

	//		return CT_INVALID;
	//	}

	//	//三牌判断
	//	if (AnalyseResult.bThreeCount>0)
	//	{
	//		//连牌判断
	//		if (AnalyseResult.bThreeCount>1)
	//		{
	//			if (AnalyseResult.bThreeLogicVolue[0]==15) return CT_INVALID;
	//			for (BYTE i=1;i<AnalyseResult.bThreeCount;i++)
	//			{
	//				if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) return CT_INVALID;
	//			}
	//		}

	//		//牌形判断
	//		if (AnalyseResult.bThreeCount*3==bCardCount) return CT_THREE_LINE;
	//		if (AnalyseResult.bThreeCount==(bCardCount-AnalyseResult.bThreeCount*3)) return CT_THREE_LINE_TAKE_ONE;
	//		if ((AnalyseResult.bDoubleCount==AnalyseResult.bThreeCount)&&(bCardCount==(AnalyseResult.bThreeCount*3+AnalyseResult.bDoubleCount*2))) return CT_THREE_LINE_TAKE_DOUBLE;

	//		return CT_INVALID;
	//	}

	//	//两张类型
	//	if (AnalyseResult.bDoubleCount>=3)
	//	{
	//		//二连判断
	//		if (AnalyseResult.bDoubleLogicVolue[0]!=15)
	//		{
	//			for (BYTE i=1;i<AnalyseResult.bDoubleCount;i++)
	//			{
	//				if (AnalyseResult.bDoubleLogicVolue[i]!=(AnalyseResult.bDoubleLogicVolue[0]-i)) return CT_INVALID;
	//			}
	//			if (AnalyseResult.bDoubleCount*2==bCardCount) return CT_DOUBLE_LINE;
	//		}

	//		return CT_INVALID;
	//	}
	//	
	//	//单张判断
	//	if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
	//	{
	//		BYTE bLogicValue=GetCardLogicValue(bCardData[0]);
	//		if (bLogicValue>=15) return CT_INVALID;
	//		for (BYTE i=1;i<AnalyseResult.bSignedCount;i++)
	//		{
	//			if (GetCardLogicValue(bCardData[i])!=(bLogicValue-i)) return CT_INVALID;
	//		}

	//		return CT_ONE_LINE;
	//	}
	//	
	//	return CT_INVALID;
	//}

	return CT_INVALID;
}

//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicVolue[16];
	for (BYTE i=0;i<bCardCount;i++)	bLogicVolue[i]=GetCardValue(bCardData[i]);

	//排序操作
	bool bSorted=true;
	BYTE bTempData,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicVolue[i]<bLogicVolue[i+1])||
			   ((bLogicVolue[i]==bLogicVolue[i+1])&&(bCardData[i]<bCardData[i+1])))
			{
				//交换位置
				bTempData=bCardData[i];
				bCardData[i]=bCardData[i+1];
				bCardData[i+1]=bTempData;
				bTempData=bLogicVolue[i];
				bLogicVolue[i]=bLogicVolue[i+1];
				bLogicVolue[i+1]=bTempData;
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
	BYTE bDeleteCount=0,bTempCardData[16];
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
//红黑搭配
bool CGameLogic::IsRedAndBlackDouble(const BYTE bCardData[],BYTE bCardCount)
{
	if(bCardCount>4||bCardCount<2)return false;
	if(GetCardValue(bCardData[0])!=GetCardValue(bCardData[1]))return false;
	int bTempCardColorFirst,bTempCardColorLast;
	 bTempCardColorFirst=GetCardColor(bCardData[0]);
	 bTempCardColorLast=GetCardColor(bCardData[1]);
	 if(abs(bTempCardColorFirst-bTempCardColorLast)==0x10||abs(bTempCardColorFirst-bTempCardColorLast)==0x30)
		 return true;

	 return false;
}
bool CGameLogic::IsRedAndBlackDouble(BYTE bFirstCardData,BYTE bNextCardData)
{
	if(!bFirstCardData||!bNextCardData)return false;
	int bTempCardColorFirst,bTempCardColorLast;
	 bTempCardColorFirst=GetCardColor(bFirstCardData);
	 bTempCardColorLast=GetCardColor(bNextCardData);
	 if((abs(bTempCardColorFirst-bTempCardColorLast)==0x10)||abs(bTempCardColorFirst-bTempCardColorLast)==0x30)
		 return true;


	 return false;
	 

}
//抢刺
bool CGameLogic::IsQiangCi(const BYTE bCardData[],BYTE bCardCount)
{
	if(bCardCount!=16)return false;
	if(GetCardType(bCardData,bCardCount)==CT_DROGONANDPHONIX||GetCardType(bCardData,bCardCount)==CT_EIGHTDOUBLE)
		return true;
	
	return false;

}
//牛牌
bool CGameLogic::IsOx(const BYTE bCardData[],BYTE bCardCount)
{
	if(bCardCount>3)return false;
	for(BYTE i=0;i+1<bCardCount;i++)
	{
		if(GetCardLogicValue(bCardData[i])!=GetCardLogicValue(bCardData[1])||GetCardLogicValue(bCardData[i])!=0x09)
			return false;
	}
		if(IsRedAndBlackDouble(bCardData[0],bCardData[1]))
			return true;
		if(bCardCount==3&&IsRedAndBlackDouble(bCardData[1],bCardData[2]))
			return true;

		return false;
}
//喜牌
bool CGameLogic::IsXi(const BYTE bCardData[],BYTE bCardCount)
{
		if(bCardCount>3)return false;
	for(BYTE i=0;i+1<bCardCount;i++)
	{
		if(GetCardLogicValue(bCardData[i])!=GetCardLogicValue(bCardData[1])||GetCardLogicValue(bCardData[i])!=0x05)
			return false;
	}
		if(IsRedAndBlackDouble(bCardData[0],bCardData[1]))
			return true;
		if(bCardCount==3&&IsRedAndBlackDouble(bCardData[1],bCardData[2]))
			return true;

		return false;
}
//有摆
bool CGameLogic::IshavingBai(const BYTE bCardData[],BYTE bCardCount)
{
	BYTE bK=0;
	BYTE b10=0;
	BYTE b8=0;
	for(BYTE i=0;i<bCardCount;i++)
	{
		if(GetCardValue(bCardData[i])==0x0D)bK++;
		if(GetCardValue(bCardData[i])==0x0A)b10++;
		if(GetCardValue(bCardData[i])==0x08)b8++;

	}
	if(bK&&b10&&b8)return true;


	return false;
}
//有亮
bool CGameLogic::IsHavingFish(const BYTE bCardData[],BYTE bCardCount)
{
	BYTE b1=0;
	BYTE b2=0;
	BYTE b3=0;
	for(BYTE i=0;i<bCardCount;i++)
	{
		if(GetCardValue(bCardData[i])==0x01)b1++;
		if(GetCardValue(bCardData[i])==0x02)b2++;
		if(GetCardValue(bCardData[i])==0x03)b3++;

	}
	if(b2&&b1&&b3)return true;


	return false;

}
//摆亮
bool CGameLogic::IsBaiLiang(const BYTE bCardData[],BYTE bCardCount)
{
	if(!IshavingBai(bCardData,bCardCount))return false;
	BYTE bTempK=0;
	for(BYTE i=0;i<bCardCount;i++)
		if(GetCardValue(bCardData[i])==0x0D)
			bTempK++;
	if(bTempK==4)
		return true;


	return false;

}
//鱼亮
bool CGameLogic::IsFishLiang(const BYTE bCardData[],BYTE bCardCount)
{
	if(!IsHavingFish(bCardData,bCardCount))return false;
	BYTE bTemp1=0;
	for(BYTE i=0;i<bCardCount;i++)
		if(GetCardValue(bCardData[i])==0x01)
			bTemp1++;
	if(bTemp1==4)
		return true;


	return false;

}
//喜包亮
bool CGameLogic::IsXiBaoLiang(const BYTE bCardData[],BYTE bCardCount)
{
	if(bCardCount>7||bCardCount<6)return false;
	BYTE bSameCount=1;
	for(BYTE i=0;i+1<bCardCount;i++)
	{
		if(GetCardValue(bCardData[i])==GetCardValue(bCardData[i+1])&&GetCardValue(bCardData[i])!=0x05)
			bSameCount++;
		
		if(bSameCount==4)break;

	}
	if(bSameCount<4)return false;
	ASSERT(bSameCount<=4);

	BYTE bTempXi[3];
	ZeroMemory(bTempXi,sizeof(bTempXi));
	BYTE bTempFCount=0;
	for(BYTE j=0;j<bCardCount;j++)
		if(GetCardValue(bCardData[j])==0x05)
			bTempXi[bTempFCount++]=bCardData[j];
	if(IsRedAndBlackDouble(bTempXi,bTempFCount))
		return true;
	ASSERT(!IsRedAndBlackDouble(bTempXi,bTempFCount));


	return false;




}
	
//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);
	ASSERT(bCardColor!=0x40);
	//转换数值
	if (bCardColor==0x40) return bCardValue+15;
	ASSERT(bCardColor!=0x40);

	if(bCardValue==0x0B)return bCardValue-1;;
	return bCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//获取类型
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);
	BYTE bTempFirstList[16];
	BYTE bTempNextList[16];
	ZeroMemory(bTempFirstList,sizeof(bTempFirstList));
	ZeroMemory(bTempNextList,sizeof(bTempNextList));
	CopyMemory(bTempFirstList,bFirstList,sizeof(bFirstList));
	CopyMemory(bTempNextList,bNextList,sizeof(bNextList));

	//类型判断
	if (bFirstType==CT_INVALID) return false;
	/*if (bFirstType==CT_MISSILE_CARD) return true;*/

	//炸弹判断
	if ((bFirstType==CT_LIANGZI)&&(bNextType!=CT_LIANGZI)) return false;
	if ((bFirstType!=CT_LIANGZI)&&(bNextType==CT_LIANGZI)) return false;
	if(bFirstType==CT_BAI||bFirstType==CT_FISH||bNextType==CT_BAI||bNextType==CT_FISH)
	{
		if (bFirstType==CT_BAI&&bNextType==CT_FISH)return true;
		else return false;
	}

	//规则判断
	if ((bFirstType!=bNextType)||(bFirstCount!=bNextCount)) return false;
 
	//开始对比
	switch (bNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
		{
			if(GetCardLogicValue(bFirstList[0])==0x03&&GetCardLogicValue(bNextList[0])==0x06)return false;
			if(GetCardLogicValue(bFirstList[0])==0x06&&GetCardLogicValue(bNextList[0])==0x03)return false;
			if(GetCardLogicValue(bFirstList[0])==0x05&&IsRedAndBlackDouble(bFirstList[0],bFirstList[1]))return false;
			if(GetCardLogicValue(bNextList[0])==0x05&&IsRedAndBlackDouble(bNextList,bNextCount))return false;
			if(GetCardLogicValue(bFirstList[0])==0x09&&IsRedAndBlackDouble(bFirstList[0],bFirstList[1]))
				if(GetCardLogicValue(bNextList[0])!=0x09&&GetCardLogicValue(bNextList[0])!=0x0D)return true;
				else return false;
			if(GetCardLogicValue(bNextList[0])==0x09&&IsRedAndBlackDouble(bNextList[0],bNextList[1]))return false;
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			return bFirstLogicValue>bNextLogicValue;
				
		}
	case CT_THREE:
		{
			if(GetCardLogicValue(bFirstList[0])==0x03&&GetCardLogicValue(bNextList[0])==0x06)return false;
			if(GetCardLogicValue(bFirstList[0])==0x06&&GetCardLogicValue(bNextList[0])==0x03)return false;
			if(GetCardLogicValue(bFirstList[0])==0x09&&IsRedAndBlackDouble(bFirstList,3))
				for(BYTE i=0;i<3;i++) bTempFirstList[i]+=0x04;
			if(GetCardLogicValue(bNextList[0])==0x09&&IsRedAndBlackDouble(bNextList,3))return false;
			if(GetCardLogicValue(bFirstList[0])==0x05||GetCardLogicValue(bNextList[0])==0x05)return false;

			

			if(GetCardLogicValue(bFirstList[0])==0x03)
				bTempFirstList[0]+=0x03;
			if(GetCardLogicValue(bNextList[0])==0x03)
				bTempNextList[0]+=0x03;

			BYTE bFirstLogicValue=GetCardLogicValue(bTempFirstList[0]);
			BYTE bNextLogicValue=GetCardLogicValue(bTempNextList[0]);
			return bFirstLogicValue>bNextLogicValue;
			
		}
	/*case CT_ONE_LINE:*/
	/*case CT_DOUBLE_LINE:
	case CT_THREE_LINE:*/
	case CT_LIANGZI:
		{
			if(GetCardLogicValue(bFirstList[0])==0x03)
				for(BYTE i=0;i<4;i++)bTempFirstList[0]+=0x03;
			if(GetCardLogicValue(bNextList[0])==0x03)
				for(BYTE i=0;i<4;i++)bTempNextList[0]+=0x03;
			BYTE bFirstLogicValue=GetCardLogicValue(bTempFirstList[0]);
			BYTE bNextLogicValue=GetCardLogicValue(bTempNextList[0]);
			return bFirstLogicValue>bNextLogicValue;
		}
	/*case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextList,bNextCount,NextResult);
			AnalysebCardData(bFirstList,bFirstCount,FirstResult);
			return FirstResult.bThreeLogicVolue[0]>NextResult.bThreeLogicVolue[0];
		}*/
	/*case CT_FOUR_LINE_TAKE_ONE:
	case CT_FOUR_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextList,bNextCount,NextResult);
			AnalysebCardData(bFirstList,bFirstCount,FirstResult);
			return FirstResult.bFourLogicVolue[0]>NextResult.bFourLogicVolue[0];
		}*/
	}
	
	return false;
}

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

//////////////////////////////////////////////////////////////////////////
