1。色子。。。。 撒2次。。。
庄家。。。。
6+7。。。。

17*
14*。。。。

2。听牌。。。。。

3。怎么当庄。。。。
轮流当庄。。。

1。计算分数不一样。。。。
2。打过的不能胡。。。。。
自己打过的不能胡。。。。

3.碰牌。。。

4.剩10对牌，就黄牌。。。。

5.连黄3盘就赔。。。


CARD_HEAP_DOUBLE_H.bmp
CARD_HEAP_DOUBLE_V.bmp

IDB_CARD_HEAP_DOUBLE_H

m_ImageHeapDoubleV

//设置麻将
bool CHeapCard::SetCardData(WORD wMinusHeadCount, WORD wMinusLastCount, WORD wFullCount)

void CCardControl::SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)

					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+i*13;


				nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+i*13;


//方向枚举
enum enDirection
{
	Direction_East,					//东向
	Direction_South,				//南向
	Direction_West,					//西向
	Direction_North,				//北向
};


class CGameClientDlg : public CGameFrameDlg


	BYTE							m_cbLeftCardCount;					//剩余数目
	
	//荒庄结束
	if (m_cbLeftCardCount==0)
		
		
	WORD							wSiceCount;		
	
	
						//骰子点数		
						
						
						
	m_wSiceCount=MAKEWORD(rand()%6+1,rand()%6+1);
	
//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;							//骰子点数
	
	
//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lCellScore;							//单元积分
	WORD							wSiceCount;							//骰子点数
	
	
	
			StatusPlay.wSiceCount=m_wSiceCount;
			
			
LRESULT CGameClientDlg::OnShowSZ(WPARAM wParam, LPARAM lParam)


	case SUB_S_GAME_START:		//游戏开始
		{
				strTemp.Format("游戏开始");
				theApp.WriteLog(strFile, strTemp);
				CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
				cbSiceFirst=HIBYTE(pGameStart->wSiceCount) -1;
				cbSiceSecond=LOBYTE(pGameStart->wSiceCount) -1;

				memcpy( (char *)&myGameStart , (char *) pBuffer, wDataSize);
				PostMessage(IDM_SHOW_SZ,0,0);
				return true;
			//return OnSubGameStart(pBuffer,wDataSize);
		}
		
LRESULT CGameClientDlg::OnShowSZ(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_nDiceCount1 = cbSiceFirst;
	m_GameClientView.m_nDiceCount2 = cbSiceSecond;

	//strTemp.Format("m_nDiceCount1  =%d, m_GameClientView.m_nDiceCount2", m_GameClientView.m_nDiceCount1,m_GameClientView.m_nDiceCount2);
	//theApp.WriteLog(strFile, strTemp);
	//环境处理
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	m_GameClientView.m_tDice = 1;
	AfxBeginThread( MyThreadProc, this);
	return 0;
}

//显示用户MJ
void CGameClientDlg::ShowUserMJ()
{
	m_GameClientView.m_tDice ++;
	m_GameClientView.UpdateGameView(NULL);

	OnSubGameStart(&myGameStart ,sizeof(myGameStart));
	
}

	int									m_nDiceCount1;					///骰子数1
	int									m_nDiceCount2;					//骰子数2
						
						
						
			m_wHeapHand=wTakeChairID;
			m_wHeapTail=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
			
			
	WORD							m_wHeapHand;						//堆立头部
	WORD							m_wHeapTail;						//堆立尾部
											
											
	WORD wTakeChairID=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
	m_wHeapTail=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
	
	
	
#define HEAP_FULL_COUNT				34	


//麻将定义
#define HEAP_FULL_COUNT				34									//堆立全牌								//堆立全牌
34*4 = 136

17...

				
				
				//计算数目
				BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceSecond-1)*2:0);
				
		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceSecond-1)*2:0);																								
		
		
IDB_CARD_HEAP_DOUBLE_H  BITMAP                  "res\\CARD_HEAP_DOUBLE_H.bmp"

	m_ImageHeapDoubleV.LoadFromResource(hInstance,IDB_CARD_HEAP_DOUBLE_V);
	m_ImageHeapDoubleH.LoadFromResource(hInstance,IDB_CARD_HEAP_DOUBLE_H);
	
	
				for (WORD i=wDoubleHeap;i<(wFinallyIndex+3);i++)
				{
					nXPos=(m_ControlPoint.x+ 50);
					nYPos=m_ControlPoint.y  +i*13;
					g_CardResource.m_ImageHeapDoubleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
				
	//麻将变量
protected:
	WORD							m_wFullCount;						//全满数目
	WORD							m_wMinusHeadCount;					//头部空缺
	WORD							m_wMinusLastCount;					//尾部空缺
	
	
	CHeapCard						m_HeapCard[4];						//堆立麻将
	
	
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);

m_bHearStatus

bool CGameLogic::AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)

	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);


	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult)

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)

	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//用户麻将

//.什么情况出现听牌？？？
1。开始发牌。。。。
2。出牌。。。。。。
ACTION_EXPLAIN.bmp

IDB_ACTION_EXPLAIN      BITMAP                  "res\\ACTION_EXPLAIN.bmp"

	m_ImageActionExplain.SetLoadInfo(IDB_ACTION_EXPLAIN,hInstance);


	m_btTingCard.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()-10,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btGiveUp.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()*2-20,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btChiHu.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()*3-30,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);

//设置麻将
void CControlWnd::SetControlInfo(BYTE cbCenterCard, BYTE cbActionMask, tagGangCardResult & GangCardResult)

	//按钮控制
	m_btChiHu.EnableWindow(cbActionMask&WIK_CHI_HU);

#define WIK_CHI_HU					0x40								//吃胡类型

0x01 & 0x40 = 

	AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_TING_CARD,0);

	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
//麻将操作
LRESULT CGameClientDlg::OnCardOperate(WPARAM wParam, LPARAM lParam)

	SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

	case SUB_C_OPERATE_CARD:	//操作消息

			return OnUserOperateCard(pUserData->wChairID,pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);

1.ting ---->
2.

CControlWnd						m_ControlWnd;	

WIK_TING_CARD
OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)

	//听牌
bool CGameClientDlg::OnSubTingCard(const void * pBuffer, WORD wDataSize)


	//构造麻将
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
	//分析麻将
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);


AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,wChiHuRight,ChiHuResult);



	//组合麻将
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];	//组合麻将


	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户

QQ:115891940
		

	WORD							m_wBankerUser;							//庄家用户