#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define TAX_MIN_NUM	10 //要抽税的最少点
#define KIND_ID						ID_GAME_QZ_MJ									//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("泉州麻将")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_MJ_FREE					GS_FREE								//空闲状态
#define GS_MJ_BEFOREPLAY			(GS_PLAYING+1)						//游戏前状态
#define GS_MJ_PLAY					(GS_PLAYING+2)						//游戏状态



//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心麻将
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//游牌状态
enum enGoldSwim
{
	GoldSwim_None,					//无游牌状态
	GoldSwim_Single,				//游金状态
	GoldSwim_Double,				//双游状态
	GoldSwim_Three					//三游状态
};

//留牌张数
#define	MAX_LEFT_COUNT				16									//留牌张数

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送麻将
#define SUB_S_OPERATE_NOTIFY		103									//操作提示
#define SUB_S_OPERATE_RESULT		104									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_OPENDOOR				107									//开门命令
#define SUB_S_OPENGOLD				108									//开金命令
#define SUB_S_REPAIRFLOWER			109									//补花命令
#define SUB_S_END_FLOWER			110									//补花结束
#define SUB_S_GOLD_SWIM				111									//游牌状态

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
	CMD_WeaveItem					WeaveItemArray[4][5];				//组合麻将
};

//开门状态
struct CMD_S_OpenDoor
{
	WORD							wDiceCount;							//骰子大小
	BYTE							cbDoorCard;							//门牌大小
	WORD							wOpenGoldUser;						//开金玩家
};

//开金状态
struct CMD_S_OpenGold
{
	WORD							wDiceCount;							//骰子大小
	BYTE							cbGoldCard[4];						//金牌麻将
	BYTE							cbKeyGoldCard;						//关键麻将
	BYTE							cbFlowerCard[8];					//花牌麻将
	
};

//补花状态
struct CMD_S_RepairFlower
{

	WORD							wChairID;							//补花玩家
	BYTE							cbHandFlower;						//手中花牌
	BYTE							cbCardData;							//补花麻将
	BYTE							cbCardPos;							//补牌位置

};

//游戏开始 
struct CMD_S_GameStart
{	
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户	
	BYTE							cbCardData[17];						//麻将列表
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
	bool							bRepairFlower;						//补花动作	
	bool							bGameSatusPlay;						//游戏状态中
};

//操作提示
struct CMD_S_OperateNotify
{
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
	bool							wWinner[4];							//赢家
	LONG							lGameScore[4];						//游戏积分
	WORD							wChiHuKind[4];						//胡牌类型
	BYTE							cbCardCount[4];						//麻将数目
	BYTE							cbCardData[4][17];					//麻将数据
	char							strEnd[100];
	int								lGameTax;
	bool							mbChui[4];							//捶牌用户
	BYTE							bFlowerCount[4];					//花朵个数
	BYTE							bGoldCount[4];						//金牌个数
	BYTE							bGangCount[4];						//杠牌翻数 
	BYTE							bGoOnBankerTime;					//连庄次数
	bool							bAllSidesCard[4];					//东南西北
	bool							bAllSeasonCard[4];					//春夏秋冬
	bool							bAllPlantCard[4];					//梅兰竹菊
	LONG							lCellScore;							//底分大小
};

//游牌状态
struct CMD_S_GoldSwim
{
	enGoldSwim						GoldSwim[4];						//游牌状态	
};


//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			2									//操作麻将
#define SUB_C_OPENDOOR				3									//开门命令
#define SUB_C_OPENGOLD				4									//开金命令
#define SUB_C_REPAIRFLOWER			5									//补花命令

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

//补花命令
struct CMD_C_RepairFlower
{
	WORD							wChairID;							//补花玩家
	BYTE							cbCurrentCard;						//当前麻将
};


//////////////////////////////////////////////////////////////////////////

#endif