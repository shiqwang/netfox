//比较结果 A=B,A<B,A>B  排前面的大AB  A>B
enum COMP_RESULT_TYPE{COMP_RESULT_A_B,COMP_RESULT_BA, COMP_RESULT_AB};

struct CARDS{
	int iColor;//color
	int iIndex;//num
	int iState;//status
	int iValue;//value
};

enum PLAYERS{
	SOUTH,   // 0
	EAST,    // 1
	NORTH,
	WEST
};

//牌花色
enum CARDS_COLOR{
	SPADE = 0,			//黑桃
	HEART = 1,			//红心
	DIAMOND = 2,		//方块
	CLUB = 3			//梅花
};

//梅花 78，方块 79，红桃 80，黑桃 81
//81-0,1,2,3

//牌的状态
enum CARDS_STATE {
	NOT_AVAILABLE = -1,
	AT_HAND = 0,
	CAN_PULL_OUT = 1,     //可以出队
	PULL_OUT = 2,         //
	SHOW_A_MINUTE = 3,   //显示正面
	HIDE_A_MINUTE = 4,   //显示背面
	AT_TABLE = 5,        //在桌面显示
	KEEP_IT = 6,         //占用
	LAST_FLAG,
};
//游戏规则种类 4种
enum SAI_ZI_WIN_TYPE{SAI_ZI_WIN_TYPE_ALLNUM=0,SAI_ZI_WIN_TYPE_LEVEL,SAI_ZI_WIN_TYPE_JO,SAI_ZI_WIN_TYPE_ONE};

enum {DEL_OP=0,ADD_OP};

enum    USERSTATUS						//状态定义
{
	STATUSNOTDEFINE	=0,	//未定义
	IDLESSE			=1,	//空闲	
	INGAMES			=2,	//进入了游戏，但游戏未开始
	PLAYING			=3,	//游戏进行中
	READY			=4,	//在一个桌子中，并已准备好
	NOTREADY		=5,	//坐在了一张桌子上，但未准备好
};

enum    GAMESTATUS						//状态定义
{
	GAME_IDLESSE			=1,	//空闲	
	GAME_PLAYING			=2,	//游戏进行中
	GAME_OVER			=3,	//游戏结束

};
//玩家类型,旁观者,分组1,分组2.....
enum PLAYER_TYPE{PLAYER_RED=0,PLAYER_BLACK,PLAYER_OTHER};

//语音,视频数据类
enum DATA_TYPE{DATA_TYPE_CMD,DATA_TYPE_AUDIO, DATA_TYPE_VIDEO};

//语音视频关,开
enum AV_OP_TYPE{AV_OP_TYPE_CLOSE, AV_OP_TYPE_OPEN};


//建立的UDP SOCKET类型
enum UDP_SOCKET_TYPE{UDP_SOCKET_TYPE_SEND_CMD,UDP_SOCKET_TYPE_RECV_CMD,UDP_SOCKET_TYPE_SEND_VIDEO,UDP_SOCKET_TYPE_RECV_VIDEO,UDP_SOCKET_TYPE_SEND_AUDIO,UDP_SOCKET_TYPE_RECV_AUDIO};

//权限类型
//普通玩家,代理,游戏管理(荷官),系统设置,房间管理,高级用户管理
enum PurviewType{PurviewTypePlayer,PurviewTypeAgent, PurviewTypeGameManager, PurviewTypeSetting, PurviewTypeRoomManager, PurviewTypeUserManager, PurviewTypeMaxNum};

