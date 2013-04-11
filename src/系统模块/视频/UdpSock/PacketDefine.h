#pragma once

//封包
#define PACKET_LOGIN		0x0003 //登陆
#define PACKET_LOGIN_OK		0x0103 //登陆ok
#define PACKET_LOGIN_ERR	0x0403 //返回错误
#define PACKET_QUIT			0x0503 //退出
#define PACKET_ADDCLIENT	0x0203 //添加客户端
#define PACKET_USER_CONNECT	0x0303 //用户互相登陆
#define PACKET_USER_CONNECT_OK	0x0603 //确认连接
#define PACKET_USER_VEDIO	0x0104 //视频包
#define PACKET_USER_VEDIO_OK	0x0204 //视频包完毕
#define PACKET_USER_VEDIO_NEW	0x0304 //新视频
#define PACKET_USER_GET_NEW_VEDIO	0x0404 //新视频包完毕
#define PACKET_USER_GET_VEDIO 0x0504 //请求视频
#define PACKET_USER_VEDIO_ERR 0x0604 //视频错误
#define PACKET_USER_SOUND	0x0105 //声音包
#define PACKET_USER_SOUND_OK	0x0205 //声音完毕
#define PACKET_USER_SOUND_NEXT	0x0305 //声音确认
#define PACKET_USER_GET_SOUND 0x0405 //取缺少的声音包
#define PACKET_USER_SOUND_ERR 0x0505 //声音损坏

//交换IP
#define PACKET_SWAP_CONNECT	0x0102 //用户互相登陆