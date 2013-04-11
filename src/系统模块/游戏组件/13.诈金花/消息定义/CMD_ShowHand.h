#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

#include "..\..\开发库\include\GlobalDef.h"


//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define TAX_MIN_NUM	10 //要抽税的最少点

#define KIND_ID							ID_THREELEAF								//游戏 I D
#define GAME_PLAYER						6									//游戏人数
#define GAME_NAME						TEXT("诈金花游戏")					//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

//////////////////////////////////////////////////////////////////////////
//命令码定义

#define SUB_C_ADD_GOLD					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃跟注
#define SUB_C_KAIPAI	                3                                   //开牌                      //add
#define SUB_C_KANPAI	                4                                   //看牌                      //add
#define SUB_C_NOKANPAI	                5                                   //不看牌                    //add

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_GOLD					101									//加注结果
#define SUB_S_GIVE_UP					102									//放弃跟注
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束
#define SUB_S_KAIPAI	                105                                 //开牌                      //add
#define SUB_S_KANPAI	                106                                 //看牌                      //add
#define SUB_S_OVER	                    107                                                             //add

//////////////////////////////////////////////////////////////////////////
//消息结构体

//游戏状态
struct CMD_S_StatusFree
{
	LONG								dwBasicGold;						//基础金币
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								bPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE								bTableCardCount[GAME_PLAYER];		//扑克数目
	BYTE								bTableCardArray[GAME_PLAYER][3];	//扑克数组
	LONG								lMaxGold;							//最大下注
	LONG								lBasicGold;							//基础金币
	LONG								lTurnMaxGold;						//最大下注
	LONG								lTurnBasicGold;						//最少下注
	LONG								lTableGold[2*GAME_PLAYER];			//桌面金币
};

//游戏开始
struct CMD_S_GameStart
{
	bool                                m_Begin;                             //新一轮开始标记
	WORD				 				wCurrentUser;						//当前玩家
	LONG								lMaxGold;							//最大下注
	LONG								lTurnMaxGold;						//最大下注
	LONG								lTurnBasicGold;						//最少下注
	BYTE								bCardData[GAME_PLAYER][3];			//用户扑克    //add 每人手上有3张牌
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wUserChairID;						//放弃用户
};

//用户加注
struct CMD_C_AddGold
{
	bool								m_Begin;                            //add
	LONG								lGold;								//加注数目
	bool                                IsKanPai;                           //是否已经看牌按钮  //add
};

//加注结果
struct CMD_S_AddGold
{
	WORD								wLastChairID;						//上一用户
	WORD								wCurrentUser;						//当前用户
	LONG								lLastAddGold;						//加注数目
	LONG								lCurrentLessGold;					//最少加注
};

//发牌数据包
struct CMD_R_SendCard
{
	LONG								lMaxGold;							//最大下注
	WORD								wCurrentUser;						//当前用户
	BYTE								bUserCard[GAME_PLAYER];				//用户扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG								lTax;								//游戏税收
	LONG								lGameGold[GAME_PLAYER];				//游戏得分
	BYTE								bUserCard[GAME_PLAYER];				//用户扑克
};

//////////////////////////////////////////////////////////////////////////

#endif