#define FACE_ARRAY_NUM      20 //20表情

#define RTF_CTRL_ID   WM_USER+101

//面具图片文件文件夹
#define    FACE_PIC_FOLDER_STR            "CustomFaceRecv"
#define    SEND_FILE_MAX_SIZE     1024*500 //最大传送文件不能大于500k
//界面位置定义

#define ONE_PLAYER_WIDTH       178  //宽度
#define ONE_PLAYER_HIGH        176  //高度
#define ONE_PLAYER_BLANK_W        20  //间隔
#define ONE_PLAYER_BLANK_H      15  //间隔
#define PLAYER_ROW_NUM              5   //每一排数目
#define PLAYER_COL_NUM          2  //行数

#define VIDEO_SHOW_ADD_W       10  //补偿量
#define VIDEO_SHOW_ADD_H       25  //

//#define BK_PLAYER_WIDTH       178   //宽度
//#define BK_PLAYER_HIGH        176  //高度
//#define BK_PLAYER_BLANK_W        15  //间隔
//#define BK_PLAYER_BLANK_H       15  //间隔

//公共按钮位置设置
#define COMMON_BUT_ROW_NUM     4
#define COMMON_BUT_COL_NUM     2
#define COMMON_BUT_WIDTH       90
#define COMMON_BUT_HIGH        35
#define COMMON_BUT_BLANK_W     10
#define COMMON_BUT_BLANK_H     10



//大chat room
// w= 1, 3, 1
// h = 1,1,3
//SCREEN_W
//SCREEN_H
#define CHAT_ROOM_BLANK_W   5  //间隔
#define CHAT_ROOM_BLANK_H   5  //间隔


//公用定义
#define DEFAULT_ROLL_STR  "欢迎光临!飞扬创作提供技术支持(cctvdodo.zj.com)"

//UDP socket 
//控制命令,语音,视频,文件传送
//语音端口发送,接收语音,控制,文件数据
#define ONE_ROOM_MIC_LIST_MAX_NUM  10 //某房间的麦序最大数


//VIDEO
#define VIDEO_PIC_W    176
#define VIDEO_PIC_H    170

#define VIDEO_SHOW_W    176
#define VIDEO_SHOW_H    144

//动作文件
#define ACTION_FILE_LENGTH  1024*3  //3K
#define ACTION_FILE_NAME   "Chat//Actions.txt"

//用户列表
#define CHAT_ROOM_USER_LIST_COL_NUM  3 //3列

//
#define CLIENT_REC_MESSAGE    10001 //客户端接收到消息
//#define WM_CMD_REFRUSH_TREE   90000  //刷新树图
//MAIN
#define FLY_CMD_REFRUSH_TREE   30001  //刷新树图  90001不能用
#define FLY_CMD_REFRUSH_LIST   30002  //刷新List  90001不能用
#define FLY_CMD_REFRUSH_LIST_ONLY_ONE   30003  //刷新List 查询房间

//CHATROOM
#define FLY_CMD_REFRUSH_ROOM_PLAYER_LIST   40000  //刷新List  
#define FLY_CMD_CHAT_INSERT_ONE_MSG 30003 // WM_USER+40000  //插入一一条聊天记录 不能用??
#define FLY_CMD_REFRUSH_ONE_ROOM_INFO   40002  //刷新房间数据  
#define FLY_CMD_REFRUSH_ROOM_MIC_LIST   40003  //刷新M数据  
#define FLY_CMD_REFRUSH_ONE_ROOM_LIST   40004  //刷新一个房间列表  
#define FLY_CMD_REFRUSH_KIND_LIST   40005  //刷新一个类型列表 
#define FLY_CMD_INTO_ONE_ROOM   40006  //刷新一个类型列表   
#define FLY_CMD_SHOW_FACE_PIC   40007  //显示面具图
#define FLY_CMD_START_SEND_HEART_MSG   WM_USER+ 4008 //40008  //启动心跳程序
#define FLY_CMD_REFRUSH_SOUND_CTRL   40009  //变动语音进度条

//#define ROOM_LIST_COL_NUM    11   //房间列表COL数
#define ROOM_COL_NUM    13+3+1
#define DB_ROOM_COL_NUM    13+3   //数据库COL
 //
#define GRID_COL_NUM   8
#define GRID_COL_ROOM_ID  2 //房间号所在列
#define GRID_COL_ADD_ROOM  6 //添加房间
#define GRID_COL_INTO_ROOM  7 //进入房间

//179,241,169--->背景绿色
//210,255,210--->间隔色
//0,176,157--->树图色
#define GRID_BACK_COLOR RGB(179,241,169) //绿色
#define GRID_ROW_COLOR RGB(210,255,210)

#define DLG_ALL_BACK_COLOR RGB( 213,222, 242)//灰色

//公用定义

#define MAX_FRIENDS_NUM 50   //最大好友数
//home 4.0
//#define XML_DOC_VERSION "MSXML2.DOMDocument.3.0"

//
#define SELECT_ONLINE_PLAYER_COL_NUM  2


//菜单
#define ID_GFX_VOIPITEM   1000+1
//#define _ID_GFX_FILEITEM	"传送文件"
#define ID_GFX_FILEITEM    1000+2

/*
typedef struct OneRowData
{
	CString headStr;
	int imageInt;
	int colWidth;
} oneRowData;
*/

//Add by 2006-10-3
#define MAX_SEND_CHAT_MSG_NUM 5  //5s,5
#define MAX_GET_CHAT_MSG_NUM  25
#define MAX_GET_CHAT_MSG_NUM_TEN  10  //10人聊天 
//End add



//控件显示位置
#define REG_CONTROL_X    73
#define REG_CONTROL_Y    70
#define REG_CONTROL_H     20 //高度
#define REG_CONTROL_W     95 //chang度
#define REG_CONTROL_W1     120 //chang度

#define REG_CONTROL_Y_PWD    110
#define REG_CONTROL_Y_PWD1    270


#define REG_BUTTON_X    60
#define REG_BUTTON_Y    180
#define REG_BUTTON_H     23 //高度
#define REG_BUTTON_W     83 //chang度