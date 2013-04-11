#ifndef GLOBAL_DEF_HEAD_FILE
#define GLOBAL_DEF_HEAD_FILE

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
//常量定义

const TCHAR	szProductName[]=TEXT("棋牌游戏空间");								//产品名字
const TCHAR szSystemRegKey[]=TEXT("HappyGame");								//注册主键
const TCHAR szStationPage[]=TEXT("127.0.0.1");						    //游戏主页

/////////////////////////////////////////////////////////////////////////////////////////
//游戏标识

//通用游戏
#define ID_PLAZA						0									//大厅

#define ID_SHOWHAND					1								  //梭哈
#define ID_CHINA_CHESS			2						    	//象棋
#define ID_WEIQI						3									//围棋
#define ID_HZMJ		4	//	杭州麻将
#define ID_CHESS						5									//国际象棋
#define ID_LAND							6									//斗地主
#define ID_SPARROW					7							  	//麻将
#define ID_RUN_FAST					8							  	//跑得快
#define ID_HOE							9								//锄大地

//测试游戏
#define ID_BACK_JACK					10									//黑杰克
#define ID_UPGRADE						11									//拖拉机
#define ID_FOURENSIGN					12									//四国军棋
#define ID_THREELEAF					13									//诈金花
#define ID_UNCOVERPIG					14									//拱猪
#define ID_PAOPAI							15									//跑牌

#define ID_FiveChess		16	//	五子棋
#define ID_BWChess		17	//	黑白棋
#define ID_CrazyLand		18	//	疯狂斗地主
#define ID_3V1Card		19	//	三打一
#define ID_510KCard		20	//	510K
#define ID_SKCard		21	//	双扣
#define ID_XZMJ		22	//	忻州麻将
#define ID_SCMJ		23	//	四川麻将
#define ID_2RenMJ		24	//	2人麻将
#define ID_HBMJ		25	//	湖北麻将
#define ID_DQMJ		26	//	大庆麻将
#define ID_SHMJ		27	//	上海麻将
#define ID_TWMJ		28	//	台湾麻将
#define ID_CSMJ		29	//	长沙麻将
#define ID_LLK		30	//	连连看
#define ID_DDP		31	//	对对碰
#define ID_ELS		32	//	疯狂俄罗斯
#define ID_MaZu		33	//	玛祖
#define ID_WaJin		34	//	挖金子
#define ID_PaoPao		35	//	泡泡龙
#define ID_Horse		36	//	赛马
#define ID_Down100		37	//	地狱一百层
#define ID_GXDSCard		38	//	广西独食
#define ID_XZUupdate    39	//	忻州拖拉机	XZUpdate.exe
#define ID_GAME_XN	40	//	掀牛	GS_TS_LiftOx.exe
#define ID_GAME_81	41	//	守把一	DEightOne.exe
#define ID_GAME_BRIDGE	42	//	桥牌	Bridge.exe
#define ID_GAME_7KING	43	//	七王	SevenThree.exe
#define ID_GAME_NINE	44	//	灭十	CNine.exe
#define ID_GAME_CLOSE_CARD	45	//	关牌	NT_Close.exe
#define ID_GAME_GJ	46	//	够级	GJ.exe
#define ID_GAME_DD	47	//	对调	CF_DD.exe
#define ID_GAME_GOLD_COW	48	//	斗牛	GoldCow.exe
#define ID_GAME_LAND_LZ	49	//	赖子斗地主	land_laiziOne.exe
#define ID_GAME_240	50	//	240分	TwoForty.exe
#define ID_GAME_BH	51	//	保皇	BH.exe
#define ID_GAME_SDMJ	52	//	山东麻将	SD_Mj.exe
#define ID_GAME_QZ_MJ		53	//	泉州麻将
#define ID_GAME_BIG_2		54	//	大老二
#define ID_GAME_QZ_80		55	//	泉州80分
#define ID_GAME_QZ_CDP		56	//	泉州炒地皮
#define ID_GAME_QZ_YC		57	//	泉州硬吃	

#define ID_GDMJ						95 //广东麻将
#define ID_TSWAKEN					96 //天水挖坑
#define ID_TSMJ						97//天水麻将
#define ID_WAKEN					98//挖坑
#define ID_PKJ						99									//扑克机
#define ID_IQGame	101	//IQGame
/////////////////////////////////////////////////////////////////////////////////////////
//宏定义

#define MAX_CHAIR						8									//最大游戏者
#define MAX_CHAT_LEN					512									//聊天长度
#define INVALID_TABLE					((WORD)(-1))						//无效桌子号
#define INVALID_CHAIR					((WORD)(-1))						//无效椅子号
#define MAIN_DLG_CLASSNAME				TEXT("114PKGamePlaza")				//窗口注册名

//端口定义
#define PORT_LOGON_SERVER				9000								//登陆服务器
#define PORT_CENTER_SERVER				9010								//中心服务器

//网络数据定义
#define SOCKET_VER						68									//数据包版本
#define SOCKET_PACKAGE					2046								//最大网络包
#define SOCKET_BUFFER					(sizeof(CMD_Head)+SOCKET_PACKAGE+2*sizeof(DWORD))

/////////////////////////////////////////////////////////////////////////////////////////

//数据包结构信息
struct CMD_Info
{
	WORD								wDataSize;							//数据大小
	BYTE								cbCheckCode;						//效验字段
	BYTE								cbMessageVer;						//版本标识
};

//数据包命令信息
struct CMD_Command
{
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//数据包传递包头
struct CMD_Head
{
	CMD_Info							CmdInfo;							//基础结构
	CMD_Command							CommandInfo;						//命令信息
};

//网络数据包缓冲
struct CMD_Buffer
{
	CMD_Head							Head;								//数据包头
	BYTE								cbBuffer[SOCKET_PACKAGE];			//数据缓冲
};

//内核命令码
#define MDM_KN_COMMAND					0									//内核命令
#define SUB_KN_DETECT_SOCKET			1									//检测命令

//检测结构信息
struct CMD_KN_DetectSocket
{
	DWORD								dwSendTickCount;					//发送时间
	DWORD								dwRecvTickCount;					//接收时间
};

/////////////////////////////////////////////////////////////////////////////////////////

//IPC 数据定义
#define IPC_VER							0x0001								//IPC 版本
#define IPC_IDENTIFIER					0x0001								//标识号码
#define IPC_PACKAGE						4096								//最大 IPC 包
#define IPC_BUFFER						(sizeof(IPC_Head)+IPC_PACKAGE)		//缓冲长度

//IPC 数据包头
struct IPC_Head
{
	WORD								wVersion;							//IPC 版本
	WORD								wDataSize;							//数据大小
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//IPC 缓冲结构
struct IPC_Buffer
{
	IPC_Head							Head;								//数据包头
	BYTE								cbBuffer[IPC_PACKAGE];				//数据缓冲
};

//////////////////////////////////////////////////////////////////////////

//长度宏定义
#define TYPE_LEN						32									//种类长度
#define KIND_LEN						32									//类型长度
#define STATION_LEN						32									//站点长度
#define SERVER_LEN						32									//房间长度
#define MODULE_LEN						32									//进程长度

//游戏类型
#define GAME_GENRE_SCORE				0x0001								//点值类型
#define GAME_GENRE_GOLD					0x0002								//金币类型
#define GAME_GENRE_MATCH				0x0004								//比赛类型
#define GAME_GENRE_EDUCATE				0x0008								//训练类型

//游戏类型结构
struct tagGameType
{
	WORD								wSortID;							//排序号码
	WORD								wTypeID;							//种类号码
	TCHAR								szTypeName[TYPE_LEN];				//种类名字
};

//游戏名称结构
struct tagGameKind
{
	WORD								wSortID;							//排序号码
	WORD								wTypeID;							//类型号码
	WORD								wKindID;							//名称号码
	DWORD								dwMaxVersion;						//最新版本
	DWORD								dwOnLineCount;						//在线数目
	TCHAR								szKindName[KIND_LEN];				//游戏名字
	TCHAR								szProcessName[MODULE_LEN];			//进程名字
};

//游戏进程结构
struct tagGameProcess
{
	WORD								wSortID;							//排序号码
	WORD								wTypeID;							//类型号码
	WORD								wKindID;							//名称号码
	WORD								wServerPort;						//房间端口
	DWORD								dwServerAddr;						//房间地址
	DWORD								dwMaxVersion;						//最新版本
	DWORD								dwOnLineCount;						//在线数目
	TCHAR								szKindName[KIND_LEN];				//游戏名字
	TCHAR								szProcessName[MODULE_LEN];			//进程名字
};

//游戏站点结构
struct tagGameStation
{
	WORD								wSortID;							//排序号码
	WORD								wKindID;							//名称号码
	WORD								wJoinID;							//挂接号码
	WORD								wStationID;							//站点号码
	TCHAR								szStationName[STATION_LEN];			//站点名称
};

//游戏房间列表结构
struct tagGameServer
{
	WORD								wSortID;							//排序号码
	WORD								wKindID;							//名称号码
	WORD								wServerID;							//房间号码
	WORD								wStationID;							//站点号码
	WORD								wServerPort;						//房间端口
	DWORD								dwServerAddr;						//房间地址
	DWORD								dwOnLineCount;						//在线人数
	TCHAR								szServerName[SERVER_LEN];			//房间名称
};

//////////////////////////////////////////////////////////////////////////

//用户状态定义
#define US_NULL							0x00								//没有状态
#define US_FREE							0x01								//站立状态
#define US_SIT							0x02								//坐下状态
#define US_READY						0x03								//同意状态
#define US_LOOKON						0x04								//旁观状态
#define US_PLAY							0x05								//游戏状态
#define US_OFFLINE						0x06								//断线状态

//长度宏定义
#define NAME_LEN						32									//名字长度
#define PASS_LEN						33									//密码长度
#define GROUP_LEN						32									//社团长度

//用户积分信息
struct tagUserScore
{
	LONG								lGold;								//用户金币
	LONG								lScore;								//用户分数
	LONG								lWinCount;							//胜利盘数
	LONG								lLostCount;							//失败盘数
	LONG								lDrawCount;							//和局盘数
	LONG								lFleeCount;							//断线数目
	LONG								lExperience;						//用户经验
};

//用户状态信息
struct tagUserStatus
{
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	WORD								wNetDelay;							//网络延时
	BYTE								cbUserStatus;						//用户状态
};

//用户基本信息结构
struct tagUserInfoHead
{
	//用户属性
	WORD								wFaceID;							//头像索引
	BYTE								cbGender;							//用户性别  0 =woman, 1 =man
	BYTE								cbMember;							//会员等级
	DWORD								dwUserID;							//用户 I D
	DWORD								dwGroupID;							//社团索引
	DWORD								dwUserRight;						//用户等级
	DWORD								dwMasterRight;						//管理权限

	//用户状态
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	WORD								wNetDelay;							//网络延时
	BYTE								cbUserStatus;						//用户状态

	//用户积分
	tagUserScore						UserScoreInfo;						//积分信息
};

//用户信息结构
struct tagUserData
{
	//用户属性
	WORD								wFaceID;							//头像索引
	BYTE								cbGender;							//用户性别  0 =woman, 1 =man
	BYTE								cbMember;							//会员等级
	DWORD								dwUserID;							//用户 I D
	DWORD								dwGroupID;							//社团索引
	DWORD								dwUserRight;						//用户等级
	DWORD								dwMasterRight;						//管理权限
	TCHAR								szName[NAME_LEN];					//用户名字
	TCHAR								szGroupName[GROUP_LEN];				//社团名字

	//用户积分
	LONG								lGold;								//用户金币
	LONG								lScore;								//用户分数
	LONG								lWinCount;							//胜利盘数
	LONG								lLostCount;							//失败盘数
	LONG								lDrawCount;							//和局盘数
	LONG								lFleeCount;							//断线数目
	LONG								lExperience;						//用户经验

	//用户状态
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	WORD								wNetDelay;							//网络延时
	BYTE								cbUserStatus;						//用户状态

	//其他信息
	BYTE								cbCompanion;						//用户关系
};

//////////////////////////////////////////////////////////////////////////

//机器序列号结构
struct tagClientSerial
{
	DWORD								dwComputerID[3];					//机器序列
};

//配置缓冲结构
struct tagOptionBuffer
{
	BYTE								cbBufferLen;						//数据长度
	BYTE								cbOptionBuf[32];					//设置缓冲
};

/////////////////////////////////////////////////////////////////////////////////////////
/*
#define MAX_SCENE_NUM	10//最大场景数
#define ONE_SCENE_MAX_TEXT_NUM	15//每个场景TEXT最大数

//声音文字
typedef struct _SOUND_TEXT_ITEM
{
	char showInfo[50];
	char playSound[50];
}
SOUND_TEXT_ITEM;

//场景
typedef struct _SCENE_ITEM
{
	int	 useTextInt;//已经使用的个数
	char TypeName[50];
	SOUND_TEXT_ITEM TextList[ONE_SCENE_MAX_TEXT_NUM];
}
SCENE_ITEM;
*/

#endif