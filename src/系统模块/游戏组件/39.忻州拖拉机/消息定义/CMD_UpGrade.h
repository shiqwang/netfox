#ifndef CMD_UPGRADE_HEAD_FILE
#define CMD_UPGRADE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define TAX_MIN_NUM	10 //要抽税的最少点
#define KIND_ID						ID_XZUupdate							//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("忻州拖拉机")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

//游戏状态
#define GS_UG_FREE					GS_FREE								//空闲状态
#define GS_UG_CALL					(GS_PLAYING+1)						//叫牌状态
#define GS_UG_BACK					(GS_PLAYING+2)						//留底状态
#define GS_UG_PLAY					(GS_PLAYING+3)						//游戏状态
#define GS_UG_WAIT					(GS_PLAYING+4)						//等待状态

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_SEND_CARD				101									//发送扑克
#define SUB_S_CALL_CARD				102									//用户叫牌
#define SUB_S_SEND_CONCEAL			103									//底牌扑克
#define SUB_S_GAME_PLAY				104									//游戏开始
#define SUB_S_OUT_CARD				105									//用户出牌
#define SUB_S_THROW_RESULT			106									//甩牌结果
#define SUB_S_TURN_BALANCE			107									//一轮统计
#define SUB_S_GAME_END				108									//游戏结束

//显示其中一底牌，如果没有人叫主
#define SUB_S_AUTO_CHOU_CARD				109									//自动抽牌

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//叫牌状态
struct CMD_S_StatusCall
{
	//游戏变量
	WORD							wBankerUser;						//庄家用户
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//发牌变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表

	//叫牌信息
	BYTE							cbComplete;							//完成标志
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
};

//留底状态
struct CMD_S_StatusBack
{
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbMainColor;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户

	//用户变量
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//扑克变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[60];						//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbMainColor;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//用户变量
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	WORD							wFirstOutUser;						//出牌用户

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户

	//扑克变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	//出牌变量
	BYTE							cbOutCardCount[4];					//出牌数目
	BYTE							cbOutCardData[4][52];				//出牌列表

	//得分变量
	BYTE							cbScoreCardCount;					//扑克数目
	BYTE							cbScoreCardData[48];				//得分扑克
};

//等待状态
struct CMD_S_StatusWait
{
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//用户变量
	WORD							wBankerUser;						//庄家用户
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家用户
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表
};

//用户叫牌
struct CMD_S_CallCard
{
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
};

//底牌扑克
struct CMD_S_SendConceal
{
	BYTE							cbMainColor;						//主牌花色数值
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	BYTE		isSysChou;//是否系统自动选择牌 0 =no, 1=yes
	BYTE		cbChouCard;//选择的牌
	WORD							wCallCardUser;						//叫牌用户
	BYTE							cbMainValue;						//主牌数值

};

//系统自动抽底牌
struct CMD_S_AutoChouCard
{
	BYTE							cbCallCard;							//抽牌扑克
	BYTE							cbCallCount;						//抽牌数目
};

//游戏开始
struct CMD_S_GamePlay
{
	WORD							wCurrentUser;						//当前用户
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//扑克数目
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[52];						//扑克列表
};

//甩牌结果
struct CMD_S_ThrowResult
{
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbThrowCardCount;					//扑克数目
	BYTE							cbResultCardCount;					//扑克数目
	BYTE							cbCardDataArray[104];				//扑克数组
};

//一轮统计
struct CMD_TurnBalance
{
	WORD							wTurnWinner;						//一轮胜者
	WORD				 			wCurrentUser;						//当前玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lScore[4];							//用户得分
	WORD							wGameScore;							//游戏得分
	WORD							wConcealTime;						//扣底倍数
	WORD							wConcealScore;						//底牌积分
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	LONG							lGameTax;							//游戏税收
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_CALL_CARD				1									//用户叫牌
#define SUB_C_CALL_FINISH			2									//叫牌完成
#define SUB_C_CONCEAL_CARD			3									//底牌扑克
#define SUB_C_OUT_CARD				4									//用户出牌

//用户叫牌
struct CMD_C_CallCard
{
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
};

//底牌扑克
struct CMD_C_ConcealCard
{
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[52];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif