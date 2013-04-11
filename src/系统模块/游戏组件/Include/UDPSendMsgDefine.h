//UDPSendMsgDefine.h
/*
*功能说明：UDP 消息定义文件

*/

//服务器端口号
#define SERVER_PORT   8765

#define UDP_SOCKET_RECV_PORT   40001 //接收视频端口
#define UDP_SOCKET_RECV_PORT1   40003 //接收视频端口
#define UDP_SOCKET_RECV_PORT2   40005 //接收视频端口
#define UDP_SOCKET_RECV_PORT3   40007 //接收视频端口
//	int recAiduoPort[REC_AIDIO_PORT_NUM];//接收音频端口      
#define UDP_SOCKET_RECV_AUDIO_PORT  50001 //接收音频端口

#define UDP_SOCKET_REG_PORT     60001 //UDP 注册端口

//#define HEART_CHECK_TIME  1000*20  //心跳检查周期 20s
#define HEART_CHECK_TIME  50  //心跳检查周期 20s

//pack flag
#define FLAG_AUDIO 0x3131
#define FLAG_VIDEO 0x3132
#define FLAG_CMD   0x3133
#define FLAG_MP3   0x3134 //Add by 2005-11-10
#define FLAG_FILE   0x3135 //Add by 2005-11-15文件传送


//消息
#define CMD_SEND_REG      1  //注册信息
#define CMD_SEND_HEART      2  //心跳信息
#define CMD_SEND_UN_REG      3  //取消注册信息
#define CMD_SEND_GET_TABLE_PLAYER_LIST      4  //获取table用户列表消息

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


#define VIDEO_SHOW_W    176
#define VIDEO_SHOW_H    144

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

#define IMAGE_WIDTH       QCIF_WIDTH
#define IMAGE_HEIGHT      QCIF_HEIGHT

#define MAX_UDP_SIZE   1024*8   //太小会产生马赛克