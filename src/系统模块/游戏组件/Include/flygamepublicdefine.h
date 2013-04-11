//URL 
#define HOME_WEB_URL  "http://www.jiauu.com/"
#define BBS_WEB_URL  "http://www.jiauu.com/mdm/mdm1/"

//消息文件
#define		WM_PLAY_DT_SOUND			3456					//

//动态语音
#define DT_SOUND_STR	"PlaySound:"

//颜色定义
#define UI_COLOR_BK_BLUE	RGB(0,90,137)//背景蓝色
#define UI_COLOR_TRANT_RED	RGB(255, 0, 255)//透明红色
#define UI_COLOR_RED	RGB(255, 0, 0)//红色
#define UI_COLOR_BLACK	RGB(0, 0, 0)//黑色
#define UI_COLOR_WHITE	RGB(255, 255, 255)//白色
#define UI_COLOR_GRAY	RGB(222, 211, 210)//灰色
#define UI_COLOR_CHAT_BK	RGB(7,78,108)//聊天背景色
#define UI_COLOR_USER_LIST_BK	RGB(10,130,180)//用户列表背景色

//功能定义
#define REMOTE_VIDEO_MAX_NUM  4 //最多可以显示4路远程视频


//语音,视频数据类
//enum DATA_TYPE{DATA_TYPE_CMD,DATA_TYPE_AUDIO, DATA_TYPE_VIDEO};

//语音视频关,开
//enum AV_OP_TYPE{AV_OP_TYPE_CLOSE, AV_OP_TYPE_OPEN};

//设置规则定时器
#define SET_RULE_TIMER_ID	1234	
#define SET_RULE_TIME_NUM	100

//操作背景栏
#define OP_BACK_PIC_W	35
#define TITLE_PIC_H	35
//视频
#define VIDEO_X_OFF	7
#define VIDEO_Y_OFF	(26+25)
#define VIDEO_W	        160
#define VIDEO_H	        120
#define VIDEO_PIC_W	        175
#define VIDEO_PIC_H	        180

//OP BACK
#define OP_BACK_W	        34
//ready
#define READY_PIC_W 83

//TIME
#define TIME_PIC_W 30
#define TIME_PIC_H 21

//NT
#define NT_PIC_W 30
#define NT_PIC_H 30

//CARD
#define CARD_PIC_H	95

#define AV_TOOLS_ALL_BUTTON_W  120
#define AV_TOOLS_BUTTON_H      20

#define AV_TOOLS_W    45
#define AV_TOOLS_H    130

//即时通讯
#define FLY_QQ_WINDOW_NAME_STR  "FlyQQ 2007"

//状态，开关
#define CLOSE_STATUS 0
#define OPEN_STATUS 1

//清空命令
#define CLEAR_ALL_CMD   1000 //清空命令

//牌点
#define BLACK_3	0x32  //黑桃3
#define BLACK_A	0x3D  //黑桃A

#define FANG_KUAI_4  0x03 //方块4

//定义时间间隔游戏使用期限
#define MAX_USE_TIME	200804 
#define MAX_MIN_TIME	200801

#define FLY_GAME_CLIENT_VER	20071028

//用户最小ID
#define USER_MIN_ID	2000

//UI TYPE
#define UI_TYPE_NUM     0 //1// 0


#define TOP_PIC_H							82
#define WEB_PIC_H							80 //web
#define GAME_HOME_PIC_H							77 //web

#define WEB_BUTTON_H	54 //WEB头
#define HIDE_BUTTON_X                       310 //263//隐藏按钮

#define ROOM_SEARCH_BAR_LEFT				47 //12//15							//
#define ROOM_SEARCH_BAR_TOP					(116+TOP_PIC_H) //111							//
#define ROOM_SEARCH_BAR_WIDTH				250 //200//267							//
#define ROOM_SEARCH_BAR_HEIGHT				136							//

#define ROOM_LIST_BAR_POS_X					ROOM_SEARCH_BAR_LEFT //12//15							//
#define ROOM_LIST_BAR_POS_Y					ROOM_SEARCH_BAR_TOP //(110+20) //286							//
#define ROOM_LIST_BAR_WIDTH					255 //235//268 //246   游戏列表
#define ROOM_WEB_POS_X						318 //274 //303

//用户UI定义
//flash ad
#define	FLASH_AD_X	136
#define	FLASH_AD_Y	20
#define	FLASH_AD_W	320
#define	FLASH_AD_H	52


//顶端大按钮
#define TOP_BIG_BTN_R_W   0 //50 //离右边宽度
#define TOP_BIG_BTN_BLANK 7 //8
#define TOP_BIG_BTN_NUM  5
#define TOP_BIG_BTN_Y   23
#define TOP_BIG_BTN_W	71
#define TOP_BIG_BTN_H	50

//顶端小按钮
#define TOP_SMALL_BTN_BLANK 1
#define TOP_SMALL_BTN_Y  0
#define TOP_SMALL_BTN_W	20
#define TOP_SMALL_BTN_H	19

//web button
#define TOP_WEB_BTN_R_W   -50 //50 //离右边宽度
#define TOP_WEB_BTN_BLANK 5 //8
#define TOP_WEB_BTN_NUM  5
#define TOP_WEB_BTN_X   (ROOM_WEB_POS_X+10)
#define TOP_WEB_BTN_Y   137
#define TOP_WEB_BTN_W	51
#define TOP_WEB_BTN_H	24

