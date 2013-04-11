#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define TAX_MIN_NUM	10 //要抽税的最少点

#define KIND_ID							ID_GDMJ //ID_GD_MJ							//游戏 I D
#define GAME_PLAYER						4									//游戏人数
#define GAME_NAME						TEXT("广东麻将游戏")						//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH|GAME_GENRE_SCORE)	//游戏类型


//游戏状态
#define GS_WK_FREE						GS_FREE								//等待开始
#define GS_WK_PLAYING					GS_PLAYING							//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_SEND_MJ					100									//发牌
#define SUB_S_GAME_START				101									//游戏开始
#define	SUB_S_ADDFLOWER_MJ				102									//用户补花
#define SUB_S_HU_MJ						103									//用户胡牌
#define SUB_S_TING_MJ					104									//用户听牌
#define SUB_S_GANG_MJ					105									//用户杠牌
#define SUB_S_PENG_MJ					106									//用户碰牌
#define SUB_S_CHI_MJ					107									//用户吃牌
#define SUB_S_GIVEUP					108									//用户放弃
#define SUB_S_OUT_MJ					109									//用户出牌
#define SUB_S_TOUCH_MJ					110									//用户摸牌
#define SUB_S_GAME_END					111									//游戏结束
#define SUB_S_SELECT_GANG				112									//杠牌选择模式
#define SUB_S_SELECT_CHI				113									//吃牌选择模式
#define SUB_S_END_EMPTY					114									//流局
//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_ADDFLOWER_MJ				1									//用户补花
#define SUB_C_HU_MJ						2									//用户胡牌
#define SUB_C_TING_MJ					3									//用户听牌
#define SUB_C_GANG_MJ					4									//用户杠牌
#define SUB_C_PENG_MJ					5									//用户杠牌
#define SUB_C_CHI_MJ					6									//用户碰牌
#define SUB_C_GIVEUP					7									//用户吃牌
#define SUB_C_OUT_MJ					8									//用户放弃
#define SUB_C_TOUCH_MJ					9									//用户摸牌
#define SUB_C_SELECT_GANG				10									//用户选择杠牌
#define SUB_C_SELECT_CHI				11									//用户选择吃牌

//////////////////////////////////////////////////////////////////////////

#define	HAND_MJ_MAX						14

#define ORG_MJ_MAX						136

#define BASE_SCORE						1									//底分
//游戏状态--空闲
struct CMD_S_StatusFree
{
	LONG 							lBaseScore;								//基础积分
	LONG							lBaseGold;								//基础金币
};

//游戏状态--进行
struct CMD_S_StatusPlay
{
	LONG							lBaseScore;								//基础积分
	WORD							wCurrentUser;							//当前玩家
	WORD							wLastOutUser;							//上轮出牌玩家
	BYTE							byOutMj;								//出牌
	BYTE							byMjs[14];								//手上麻将
};

//发送麻将
struct CMD_S_SendMj
{
	BYTE							byMjs[14];								//所发的麻将牌
	BYTE							byMjCount;								//麻将数目
	WORD							wBankerUser;							//当前庄家
	int								nHu;									//是否胡牌
	int								nGang;									//是否暗杠
	BYTE							byStartMjPos;							//开始摸牌子
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;							//庄家
	WORD							wQuanFeng;								//圈风
	int								nSartNumber1;							//骰子数
	int								nSartNumber2;							//骰子数
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;								//游戏税收
	LONG							lGameScore[4];							//游戏积分
	char							strEnd[300];							//番数结算
	BYTE							byMjs[4][14];							//手上麻将
	//BYTE							byMjCount[4];							//麻将数目
	//BYTE							byMj[ORG_MJ_MAX];						//麻将列表
};
struct CMD_S_GameEndEmpty
{
	BYTE							byMjs[4][14];							//麻将牌
};
//用户出牌
struct CMD_S_OutMj
{
	BYTE							byOutMj;								//打出的牌
	//WORD							wCurrentUser;							//当前玩家
	WORD							wOutMjUser;								//出牌玩家
	int								nHu;									//能否胡
	int								nGang;									//能否杠
	bool							bPeng;									//能否碰
	int								nChi;									//能否吃,返回吃牌信息，0-不成功,1-**@, 2-*@*, 3-@@*
	//int								nRight;								//权限,5-胡,3-杠,2-碰,1-吃
};

//用户转换
struct CMD_S_TransUser
{

	WORD							wCurrentUser;
};

//摸牌
struct CMD_S_TouchMj
{
	BYTE							byTouchMj;								//摸牌
	WORD							wCurrentUser;							//当前玩家
	int								nHu;									//是否能胡牌
	int								nGang;									//是否自摸杠
};

//杠牌选择
struct CMD_S_GangMode
{
	BYTE							byMjGang[3];							//可杠的牌
	int								nGangType;								//杠牌类型
};
//杠牌
struct CMD_S_Gang
{
	WORD							wActionUser;							//操作玩家
	WORD							wOutMjUser;								//出牌玩家
	BYTE							mjFirst;								//胡牌块着张
	int								blockStyle;								//类型
	int								gangType;								//杠牌类型,1-明,2-补,3-暗
};
//吃牌
struct CMD_S_Chi
{
	WORD							wActionUser;							//操作玩家
	WORD							wOutMjUser;								//出牌玩家
	BYTE							mjFirst;								//胡牌块着张
	int								blockStyle;								//类型
	int								nChiType;								//吃牌类型,0-无,1-@@*, 2-@*@, 4-*@@
};
//吃牌选择
struct CMD_S_ChiMode
{
	int								nChiType;								//吃牌类型
};
//碰牌
struct CMD_S_Operation
{
	WORD							wActionUser;							//操作玩家
	WORD							wOutMjUser;								//出牌玩家
	BYTE							mjFirst;								//胡牌块着张
	int								blockStyle;								//类型
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//客户端命令结构
//用户操作
struct CMD_C_Operation
{
	WORD							wCurrentUser;							//当前用户
	WORD							wLastUser;								//上次出牌用户
	int								nRight;									//权限,6-胡,4-杠,3-碰,2-吃, 0-放弃			
};

//吃操作
struct CMD_C_Chi
{
	int								nRight;									//权限
	int								nChiType;								//吃牌类型,0-无,1-@@*, 2-@*@, 3-*@@
};

//杠操作
struct CMD_C_Gang
{
	int								nRight;									//权限
	int								nGangType;								//杠牌类型,0-无, 1-明杠,2-补杠, 4-暗杠
//	bool							bGangeMode;								//是否进入杠牌选择模式
};

//杠牌选择
struct CMD_C_SelectGang
{
	int								nGangType;								//杠牌类型
	BYTE							byGangMj;								//所杠的牌
};
//胡操作
struct CMD_C_Hu
{
	int								nRight;									//胡牌权限
};
#endif