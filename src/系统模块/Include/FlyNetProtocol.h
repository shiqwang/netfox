//UDP SOCKET
/*主要功能
*可以支持多端口,最多UDP_NUM路
*
*
*
*
*
*/
#define UDP_NUM   6 //可以建立10路 控制命令,视频,语音
extern "C" int FlyNetRecvFromIp(char *pBuf, int iLen,char *pszRemoteSockAddr, int uPort,  int udpID); 

extern "C"  int FlyNetRecvDataFromIpAndPort(char *pBuf, int iLen,char *pszRemoteSockAddr, int *uPort, int udpID);//Add by 2005-11-2
extern "C"  BOOL FlyNetIsSocket(int udpID);
//建立指定端口 SOCKET
//返回:成功,失败
extern "C"  BOOL FlyNetCreate(UINT uSocketPort, int udpID);

//删除socket
extern "C"  int FlyNetDestroy();
//发送信息到指定IP和PORT
extern "C"  int FlyNetSendToPort(const char *pBuf, int iLen, char *pszRemoteSockAddr,unsigned short uPort, int udpID) ;
//初始化
extern "C"  int FlyNetInitializeSocket();
//清除
extern "C"  int FlyNetCleanUpSocket();

