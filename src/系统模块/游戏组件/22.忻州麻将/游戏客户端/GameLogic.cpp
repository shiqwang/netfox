#include "StdAfx.h"
#include "GameLogic.h"

#include "GameClient.h"
/*
忻州麻将：
 1、牌局： 不带花 136 张。4人游戏。

2、打法：
（1）不可吃、可碰、可杠。

3、胡牌： 
基本牌型：
平胡：停牌后打出的牌被别人胡牌称平胡。
自摸：自己揭牌胡牌是自摸,
包胡：在没有停牌前放胡就需要包胡，另外一种就是停牌时打出的一张牌别人胡牌后也算包胡，包胡就是一人承担其他玩家的所有输的分数。
七对 ：7对可以胡牌。
杠胡：明杠上花一律算平胡，暗杠上花算自摸。

4、听牌： 
（1）听牌后才能胡牌，听牌后不能换牌。
（2）停牌后放胡必须胡牌。(!!)

5.黄牌：留10墩底牌，揭完牌没有胡牌就黄牌，黄牌后庄家不变。连黄三庄庄家赔闲家一平胡的分。庄家不变。

*/

//应用程序对象
extern CGameClientApp theApp;
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
//东 南 西 北红中 发财 白板
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,						//字
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,						//字
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,						//字
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,						//字
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
	BYTE cbValue=(cbCardData&MASK_VALUE);//点数
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;//花色
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=7)&&(cbColor==3)));
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
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sGetWeaveCard.log",strTime);

			strTemp.Format("GetWeaveCard  BYTE cbWeaveKind=%d, BYTE cbCenterCard=%d, BYTE cbCardBuffer[4]",  cbWeaveKind,  cbCenterCard);
			theApp.WriteLog(strFile, strTemp);
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
	case WIK_FILL:		//补牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
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
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sGetUserActionRank.log",strTime);

			strTemp.Format("GetUserActionRank");
			theApp.WriteLog(strFile, strTemp);
	//胡牌等级
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (cbUserAction&(WIK_FILL|WIK_GANG)) { return 3; }

	//碰牌等级
	if (cbUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//胡牌等级
BYTE CGameLogic::GetChiHuActionRank(tagChiHuResult & ChiHuResult)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sGetChiHuActionRank.log",strTime);

			strTemp.Format("GetChiHuActionRank");
			theApp.WriteLog(strFile, strTemp);
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
/*
//吃胡判断
BYTE CGameLogic::EstimateChiHu(BYTE cbCardIndex[MAX_INDEX])
{
	//变量定义
	BYTE cbActionMask=WIK_NULL;

	//特殊胡牌
	if (IsSiXi(cbCardIndex)==true) cbActionMask|=WIK_CHI_HU;
	else if (IsQueYiSe(cbCardIndex)==true) cbActionMask|=WIK_CHI_HU;
	else if (IsBanBanHu(cbCardIndex)==true) cbActionMask|=WIK_CHI_HU;
//	else if (IsLiuLiuShun(cbCardIndex)==true) cbActionMask|=WIK_CHI_HU;

	return cbActionMask;
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
*/
//碰牌判断
BYTE CGameLogic::EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sEstimatePengCard.log",strTime);

			strTemp.Format("EstimatePengCard");
			theApp.WriteLog(strFile, strTemp);
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sEstimateGangCard.log",strTime);

			strTemp.Format("EstimateGangCard");
			theApp.WriteLog(strFile, strTemp);
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?(WIK_GANG|WIK_FILL):WIK_NULL;
}

//杠牌分析
BYTE CGameLogic::AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAnalyseGangCard.log",strTime);

			strTemp.Format("AnalyseGangCard");
			theApp.WriteLog(strFile, strTemp);
	//设置变量
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=(WIK_GANG|WIK_FILL);
			GangCardResult.cbCardData[GangCardResult.cbCardCount]=(WIK_GANG|WIK_FILL);
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
				cbActionMask|=(WIK_GANG|WIK_FILL);
				GangCardResult.cbCardData[GangCardResult.cbCardCount]=(WIK_GANG|WIK_FILL);
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

//吃胡分析
/*
BYTE CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAnalyseChiHuCard.log",strTime);

			strTemp.Format("AnalyseChiHuCard  cbWeaveCount=%d,  cbCurrentCard=%d",   cbWeaveCount,  cbCurrentCard);
			theApp.WriteLog(strFile, strTemp);

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

	//权位处理
	if ((cbCurrentCard!=0)&&(cbWeaveCount==4)) wChiHuRight|=CHR_QUAN_QIU_REN;
	if (IsQingYiSe(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true) wChiHuRight|=CHR_QING_YI_SE;

			strTemp.Format("wChiHuRight=%d",   wChiHuRight);
			theApp.WriteLog(strFile, strTemp);

	//牌型判断
	if (IsQiXiaoDui(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true) wChiHuKind|=CHK_QI_XIAO_DUI;
	if (IsHaoHuaDui(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true) wChiHuKind|=CHK_HAO_HUA_DUI;
//	if (IsJiangJiangHu(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true) wChiHuKind|=CHK_JIANG_JIANG;

			strTemp.Format("wChiHuKind=%d",   wChiHuKind);
			theApp.WriteLog(strFile, strTemp);

	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);


			strTemp.Format("AnalyseItemArray.GetCount()=%d",   AnalyseItemArray.GetCount());
			theApp.WriteLog(strFile, strTemp);

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
			bool bSymbolEye=((cbEyeValue==2)||(cbEyeValue==5)||(cbEyeValue==8));

			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_FILL|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;
			}//End for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)

			//牌型判断
			ASSERT((bLianCard==true)||(bPengCard==true));

			//碰碰牌型
			if ((bLianCard==false)&&(bPengCard==true)) wChiHuKind|=CHK_PENG_PENG;
			if ((bLianCard==true)&&(bPengCard==true)&&((bSymbolEye==true)||(bNeedSymbol==false))) wChiHuKind|=CHK_JI_HU;
			if ((bLianCard==true)&&(bPengCard==false)&&((bSymbolEye==true)||(bNeedSymbol==false))) wChiHuKind|=CHK_PING_HU;
		}//End for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
	}//End if (AnalyseItemArray.GetCount()>0)

	//结果判断
	if (wChiHuKind!=CHK_NULL)
	{
		//设置结果
		ChiHuResult.wChiHuKind=wChiHuKind;
		ChiHuResult.wChiHuRight=wChiHuRight;

		return WIK_CHI_HU;
	}//End if (wChiHuKind!=CHK_NULL)

	return WIK_NULL;
}
*/
/*
//四喜胡牌
bool CGameLogic::IsSiXi(BYTE cbCardIndex[MAX_INDEX])
{
	//胡牌判断
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]==4) return true;
	}

	return false;
}
*/
//缺一色牌
bool CGameLogic::IsQueYiSe(BYTE cbCardIndex[MAX_INDEX])
{
	//胡牌判断
	BYTE cbIndex[]={0,9,18};
	BYTE i = 0;
	BYTE j = 0;
	for (i=0;i<CountArray(cbIndex);i++)
	{
		for (j=cbIndex[i];j<(cbIndex[i]+9);j++)
		{
			if (cbCardIndex[j]!=0) break;
		}
		if (j==(cbIndex[i]+9)) return true;
	}

	return false;
}
/*
//板板胡牌
bool CGameLogic::IsBanBanHu(BYTE cbCardIndex[MAX_INDEX])
{
	//胡牌判断
	for (BYTE i=1;i<MAX_INDEX;i+=3) if (cbCardIndex[i]!=0) return false;

	return true;
}

//六六顺牌
bool CGameLogic::IsLiuLiuShun(BYTE cbCardIndex[MAX_INDEX])
{
	//胡牌判断
	BYTE cbPengCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if ((cbCardIndex[i]>=3)&&(++cbPengCount>=2)) return true;
	}

	return false;
}
*/
//清一色牌
bool CGameLogic::IsQingYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsQingYiSe.log",strTime);

			strTemp.Format("IsQingYiSe");
			theApp.WriteLog(strFile, strTemp);
	//胡牌判断
	BYTE cbCardColor=0xFF;
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]!=0)
		{
			//花色判断
			if (cbCardColor!=0xFF) return false;

			//设置花色
			cbCardColor=(SwitchToCardData(i)&MASK_COLOR);

			//设置索引
			i=(i/9+1)*9;
		}
	}

	//组合判断
	for (BYTE i=0;i<cbItemCount;i++)
	{
		BYTE cbCenterCard=WeaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR)!=cbCardColor) return false;
	}

	return true;
}

//七小对牌
bool CGameLogic::IsQiXiaoDui(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsQiXiaoDui.log",strTime);

			strTemp.Format("IsQiXiaoDui");
			theApp.WriteLog(strFile, strTemp);
	//组合判断
	if (cbWeaveCount!=0) return false;

	//麻将判断
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndex[i];
		if ((cbCardCount!=0)&&(cbCardCount!=2)&&(cbCardCount!=4)) return false;
	}

	return true;
}

//豪华对牌
bool CGameLogic::IsHaoHuaDui(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	//变量定义
	bool bFourCard=false;

	//组合判断
	for (BYTE i=0;i<cbItemCount;i++)
	{
		//杆补判断
		if (WeaveItem[i].cbWeaveKind!=WIK_FILL) return false;
		if (WeaveItem[i].cbWeaveKind!=WIK_GANG) return false;

		//设置变量
		bFourCard=true;
	}

	//麻将判断
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		//四牌判断
		if (cbCardIndex[i]==4)
		{
			bFourCard=true;
			continue;
		}

		//对牌判断
		if ((cbCardIndex[i]!=0)&&(cbCardIndex[i]!=2)) return false;
	}

	//结果判断
	if (bFourCard==false) return false;

	return true;
}
/*
//将将胡牌
bool CGameLogic::IsJiangJiangHu(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	//组合判断
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		//类型判断
		BYTE cbWeaveKind=WeaveItem[i].cbWeaveKind;
		if ((cbWeaveKind!=WIK_PENG)&&(cbWeaveKind!=WIK_GANG)&&(cbWeaveKind!=WIK_FILL)) return false;

		//数值判断
		BYTE cbCenterValue=(WeaveItem[i].cbCenterCard&MASK_VALUE);
		if ((cbCenterValue!=2)&&(cbCenterValue!=5)&&(cbCenterValue!=8)) return false;
	}

	//麻将判断
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if ((i%3!=1)&&(cbCardIndex[i]!=0)) return false;
	}

	return true;
}
*/
//麻将转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<34);
	return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
}

//麻将转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
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

//分析麻将
bool CGameLogic::AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAnalyseCard.log",strTime);

			strTemp.Format("AnalyseCard cbWeaveCount=%d", cbWeaveCount);
			theApp.WriteLog(strFile, strTemp);
	//计算数目
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) cbCardCount+=cbCardIndex[i];

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0)) return false;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_COUNT-2];
	ZeroMemory(KindItem,sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

	//单吊判断
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==4));

		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX;i++)
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
			//同牌判断
			if (cbCardIndex[i]>=3)
			{
				KindItem[cbKindItemCount].cbCenterCard=i;
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				KindItem[cbKindItemCount++].cbWeaveKind=WIK_PENG;
			}
			
			//连牌判断
			if ((i<(MAX_INDEX-2))&&(cbCardIndex[i]>0)&&((i%9)<7))
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
		BYTE i = 0;
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[4]={0,1,2,3};
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			for (i=0;i<cbLessKindItem;i++) pKindItem[i]=&KindItem[cbIndex[i]];

			//数量判断
			bool bEnoughCard=true;
			for (i=0;i<cbLessKindItem*3;i++)
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
				for (i=0;i<MAX_INDEX;i++)
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
					for (i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
					}

					//设置牌型
					for (i=0;i<cbLessKindItem;i++) 
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
				for (i=cbLessKindItem-1;i>0;i--)
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

//////////////////////////////////////////////////////////////////////////
