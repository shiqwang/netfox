#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						ID_GAME_81								//游戏 I D
#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("手把一")						//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_PLAYING				GS_PLAYING+1						//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
#define	SUB_S_EIGHT_ONE				106									//八一牌

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusScore
{
	LONG							lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[16];						//手上扑克
};

//游戏状态
struct CMD_S_StatusPlay
{
//	WORD							wBombTime;							//炸弹倍数
	LONG							lBaseScore;							//基础积分
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[16];						//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[16];					//出牌列表
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[16];						//扑克列表
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[16];						//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};

//报八一
struct CMD_S_EightOne
{
	WORD							wEightOneUser;						//叫八一的玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	LONG							lGameScore[3];						//游戏积分
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bCardData[48];						//扑克列表 
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

//#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌

////用户叫分
//struct CMD_C_LandScore
//{
//	BYTE							bLandScore;							//地主分数
//};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[16];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif