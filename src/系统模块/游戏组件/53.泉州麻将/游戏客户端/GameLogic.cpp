#include "StdAfx.h"
#include "GameLogic.h"
#include ".\gamelogic.h"

//////////////////////////////////////////////////////////////////////////

//麻将数据
BYTE const CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
	0x31,0x31,0x31,0x31,												//东风
	0x32,0x32,0x32,0x32,												//西风
	0x33,0x33,0x33,0x33,												//南风
	0x34,0x34,0x34,0x34,												//北风
	0x35,0x35,0x35,0x35,												//中
	0x36,0x36,0x36,0x36,												//发
	0x37,0x37,0x37,0x37,												//白
	0x41,0x42,0x43,0x44,												//春夏秋冬
	0x45,0x46,0x47,0x48													//梅兰菊竹
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

//混乱麻将
void CGameLogic::RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));	

	//混乱麻将
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);

	return;
}

//删除麻将
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//效验麻将
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//删除麻将
	BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex]>0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);

	return false;
}

//删除麻将
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//删除麻将
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//效验麻将
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//删除麻将
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//错误断言
			ASSERT(FALSE);

			//还原删除
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//删除麻将
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//删除麻将
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//检验数据
	ASSERT(cbCardCount<=14);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[14];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零麻将
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}

	//成功判断
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//清理麻将
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=7)&&(cbColor==3))||((cbValue>=1)&&(cbValue<=8)&&(cbColor==4)));
}

//麻将数目
BYTE CGameLogic::GetCardCount(BYTE cbCardIndex[MAX_INDEX])
{
	//数目统计
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合麻将
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard+1;
			cbCardBuffer[1]=cbCenterCard+2;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}	
	case WIK_GANG:		//杠牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

//动作等级
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//胡牌等级
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (cbUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (cbUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//胡牌等级
BYTE CGameLogic::GetChiHuActionRank(tagChiHuResult & ChiHuResult)
{
	//变量定义
	BYTE cbChiHuOrder=0;
	WORD wChiHuRight=ChiHuResult.wChiHuRight;
	WORD wChiHuKind=(ChiHuResult.wChiHuKind&0xFF00)>>4;

	//大胡升级
	for (BYTE i=0;i<8;i++)
	{
		wChiHuKind>>=1;
		if ((wChiHuKind&0x0001)!=0) cbChiHuOrder++;
	}

	//权位升级
	for (BYTE i=0;i<16;i++)
	{
		wChiHuRight>>=1;
		if ((wChiHuRight&0x0001)!=0) cbChiHuOrder++;
	}

	return cbChiHuOrder;
}

//吃牌判断
BYTE CGameLogic::EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (cbCurrentCard>=0x31) return WIK_NULL;

	//变量定义
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//吃牌判断
	BYTE cbEatKind=0,cbFirstIndex=0;
	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
		{
			//吃牌判断
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];
			if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0)) continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0)) continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0)) continue;

			//设置类型
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}	 

//碰牌判断
BYTE CGameLogic::EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//杠牌判断	
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//杠牌分析
BYTE CGameLogic::AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//设置变量
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]==4)
		{
			
			cbActionMask|=WIK_GANG;			
			GangCardResult.cbCardData[GangCardResult.cbCardCount]=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//组合杠牌
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				
				cbActionMask|=WIK_GANG;				
				GangCardResult.cbCardData[GangCardResult.cbCardCount]=WIK_GANG;

				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult,BYTE cbGoldCard[],BYTE cbGoldCount,bool &bGoldCardEye)
{
	//变量定义
	WORD wChiHuKind=CHK_NULL;
	static CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));
	bGoldCardEye = false;

	//构造麻将
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//插入麻将
	if (cbCurrentCard!=0) cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;	

	//金牌数目
	BYTE bGetCount=0;
	bGetCount = GetGoldCardCount(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbGoldCard,cbGoldCount);


	//特殊牌型
	//天游牌型
	if(bGetCount==3)wChiHuKind|=CHK_SKY_SWIM;
	//单游牌型
	if(wChiHuRight==CHR_SINGLE_SWIM)wChiHuKind|=CHK_SINGLE_SWIM;
	//双游牌型
	else if(wChiHuRight==CHR_DOUBLE_SWIM)wChiHuKind|=CHK_DOUBLE_SWIM;
	//三游牌型
	else if(wChiHuRight==CHR_THREE_SWIM)wChiHuKind|=CHK_THREE_SWIM;

	
	/*if(bGetCount==1)
		wChiHuRight|=CHR_SINGLE_SWIM;//游金权位
	else if(bGetCount==2)
		wChiHuRight|=CHR_DOUBLE_SWIM;//双游权位
	else if(bGetCount>2)
		wChiHuRight|=CHR_THREE_SWIM;//三游权位*/

	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		//眼牌需求
		bool bNeedSymbol=((wChiHuRight&0xFF00)==0);

		//牌型分析
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			bool bLianCard=false,bPengCard=false;
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

			//牌眼类型
			BYTE cbEyeValue=pAnalyseItem->cbCardEye&MASK_VALUE;

			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];				
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;
				
			}


			//牌眼分析
			for(BYTE i=0;i<cbGoldCount;i++)
			{
				if(pAnalyseItem->cbCardEye==cbGoldCard[i])
				{
					bGoldCardEye=true;
					break;
				}
			}

			//牌型判断
			ASSERT((bLianCard==true)||(bPengCard==true));
			
			if (bLianCard==true) wChiHuKind|=CHK_PING_HU;
			else if ((bLianCard==false)&&(bPengCard==true)) wChiHuKind|=CHK_PING_HU;	
			
			//碰碰牌型
			//if ((bLianCard==false)&&(bPengCard==true)) wChiHuKind|=CHK_PENG_PENG;		

			
		
		}
		
	}	

	//结果判断
	if (wChiHuKind!=CHK_NULL)
	{
		//设置结果
		ChiHuResult.wChiHuKind=wChiHuKind;
		ChiHuResult.wChiHuRight=wChiHuRight;

		return WIK_CHI_HU;
	}


	return WIK_NULL;
}

//麻将转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<42);	
	if(cbCardIndex<34)
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	else
		return (0x41+cbCardIndex-34);

}

//麻将转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));	
	if(cbCardData<0x41)
		return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
	else 
		return (34+(cbCardData&MASK_VALUE)-1);

}

//麻将转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//转换麻将
	BYTE cbPosition=0;
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//麻将转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换麻将
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult)
{
	//变量定义
	WORD wChiHuKind=CHK_NULL;
	static CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));


	//构造麻将
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//插入麻将
	if (cbCurrentCard!=0) cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;		

	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		//眼牌需求
		bool bNeedSymbol=((wChiHuRight&0xFF00)==0);

		//牌型分析
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			bool bLianCard=false,bPengCard=false;
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

			//牌眼类型
			BYTE cbEyeValue=pAnalyseItem->cbCardEye&MASK_VALUE;

			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];				
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;
				
			}
			

			//牌型判断
			ASSERT((bLianCard==true)||(bPengCard==true));
			
			if (bLianCard==true) wChiHuKind|=CHK_PING_HU;
			else if ((bLianCard==false)&&(bPengCard==true)) wChiHuKind|=CHK_PING_HU;				
		
		}
		
	}	

	//结果判断
	if (wChiHuKind!=CHK_NULL)
	{
		//设置结果
		ChiHuResult.wChiHuKind=wChiHuKind;
		ChiHuResult.wChiHuRight=wChiHuRight;

		return WIK_CHI_HU;
	}


	return WIK_NULL;
}

//分析麻将
bool CGameLogic::AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	BYTE i, j;

	//计算数目
	BYTE cbCardCount=0;
	for ( i=0;i<MAX_INDEX;i++) cbCardCount+=cbCardIndex[i];

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0)) return false;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_COUNT-2];
	ZeroMemory(KindItem,sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==5);

	//单吊判断
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==5));

		//牌眼判断
		for ( i=0;i<MAX_INDEX;i++)
		{
			if (cbCardIndex[i]==2)
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
				}
				AnalyseItem.cbCardEye=SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//拆分分析
	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX;i++)
		{

			if(cbCardIndex[i]==0)continue;		
			

			//同牌判断
			if (cbCardIndex[i]>=3)
			{
				KindItem[cbKindItemCount].cbCenterCard=i;
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				if(cbCardIndex[i]==3)
					KindItem[cbKindItemCount++].cbWeaveKind=WIK_PENG;
				else
					//KindItem[cbKindItemCount++].cbWeaveKind=WIK_FILL;
					KindItem[cbKindItemCount++].cbWeaveKind=WIK_GANG;
			}
			
			
			
			
			//连牌判断:筒索万才有连牌			
			if ((i<(MAX_INDEX-17))&&(cbCardIndex[i]>0)&&((i%9)<7))
			{
				for (BYTE j=1;j<=cbCardIndex[i];j++)
				{
					if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
					{
						KindItem[cbKindItemCount].cbCenterCard=i;
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
						KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
						KindItem[cbKindItemCount++].cbWeaveKind=WIK_LEFT;
					}
				}
			}			
			
		}
	}

	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[5]={0,1,2,3,4};
		tagKindItem * pKindItem[5];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			for (BYTE i=0;i<cbLessKindItem;i++) pKindItem[i]=&KindItem[cbIndex[i]];

			//数量判断
			bool bEnoughCard=true;
			for (BYTE i=0;i<cbLessKindItem*3;i++)
			{
				//存在判断
				BYTE cbCardIndex=pKindItem[i/3]->cbCardIndex[i%3];
				if (cbCardIndexTemp[cbCardIndex]==0)
				{
					bEnoughCard=false;
					break;
				}
				else cbCardIndexTemp[cbCardIndex]--;
			}

			//胡牌判断
			if (bEnoughCard==true)
			{
				//牌眼判断
				BYTE cbCardEye=0;
				for (BYTE i=0;i<MAX_INDEX;i++)
				{
					if (cbCardIndexTemp[i]==2)
					{
						cbCardEye=SwitchToCardData(i);
						break;
					}
				}

				//组合类型
				if (cbCardEye!=0)
				{
					//变量定义
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

					//设置组合
					for (BYTE i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
					}

					//设置牌型
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
					}

					//设置牌眼
					AnalyseItem.cbCardEye=cbCardEye;

					//插入结果
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				for (BYTE i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0) break;
			}
			else cbIndex[cbLessKindItem-1]++;
			
		} while (true);

	}

	return (AnalyseItemArray.GetCount()>0);
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuLevity(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult &ChiHuResult,BYTE cbGoldCard[],BYTE cbGoldCount,BYTE cbReCardIndex[MAX_INDEX])
{
	//变量定义
	WORD wChiHuKind=CHK_NULL;
	static CAnalyseItemArray AnalyseItemArray;
	ZeroMemory(cbReCardIndex,sizeof(BYTE)*MAX_INDEX);

	//设置变量
	AnalyseItemArray.RemoveAll();
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));


	//构造麻将
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
	CopyMemory(cbReCardIndex,cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//插入麻将
	if (cbCurrentCard!=0)
	{
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
		cbReCardIndex[SwitchToCardIndex(cbCurrentCard)]++;
	}

	//金牌数目
	BYTE bGoldCount=0;
	bGoldCount = GetGoldCardCount(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbGoldCard,cbGoldCount);

	//特殊牌型
	//天游牌型
	if(bGoldCount==3)wChiHuKind|=CHK_SKY_SWIM;
	//单游牌型
	if(wChiHuRight==CHR_SINGLE_SWIM)wChiHuKind|=CHK_SINGLE_SWIM;
	//双游牌型
	else if(wChiHuRight==CHR_DOUBLE_SWIM)wChiHuKind|=CHK_DOUBLE_SWIM;
	//三游牌型
	else if(wChiHuRight==CHR_THREE_SWIM)wChiHuKind|=CHK_THREE_SWIM;	

	//统计除吃碰牌以外的金牌
	bGoldCount=GetGoldCardCount(cbCardIndexTemp,NULL,0,cbGoldCard,cbGoldCount);

	
	AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,0,0,ChiHuResult);

	wChiHuKind|=ChiHuResult.wChiHuKind;

	//一张金牌
	if((wChiHuKind==CHK_NULL)&&(bGoldCount==1))
	{
		//构造扑克
		BYTE cbTempCard[MAX_INDEX];
		CopyMemory(cbTempCard,cbCardIndexTemp,sizeof(cbTempCard));

		//剔出金牌
		for(BYTE i=0;i<cbGoldCount;i++)
		{
			BYTE cbIndex=SwitchToCardIndex(cbGoldCard[i]);
			if(cbTempCard[cbIndex]>0)
			{
				cbTempCard[cbIndex]=0;				
			}
		}

		//判断胡牌
		BYTE  cbCardDataTemp[34]=
		{
			0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
			0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
			0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
			0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//字
		};

		tagWeaveItem *pWeaveItem=WeaveItem;

		//中间变量
		BYTE bHuKind=WIK_NULL;
		tagChiHuResult  ChiHuResult;

		//能否胡牌
		for (BYTE i=0;i<34;i++)
		{
			ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));
			bHuKind=WIK_NULL;
			bHuKind=AnalyseChiHuCard(cbTempCard,pWeaveItem,cbWeaveCount,cbCardDataTemp[i],0, ChiHuResult);
			if (bHuKind!=WIK_NULL)
			{
				//加载扑克
				cbTempCard[SwitchToCardIndex(cbCardDataTemp[i])]++;

				//取得扑克
				CopyMemory(cbReCardIndex,cbTempCard,sizeof(cbTempCard));

				break;
			}
				
		}

		wChiHuKind|=ChiHuResult.wChiHuKind;	

	}
	else if((wChiHuKind==CHK_NULL)&&(bGoldCount==2))//两张金牌
	{
		//构造扑克
		BYTE cbTempCard[MAX_INDEX];
		CopyMemory(cbTempCard,cbCardIndexTemp,sizeof(cbTempCard));

		//剔出金牌
		for(BYTE i=0;i<cbGoldCount;i++)
		{
			BYTE cbIndex=SwitchToCardIndex(cbGoldCard[i]);
			if(cbTempCard[cbIndex]>0)
			{
				cbTempCard[cbIndex]=0;				
			}
		}

		//判断胡牌
		BYTE  cbCardDataTemp[34]=
		{
			0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
			0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
			0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
			0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//字
		};

		tagWeaveItem *pWeaveItem=WeaveItem;

		//中间变量
		BYTE bHuKind=WIK_NULL;
		tagChiHuResult  ChiHuResult;

		
	

		//抽取扑克
		for (BYTE i=0;i<34;i++)
		{
			//加载扑克
			cbTempCard[SwitchToCardIndex(cbCardDataTemp[i])]++;
			ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));

			pWeaveItem=WeaveItem;

			//能否胡牌
			bHuKind=WIK_NULL;
			for(BYTE j=0;j<34;j++)
			{	
				ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));
				bHuKind=WIK_NULL;
				bHuKind=AnalyseChiHuCard(cbTempCard,pWeaveItem,cbWeaveCount,cbCardDataTemp[j],0, ChiHuResult);
				if (bHuKind!=WIK_NULL) break;
					
			}

			BYTE j;

			if(bHuKind!=WIK_NULL)
			{
				//加载扑克
				cbTempCard[SwitchToCardIndex(cbCardDataTemp[i])]++;
				cbTempCard[SwitchToCardIndex(cbCardDataTemp[j])]++;

				//取得扑克
				CopyMemory(cbReCardIndex,cbTempCard,sizeof(cbTempCard));

				break;
			}

			//剔出扑克
			cbTempCard[SwitchToCardIndex(cbCardDataTemp[i])]--;
		}

		wChiHuKind|=ChiHuResult.wChiHuKind;

	}
	

	//结果判断
	if (wChiHuKind!=CHK_NULL)
	{
		//设置结果
		ChiHuResult.wChiHuKind=wChiHuKind;
		ChiHuResult.wChiHuRight=wChiHuRight;

		return WIK_CHI_HU;
	}


	return WIK_NULL;

}
/*
// 游金状态
bool CGameLogic::IsGoldSwimSingleStatus(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE bGoldCard[],BYTE bGoldCount)
{
	//构造扑克
	BYTE cbTempCard[MAX_INDEX];
	CopyMemory(cbTempCard,cbCardIndex,sizeof(cbTempCard));

	BYTE  cbCardDataTemp[34]=
	{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//字
	};

	tagWeaveItem *pWeaveItem=WeaveItem;

	BYTE cbTmpGoldCard[4]={0,0,0,0};

	//抽取扑克
	for (BYTE i=0;i<34;i++)
	{	
		tagChiHuResult  ChiHuResult;
		BYTE bHuKind=WIK_NULL;
		bHuKind=AnalyseChiHuCard(cbTempCard,pWeaveItem,cbWeaveCount,cbCardDataTemp[i],0, ChiHuResult,cbTmpGoldCard,4);
		if (bHuKind!=WIK_NULL) return true;
			
	}

	return false;
}*/

// 有无花牌
bool CGameLogic::IsHaveFlowerCard(BYTE cbCardIndex[MAX_INDEX],BYTE bFlowerCard[],BYTE bFlowerCount)
{
	//麻将判断
	for(BYTE i=0;i<bFlowerCount;i++)
	{
		if(bFlowerCard[i]==0)continue;

		ASSERT(IsValidCard(bFlowerCard[i]));

		if(cbCardIndex[SwitchToCardIndex(bFlowerCard[i])]>0)return true;
	}
	return false;
}

//是否是花牌
bool CGameLogic::IsFlowerCard(BYTE cbCardData,BYTE bFlowerCard[],BYTE bFlowerCount)
{
	for(BYTE i=0;i<bFlowerCount;i++)
	{
		if(bFlowerCard[i]==0)continue;
		ASSERT(IsValidCard(bFlowerCard[i]));
		if(cbCardData==bFlowerCard[i])return true;
	}

	return false;
}

//获取花牌
BYTE CGameLogic::GetFlowerCard(BYTE cbCardIndex[MAX_INDEX],BYTE bFlowerCard[],BYTE bFlowerCount)
{
	BYTE cbCardData=0;
	for(BYTE i=0;i<bFlowerCount;i++)
	{
		if(bFlowerCard[i]==0)continue;

		ASSERT(IsValidCard(bFlowerCard[i]));
		if(cbCardIndex[SwitchToCardIndex(bFlowerCard[i])]>0)
		{
			cbCardData = bFlowerCard[i];
			break;
		}
	}

	return cbCardData;

}

//金牌判断
bool CGameLogic::IsGoldCard(BYTE cbCardData,BYTE bGoldCard[],BYTE bGoldCount)
{
	for(BYTE i=0;i<bGoldCount;i++)
	{
		if(bGoldCard[i]==0)continue;

		if(cbCardData==bGoldCard[i])return true;

	}
	return false;
}

//金牌数目
BYTE CGameLogic::GetGoldCardCount(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[],BYTE cbItemCount,BYTE bGoldCard[],BYTE bGoldCount)
{
	BYTE bReturnGoldCount=0;

	for(BYTE i=0;i<cbItemCount;i++)
	{
		BYTE cbCardData=WeaveItem[i].cbCenterCard;

		

		if(WeaveItem[i].cbWeaveKind==WIK_PENG)
		{

			if(IsGoldCard(cbCardData,bGoldCard,bGoldCount))	//金牌已经碰牌	
			{
				bReturnGoldCount=3;
				return bReturnGoldCount;
			}
		}
		else if(WeaveItem[i].cbWeaveKind!=WIK_GANG)
		{
			BYTE bCardData[3]={0,0,0};//吃牌数据
			bool bEateType=false;	  //吃牌判断
			switch(WeaveItem[i].cbWeaveKind)
			{
			case WIK_LEFT://左吃
				{
					bCardData[0]=cbCardData;
					bCardData[1]=cbCardData+0x01;
					bCardData[2]=cbCardData+0x02;
					bEateType=true;				
					break;
				}
			case WIK_CENTER://中吃
				{
					bCardData[0]=cbCardData-0x01;
					bCardData[1]=cbCardData;
					bCardData[2]=cbCardData+0x01;
					bEateType=true;
				break;
				}
			case WIK_RIGHT://右吃
				{
					bCardData[0]=cbCardData-0x02;
					bCardData[1]=cbCardData-0x01;
					bCardData[2]=cbCardData;
					bEateType=true;
					break;
				}
			}

			if(bEateType)
			{
				for(BYTE j=0;j<3;j++)
				{
					//吃牌中有金牌
					if(IsGoldCard(bCardData[j],bGoldCard,bGoldCount))bReturnGoldCount++;					
				}
			}

		}

		if(bReturnGoldCount>2)return bReturnGoldCount;
	}

	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbCardIndex[i]>0)
		{
			BYTE cbCard=SwitchToCardData(i);

			//手中牌有金牌
			if(IsGoldCard(cbCard,bGoldCard,bGoldCount))bReturnGoldCount+=cbCardIndex[i];

			if(bReturnGoldCount>2)return bReturnGoldCount;
		}
	}

	return bReturnGoldCount;
}

//东西南北
bool CGameLogic::IsExistAllSidesCard(BYTE cbCardIndex[MAX_INDEX])
{
	if(cbCardIndex[SwitchToCardIndex(0x31)]&&cbCardIndex[SwitchToCardIndex(0x32)]&&
		cbCardIndex[SwitchToCardIndex(0x33)]&&cbCardIndex[SwitchToCardIndex(0x34)])return true;

	return false;

}
	//梅兰竹菊
bool CGameLogic::IsExistAllPlantCard(BYTE cbCardIndex[MAX_INDEX])
{
	if(cbCardIndex[SwitchToCardIndex(0x41)]&&cbCardIndex[SwitchToCardIndex(0x42)]&&
		cbCardIndex[SwitchToCardIndex(0x43)]&&cbCardIndex[SwitchToCardIndex(0x44)])return true;

	return false;

}
//春夏秋冬
bool CGameLogic::IsExistAllSeasonCard(BYTE cbCardIndex[MAX_INDEX])
{
	if(cbCardIndex[SwitchToCardIndex(0x45)]&&cbCardIndex[SwitchToCardIndex(0x46)]&&
		cbCardIndex[SwitchToCardIndex(0x47)]&&cbCardIndex[SwitchToCardIndex(0x48)])return true;

	return false;

}

//////////////////////////////////////////////////////////////////////////