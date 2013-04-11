#include "StdAfx.h"
#include "GameLogic.h"
/*
忻州麻将规则

 
//136,4
忻州麻将采用标准136张牌打法，分东南西北四家，每人13张牌，庄家起手14张，忻州麻将最大特点就是能碰不能吃，
和牌有砍和，
清一色，一条龙，缺一门，七小对等牌型。

 

和牌方法

和牌之前必须先听牌，听牌后不能跟换手中牌张(也不可以碰杠)，听牌时打出去的牌面朝下，
其余3个玩家无法看到听牌时打出的牌.

 

和牌种类

砍  和：例如：12和3，89和7，46和5，2和2。（七小对不算）

清一色  手上牌都是同一种花色

一条龙：牌里出现万，条，饼任意一门的123456789即可

七小对：7对牌，必须为7个不相同对。

缺一门：和牌时牌里没有万，饼，条任意一门，缺少2门的不算却和

 

和牌点数计算

和  牌：1点

砍  和：1点（吊牌也属于砍和）

清一色：5点

一条龙：5点

七小对：1点（七小对也属砍胡，不累加）

缺一门：1点

庄家：(连N庄) N点。初始1点

注意：

1．  放炮者没有听牌多加1点（听牌则同其他玩家支付同样点数）

2．  庄家输牌要加扣连庄点

3．  自摸所有点数翻倍。

4．  明杠1点，暗杠2点，流局无效（自摸不加倍）

 

得分举例：

1 非庄家 和牌，砍和，缺一门，自摸。

庄家（连庄2次）支付6+2*2=10点，其余玩家支付3*2=6点 （和牌1点，砍和1点，缺一门1点）。

2 有人放炮庄家和牌，连庄2次，一条龙。（有个暗杠）

放炮者（未听牌）支付9点，其余玩家支付8点（和牌1点，庄家2点，一条龙5点）

因为有个暗杠，都再多付2点（如果是自摸也是2点）。

*/
//////////////////////////////////////////////////////////////////////////

void WriteLog(CString strFileName, CString strText)
{
	try
	{
        CTime tm = CTime::GetCurrentTime();
        CString strTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
		//BOOL bFull = FALSE;
		CStdioFile file;
		if( file.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite) != 0)
		{
			file.SeekToEnd();
			file.WriteString(strTime);
			file.WriteString(strText);
			file.WriteString(_T("\n\n"));
			//if(file.GetLength() > 2000000)
			//	bFull = TRUE;
			file.Close();
		}
		/*
		if(!bFull) return;
		if( file.Open(strFileName, CFile::modeCreate|CFile::modeReadWrite) != 0)
		{
		file.SeekToEnd();
		file.WriteString(strTime);
		file.WriteString(strText);
		file.WriteString(_T("\n"));
		file.Close();
		}
		*/
	}
	catch(...)
	{
	}
}

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

	//工作目录
/*	TCHAR m_szWorkDirectory[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),m_szWorkDirectory,sizeof(m_szWorkDirectory));
	int nModuleLen=lstrlen(m_szWorkDirectory);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	m_szWorkDirectory[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(m_szWorkDirectory);

	//读取麻将
	TCHAR szFile[MAX_PATH]=TEXT(""),szItem[20];
	_sntprintf(szFile,CountArray(szFile),TEXT("%s\\CardInfo.INI"),m_szWorkDirectory);

	for (BYTE i=0;i<CountArray(m_cbCardDataArray);i++)
	{
		TCHAR szRead[64]=TEXT("");
		_sntprintf(szItem,CountArray(szItem),TEXT("Item%d"),i+1);
		GetPrivateProfileString(TEXT("CardData"),szItem,TEXT(""),szRead,sizeof(szRead),szFile);
		cbCardData[i]=atoi(szRead);
		if (cbCardData[i]==0) break;
	}
	if (i==CountArray(m_cbCardDataArray)) return;*/

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
	WriteLog(strFile, strTemp);
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

	strTemp.Format("GetChiHuActionRank wChiHuKind=%d, wChiHuRight=%d" , ChiHuResult.wChiHuKind, ChiHuResult.wChiHuRight) ;
	WriteLog(strFile, strTemp);
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

	strTemp.Format("GetChiHuActionRank cbChiHuOrder=%d", cbChiHuOrder);
	WriteLog(strFile, strTemp);

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
		}//End if
	}//End for

	return cbEatKind;
}

//碰牌判断
BYTE CGameLogic::EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sEstimatePengCard.log",strTime);

			strTemp.Format("EstimatePengCard");
			WriteLog(strFile, strTemp);
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
			WriteLog(strFile, strTemp);
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?(WIK_GANG|WIK_FILL):WIK_NULL;
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
			cbActionMask|=(WIK_GANG|WIK_FILL);
			GangCardResult.cbCardData[GangCardResult.cbCardCount]=(WIK_GANG|WIK_FILL);
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}//End if
	}//End for

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
			}//End if
		}//End if
	}//End for

	return cbActionMask;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAnalyseChiHuCard.log",strTime);

	strTemp.Format("AnalyseChiHuCard  cbWeaveCount=%d,  cbCurrentCard=%d",   cbWeaveCount,  cbCurrentCard);
	WriteLog(strFile, strTemp);

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
	if ((cbCurrentCard!=0)&&(cbWeaveCount==4)) wChiHuRight|=CHK_QUAN_QIU_REN;
	if (IsQingYiSe(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true) wChiHuRight|=CHK_QING_YI_SE;

	strTemp.Format("wChiHuRight=%d",   wChiHuRight);
	WriteLog(strFile, strTemp);
	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	strTemp.Format("AnalyseItemArray.GetCount()=%d",   AnalyseItemArray.GetCount());
	WriteLog(strFile, strTemp);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		//眼牌需求
		bool bNeedSymbol=((wChiHuRight&0xFF00)==0);

		bNeedSymbol = false;//固定

		//牌型分析
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			bool bLianCard=false,bPengCard=false;
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

			//牌眼类型
			BYTE cbEyeValue=pAnalyseItem->cbCardEye&MASK_VALUE;
			//2,5,8将
			bool bSymbolEye=((cbEyeValue==2)||(cbEyeValue==5)||(cbEyeValue==8));
			//没有将！！
			bSymbolEye = true;


			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_FILL|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;
			}//End for

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
	}

	return WIK_NULL;
}
//是否为一条龙
//牌里出现万，条，饼任意一门的123456789即可
bool CGameLogic::IsOneLongMJ(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsOneLongMJ.log",strTime);

	strTemp.Format("into IsOneLongMJ");
	WriteLog(strFile, strTemp);

	bool resultBool = false;

	//胡牌判断
	BYTE cbIndex[]={0,9,18};
	BYTE i = 0;
	BYTE j = 0;
	for (i=0;i<CountArray(cbIndex);i++)
	{
		bool isLong = true;
		for (j=cbIndex[i];j<(cbIndex[i]+9);j++)
		{
			strTemp.Format("cbCardIndex[j]=%d", cbCardIndex[j]);
			WriteLog(strFile, strTemp);

			if (cbCardIndex[j]==0) 
			{
				strTemp.Format("isLong=false");
				WriteLog(strFile, strTemp);
				isLong=false;
			}//End if
		}//End for

		if (isLong)
			return true;
		
	}//End for

	return resultBool;

}
//缺一色牌
bool CGameLogic::IsQueYiSe(BYTE cbCardIndex[MAX_INDEX])
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsQueYiSe.log",strTime);

	strTemp.Format("IsQueYiSe");
	WriteLog(strFile, strTemp);

	//胡牌判断
	BYTE cbIndex[]={0,9,18};
	BYTE i = 0;
	BYTE j = 0;
	
	int mjColorInt = 0;//花色种类
	
	for (i=0;i<CountArray(cbIndex);i++)
	{
		int tempInt = 0;
		for (j=cbIndex[i];j<(cbIndex[i]+9);j++)
		{
			if (cbCardIndex[j]!=0) 
			{
				//mjColorInt++;
				tempInt++;
				strTemp.Format("i=%d, j=%d",  i, j);
				WriteLog(strFile, strTemp);

				//break;
			}
		}

		if ( tempInt > 0 ) 
		{
			mjColorInt++;
		}
		strTemp.Format("mjColorInt=%d", mjColorInt);
		WriteLog(strFile, strTemp);
		//if (j==(cbIndex[i]+9)) return true;
	}//End for  3

	if(2 == mjColorInt)
	{
		return true;
	}

	return false;
}

//七小对牌
bool CGameLogic::IsQiXiaoDui(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsQiXiaoDui.log",strTime);

	strTemp.Format("IsQiXiaoDui");
	WriteLog(strFile, strTemp);

	//组合判断
	if (cbWeaveCount!=0) return false;

	//麻将判断
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndex[i];
		if ((cbCardCount!=0)&&(cbCardCount!=2)&&(cbCardCount!=4)) return false;
	}//End for

	return true;
}

//是否为砍和
//砍  和：例如：12和3，89和7，46和5，2和2。（七小对不算）
bool CGameLogic::IsKanMJ(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsKanMJ.log",strTime);

	strTemp.Format("IsKanMJ");
	WriteLog(strFile, strTemp);	
	bool resultBool = false;

	return resultBool;
}
//清一色牌
bool CGameLogic::IsQingYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sIsQingYiSe.log",strTime);

	strTemp.Format("IsQingYiSe");
	WriteLog(strFile, strTemp);
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
	}//End for

	//组合判断
	for (BYTE i=0;i<cbItemCount;i++)
	{
		BYTE cbCenterCard=WeaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR)!=cbCardColor) return false;
	}//End for

	return true;
}

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
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sSwitchToCardData.log",strTime);

	strTemp.Format("into SwitchToCardData");
	WriteLog(strFile, strTemp);

	//转换麻将
	BYTE cbPosition=0;
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]!=0)
		{
			strTemp.Format("cbCardIndex[%d]=%d", i,cbCardIndex[i]);
			WriteLog(strFile, strTemp);

			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);

				strTemp.Format("cbCardData[%d]=%d", i,cbCardData[cbPosition]);
				WriteLog(strFile, strTemp);

			}//End for
		}//End if
	}//End for (BYTE i=0;i<MAX_INDEX;i++)

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
	}//End for (BYTE i=0;i<cbCardCount;i++)

	return cbCardCount;
}

//分析麻将
bool CGameLogic::AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAnalyseCard.log",strTime);

	// 0, 1, 2
	strTemp.Format("AnalyseCard cbWeaveCount=%d", cbWeaveCount);
	WriteLog(strFile, strTemp);

	//计算数目
	BYTE cbCardCount=0;
	BYTE i = 0;
	for (i=0;i<MAX_INDEX;i++) 
	{
	strTemp.Format("cbCardIndex[%d]=%d", i,cbCardIndex[i]);
//	WriteLog(strFile, strTemp);
//0,1
		cbCardCount+=cbCardIndex[i];
	}//End if

	strTemp.Format("AnalyseCard cbCardCount=%d", cbCardCount);
	WriteLog(strFile, strTemp);


	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	//11,14
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0)) 
	{
		strTemp.Format("AnalyseCard 返回！");
		WriteLog(strFile, strTemp);

		return false;
	}

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_COUNT-2];
	ZeroMemory(KindItem,sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

//3,4
	strTemp.Format("cbLessKindItem=%d", cbLessKindItem);
	WriteLog(strFile, strTemp);

	//单吊判断
	if (cbLessKindItem==0)
	{
		strTemp.Format("单吊判断");
		WriteLog(strFile, strTemp);

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
				}//End for
				AnalyseItem.cbCardEye=SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.Add(AnalyseItem);
				strTemp.Format("AnalyseCard 返回22！");
				WriteLog(strFile, strTemp);
				return true;
			}//End if (cbCardIndex[i]==2)
		}//End for (BYTE i=0;i<MAX_INDEX;i++)
		strTemp.Format("AnalyseCard 返回2！");
		WriteLog(strFile, strTemp);
		return false;
	}//End if

	//拆分分析
	if (cbCardCount>=3)
	{
		strTemp.Format("拆分分析");
		WriteLog(strFile, strTemp);

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
				strTemp.Format("同牌判断");
				WriteLog(strFile, strTemp);
			}//End if (cbCardIndex[i]>=3)
			
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
					}//End if
				}//End for (BYTE j=1;j<=cbCardIndex[i];j++)
				strTemp.Format("连牌判断");
				WriteLog(strFile, strTemp);
			}//End if
		}//End for (BYTE i=0;i<MAX_INDEX;i++)
	}//End if (cbCardCount>=3)

	strTemp.Format("cbKindItemCount=%d, cbLessKindItem=%d", cbKindItemCount,cbLessKindItem);
	WriteLog(strFile, strTemp);
	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		strTemp.Format("组合分析");
		WriteLog(strFile, strTemp);

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
			}//End for (i=0;i<cbLessKindItem*3;i++)

			//胡牌判断
			if (bEnoughCard==true)
			{
				strTemp.Format("胡牌判断");
				WriteLog(strFile, strTemp);

				//牌眼判断
				BYTE cbCardEye=0;
				for (BYTE i=0;i<MAX_INDEX;i++)
				{
					if (cbCardIndexTemp[i]==2)
					{
						cbCardEye=SwitchToCardData(i);
						break;
					}
				}//End for

				strTemp.Format("cbCardEye=%d", cbCardEye);
				WriteLog(strFile, strTemp);

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
					}//End for

					//设置牌型
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
					}//End for

					//设置牌眼
					AnalyseItem.cbCardEye=cbCardEye;

					//插入结果
					AnalyseItemArray.Add(AnalyseItem);
				}//End if
			}//End if (bEnoughCard==true)

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

/*
和  牌：1点

砍  和：1点（吊牌也属于砍和）

清一色：5点

一条龙：5点

七小对：1点（七小对也属砍胡，不累加）

缺一门：1点
*/
//////////////////////////////////////////////////////////////////////////
//计算MJ的分数  基础分 + 番数
int CGameLogic::GetWinMJNum(BYTE cbCardIndex[MAX_INDEX], BYTE cbItemCount)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sGetWinMJNum.log",strTime);

	strTemp.Format("计算得分") ;
	WriteLog(strFile, strTemp);

	int resultMj = 0;//和牌

	strTemp.Format("输入牌：(%d)", cbItemCount) ;
	WriteLog(strFile, strTemp);

	CString MJListStr[4]={"万","条","筒","字"};
	CString myCardStr;

	for (int i=0; i <cbItemCount ;i++)
	{
		BYTE cbCardData;
		cbCardData = SwitchToCardData(i);
		BYTE cbValue=(cbCardData&MASK_VALUE);//点数
		BYTE cbColor=(cbCardData&MASK_COLOR)>>4;//花色


		myCardStr.Format("%d %s",cbValue,  MJListStr[cbColor]);
		strTemp.Format("牌%s, i=%d（0x%x）,张数=%d", myCardStr, i, cbCardData,cbCardIndex[i] ) ;
		WriteLog(strFile, strTemp);

	}//End for

	tagWeaveItem WeaveItem[4];
	memset((char *)WeaveItem , 0 , sizeof(tagWeaveItem)*4);

	//是否为砍和
	//砍  和：例如：12和3，89和7，46和5，2和2。（七小对不算）
	if ( IsKanMJ( cbCardIndex,  WeaveItem,  cbItemCount) )
	{
		resultMj++;
		strTemp.Format("砍和") ;
		WriteLog(strFile, strTemp);
	}
	else
	{
		//七小对牌
		if (  IsQiXiaoDui( cbCardIndex,  WeaveItem,  cbItemCount))
		{
			resultMj++;
			strTemp.Format("七小对") ;
			WriteLog(strFile, strTemp);
		}
	}
	//缺一色牌
//缺一门：和牌时牌里没有万，饼，条任意一门，缺少2门的不算却和
	if (  IsQueYiSe( cbCardIndex))
	{
		resultMj++;
		strTemp.Format("缺一色") ;
		WriteLog(strFile, strTemp);
	}


	//清一色牌 是否为清一色
	if ( IsQingYiSe( cbCardIndex, WeaveItem,  cbItemCount) )
	{
		resultMj+=5;
		strTemp.Format("清一色") ;
		WriteLog(strFile, strTemp);
	}

	//是否为一条龙
	//牌里出现万，条，饼任意一门的123456789即可
	if (  IsOneLongMJ( cbCardIndex,  WeaveItem,  cbItemCount))
	{
		resultMj+=5;
		strTemp.Format("一条龙") ;
		WriteLog(strFile, strTemp);
	}

	strTemp.Format("结果：(%d)", resultMj) ;
	WriteLog(strFile, strTemp);

	return resultMj;
}