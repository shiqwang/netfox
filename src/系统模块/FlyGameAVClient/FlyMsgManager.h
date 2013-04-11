// FlyMsgManager.h: interface for the CFlyMsgManager class.
//
//////////////////////////////////////////////////////////////////////
/*************************
编程总结
1.编译不过,把包含文件拷贝到VC/系统目录下
问题:error LNK2001: unresolved external symbol _ICDecompress
办法:
#include <vfw.h>
#pragma comment(lib,"vfw32")

  问题:UDP无法接收数据
  办法:接收的UDPID 搞错了,多了类型定义
  
	问题:显示图像效率太低
	办法:
	HWND myhwnd=::GetForegroundWindow();
	CWnd *m_hMyWnd;
	m_hMyWnd = FindWindow(NULL,"你的进程");
	
	  HWND GetActiveWindow(VOID)
	  AfxGetMainWnd()
	  
		IsWindow(HWND)可以判断是不是窗口
		
		  GetActiveWindow()与::GetActiveWindow()
		  
**************************/
#if !defined(AFX_FLYMSGMANAGER_H__5E21602D_D9A5_4458_9590_39DB9437ADBE__INCLUDED_)
#define AFX_FLYMSGMANAGER_H__5E21602D_D9A5_4458_9590_39DB9437ADBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define IOME_STOP        0
#define IOME_SOCKET      1
#define IOME_AUDIO_IN    2
#define IOME_AUDIO_OUT   3
#define IOME_VIDEO_IN    4
#define IOME_VIDEO_OUT   5
#define IOME_AUDIO_CODEC 6
#define IOME_VIDEO_CODEC 7


#define IOMN_TXT                6//lParam point to the txt
#define IOMN_VQSET               7//Hiword :q

///status
#define STA_FREE    0   //空闲
#define STA_BUSY    1   //繁忙

#define IOM_AUDIO       WM_USER+51
#define IOM_VIDEO       WM_USER+52
#define IOM_MP3     WM_USER+53




//AVIO版本
#define VER_AVIO 102

//msg
//the LOWORD wParam specify the notify code
#define IOM_NOTIFY     WM_USER+50

//the HIWORD of wParam specify the err code
#define IOMN_ERR               5    

enum {AV_EFFECT_Emboss=0,AV_EFFECT_Mosaic,AV_EFFECT_Negate, AV_EFFECT_FlipVert,AV_EFFECT_FlipHorz };

//网络连接方式:直连,P2P,服务器中转
enum NET_LINK_TYPE{NET_LINK_TYPE_NO,NET_LINK_TYPE_DIRECT,NET_LINK_TYPE_P2P,NET_LINK_TYPE_SERVER};


#define MAX_NET_TEST_NUMBER 5  //连续发送网络测试消息次数

#include "../Include/FlyNetProtocol.h"
#include "../Include/UtilitFun.h"

#include "VideoWindow.h"

#include "PlayerDataManager.h"
#include "CaptureVideo.h"
#include "CodecMgr.h"


#include "AudioRec.h"
#include "AudioPlay.h"

#define REMOTE_VIDEO_MAX_NUM  4 //最多可以显示4路远程视频
class CFlyMsgManager : public CWnd 
{
private:
	CVideoWindow m_VideoWindow[REMOTE_VIDEO_MAX_NUM];

public:
	//功能:发送网络心跳
	//输入:
	//输出:
	void SendHeartNetMsg();

	//功能:INIT所有用户的网络状态
	//输入:
	//输出:
	void InitAllPlayerNetLinkMode();

	//功能:目前所有用户的网络状态
	//输入:
	//输出:
	void GetAllPlayerNetLinkMode();
	
	//功能:获取某用户网络状态
	//输入:
	//输出:
	int GetOnePlayerNetLinkStatus(CString userStr);
	

	
	
	//功能:查询某用户网络连接方式
	//输入:
	//输出:
	void GetOnePlayerNetLinkMode(char *pszRemoteSockAddr,int portInt,CString userStr);
	
	//功能:发送网络测试消息  去,会
	//输入:IP,PORT,UDPID,USERID
	//输出:
	void SendUdpNetTestMsg(char *pszRemoteSockAddr,int portInt, int udpID, int userID, int messageInt = CMD_SEND_NET_TEST);
	
	
	//功能:发送P2P消息
	//输入:IP,PORT,UDPID,USERID
	//输出:
	void SendUdpNetP2PMsg(char *pszRemoteSockAddr,int udpID, int myuserID, int otheruserID);
	
	
	//CMD_SEND_NET_P2PSOMEONEWANTTOCALLYOU	对方想CALL你
	//CMD_SEND_NET_P2PTRASH	P2P回应
	
	//功能:发送P2P消息
	//输入:IP,PORT,UDPID,USERID
	//输出:
	void SendUdpNetP2PLinkMsg(char *pszRemoteSockAddr,int portInt, int udpID, int userID, int messageInt =CMD_SEND_NET_P2PSOMEONEWANTTOCALLYOU);
	
	
	
	//功能:显示视频
	//输入:
	//输出:
	void DisplayRemoteVedio(int deskInt);

	//功能:显示视频
	//输入:
	//输出:
	void DisplayRemoteVedioNew(int deskInt);

	
	//功能:设置句柄,刷新RECT
	//输入:
	//输出:
	void SetRemoteShow(HWND myhRemoteWnd, int deskInt);
	CRect remoteRect[REMOTE_VIDEO_MAX_NUM];
	
	HWND hRemoteWnd[REMOTE_VIDEO_MAX_NUM];
	HBITMAP m_hBmp[REMOTE_VIDEO_MAX_NUM];//注意初始化
	
	CWnd * showHwd;
	
	BITMAPINFO* GetBitampInfo();
	
	char* GetRemoteVideo(UINT& len);
	
	
	
//	void WndProc(WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(WPARAM wParam, LPARAM lParam);
	HWND m_hwndRemoteVideoRcv;
	
	char m_VideoRemote[76032];
	
	bool m_StopRecv;//Y=停止接收
	
	void OnDecodeVideoData(PACK_VIDEO* ppv,int len);
	
	//功能:向所有玩家发送视频数据
	//输入:
	//输出:
	void SendVideoDataToAll(char * buf, int lengInt);
	
	
	//功能:发送视频数据
	//输入:
	//输出:
	void SendVideoDataToClient(char * buf, int lengInt, char *ipStr, int portInt );
	
//语音
	BOOL DestroyAudioRec();
	BOOL InitAudioRec();

	void DestroyAudioCodec();
	BOOL InitAudioCodec();

	BOOL DestroyAudioPlay();
	BOOL InitAudioPlay();


	void OnEncodeAudioData(char* pa,int len);
	void OnDecodeAudioData(PACK_AUDIO* ppa,int len);
	PACK_AUDIO m_AudioPack;

	HWND m_hwndLocalAudioRcv;
	HWND m_hwndRemoteAudioRcv;

		HANDLE m_hSockAudioThread;

	static DWORD WINAPI AudioInThreadProc(LPVOID lpParameter);
	static DWORD WINAPI AudioOutThreadProc(LPVOID lpParameter);

	static DWORD WINAPI SockAudioThreadProc(LPVOID lpParameter);//音频数据

	char m_AudioRemote[SIZE_AUDIO_FRAME];
	char m_AudioLocal[SIZE_AUDIO_FRAME];

	int m_iAudioBuf;

	CAudioPlay m_AudioPlay;
	CAudioRec m_AudioRec;

	DWORD m_dwAudioOutId;
	DWORD m_dwAudioInId;
	DWORD m_dwSockAudioThreadId;

	HANDLE m_hAudioOut;
	HANDLE m_hAudioIn;

//启动发送语音
BOOL EnableAudio(BOOL b);

//发送视频到所有人员
//opType=0=audio ,1=video 
void SendAVDataToOneGroup(const char *pBuf, int iLen, int opType);

	//功能:发送语音数据
	//输入:
	//输出:
	void SendAudioDataToClient(char * buf, int lengInt, char *ipStr, int portInt );

	HWND m_hwndMainWnd;
	
	char* m_VideoLocal;
	
	UINT m_nFrameCount;
	UINT m_nLastFrameCount;
	UINT m_nDropRate;
	UINT m_nFps;
	UINT m_uDataRcv;
	UINT m_uDataSend;
	UINT m_nCurVid;
	UINT m_nLastVid;
	
	DEVICE_SUPPORT m_ds;
	int noAvEncodeInt;//0默认,1为不成功
	//初始化视频编码解码器
	BOOL InitVideoCodec();
	void DestroyVideoCodec();
	
	char m_VideoPack[10000];
	BYTE m_idVideo;
	BYTE m_session;
	
	CCodecMgr m_CodecMgr;
	
	BOOL m_bVideoSend;//是否发送视频
	BOOL m_bAudioSend;//是否发送音频
	BOOL m_bRecordVoice;//录音控制
	BOOL m_bRecordAvi;//录像
	BOOL m_bVideoRemoteShow;//显示对方视频
	BOOL m_bVideoLocalShow;//显示本地视频	
	
	void OnEncodeVideoData(char *pv, int len);
	
	CCaptureVideo m_cap; 
	
private:
	PlayerDataManager  myPlayerDataManager;
	
protected:
	
	//功能:停止一个线程
	//输入:
	//输出:
	void StopOneThread(HANDLE myH);
	
	//线程函数
	HANDLE m_hMsgRecvSockThread;
	DWORD m_dwMsgRecvSockThreadId;
	//功能:接收控制命令
	//输入:
	//输出:
	static DWORD WINAPI MsgRecvSockThreadProc(LPVOID lpParameter);
	CUtilitFun myCUtilitFun;
	
	HANDLE m_hVideoRecvSockThread;
	DWORD m_dwVideoRecvSockThreadId;
	static DWORD WINAPI VideoRecvSockThreadProc(LPVOID lpParameter);
	
//心跳线程
	HANDLE m_hNetHeartSendSockThread;
	DWORD m_dwNetHeartSendSockThreadId;
	static DWORD WINAPI NetHeartSendSockThreadProc(LPVOID lpParameter);
	
	
	
public:
	CString  avServerIPStr;//服务器IP
	int m_portInt;
	int m_userID;//用户ID
	int	m_room_id;//房间号
	int m_table_id;//桌号
	int m_desk_id;//凳子号
	
public:
	int UdpSocketArray[UDP_NUM];
	
	BOOL DestroySocket();
	void InitSocketList();
	//初始化，创建SOCKET，并监听
	BOOL Init( /*CWnd * myhwd, */
		HWND myhWnd,
		char * serverIP,
		int portInt,
		int userID, 
		int roomID, 
		int tableID,
		int deskID);
	
	//功能:通过CMD发送信息到服务器
	//输入:
	//输出:
	void SendUDPMsgToServer(char * buf, int lengInt);

	//功能:通过CMD发送信息到服务器
	//输入:
	//输出:
	void SendUDPTestMsgToServer(char * buf, int lengInt, int portInt, int udpID);
	
	
	//功能:增加一个玩家
	//输入:
	//输出:
	void SendAddOnePlayerMsg();
	
	
	//功能:
	//输入:
	//输出:
	void SendAVUnRegMsg();
	
	void SendAVRegMsg();
	
	void SendAVGetOneTablePlayerListMsg( );
	
	void SendUdpRegMsg(char *pszRemoteSockAddr, int userID, int avType , int poInt, int udpID);//发送UDP注册消息
	
	void SendUdpUnRegMsg(char *pszRemoteSockAddr,int userID,  int avType , int poInt, int udpID);//发送UDP注册消息
	
	void SendUdpGetOneTablePlayerListMsg( char *pszRemoteSockAddr, int userID,  int udpID);//发送UDP注册消息
	
public:
	CFlyMsgManager();
	virtual ~CFlyMsgManager();
protected:
	//{{AFX_MSG(CTest2)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

#endif // !defined(AFX_FLYMSGMANAGER_H__5E21602D_D9A5_4458_9590_39DB9437ADBE__INCLUDED_)
