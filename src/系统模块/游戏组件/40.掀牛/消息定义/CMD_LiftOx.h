#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						ID_GAME_XN							//游戏 I D
#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("掀牛")						//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_SCORE					GS_PLAYING							//叫分状态
#define GS_WK_PLAYING				GS_PLAYING+1						//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_USER_COUPAI			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
#define SUB_S_USER_XIANPAI			106									//掀牌命令
#define	SUB_S_USER_QIANGCI			107									//抢刺
#define	SUB_S_USER_GIVEUP_QIANGCI	108									//放弃抢刺

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusScore
{
	BYTE							bLandScore;							//地主分数
	LONG							lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[17];						//手上扑克
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wLandUser;							//坑主玩家
	WORD							wBombTime;							//炸弹倍数
	LONG							lBaseScore;							//基础积分
	BYTE							bLandScore;							//地主分数
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[16];						//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[16];					//出牌列表
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wBankUser;						//当前玩家
	BYTE							bCardData[16];						//扑克列表
};

//用户叫分
struct CMD_S_LandScore
{
	WORD							bLandUser;							//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bLandScore;							//上次叫分
	BYTE							bCurrentScore;						//当前叫分
};
//用户扣牌
struct CMD_S_User_CouPai
{
	WORD							wCouPaiUserID;						//扣牌玩家
	WORD							wCurrentUser;						//当前玩家
};
//用户掀牌
struct CMD_S_User_XianPai
{
	WORD							wXianPaiUserID;						//掀牌玩家
	WORD							wCurrentUser;						//当前玩家
};
//用户抢刺
struct CMD_S_User_QiangCi
{
	WORD							wQiangCiUserID;						//抢刺玩家
};
//放弃抢刺
struct CMD_S_GiveUp_QiangCi
{
	WORD							wGiveUpQiangCiUserID;				//放弃抢刺
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wBankUser;							//地主玩家
	//BYTE							bLandScore;							//地主分数
	//WORD				 			wCurrentUser;						//当前玩家
	//BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[16];						//扑克列表
};

//用户出牌
struct CMD_S_OutCard
{
	bool							bNewTurn;							//一轮开始
	bool							bMostCard;							//最大
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[16];						//扑克列表
	BYTE							bZhanPaiData[16];					//一轮点牌列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bZhanpaiUser;						//占牌用户
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bZhanPaiData[16];					//一轮点牌列表
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

#define SUB_C_COUPAI				1									//用户抢刺
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_XIANPAI				4									//用户掀牌
#define SUB_C_QIANGCI				5									//用户抢刺
#define SUB_C_GIVEUP_QIANGCI		6									//放弃抢刺

////扣牌结构
//struct CMD_C_CouPai
//{
//	BYTE							bLandScore;							//地主分数
//};
//掀牌结构
struct CMD_C_XianPai
{
	
};
//抢刺结构
struct CMD_C_QiangCi
{
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[16];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif