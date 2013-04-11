#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include ".\gameclientdlg.h"
#include "GameOption.h"
#include "Struct.h"


//////////////////////////////////////////////////////////////////////////
///////////////////////////计时器线程//////////////////////////////////////////////////////
UINT MyThreadProc( LPVOID pParam )
{
	CGameClientDlg *GameView = (CGameClientDlg*)pParam;
	while( GameView->m_GameClientView.m_tDice > 0 )
	{
		Sleep( 50 );
		if( GameView->m_GameClientView.m_tDice<12)
		{
			GameView->m_GameClientView.m_tDice ++;
			GameView->m_GameClientView.UpdateGameView( NULL );
		}
		else
		{
			Sleep( 1000 );
			GameView->m_GameClientView.m_Timer.ShowWindow( SW_SHOW );
			GameView->m_GameClientView.m_tDice= 0;
			GameView->m_GameClientView.SetTimer(ID_OUTFLAG_TIME, 150,(TIMERPROC) NULL);
			GameView->m_GameClientView.UpdateGameView( NULL );
		}
	}
	while( GameView->m_GameClientView.m_bActionOut== true )
	{
		Sleep(50);
		//int m_playTime = timeGetTime()*1000;
		//if( (m_playTime-GameView->m_GameClientView.m_tOutStartTime)%2==1)
		{
			GameView->m_GameClientView.m_tOut += 1;
			GameView->m_GameClientView.m_bPlay=false;
			if( GameView->m_GameClientView.m_tOut==10)
			{
				GameView->m_GameClientView.m_tOut = 0;
				GameView->m_GameClientView.m_bActionOut= false;
				GameView->m_GameClientView.m_iUpMj = -1;
			}
			GameView->m_GameClientView.UpdateGameView(NULL);
		}
	}
	return 0;
}
BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
//	ON_WM_TIMER()

	ON_MESSAGE( IDM_START, 	 OnStart )
	ON_MESSAGE( IDM_HU_MJ,	 OnHuMj )
	ON_MESSAGE( IDM_TING_MJ, OnTingMj )
	ON_MESSAGE( IDM_GANG_MJ, OnGangMj )
	ON_MESSAGE( IDM_PENG_MJ, OnPengMj )
	ON_MESSAGE( IDM_CHI_MJ,  OnChiMj )
	ON_MESSAGE( IDM_GIVEUP,  OnGiveUp )
	ON_MESSAGE( IDM_OUT_MJ,  OnOutMj )
	ON_MESSAGE( IDM_LEFTHIT_MJ, OnLeftHitMj )
	ON_MESSAGE( IDM_RIGHTHIT_MJ, OnRightHitMj )
	
	ON_MESSAGE( IDM_SELECT_CHI, OnSelectChi)
	ON_MESSAGE( IDM_SELECT_GANG, OnSelectGang )

	ON_MESSAGE( IDM_TIMER_OUT, OnCmdTimerOut )

	ON_MESSAGE( IDM_GAMESET, OnGameSet )
	ON_MESSAGE( IDM_SHOWMINI, OnShowMini)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//设置变量
	//m_bDeasilOrder = true;
	log.OpenNewFile( "广东手上麻将.log", 5 );
	memset( m_byHandMjs, 0, sizeof( m_byHandMjs ) );
	//m_byHandMjCount = 14;
	//for( BYTE i=0; i<m_byHandMjCount; i++ )
	//{
	//	m_byHandMjs[i] = 0;
	//}
	memset( m_byMjCount, 0, sizeof( m_byMjCount ) );

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//__super::OnInitDialog();
	//设置标题
	SetWindowText(TEXT("茶馆游戏中心 -- 广东麻将"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//载入配置
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;
	//设置控件
	m_GameClientView.SetUserOrder( m_bDeasilOrder );
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;

	//配置数据
	if( GameOption.DoModal()==IDOK )
	{
		//设置控件
		EnableSound(GameOption.m_bEnableSound);

		//保存配置文件
	}
	return ;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch( wSubCmdID )
	{
		//发送麻将
	case SUB_S_SEND_MJ:
		{
			return OnSubSendMj( pBuffer, wDataSize );
		}
		//游戏开始
	case SUB_S_GAME_START:
		{
			return OnSubGameStart( pBuffer, wDataSize );
		}
	//case SUB_S_SHOW_BUTTON:
	//	{
	//		return OnSubShowButton( pBuffer, wDataSize );
	//	}
	case SUB_S_OUT_MJ:
		{
			return OnSubOutMj( pBuffer, wDataSize );
		}
	case SUB_S_TOUCH_MJ:
		{
			return OnSubTouchMj( pBuffer, wDataSize );
		}
	case SUB_S_GANG_MJ:
		{
			return OnSubGangMj( pBuffer, wDataSize);
		}
	case SUB_S_SELECT_GANG:
		{
			return OnSubSelectGang( pBuffer, wDataSize);
		}
	case SUB_S_SELECT_CHI:
		{
			CMD_S_ChiMode *ChiMode= (CMD_S_ChiMode *)pBuffer;
			m_GameClientView.m_nChiMode = ChiMode->nChiType;
			return true;
		}
	case SUB_S_PENG_MJ:
		{
			return OnSubPengMj( pBuffer, wDataSize );
		}
	case SUB_S_CHI_MJ:
		{
			return OnSubChiMj( pBuffer, wDataSize );
		}
	case SUB_S_GAME_END:
		{
			return OnSubGameEnd( pBuffer, wDataSize );
		}
	case SUB_S_END_EMPTY:
		{
			return OnSubEndEmpty( pBuffer, wDataSize);
		}
	}
	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch( cbGameStation )
	{
	case GS_WK_FREE:		//空闲状态
		{
			//校验数据
			if( wDataSize != sizeof( CMD_S_StatusFree ) ) return false;
			CMD_S_StatusFree* pStatusFree = ( CMD_S_StatusFree* )pBuffer;

			//设置
			m_GameClientView.m_btStart.ShowWindow( SW_SHOW );
			m_GameClientView.m_btChi.ShowWindow( SW_SHOW );
			m_GameClientView.m_btPeng.ShowWindow( SW_SHOW );
			m_GameClientView.m_btGang.ShowWindow( SW_SHOW );
			m_GameClientView.m_btHu.ShowWindow( SW_SHOW );
			m_GameClientView.m_btGiveUp.ShowWindow( SW_SHOW );
			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//校验数据
			//MessageBox("Playing" );
			//if( wDataSize != sizeof( CMD_S_StatusPlay ) ) return false;
			CMD_S_StatusPlay* pStatusPlay = ( CMD_S_StatusPlay* )pBuffer;

			//设置变量
			CopyMemory( m_byHandMjs, pStatusPlay->byMjs, sizeof(BYTE)*14 );
		}
	}
	return true;
	//return false;
}

//////////////////////////////////////////////////////////////////////////
//消息响应
//void CGameClientDlg::OnTimer(UINT nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CGameFrameDlg::OnTimer(nIDEvent);
//}

//开始
LRESULT CGameClientDlg::OnStart( WPARAM wParam, LPARAM lParam )
{
	m_GameClientView.m_bDeasilOrder = true;

	//隐藏控件
	m_GameClientView.CleanUpView();
	m_GameClientView.m_btStart.ShowWindow( SW_HIDE );

	//画准备
	SendUserReady( NULL, 0 );
	PlayGameSound(AfxGetInstanceHandle(),TEXT("Ready"));

	//更新视图
	m_GameClientView.UpdateGameView( NULL );
	return 0;
}

//出牌
LRESULT CGameClientDlg::OnOutMj( WPARAM wParam, LPARAM lParam )
{
	AfxBeginThread( MyThreadProc, this/*->m_GameClientView*/ );
	//预先显示界面
	m_byOutMj = m_GameClientView.m_byHandMjs[2][m_GameClientView.m_iUpMj];
	m_GameClientView.m_byOutMj = m_byOutMj;
	m_GameClientView.m_wLastUser = 2;

	m_GameClientView.m_vcOutMjs[2].push_back( m_GameClientView.m_byHandMjs[2][m_GameClientView.m_iUpMj] );
	m_GameClientView.m_nActionOutFlag = 2;

	//发送数据
	m_GameLogic.TransMjIToD( m_GameClientView.m_byHandMjs[2][m_GameClientView.m_iUpMj], &m_byOutMj );
	SendData( SUB_C_OUT_MJ, &m_byOutMj, sizeof( BYTE ) );
	
	//设置出牌标志
	m_GameClientView.m_bPlay = false;
	m_GameClientView.HideButton();

	//预先显示界面
	//m_byHandMjCount -= 1;
	//m_GameClientView.m_byHandMjs[2][m_GameClientView.m_iUpMj] = m_byHandMjs[m_GameClientView.m_iUpMj];
	//m_GameClientView.m_byHandMjCount[2] -= 1;

	m_GameClientView.m_byHandMjs[2][m_GameClientView.m_iUpMj] = 0;
	if( m_GameClientView.m_iUpMj != HAND_MJ_MAX-1 )
	{
		m_GameLogic.LMove( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2] );	
		m_GameClientView.m_byHandMjCount[2] -= 1;
		m_GameLogic.InsertMjList( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2] );
	}
	else
	{
		m_GameClientView.m_byHandMjCount[2] -= 1;
	}

	//m_GameLogic.InsertMjList( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2] );

	//log.DrawLine( 1, 5);
	//for( int i=0; i<14; i++)
	//{
	//	REPORT_V( log, "i", i, 5 );
	//	REPORT_V( log, "手上麻将",m_byHandMjs[i], 5 );				
	//}
	//计时器
	m_GameClientView.m_Timer.SetPause( true );
	m_GameClientView.m_bAction = false;
	m_GameClientView.UpdateGameView( NULL );
	return 0;
}

//放弃出牌
LRESULT CGameClientDlg::OnGiveUp( WPARAM wParam, LPARAM lParam )
{
//#ifdef DEBUG_GAME
//	for( int i=0; i<4; i++ )
//	{
//		m_GameLogic.CleanHu( &m_GameClientView.m_hu[i] );
//	}
//#endif

	CMD_C_Operation Operation;
	Operation.nRight = 0;
	Operation.wCurrentUser = GetMeChairID();
	Operation.wLastUser = m_GameClientView.m_wLastUser;

	SendData( SUB_C_GIVEUP, &Operation, sizeof( CMD_C_Operation ) );
	//预先隐藏
	m_GameClientView.HideButton();
	if( m_nChiMode || m_nGangMode)
	{
		m_nGangMode = 0;
		m_nChiMode = 0;
	}
	//计时器
	if( !m_GameClientView.m_bPlay)
	{
		m_GameClientView.m_bAction = false;
	}
	return 0;
}

//胡牌
LRESULT CGameClientDlg::OnHuMj( WPARAM wParam, LPARAM lParam )
{
	CMD_C_Hu hu;
	hu.nRight = 6;
	
	SendData( SUB_C_HU_MJ, &hu, sizeof(CMD_C_Hu));
	//预先隐藏
	m_GameClientView.HideButton();
	if( m_nChiMode || m_nGangMode )
	{
		m_nGangMode=0;
		m_nChiMode=0;
	}
	PlayGameSound(AfxGetInstanceHandle(),TEXT("hu3"));
	return 0;
}

//听牌
LRESULT CGameClientDlg::OnTingMj( WPARAM wParam, LPARAM lParam )
{
	MJ_BLOCK block;
	block.first= 2;
	block.style= BLOCK_AN_GANG;
	for( int i=0; i<4;i++)
	{
		m_GameLogic.AddBlock( &m_GameClientView.m_hu[i], &block );
	}

	return 0;
}

//杠牌
LRESULT CGameClientDlg::OnGangMj( WPARAM wParam, LPARAM lParam )
{
	CMD_C_Gang Gang;
	Gang.nRight= 4;
	Gang.nGangType= m_nGangMode;
	SendData( SUB_C_GANG_MJ, &Gang, sizeof(CMD_C_Gang) );
	return 0;
}

//选择杠牌
LRESULT CGameClientDlg::OnSelectGang( WPARAM wParam, LPARAM lParam )
{
	CMD_C_SelectGang SelectGang;
	m_GameLogic.TransMjIToD( m_GameClientView.m_byHandMjs[2][m_GameClientView.m_iUpMj],&SelectGang.byGangMj );
	SelectGang.nGangType= m_GameClientView.m_nGangType;
	SendData( SUB_C_SELECT_GANG, &SelectGang, sizeof(CMD_C_SelectGang) );
	return 0;
}
//碰牌
LRESULT CGameClientDlg::OnPengMj( WPARAM wParam, LPARAM lParam )
{
	//发送数据
	CMD_C_Operation Peng;
	Peng.nRight = 3;
	Peng.wCurrentUser = GetMeChairID();
	Peng.wLastUser    = m_GameClientView.m_wCurrentUser;
	SendData( SUB_C_PENG_MJ, &Peng, sizeof( CMD_C_Operation ) );

	//返回成功再取消
	//m_GameClientView.HideButton();
	return 0;
}

//吃牌
LRESULT CGameClientDlg::OnChiMj( WPARAM wParam, LPARAM lParam )
{
	CMD_C_Chi Chi;
	Chi.nRight= 2;
	Chi.nChiType = m_nChiMode;
	SendData( SUB_C_CHI_MJ, &Chi,sizeof( CMD_C_Chi ) );
	return 0;
}

//选择吃牌
LRESULT CGameClientDlg::OnSelectChi( WPARAM wParam, LPARAM lParam )
{
	CMD_C_Chi Chi;
	m_GameClientView.m_bAction = false;
	Chi.nRight= 2;
	Chi.nChiType = m_GameClientView.m_nChiType;
	SendData( SUB_C_CHI_MJ, &Chi,sizeof( CMD_C_Chi ) );
	return 0;
}
//超时处理
LRESULT CGameClientDlg::OnCmdTimerOut( WPARAM wParam, LPARAM lParam )
{
	//出第十三张牌
	if( m_GameClientView.m_bPlay )
	{
		m_GameClientView.m_iUpMj = 13;
		SendMessage( IDM_OUT_MJ, 1, 1 );
		m_GameClientView.m_iUpMj = -1;
	}
	else if( m_GameClientView.m_bAction )
	{
		m_GameClientView.m_bAction = false;
		SendMessage( IDM_GIVEUP, 1, 1 );
	}

	return 0;
}

//左键
LRESULT CGameClientDlg::OnLeftHitMj( WPARAM wParam, LPARAM lParam )
{
	return 0;
}

//右键
LRESULT CGameClientDlg::OnRightHitMj( WPARAM wParam, LPARAM lParam )
{
	return 0;
}

/////////////////////////////////////////////////////辅助函数
//用户转换
WORD CGameClientDlg::TransViewID( WORD wChairID )
{
	WORD transID;
	WORD myChairID = GetMeChairID();
	//逆时针
	if( !m_bDeasilOrder )
	{
		switch ( myChairID )
		{
		case 0:
			transID = wChairID+2;
			break;
		case 1:
			transID = wChairID+1;
			break;
		case 2:
			transID = wChairID+0;
			break;
		case 3:
			transID = wChairID+3;
			break;
		}
		if( transID > 3 )
			transID -= 4;

		//switch( transID)
		//{
		//case 1:
		//	transID = 3;
		//	break;
		//case 3:
		//	transID = 1;
		//	break;
		//default:
		//	break;
		//}
		//if( transID == 1 )
		//{
		//	transID = 3;
		//}
		//else if( transID == 3 )
		//{
		//	transID = 1;
		//}
		return transID;
	}
	//顺时针
	else
	{
		switch ( myChairID )
		{
		case 0:
			transID = wChairID+2;
			break;
		case 1:
			transID = wChairID+1;
			break;
		case 2:
			transID = wChairID+0;
			break;
		case 3:
			transID = wChairID+3;
			break;
		}
		if( transID > 3 )
			transID -= 4;
		return transID;
	}

}

//庄位转换
void CGameClientDlg::SetBanker( WORD wBanker )
{
	//逆时针
	if( !m_bDeasilOrder )
	{
		for( int i=0; i<GAME_PLAYER; i++ )
		{
			if( wBanker-i< 0 )
			{
				m_GameClientView.m_byFengWei[i] = wBanker-i+GAME_PLAYER;
			}
			else
			{
				m_GameClientView.m_byFengWei[i] = wBanker-i;
			}

			////逆时针换位
			//if( m_GameClientView.m_byFengWei[i] == 1 )
			//{
			//	m_GameClientView.m_byFengWei[i] = 3;
			//}
			//else if( m_GameClientView.m_byFengWei[i] == 3 )
			//{
			//	m_GameClientView.m_byFengWei[i] = 1;
			//}
		}
	}
	//顺时针
	else
	{
	}

}

/////////////////////////////////////////消息处理
//游戏开始
bool CGameClientDlg::OnSubGameStart( const void * pBuffer, WORD wDataSize )
{
	CMD_S_GameStart *GameStart= (CMD_S_GameStart*)pBuffer;
	//效验数据
	ASSERT( wDataSize == sizeof( CMD_S_GameStart ) );
	if( wDataSize != sizeof( CMD_S_GameStart ) ) return false;

	//设置数据
	m_GameClientView.m_wQuanWeiBanker= GameStart->wBankerUser;
	m_GameClientView.m_wBankerUser = TransViewID( GameStart->wBankerUser );
	SetBanker( m_GameClientView.m_wBankerUser );
	m_GameClientView.m_wQuanWei= GameStart->wQuanFeng;

	//选取大点的做
	if( GameStart->nSartNumber1>GameStart->nSartNumber2)
	{
		m_GameClientView.m_nDiceCount1 = GameStart->nSartNumber1;
		m_GameClientView.m_nDiceCount2 = GameStart->nSartNumber2;
	}
	else
	{
		m_GameClientView.m_nDiceCount1 = GameStart->nSartNumber2;
		m_GameClientView.m_nDiceCount2 = GameStart->nSartNumber1;
	}
	//m_GameClientView.SetTimer( ID_DICE_TIME, 50, 0 );	
	m_GameClientView.m_tDice = 1;
	AfxBeginThread( MyThreadProc, this);
	//减牌
	for( int i=0; i<4*HAND_MJ_MAX-3; i++)
		m_GameClientView.SubTouchMJ();
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	return true;
}

//发牌
bool CGameClientDlg::OnSubSendMj( const void * pBuffer, WORD wDataSize )
{	
	//变量定义
	CMD_S_SendMj *pSendMj = ( CMD_S_SendMj* )pBuffer;

	//效验数据
	ASSERT( wDataSize == sizeof( CMD_S_SendMj ) );
	if( wDataSize != sizeof( CMD_S_SendMj ) ) return false;

	//设置数据
	m_GameClientView.m_wQuanWeiBanker = pSendMj->wBankerUser;
	m_GameClientView.m_wBankerUser = TransViewID( pSendMj->wBankerUser );
	SetBanker( m_GameClientView.m_wBankerUser );
	//m_byHandMjCount = pSendMj->byMjCount;
//	CopyMemory( &m_byHandMj, pSendMj->byMjs, sizeof( pSendMj->byMjs ) );


	//设置玩家
	if( GetMeChairID() == pSendMj->wBankerUser )
	{
		m_GameClientView.m_bPlay = true;
		m_GameClientView.m_btHu.EnableWindow( pSendMj->nHu );
		m_GameClientView.m_btGang.EnableWindow( pSendMj->nGang );
		m_nGangMode= pSendMj->nGang;
		if( pSendMj->nHu||pSendMj->nGang )
		{
			m_GameClientView.m_btGiveUp.EnableWindow( true );
		}
	}
	else
	{
		m_GameClientView.m_bPlay = false;
	}
	//设置界面
	//BYTE BufferMj[14];
	for( WORD i=0; i<GAME_PLAYER; i++ )
	{
		if( i == TransViewID( pSendMj->wBankerUser ) )			//庄14张牌
		{
			m_GameClientView.m_byHandMjCount[i] = HAND_MJ_MAX ;
		}
		else
		{
			m_GameClientView.m_byHandMjCount[i] = HAND_MJ_MAX-1;
			//m_GameClientView.m_byHandMjs[2][13] = 0;
		}
	}
	//当前玩家
	//if( ( IsLookonMode()==false ) && ( pSendMj->wCurrentUser == GetMeChairID() ) )
	{
		for( BYTE j=0; j<m_GameClientView.m_byHandMjCount[2]; j++ )
		{		
			if( !m_GameLogic.TransMjDToI( pSendMj->byMjs[j], &m_byHandMjs[j] ) )MessageBox( "转换出错");
			REPORT_V( log, "j", j, 5 );
			REPORT_V( log, "未转换",pSendMj->byMjs[j], 5 );	
		}

		m_GameClientView.GetHandMj( m_byHandMjs, m_GameClientView.m_byHandMjCount[2] );	
		for( int i=0; i<m_GameClientView.m_byHandMjCount[2]; i++)
		{
			REPORT_V( log, "i", i, 5 );
			REPORT_V( log, "视图转换",m_byHandMjs[i], 5 );				
		}
	}
	//设置时钟
	m_GameClientView.m_Timer.SetTimerUp( m_GameClientView.m_wCurrentUser, 15, true );
	m_GameClientView.UpdateGameView( NULL );
	return true;
}

//出牌
bool CGameClientDlg::OnSubOutMj( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT( wDataSize == sizeof( CMD_S_OutMj ) );
	if( wDataSize != sizeof( CMD_S_OutMj ) ) return false;

	//变量定义
	CMD_S_OutMj *pOutMj = ( CMD_S_OutMj* )pBuffer;

	//设置数据
	m_GameLogic.TransMjDToI( pOutMj->byOutMj, &m_byOutMj );
	m_GameClientView.AddOutMj( TransViewID( pOutMj->wOutMjUser ), m_byOutMj );

	m_GameClientView.m_byOutMj = m_byOutMj;													//记录出牌

	m_GameClientView.m_wLastUser = TransViewID( pOutMj->wOutMjUser );						//上次出牌者
	m_GameClientView.m_nActionOutFlag = m_GameClientView.m_wLastUser;
	m_GameClientView.m_byHandMjCount[ m_GameClientView.m_wLastUser ] -= 1;					//牌数减一

	m_GameClientView.m_btHu.EnableWindow( pOutMj->nHu );
	m_GameClientView.m_btGang.EnableWindow( pOutMj->nGang );
	m_GameClientView.m_btPeng.EnableWindow( pOutMj->bPeng );
	m_GameClientView.m_btChi.EnableWindow( pOutMj->nChi );

	m_nGangMode= pOutMj->nGang;
	m_nChiMode= pOutMj->nChi;

	if( pOutMj->nHu || pOutMj->nGang || pOutMj->nChi || pOutMj->bPeng )
	{
		m_GameClientView.m_btGiveUp.EnableWindow( true );
		m_GameClientView.m_bAction= true;
	//	m_GameClientView.m_Timer.SetTimerUp( 2, 15, true );
	}
	else
	{
		m_GameClientView.m_bAction = false;
		m_GameClientView.m_Timer.SetPause( true );
	}
	//播放声音
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutMj->wOutMjUser)) PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));	
	
	const tagUserData *pUserData = GetUserData(pOutMj->wOutMjUser);
	if (pUserData!=NULL)
	{
		PlayPaiSound(pOutMj->byOutMj,pUserData->cbGender);	
		
	}		
	m_GameClientView.UpdateGameView( NULL );
	return true;
}

//用户摸牌
bool CGameClientDlg::OnSubTouchMj( const void *pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT( wDataSize == sizeof( CMD_S_TouchMj ) );
	if( wDataSize != sizeof( CMD_S_TouchMj ) ) return false;

	CMD_S_TouchMj *sTouchMj = ( CMD_S_TouchMj* )pBuffer;
	BYTE TouchMj = sTouchMj->byTouchMj;
	m_GameClientView.m_wCurrentUser = TransViewID( sTouchMj->wCurrentUser );
	if( GetMeChairID() == sTouchMj->wCurrentUser )
	{
		//摸牌处理
		m_GameLogic.TransMjDToI( TouchMj, &m_GameClientView.m_byHandMjs[2][HAND_MJ_MAX-1] );
		m_GameClientView.m_bPlay = true;
		m_GameClientView.m_byHandMjCount[2] += 1;

		m_GameClientView.m_btHu.EnableWindow(sTouchMj->nHu);
		m_GameClientView.m_btGang.EnableWindow(sTouchMj->nGang);
		if( sTouchMj->nHu || sTouchMj->nGang )
		{
			m_GameClientView.m_btGiveUp.EnableWindow( true );
			m_GameClientView.m_bAction= true;
		}
		//取得杠牌选择模式
		m_nGangMode= sTouchMj->nGang;
	}
	else
	{
		//非自已摸牌处理
		m_GameClientView.m_byHandMjCount[ m_GameClientView.m_wCurrentUser ] += 1;
		m_GameClientView.m_bAction= false;
	}
	m_GameClientView.m_Timer.SetTimerUp( m_GameClientView.m_wCurrentUser, 15, true );
	m_GameClientView.SubTouchMJ();
	m_GameClientView.UpdateGameView( NULL );
	return true;
}

//用户碰牌
bool CGameClientDlg::OnSubPengMj( const void *pBuffer, WORD wDataSize )
{
	//清理变量
	if( m_nChiMode || m_nGangMode)
	{
		m_nGangMode = 0;
		m_nChiMode = 0;
		m_GameClientView.m_nChiType = 0;
	}
	CMD_S_Operation *Peng = ( CMD_S_Operation* )pBuffer;
	MJ_BLOCK block;

	m_GameClientView.m_wCurrentUser = TransViewID( Peng->wActionUser );
	if( GetMeChairID() == Peng->wActionUser )
	{
		//面牌处理
		block = m_GameLogic.Peng( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2], m_byOutMj );
		m_GameLogic.AddBlock( &m_GameClientView.m_hu[2], &block );
		//成功,隐藏按扭
		m_GameClientView.HideButton();

		m_GameLogic.RMove( m_GameClientView.m_byHandMjs[2],m_GameClientView.m_byHandMjCount[2],3 );
		m_GameClientView.m_byHandMjCount[2] -= 2;
		m_GameClientView.m_bPlay = true; 
	}
	else
	{
		//成功,隐藏按扭
		m_GameClientView.HideButton();

		m_GameLogic.TransMjDToI( Peng->mjFirst, &block.first );
		block.style = (BLOCKSTYLE)Peng->blockStyle;
		m_GameLogic.AddBlock( &m_GameClientView.m_hu[ TransViewID( Peng->wActionUser ) ], &block );
		m_GameClientView.m_byHandMjCount[ TransViewID( Peng->wActionUser ) ] -= 2;
	}

	//消牌处理
	m_GameClientView.m_Timer.SetTimerUp( m_GameClientView.m_wCurrentUser, 15, true );
	//出牌区减一
	m_GameClientView.m_vcOutMjs[ TransViewID( Peng->wOutMjUser)].pop_back();
	//出牌标志无效
	m_GameClientView.m_nActionOutFlag = -1;
	m_GameClientView.UpdateGameView( NULL );
	const tagUserData *pUserData = GetUserData(Peng->wOutMjUser);
	if(pUserData->cbGender == 0)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("peng"));
		
	}
	else if(pUserData->cbGender == 1)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("peng1"));
		
	}
	return true;
}

//用户吃牌
bool CGameClientDlg::OnSubChiMj( const void *pBuffer, WORD wDataSize )
{
	CMD_S_Chi *Chi = (CMD_S_Chi *)pBuffer;
	MJ_BLOCK block;

	m_GameClientView.m_wCurrentUser =  TransViewID( Chi->wActionUser);
	if( GetMeChairID() == Chi->wActionUser )
	{
		block = m_GameLogic.Chi( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2], m_byOutMj, Chi->nChiType );
		m_GameLogic.AddBlock( &m_GameClientView.m_hu[2], &block );
		//成功,隐藏按扭
		m_GameClientView.HideButton();

		m_GameLogic.RMove( m_GameClientView.m_byHandMjs[2],m_GameClientView.m_byHandMjCount[2],3 );
		m_GameClientView.m_byHandMjCount[2] -= 2;
		m_GameClientView.m_bPlay = true; 
	}
	else
	{
		//成功,隐藏按扭
		m_GameClientView.HideButton();

		m_GameLogic.TransMjDToI( Chi->mjFirst, &block.first );
		block.style = (BLOCKSTYLE)Chi->blockStyle;
		m_GameLogic.AddBlock( &m_GameClientView.m_hu[TransViewID(Chi->wActionUser)], &block );
		m_GameClientView.m_byHandMjCount[TransViewID(Chi->wActionUser)] -= 2;
	}

	//消牌处理
	m_GameClientView.m_Timer.SetTimerUp( m_GameClientView.m_wCurrentUser, 15, true );

	//出牌区减一
	m_GameClientView.m_vcOutMjs[ TransViewID( Chi->wOutMjUser)].pop_back();
	//出牌标志无效
	m_GameClientView.m_nActionOutFlag = -1;

	m_GameClientView.UpdateGameView( NULL );
	//清理变量
	if( m_nChiMode || m_nGangMode)
	{
		m_nGangMode = 0;
		m_nChiMode = 0;
	}
	const tagUserData *pUserData = GetUserData(Chi->wOutMjUser);
	if(pUserData->cbGender == 0)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("chi"));
		
	}
	else if(pUserData->cbGender == 1)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("chi1"));
	
	}
	return true;
}

//杠牌
bool CGameClientDlg::OnSubGangMj( const void *pBuffer, WORD wDataSize )
{
	////效验数据
	//ASSERT( wDataSize== sizeof( CMD_S_Gang) );
	//if( wDataSize!= sizeof( CMD_S_Gang))
	//{
	//	MessageBox( "ASSERT,出错");
	//	return false;
	//}
	//消息处理
	CMD_S_Gang *Gang= (CMD_S_Gang *)pBuffer;

	MJ_BLOCK block;
	
	m_GameClientView.m_wCurrentUser = TransViewID( Gang->wActionUser );
	if( GetMeChairID() == Gang->wActionUser )
	{
		//成功,隐藏按扭
		m_GameClientView.HideButton();
		switch( Gang->gangType )
		{
		case 1:
			//面牌处理
			block = m_GameLogic.MGang( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2], m_byOutMj );
			m_GameLogic.AddBlock( &m_GameClientView.m_hu[2], &block );
			m_GameLogic.RMove( m_GameClientView.m_byHandMjs[2],m_GameClientView.m_byHandMjCount[2],4 );
			m_GameClientView.m_byHandMjCount[2] -= 3;
			m_GameLogic.LMove( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2] );
			m_GameClientView.m_byHandMjCount[2] -= 1;
			break;
		case 2:
			{
				BYTE GangMj;
				m_GameLogic.TransMjDToI( Gang->mjFirst, &GangMj );
				m_GameLogic.ZMGang( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2],&m_GameClientView.m_hu[2],GangMj );

				m_GameLogic.LMove( m_GameClientView.m_byHandMjs[2], m_byHandMjCount);
				m_GameClientView.m_byHandMjCount[2] -= 1;
				break;
			}
		case 4:
			{
				BYTE GangMj;
				m_GameLogic.TransMjDToI( Gang->mjFirst, &GangMj );
				block = m_GameLogic.AnGang( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2], GangMj );
				
				m_GameLogic.AddBlock( &m_GameClientView.m_hu[2], &block);
				m_GameLogic.RMove( m_GameClientView.m_byHandMjs[2],m_GameClientView.m_byHandMjCount[2],4 );
				m_GameClientView.m_byHandMjCount[2] -= 3;
				m_GameLogic.LMove( m_GameClientView.m_byHandMjs[2], m_GameClientView.m_byHandMjCount[2] );
				m_GameClientView.m_byHandMjCount[2] -= 1;
				
			break;
			}
		}
		m_GameClientView.m_bPlay = true; 
	}
	else
	{
		//成功,隐藏按扭
		m_GameClientView.HideButton();
		switch( Gang->gangType )
		{
		case 1:
			m_GameLogic.TransMjDToI( Gang->mjFirst, &block.first );
			block.style = (BLOCKSTYLE)Gang->blockStyle;
			m_GameLogic.AddBlock( &m_GameClientView.m_hu[ TransViewID( Gang->wActionUser ) ], &block );
			m_GameClientView.m_byHandMjCount[ TransViewID( Gang->wActionUser ) ] -= 3;
			break;
		case 2:
			{
				m_GameLogic.TransMjDToI( Gang->mjFirst, &block.first );
				for( int i=0; i<4; i++)
				{
					if( m_GameClientView.m_hu[ TransViewID( Gang->wActionUser ) ].block[i].first== block.first )
					{
						m_GameClientView.m_hu[ TransViewID( Gang->wActionUser ) ].block[i].style= BLOCK_GANG;
						break;
					}
				}
				m_GameClientView.m_byHandMjCount[ TransViewID( Gang->wActionUser ) ] -= 1;
			}

			break;
		case 4:
			m_GameLogic.TransMjDToI( Gang->mjFirst, &block.first );
			block.style = (BLOCKSTYLE)Gang->blockStyle;
			m_GameLogic.AddBlock( &m_GameClientView.m_hu[ TransViewID( Gang->wActionUser ) ], &block );
			m_GameClientView.m_byHandMjCount[ TransViewID( Gang->wActionUser ) ] -= 4;
			break;
		}
		//m_GameClientView.m_byHandMjCount[ TransViewID( Gang->wActionUser ) ] -= 2;
	}

	//消牌处理
	m_GameClientView.m_Timer.SetTimerUp( m_GameClientView.m_wCurrentUser, 15, true );
	//出牌区减一,,明杠才减
	if( Gang->gangType==1)
		m_GameClientView.m_vcOutMjs[ TransViewID( Gang->wOutMjUser)].pop_back();
	//出牌标志无效
	m_GameClientView.m_nActionOutFlag = -1;
	m_GameClientView.UpdateGameView( NULL );
	const tagUserData *pUserData = GetUserData(Gang->wOutMjUser);
	if(pUserData->cbGender == 0)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("gang"));
	}
	else if(pUserData->cbGender == 1)
		PlayGameSound(AfxGetInstanceHandle(),TEXT("gang1"));
	return true;
}

//杠牌选择模式
bool CGameClientDlg::OnSubSelectGang( const void *pBuffer, WORD wDataSize )
{
	CMD_S_GangMode *GangMode = (CMD_S_GangMode *)pBuffer;

	m_GameClientView.m_bAction = false;
	m_GameClientView.m_Timer.SetPause( true );
	m_GameClientView.HideButton();
	CopyMemory( &m_GameClientView.m_byGangMj, &GangMode->byMjGang, sizeof(BYTE)*3 );
	m_GameClientView.m_nGangMode = GangMode->nGangType;
	m_GameClientView.UpdateGameView( NULL );
	return true;
}
//游戏结束
bool CGameClientDlg::OnSubGameEnd( const void *pBuffer, WORD wDataSize )
{
	//效验数据
	//ASSERT( wDataSize==sizeof(CMD_S_GameEnd));
	//if(wDataSize!=sizeof(CMD_S_GameEnd)+strlen(GameEnd.strEnd)+1sizeof(CMD_S_GameEnd)+strlen(GameEnd->strEnd)+1)
	//{
	//	MessageBox( "长度不对" );
	//	return false;
	//}
	//消息处理
	CMD_S_GameEnd *pGameEnd =(CMD_S_GameEnd *)pBuffer;

	//删除定时器

	//隐藏控件
	m_GameClientView.HideButton();

	//设置积分
	for( WORD i=0; i<GAME_PLAYER; i++)
	{
		const tagUserData *pUserData = GetUserData(i);
		m_GameClientView.m_scoreView.SetGameScore(i,pUserData->szName, pGameEnd->lGameScore[i]);

		if( TransViewID(i)!=2)
		{
			for( BYTE j=0; j<14; j++ )
			{		
				if( !m_GameLogic.TransMjDToI( pGameEnd->byMjs[i][j], &m_GameClientView.m_byHandMjs[TransViewID(i)][j] ) )MessageBox( "转换出错");
			}
		}

		//if( i!=2 )
		//	m_GameLogic.TransMjDToI( pGameEnd->byMjs[i], m_GameClientView.m_byHandMjs[TransViewID(i)],14);
	}
	m_GameClientView.m_scoreView.SetGameTax(pGameEnd->lGameTax);
	m_GameClientView.m_scoreView.SetFangText(pGameEnd->strEnd);
	m_GameClientView.m_scoreView.ShowWindow( SW_SHOW );


	for(int j=0; j<4; j++)
	{
		if(  TransViewID(j)==GetMeChairID())
			continue;
		// m_GameLogic.TransMjDToI( TransViewID(j)
	}
	//m_GameClientView.AddOutMj( TransViewID( pOutMj->wOutMjUser ), m_byOutMj );
	//结束设置
	m_GameClientView.m_bEnd= true;
	m_GameClientView.m_Timer.ShowWindow( SW_HIDE);
	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));
	//设置界面
	if( IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow( SW_SHOW );
		//SetGameTimer(GetMeChairID(), IDI_START_GAME, 90;)
	}

	return true;
}

//游戏设置
LRESULT CGameClientDlg::OnGameSet( WPARAM wParam, LPARAM lParam )
{
	OnGameOptionSet();
	return 0;
}

LRESULT CGameClientDlg::OnShowMini( WPARAM wParam, LPARAM lParam )
{
	ShowWindow( SW_SHOWMINIMIZED);
	return 0;
}

//流局
bool CGameClientDlg::OnSubEndEmpty( const void *pBuffer, WORD wDataSize)
{
	CMD_S_GameEndEmpty *pEndEmpty =(CMD_S_GameEndEmpty *)pBuffer;

	//隐藏控件
	m_GameClientView.HideButton();

	//设置积分
	for( WORD i=0; i<GAME_PLAYER; i++)
	{
		//const tagUserData *pUserData = GetUserData(i);
		//m_GameClientView.m_scoreView.SetGameScore(i,pUserData->szName, pEnd->lGameScore[i]);
		if( TransViewID(i)!=2 )
		{
			for( BYTE j=0; j<14; j++ )
			{		
				if( !m_GameLogic.TransMjDToI( pEndEmpty->byMjs[i][j], &m_GameClientView.m_byHandMjs[TransViewID(i)][j] ) )MessageBox( "转换出错");
			}
		}
		//if( i!=2 )
		//	m_GameLogic.TransMjDToI( pGameEnd->byMjs[i], m_GameClientView.m_byHandMjs[TransViewID(i)],14);
	}


	for(int j=0; j<4; j++)
	{
		if(  TransViewID(j)==GetMeChairID())
			continue;
		// m_GameLogic.TransMjDToI( TransViewID(j)
	}

	//设置界面
	if( IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow( SW_SHOW );
		//SetGameTimer(GetMeChairID(), IDI_START_GAME, 90;)
	}
	//结束设置
	m_GameClientView.m_bEnd = true;
	m_GameClientView.m_bEndEmpty = true;
	m_GameClientView.m_Timer.ShowWindow( SW_HIDE );
	return true;
}
//播放麻将声音
void CGameClientDlg::PlayPaiSound(BYTE nPai,BYTE sex)
{
	if(sex == 0)
	{
		switch(nPai)
		{
		case 0x01:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("1wan"));
			break;
		case 0x02:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("2wan"));
			break;
		case 0x03:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("3wan"));
			break;
		case 0x04:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("4wan"));
			break;
		case 0x05:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("5wan"));
			break;
		case 0x06:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("6wan"));
			break;
		case 0x07:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("7wan"));
			break;
		case 0x08:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("8wan"));
			break;
		case 0x09:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("9wan"));
			break;
		case 0x11:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("1bing"));
			break;
		case 0x12:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("2bing"));
			break;
		case 0x13:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("3bing"));
			break;
		case 0x14:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("4bing"));
			break;
		case 0x15:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("5bing"));
			break;
		case 0x16:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("6bing"));
			break;
		case 0x17:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("7bing"));
			break;
		case 0x18:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("8bing"));
			break;
		case 0x19:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("9bing"));
			break;
		case 0x21:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("1tiao"));
			break;
		case 0x22:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("2tiao"));
			break;
		case 0x23:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("3tiao"));
			break;
		case 0x24:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("4tiao"));
			break;
		case 0x25:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("5tiao"));
			break;
		case 0x26:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("6tiao"));
			break;
		case 0x27:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("7tiao"));
			break;
		case 0x28:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("8tiao"));
			break;
		case 0x29:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("9tiao"));
			break;
		case 0x31:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("东feng"));
			break;
		case 0x32:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("南feng"));
			break;
		case 0x33:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("西feng"));
			break;
		case 0x34:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("北feng"));
			break;
		case 0x41:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("zhong"));
			break;
		case 0x42:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("fa"));
			break;
		case 0x43:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("bai"));
			break;			
		}
	
	}
	else if(sex == 1)
	{
		switch(nPai)
		{
		case 0x01:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("1万"));
			break;
		case 0x02:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("2万"));
			break;
		case 0x03:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("3万"));
			break;
		case 0x04:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("4万"));
			break;
		case 0x05:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("5万"));
			break;
		case 0x06:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("6万"));
			break;
		case 0x07:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("7万"));
			break;
		case 0x08:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("8万"));
			break;
		case 0x09:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("9万"));
			break;
		case 0x11:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("1饼"));
			break;
		case 0x12:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("2饼"));
			break;
		case 0x13:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("3饼"));
			break;
		case 0x14:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("4饼"));
			break;
		case 0x15:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("5饼"));
			break;
		case 0x16:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("6饼"));
			break;
		case 0x17:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("7饼"));
			break;
		case 0x18:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("8饼"));
			break;
		case 0x19:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("9饼"));
			break;
		case 0x21:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("1条"));
			break;
		case 0x22:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("2条"));
			break;
		case 0x23:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("3条"));
			break;
		case 0x24:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("4条"));
			break;
		case 0x25:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("5条"));
			break;
		case 0x26:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("6条"));
			break;
		case 0x27:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("7条"));
			break;
		case 0x28:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("8条"));
			break;
		case 0x29:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("9条"));
			break;
		case 0x31:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("东风"));
			break;
		case 0x32:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("南风"));
			break;
		case 0x33:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("西风"));
			break;
		case 0x34:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("北风"));
			break;
		case 0x41:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("中"));
			break;
		case 0x42:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("发"));
			break;
		case 0x43:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("白"));
			break;			
		}
	}
	
}