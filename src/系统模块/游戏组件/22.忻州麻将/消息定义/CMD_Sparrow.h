#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						ID_XZMJ								//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("忻州麻将游戏")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH)	//游戏类型

//游戏状态
#define GS_MJ_FREE					GS_FREE								//空闲状态
#define GS_MJ_PLAY					(GS_PLAYING+1)						//游戏状态
#define GS_MJ_HAI_DI				(GS_PLAYING+2)						//海底状态

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心麻将
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送麻将
#define SUB_S_OPERATE_NOTIFY		103									//操作提示
#define SUB_S_OPERATE_RESULT		104									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_TING_CARD				107									//听牌消息

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lCellScore;							//单元积分
	WORD							wSiceCount;							//骰子点数
	WORD							wSiceCount2;							//骰子2点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//状态变量
	BYTE							cbActionCard;						//动作麻将
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbLeftCardCount;					//剩余数目

	//出牌信息
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌麻将
	BYTE							cbDiscardCount[4];					//丢弃数目
	BYTE							cbDiscardCard[4][55];				//丢弃记录

	//麻将数据
	BYTE							cbCardCount;						//麻将数目
	BYTE							cbCardData[14];						//麻将列表

	//组合麻将
	BYTE							cbWeaveCount[4];					//组合数目
	CMD_WeaveItem					WeaveItemArray[4][4];				//组合麻将
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;							//骰子点数
	WORD							wSiceCount2;							//骰子2点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbUserAction;						//用户动作
	BYTE							cbCardData[14];						//麻将列表
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌麻将
};

//发送麻将
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//麻将数据
	BYTE							cbActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
};

//操作提示
struct CMD_S_OperateNotify
{
	WORD							wOperateUser;						//出牌用户

	WORD							wResumeUser;						//还原用户
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作麻将
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作麻将
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbChiHuCard;						//吃胡麻将
	WORD							wProvideUser;						//点炮用户
	LONG							lGameScore[4];						//游戏积分
	WORD							wChiHuKind[4];						//胡牌类型
	BYTE							cbCardCount[4];						//麻将数目
	BYTE							cbCardData[4][14];					//麻将数据

//	BYTE                            cbTingFlag[GAME_PLAYER];//听牌状态

/*
砍  和：1点（吊牌也属于砍和）
七小对：1点（七小对也属砍胡，不累加）
缺一门：1点
清一色：5点
一条龙：5点
*/
//	BYTE							cbHeType[5];//胡牌类型 
};

//用户听牌
struct CMD_S_GameTingCard
{
	WORD							wTingCardUser;						//听牌用户
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			2									//操作麻将
#define SUB_C_TING_CARD			    3								//麻将 听牌

//TING牌命令
struct CMD_C_TingCard
{
	BYTE							cbCardData;							//麻将数据
};

//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//麻将数据
};

//操作命令
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作麻将
};

//////////////////////////////////////////////////////////////////////////

#endif