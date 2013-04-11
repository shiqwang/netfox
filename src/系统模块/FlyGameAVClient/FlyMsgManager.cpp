// FlyMsgManager.cpp: implementation of the CFlyMsgManager class.
//
//////////////////////////////////////////////////////////////////////
/*
大消息定义	子消息定义	说明
FLAG_CMD		
CMD_SEND_REG	注册消息
CMD_SEND_NET_TEST	网络测试命令
CMD_SEND_NET_TEST_BAK	网络测试命令回应
CMD_SEND_NET_P2PTRANS	服务器接收
CMD_SEND_NET_P2PSOMEONEWANTTOCALLYOU	对方想CALL你
CMD_SEND_NET_P2PTRASH	P2P回应

*/
#include "stdafx.h"
#include "FlyGameAVClient.h"
#include "FlyMsgManager.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//定义客户端语音端口
#define REC_AUDIO_PORT 4567


bool g_MsgRecvSockThreadProcStop = false;//停止MSG线程
bool g_VideoRecvSockThreadProc = false;//停止视频线程
bool g_SocketListOk = false;//

long sendAVDataLength =0;//发送数据长度
int sendMaxLength=0;//发送的最大数据包

long getAVDataLength=0;//接收数据
int getMaxLength=0;//接收的最大值

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CFlyMsgManager, CWnd)
//{{AFX_MSG_MAP(CTest2)
// NOTE - the ClassWizard will add and remove mapping macros here.
ON_MESSAGE( IOM_VIDEO,WndProc)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CFlyMsgManager::CFlyMsgManager()
{
	avServerIPStr="";
	m_portInt=0;
	m_userID = -1;
	m_room_id=-1;//房间号
	m_table_id=-1;//桌号
	m_desk_id=-1;//凳子号
	
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAV.log",strTime);
	
	strTemp.Format("CFlyMsgManager()");
	theApp.WriteLog(strFile, strTemp);
	
	
	//m_ds.
	m_ds.bVideoIn=false;  //视频采集
	m_ds.bVideoOut=false; //视频输出
	m_ds.bAudioIn=false;  //声音采集
	m_ds.bAudioOut=false;  //声音输出
	m_ds.bVideoCodec=false;//视频编码
	m_ds.bAudioCodec=false;//声音编码
	//end 
	
	//	mySelfStatus = USER_STATUS_TYPE_SEND_AV;//Add by 2006-10-6 USER_STATUS_TYPE_IN;
	
	//	fileIsOpen = 0; //
	
	//	isMP3Init = 0;
	
	//	pImageList = NULL;
	//	IsLogin = false;
	//	currentRoomIDInt = 0 ; //很重要
	
	//InitDllFunction();
	
	
	//	showVideoPositionInt = -1;
	
	//	AVFileArray = new	CArray<CString,CString>;
	
	noAvEncodeInt = 0;
	
	m_StopRecv = false;	

	InitAllPlayerNetLinkMode();//

	
}

CFlyMsgManager::~CFlyMsgManager()
{
	//	DestroySocket();
}
/*
功能:建立6个端口
输入:
返回:
*/
void CFlyMsgManager::InitSocketList()
{
	if ( g_SocketListOk )
	{
		return;
	}
	else
	{
		for (int i =0; i < UDP_NUM; i++)
		{
			UdpSocketArray[i] = 0;
		}//End for
		
		
		//建立6路
		for ( i=0; i < UDP_NUM; i++)
		{/*
			if (i == UDP_SOCKET_TYPE_RECV_AUDIO)
			{
			//#define REC_AUDIO_PORT 4567
				if( FlyNetCreate( REC_AUDIO_PORT, i) )
				{
					UdpSocketArray[i] = 1;
					g_SocketListOk = true;
				}
			}
			else
			*/
			{
			
				if( FlyNetCreate( UdpSocketArray[i], i) )
				{
					UdpSocketArray[i] = 1;
					g_SocketListOk = true;
				}
				else
				{
					UdpSocketArray[i] = 0;
					CString showMsg;
					showMsg.Format("create erorr!i=%d", i);
					AfxMessageBox(showMsg);
				}
			}
			
		}//End for
	}
}
/*
功能:获得一桌的人员列表
输入:
返回:
*/
void CFlyMsgManager::SendAVGetOneTablePlayerListMsg()
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);

	//先要打开接收端口
	//主动发送消息,打开端口

	char mybuf[50] = {0};
	int lengInt = sizeof(mybuf);
	int serverPortInt = SERVER_SEND_CMD_PORT;	
	SendUDPTestMsgToServer(mybuf,  lengInt,  serverPortInt,  UDP_SOCKET_TYPE_RECV_CMD);

	
	
	char *pszRemoteSockAddr;
	int udpID = UDP_SOCKET_TYPE_SEND_CMD;	
	pszRemoteSockAddr = (LPSTR)(PCSTR)avServerIPStr;	
	
	SendUdpGetOneTablePlayerListMsg( pszRemoteSockAddr,  m_userID,   udpID);
	
}
/*
//功能:增加一个玩家
//输入:
//输出:
*/
void CFlyMsgManager::SendAddOnePlayerMsg()
{
	
}
/*
//发送AV注册消息
*/
void CFlyMsgManager::SendAVRegMsg( )
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO	
	
	int avType;
	int poInt=0;
	
	char *pszRemoteSockAddr;
	int udpID;
	
	pszRemoteSockAddr = (LPSTR)(PCSTR)avServerIPStr;
	
	strTemp.Format("SendAVRegMsg(serverIpAddress=%s", avServerIPStr);
	theApp.WriteLog(strFile, strTemp);
	
	
	avType = DATA_TYPE_CMD;
	udpID = UDP_SOCKET_TYPE_RECV_CMD;//RECV控制命令	
	SendUdpRegMsg( pszRemoteSockAddr, m_userID,   avType, poInt, udpID);
	
	
	Sleep(1000);//1s
	
	avType = DATA_TYPE_VIDEO;
	udpID = UDP_SOCKET_TYPE_RECV_VIDEO;//RECV视频	
	SendUdpRegMsg( pszRemoteSockAddr, m_userID,   avType, poInt, udpID);
		
	Sleep(1000);//1s	
	avType = DATA_TYPE_AUDIO;
	udpID = UDP_SOCKET_TYPE_RECV_AUDIO;//RECV语音	
	SendUdpRegMsg( pszRemoteSockAddr, m_userID,   avType, poInt, udpID);				
	
}

/*
功能:发送取消注册消息
输入:
返回:
  */
void CFlyMsgManager::SendAVUnRegMsg()
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	int avType = 1;
	int poInt = 0;
	
	strTemp.Format("SendAVUnRegMsg(serverIpAddress=%s", avServerIPStr);
	theApp.WriteLog(strFile, strTemp);
	
	char *pszRemoteSockAddr;
	int udpID = UDP_SOCKET_TYPE_SEND_CMD;	//视频
	pszRemoteSockAddr = (LPSTR)(PCSTR)avServerIPStr;
	
	
	
	SendUdpUnRegMsg( pszRemoteSockAddr, m_userID,    avType, poInt, udpID);			
}
/*
功能:获得一桌人员LIST
输入:
返回:
  */
void CFlyMsgManager::SendUdpGetOneTablePlayerListMsg( char *pszRemoteSockAddr, int userID,  int udpID)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	CString showMsg;
	showMsg.Format("*%d*%d*%d",
		userID,
		m_room_id,//房间号
		m_table_id//桌号
		);
	//	AfxMessageBox(showMsg);
	char * myP;
	char txt[500];
	myP = txt;
	myP = (LPSTR)(PCSTR)showMsg;
	strcpy(txt, myP);
	
	PACK_CMD *ppc=(PACK_CMD*)new char[sizeof(PACK_CMD)+strlen(txt)+1];
	strcpy(((char*)ppc)+sizeof(PACK_CMD),txt);
	ppc->flag=FLAG_CMD;
	ppc->cmd=CMD_SEND_GET_TABLE_PLAYER_LIST;
	ppc->type=0;
	ppc->data_size_extra=strlen(txt)+1;
	
	int lengInt = sizeof(PACK_CMD)+ppc->data_size_extra;
	SendUDPMsgToServer((char*)ppc,  lengInt);
	/*
	
	  int sendLength =	FlyNetSendToPort( (char*)ppc, sizeof(PACK_CMD)+ppc->data_size_extra, pszRemoteSockAddr, m_portInt,  udpID);
	  
		strTemp.Format("FlyNetSendToPort(serverIpAddress=%s,portInt =%d, sendLength=%d", pszRemoteSockAddr, m_portInt, sendLength);
		theApp.WriteLog(strFile, strTemp);
	*/
	delete [] ppc;
	ppc = NULL;
}
/*
功能:
输入:
*/
void CFlyMsgManager::SendUdpUnRegMsg(char *pszRemoteSockAddr, int userID,  int avType , int poInt, int udpID)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	CString showMsg;
	showMsg.Format("*%d*%d*%d*%d*%d*%d",   userID ,  avType,  poInt,
		m_room_id,//房间号
		m_table_id,//桌号
		m_desk_id//凳子号
		);
	
	//	AfxMessageBox(showMsg);
	char * myP;
	char txt[500];
	myP = txt;
	myP = (LPSTR)(PCSTR)showMsg;
	strcpy(txt, myP);
	
	PACK_CMD *ppc=(PACK_CMD*)new char[sizeof(PACK_CMD)+strlen(txt)+1];
	strcpy(((char*)ppc)+sizeof(PACK_CMD),txt);
	ppc->flag=FLAG_CMD;
	ppc->cmd=CMD_SEND_UN_REG;//取消注册
	ppc->type=0;
	ppc->data_size_extra=strlen(txt)+1;
	
	
	int sendLength =	FlyNetSendToPort( (char*)ppc, sizeof(PACK_CMD)+ppc->data_size_extra, pszRemoteSockAddr, m_portInt,  udpID);
	
	strTemp.Format("FlyNetSendToPort(serverIpAddress=%s,portInt =%d, sendLength=%d", pszRemoteSockAddr, m_portInt, sendLength);
	theApp.WriteLog(strFile, strTemp);
	
	delete [] ppc;
	ppc = NULL;
}
/*
//功能:发送P2P消息
//输入:IP,PORT,UDPID,USERID
//输出:
*/
void CFlyMsgManager::SendUdpNetP2PMsg(char *pszRemoteSockAddr,int udpID, int myuserID, int otheruserID)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	CString showMsg;
	showMsg.Format("*%d*%d",   myuserID, otheruserID 	);
	//	AfxMessageBox(showMsg);
	char * myP;
	char txt[500];
	myP = txt;
	myP = (LPSTR)(PCSTR)showMsg;
	strcpy(txt, myP);
	
	PACK_CMD *ppc=(PACK_CMD*)new char[sizeof(PACK_CMD)+strlen(txt)+1];
	strcpy(((char*)ppc)+sizeof(PACK_CMD),txt);
	ppc->flag=FLAG_CMD;
	ppc->cmd=CMD_SEND_NET_P2PTRANS;//服务器接收,P2P接收
	
	ppc->type=0;
	ppc->data_size_extra=strlen(txt)+1;
	
	
	int sendLength =	FlyNetSendToPort( (char*)ppc, sizeof(PACK_CMD)+ppc->data_size_extra, pszRemoteSockAddr, m_portInt,  udpID);
	
	strTemp.Format("FlyNetSendToPort(serverIpAddress=%s,portInt =%d, sendLength=%d", pszRemoteSockAddr, m_portInt, sendLength);
	theApp.WriteLog(strFile, strTemp);
	
	delete [] ppc;
	ppc = NULL;//Doctor
}
/*
//功能:发送P2P消息
//输入:IP,PORT,UDPID,USERID
//输出:
*/
void CFlyMsgManager::SendUdpNetP2PLinkMsg(char *pszRemoteSockAddr,int portInt, int udpID, int userID, int messageInt )
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	CString showMsg;
	showMsg.Format("*%d*%d",   m_userID,userID);
	//	AfxMessageBox(showMsg);
	char * myP;
	char txt[500];
	myP = txt;
	myP = (LPSTR)(PCSTR)showMsg;
	strcpy(txt, myP);
	
	PACK_CMD *ppc=(PACK_CMD*)new char[sizeof(PACK_CMD)+strlen(txt)+1];
	strcpy(((char*)ppc)+sizeof(PACK_CMD),txt);
	ppc->flag=FLAG_CMD;
	ppc->cmd= messageInt;//CMD_SEND_NET_P2PSOMEONEWANTTOCALLYOU;CMD_SEND_NET_P2PTRASH
	
	
	ppc->type=0;
	ppc->data_size_extra=strlen(txt)+1;
	
	
	int sendLength =	FlyNetSendToPort( (char*)ppc, sizeof(PACK_CMD)+ppc->data_size_extra, pszRemoteSockAddr, portInt,  udpID);
	
	strTemp.Format("FlyNetSendToPort(serverIpAddress=%s,portInt =%d, sendLength=%d", pszRemoteSockAddr, portInt, sendLength);
	theApp.WriteLog(strFile, strTemp);
	
	delete [] ppc;
	ppc = NULL;//Doctor	
}
/*
//功能:发送网络测试消息
//输入:IP,PORT,UDPID,USERID
//输出:
*/
void CFlyMsgManager::SendUdpNetTestMsg(char *pszRemoteSockAddr,int portInt, int udpID, int touserID,  int messageInt)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	CString showMsg;
	//from userid, to userid
	showMsg.Format("*%d*%d",  m_userID, touserID 	);
	//	AfxMessageBox(showMsg);
	char * myP;
	char txt[500];
	myP = txt;
	myP = (LPSTR)(PCSTR)showMsg;
	strcpy(txt, myP);
	
	PACK_CMD *ppc=(PACK_CMD*)new char[sizeof(PACK_CMD)+strlen(txt)+1];
	strcpy(((char*)ppc)+sizeof(PACK_CMD),txt);
	ppc->flag=FLAG_CMD;
	ppc->cmd= messageInt;//CMD_SEND_NET_TEST;CMD_SEND_NET_TEST_BAK;//网络测试消息
	ppc->type=0;
	ppc->data_size_extra=strlen(txt)+1;
	
	
	int sendLength =	FlyNetSendToPort( (char*)ppc, sizeof(PACK_CMD)+ppc->data_size_extra, pszRemoteSockAddr, portInt,  udpID);
	
	strTemp.Format("FlyNetSendToPort(IpAddress=%s,portInt =%d, sendLength=%d", pszRemoteSockAddr, portInt, sendLength);
	theApp.WriteLog(strFile, strTemp);
	
	delete [] ppc;
	ppc = NULL;//Doctor
}
/*
//功能:发送语音视频注册包
//1=video
//输入:用户ID,类型,位置
*/
void CFlyMsgManager::SendUdpRegMsg(char *pszRemoteSockAddr,int userID,  int avType , int poInt, int udpID)//发送UDP注册消息
{
	
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	CString showMsg;
	showMsg.Format("*%d*%d*%d*%d*%d*%d",   userID ,  avType,  poInt,
		m_room_id,//房间号
		m_table_id,//桌号
		m_desk_id//凳子号
		);
	//	AfxMessageBox(showMsg);
	char * myP;
	char txt[500];
	myP = txt;
	myP = (LPSTR)(PCSTR)showMsg;
	strcpy(txt, myP);
	
	PACK_CMD *ppc=(PACK_CMD*)new char[sizeof(PACK_CMD)+strlen(txt)+1];
	strcpy(((char*)ppc)+sizeof(PACK_CMD),txt);
	ppc->flag=FLAG_CMD;
	ppc->cmd=CMD_SEND_REG;
	ppc->type=0;
	ppc->data_size_extra=strlen(txt)+1;
	
	
	int sendLength =	FlyNetSendToPort( (char*)ppc, sizeof(PACK_CMD)+ppc->data_size_extra, pszRemoteSockAddr, m_portInt,  udpID);
	
	strTemp.Format("SendUdpRegMsg FlyNetSendToPort(serverIpAddress=%s,portInt =%d, sendLength=%d", pszRemoteSockAddr, m_portInt, sendLength);
	theApp.WriteLog(strFile, strTemp);
	
	delete [] ppc;
	ppc = NULL;//Doctor
}
/*
功能:初始化，创建SOCKET，并监听
输入:
返回:
*/
BOOL CFlyMsgManager::Init(
						  HWND myhWnd,
						  char * serverIP,
						  int portInt,
						  int userID, 
						  int roomID, 
						  int tableID,
						  int deskID
						  )
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	
	m_userID = userID;
	
	m_room_id=roomID;//房间号
	m_table_id=tableID;//桌号
	m_desk_id=deskID;//凳子号
	
	avServerIPStr.Format("%s", serverIP);
	m_portInt = portInt;
	
	strTemp.Format("Init()");
	theApp.WriteLog(strFile, strTemp);
	
	
	//
	int myInt=	FlyNetInitializeSocket();
	
	strTemp.Format("FlyNetInitializeSocket()=%d", myInt);
	theApp.WriteLog(strFile, strTemp);
	
	//
	InitSocketList();
	
	strTemp.Format("InitSocketList()");
	theApp.WriteLog(strFile, strTemp);
	
	m_bVideoSend=TRUE;

	m_bAudioSend= TRUE;
	//启动接收线程
	//控制命令
	//创建socket线程(socket为block方式)
	m_hMsgRecvSockThread=CreateThread(0,0,MsgRecvSockThreadProc,(LPVOID)this,0,&m_dwMsgRecvSockThreadId);
	
	if(!m_hMsgRecvSockThread)
	{
		AfxMessageBox("MsgRecvSockThreadProc error!");
	}
	else
	{
		strTemp.Format("MsgRecvSockThreadProc(控制命令接受线程建立成功!)");
		theApp.WriteLog(strFile, strTemp);
		
	}
	
	//视频数据
	//语音数据
	
	//创建socket线程(socket为block方式)
	m_hVideoRecvSockThread=CreateThread(0,0,VideoRecvSockThreadProc,(LPVOID)this,0,&m_dwVideoRecvSockThreadId);
	if(!m_hVideoRecvSockThread)
	{
		AfxMessageBox("VideoRecvSockThreadProc error!");
	}
	else
	{
		strTemp.Format("VideoRecvSockThreadProc(视频数据接受线程建立成功!)");
		theApp.WriteLog(strFile, strTemp);
		
	}
	
	//启动发送线程
	//视频采集
	
	
	
	m_ds.bVideoOut=true;//always ok
	if(InitVideoCodec())
	{
		m_ds.bVideoCodec=true;
	}
	else
	{
		noAvEncodeInt = 1;//没有安装解码器
	}
	
	
	//	m_bVideoSend=FALSE;//不发送
	HRESULT hr = m_cap.Init( 0, myhWnd /*myhwd->GetSafeHwnd()*/ , this);
	
	

	//创建socket线程(socket为block方式)
	m_hNetHeartSendSockThread=CreateThread(0,0,NetHeartSendSockThreadProc,(LPVOID)this,0,&m_dwNetHeartSendSockThreadId);
	if(!m_hNetHeartSendSockThread)
	{
		AfxMessageBox("NetHeartSendSockThreadProc error!");
	}
	else
	{
		strTemp.Format("NetHeartSendSockThreadProc(心跳线程建立成功!)");
		theApp.WriteLog(strFile, strTemp);
		
	}//End if

	//声音启动线程
    m_hSockAudioThread = CreateThread(0,0,SockAudioThreadProc,(LPVOID)this,0,&m_dwSockAudioThreadId);
	if(!m_hSockAudioThread)
	{
		AfxMessageBox("m_hSockAudioThread error!");
		//goto RET;
	}
	else
	{
		strTemp.Format("SockAudioThreadProc(声音线程建立成功!)");
		theApp.WriteLog(strFile, strTemp);
		
	}//End if
	//声音
	if(InitAudioCodec())
		m_ds.bAudioCodec=true;
	if(InitAudioPlay())
		m_ds.bAudioOut=true;

	EnableAudio(true);

	CString str1,str2, str3;

	if ( m_ds.bAudioCodec )
	{
		str1="成功";
	}
	else
	{}

	if ( m_ds.bAudioOut )
	{
			str2="成功";
	}
	else
	{}

	if ( m_ds.bAudioIn )
	{
			str3="成功";
	}
	else
	{}

	strTemp.Format("语音初始化:bAudioCodec=%s bAudioOut=%s bAudioIn=%s",str1,str2, str3 );
	theApp.WriteLog(strFile, strTemp);


	return true;
}


/*------------------------------------------------------------------------------*/
/*
功能:控制命令 接收Socket线程

*/
DWORD WINAPI CFlyMsgManager::MsgRecvSockThreadProc(LPVOID lpParameter)
{
	int udpInt = UDP_SOCKET_TYPE_RECV_CMD;
	CFlyMsgManager* pMgr=(CFlyMsgManager*)lpParameter;
	
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sMsgRecvSockThreadProc.log",strTime);
	//	strFile.Format("%sMsgJS.log",strTime);
	
	//	char buf[4096];
	char buf[10000];
	CString showMsg;
	
	int iLen=0;
	while(!g_MsgRecvSockThreadProcStop)
	{
		//获取UDP IP，PORT
		char *pszRemoteSockAddr;
		char myArray[200];
		pszRemoteSockAddr= myArray;
		
		int uPort;
		
		iLen=FlyNetRecvDataFromIpAndPort(buf,4096, pszRemoteSockAddr,  &uPort, udpInt);//Add by 2005-11-2
		
		
		if(iLen>0)
		{
			
			switch(*((short*)buf))//check the flag
			{
				
				//注册命令
			case FLAG_CMD:
				{
					
					
					PACK_CMD *ppc =	(PACK_CMD*)buf;
					CString  UserNO ;
					int avType;
					int poInt;
					//get text,sheti
					/*
					CString showMsg;
					showMsg.Format("%s", (char*)buf +sizeof(PACK_CMD) );
					
					  
						CString msg = showMsg;
						CString findStr = "*";
						CArray<CString,CString> *myArray;
						myArray =new CArray<CString,CString>;
						pMgr->myCUtilitFun.UnPackListStr( msg,  findStr, myArray );
						UserNO = myArray->GetAt(0);
					*/
					
					switch(	ppc->cmd )
					{
						//取消注册
					case CMD_SEND_UN_REG:
						{
						//获得用户信息
CString strKey;			USERLISTITEM* myOneItem;
							myOneItem = new USERLISTITEM;
							
							ZeroMemory(myOneItem,sizeof(USERLISTITEM));	
							
							//必须拷贝,不能直接赋值
							
							//   	myOneItem =(USERLISTITEM*) ( (char*)buf +sizeof(PACK_CMD)) ;
							CopyMemory( myOneItem, (char*)buf +sizeof(PACK_CMD), sizeof(USERLISTITEM));

							CString myUser_numStr;
							myUser_numStr.Format("%s", myOneItem->User_num);					
pMgr->myPlayerDataManager.DelOnePlayerInfo(myUser_numStr);

delete myOneItem;
		
						};
						break;
					//用户列表
					case CMD_SEND_GET_TABLE_PLAYER_LIST:
						{
							
							USERLISTITEM* myOneItem;
							myOneItem = new USERLISTITEM;
							
							ZeroMemory(myOneItem,sizeof(USERLISTITEM));	
							
							//必须拷贝,不能直接赋值
							
							//   	myOneItem =(USERLISTITEM*) ( (char*)buf +sizeof(PACK_CMD)) ;
							CopyMemory( myOneItem, (char*)buf +sizeof(PACK_CMD), sizeof(USERLISTITEM));
							
							//如果是自己就不要增加了
							CString myUser_numStr;
							myUser_numStr.Format("%s", myOneItem->User_num);	
							if (atoi(myUser_numStr) != pMgr->m_userID )
							{
								
								pMgr->myPlayerDataManager.AddOnePlayerInfo(  myOneItem );
							}
						}
						break;
						//注册信息
					case CMD_SEND_REG:
						{
							
						}
						break;
						//心跳信息
					case CMD_SEND_HEART:
						//	pMgr->SetUserLiveNum(UserNO);
						break;
					}
					
					
					//	delete myArray;
					
				}
				break;
			case FLAG_AUDIO:
				{
					
				}
				break;
			case FLAG_VIDEO:
				{
					
				}
				
				break;
			}//End switch(*((short*)buf))
		}
		else
		{
			//	if(! m_Socket.IsSocket())
			if(!FlyNetIsSocket(udpInt))
				break;
		}//End if(iLen>0)
		//destroyed,so break the loop and end the thread
	}//End while(1)
	
	//AfxMessageBox("退出while(!g_MsgRecvSockThreadProcStop)!");
	strTemp.Format("退出while(!g_MsgRecvSockThreadProcStop)");	
	//写入日志
	theApp.WriteLog(strFile, strTemp);
	g_MsgRecvSockThreadProcStop = false;//停止MSG线程
	
	
	return 0;
}

BOOL CFlyMsgManager::DestroySocket()
{
	//	m_Socket.Destroy();
	StopOneThread(m_hMsgRecvSockThread);
	//    FlyNetDestroy();
	//
	//FlyNetCleanUpSocket();
	
	
	
	return TRUE;
}
/*
//功能:停止一个线程
//输入:
//输出:
*/
void CFlyMsgManager::StopOneThread(HANDLE myH)
{
	m_bVideoSend=FALSE;
	g_MsgRecvSockThreadProcStop = true;//停止MSG线程
	g_VideoRecvSockThreadProc = true;//停止视频线程
	return;
	if(myH)
	{
		BOOL b=FALSE;
		DWORD ExitCode;
		int Timeout = 50; 
		while(Timeout)//等待线程结束，如果到一定时间还没结束，就强制结束
		{			 //因为Socket已经destroy了，所以socket线程会返回
			GetExitCodeThread(myH, &ExitCode);
			
			if (ExitCode != STILL_ACTIVE)
			{
				b=TRUE;
				// Thread has ended.
				break;
			}
			else
			{
				Sleep(10);
			}
			
			--Timeout;
		}
		if(!b)//time out ,terminate it
			TerminateThread(myH,0);
		
	}
	
	
	myH=NULL;	
}
/*
//功能:通过CMD发送信息到服务器
//输入:
//输出:
*/
void CFlyMsgManager::SendUDPTestMsgToServer(char * buf, int lengInt, int serverPortInt, int udpID)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sSendUDPTestMsgToServer.log",strTime);
	
	char *pszRemoteSockAddr;
	pszRemoteSockAddr = (LPSTR)(PCSTR)avServerIPStr;
	
	
	
	int sendLength =	FlyNetSendToPort( buf, lengInt, pszRemoteSockAddr, serverPortInt,  udpID);
	
	strTemp.Format("SendUDPTestMsgToServer(serverIpAddress=%s,serverPortInt =%d, sendLength=%d, uid=%d", pszRemoteSockAddr, serverPortInt, sendLength, udpID);
	theApp.WriteLog(strFile, strTemp);
	
	
}	
/*
//功能:通过CMD发送信息到服务器
//输入:
//输出:
*/
void CFlyMsgManager::SendUDPMsgToServer(char * buf, int lengInt)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	char *pszRemoteSockAddr;
	pszRemoteSockAddr = (LPSTR)(PCSTR)avServerIPStr;
	
	
	
	int sendLength =	FlyNetSendToPort( buf, lengInt, pszRemoteSockAddr, m_portInt,  UDP_SOCKET_TYPE_SEND_CMD);
	
	strTemp.Format("FlyNetSendToPort(serverIpAddress=%s,portInt =%d, sendLength=%d, uid=%d", pszRemoteSockAddr, m_portInt, sendLength, UDP_SOCKET_TYPE_SEND_CMD);
	theApp.WriteLog(strFile, strTemp);
	
	
}
/*
//视频压缩并发送
*/
void CFlyMsgManager::OnEncodeVideoData(char *pv, int len)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sAV.log",strTime);
	
	int rlen;
	
	strTemp.Format("OnEncodeVideoData");
//	theApp.WriteLog(strFile, strTemp);
	
	//
	if(m_bVideoSend)
	{			
		if(m_CodecMgr.EncodeVideoData(pv,len,m_VideoPack+sizeof(PACK_VIDEO),&rlen,0))
		{
			//设置参数
			((PACK_VIDEO*)m_VideoPack)->data_size=(unsigned short)rlen;
			((PACK_VIDEO*)m_VideoPack)->id=m_idVideo++;
			((PACK_VIDEO*)m_VideoPack)->session= m_session;
			
			//Add by 2005-10-22
			((PACK_VIDEO*)m_VideoPack)->video_show_position =0;//showVideoPositionInt;
			((PACK_VIDEO*)m_VideoPack)->roomID = m_room_id;
			((PACK_VIDEO*)m_VideoPack)->sendPlayerID = m_userID;//发送视频的人
			((PACK_VIDEO*)m_VideoPack)->deskID = m_desk_id;
			{
				
				//循环查询，发送视频
				//				SendAVDataToOneGroup( m_VideoPack,  rlen+sizeof(PACK_VIDEO), 1/*视频*/);
				
				//Doctor				SendAVDataToServer(m_VideoPack,  rlen+sizeof(PACK_VIDEO), DATA_TYPE_VIDEO/*视频*/);
				SendVideoDataToAll(m_VideoPack, rlen+sizeof(PACK_VIDEO));
				//统计发送的数据
				m_uDataSend+=rlen+sizeof(PACK_VIDEO);
			}
			
			//发送信息,让用户知道发送的数据
		}
		else
		{
			strTemp.Format("EncodeVideoData Error!!");
			theApp.WriteLog(strFile, strTemp);
		}
	}
	
	
	m_VideoLocal=pv;
	//控制本地图像显示
	/*
	//Add by 2006-10-4
	if (m_bVideoLocalShow)
	{
	strTemp.Format("if (m_bVideoLocalShow)");
	theApp.WriteLog(strFile, strTemp);
	
	  if(m_hwndLocalVideoRcv)
	  {
	  strTemp.Format("本地视频");
	  theApp.WriteLog(strFile, strTemp);
	  //将视频数据发送到视频显示窗口(本地视频)
	  SendMessage(m_hwndLocalVideoRcv,IOM_VIDEO,0,(LPARAM)this);
	  }
	  }
	  //End add
	*/
	
	
}

/*------------------------------------------------------------------------------*/
//压缩音频原始数据并发送出去
void CFlyMsgManager::OnEncodeAudioData(char *pa, int len)
{
	
	m_AudioPack.flag=FLAG_AUDIO;
	m_AudioPack.session=m_session;
	m_AudioPack.video_show_position = 0;//showVideoPositionInt;
	
	m_AudioPack.roomID = m_room_id;
	m_AudioPack.sendPlayerID = m_userID;//发送视频的人
	m_AudioPack.deskID = m_desk_id;

	
	if(m_CodecMgr.EncodeAudioData( pa, len,(char*)m_AudioPack.data,0))
	{			
		if ( m_bAudioSend )//控制声音
		{

			SendAVDataToOneGroup( (char*)&m_AudioPack,sizeof(PACK_AUDIO), 0);
				//		sendAudioPackNum++;
	
		}
		
		
		
		//本地录音
		//	m_AudioPack.data sizeof(m_AudioPack.data)
		//录音, 压缩输出
		if (m_bRecordVoice)
		{
			//打开文件,把数据写入,解压的数据
			//doctor 		destFileLocal->Write( (char*)m_AudioPack.data, SIZE_AUDIO_PACKED);
			//		recPackNum++;
			
			//		TRACE("Write num %d\r\n", SIZE_AUDIO_PACKED);	
			
		}//end (m_bRecordVoice)
		
		
		//end
		
		m_uDataSend+=sizeof(PACK_AUDIO);
		//		TRACE("send pack size =%d; send num=%d\r\n", sizeof(PACK_AUDIO), m_uDataSend);
	}
	/*Doctor
	if(m_hwndLocalAudioRcv)
		SendMessage(m_hwndLocalAudioRcv,IOM_AUDIO,0,(LPARAM)this);
		*/
	
}
/*------------------------------------------------------------------------------*/
/*
功能://初始化视频编码解码器
输入:
返回:
*/
BOOL CFlyMsgManager::InitVideoCodec()
{
	m_idVideo=0;
	m_nFps=0;
	m_nFrameCount=0;
	m_nLastFrameCount=0;
	m_nDropRate=0;
	m_nCurVid=0;
	m_nLastVid=0;
	BOOL bRet=FALSE;
	
	if(!m_CodecMgr.InitCodecV())
		goto RET;
	
	
	((PACK_VIDEO*)m_VideoPack)->flag=FLAG_VIDEO;
	//	((PACK_VIDEO*)m_VideoPackSmall)->flag=FLAG_VIDEO;
	
	
	bRet=TRUE;
RET:
	if(!bRet)
	{
		//		PostMessage(m_hwndMainWnd,IOM_NOTIFY,MAKEWPARAM(IOMN_ERR,IOME_VIDEO_CODEC),(LPARAM)this);
		DestroyVideoCodec();
	}
	return bRet;
}
/*------------------------------------------------------------------------------*/
void CFlyMsgManager::DestroyVideoCodec()
{
	
	
	
	m_CodecMgr.DestroyCodecV();
}

//功能:发送语音数据
//输入:
//输出:
void CFlyMsgManager::SendAudioDataToClient(char * buf, int lengInt, char *pszRemoteSockAddr, int portInt )
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sSendAudioDataToClient.log",strTime);
	
	
	
	int sendLength =	FlyNetSendToPort( buf, lengInt, pszRemoteSockAddr, portInt,  UDP_SOCKET_TYPE_SEND_AUDIO);
	
	strTemp.Format("SendAudioDataToClient(serverIpAddress=%s,portInt =%d, sendLength=%d, uid=%d", pszRemoteSockAddr, portInt, sendLength, UDP_SOCKET_TYPE_SEND_AUDIO);
	theApp.WriteLog(strFile, strTemp);		
}
/*
功能:向另外一端发送视频数据
输入:
返回:
*/
void CFlyMsgManager::SendVideoDataToClient(char * buf, int lengInt, char *pszRemoteSockAddr, int portInt )
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sCFlyMsgManager.log",strTime);
	
	
	
	int sendLength =	FlyNetSendToPort( buf, lengInt, pszRemoteSockAddr, portInt,  UDP_SOCKET_TYPE_SEND_VIDEO);
	
	strTemp.Format("SendVideoDataToClient(serverIpAddress=%s,portInt =%d, sendLength=%d, uid=%d", pszRemoteSockAddr, portInt, sendLength, UDP_SOCKET_TYPE_SEND_VIDEO);
//	theApp.WriteLog(strFile, strTemp);	
}
/*
//功能:向所有玩家发送视频数据
//输入:
//输出:
*/
void CFlyMsgManager::SendVideoDataToAll(char * buf, int lengInt)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sSendAVData.log",strTime);
	
	strTemp.Format("SendVideoDataToAll");
	//theApp.WriteLog(strFile, strTemp);
	
	char *pszRemoteSockAddr;
	int portInt;
	
	//循环
	POSITION pos = CMapUserList.GetStartPosition();
	int num = 0;
	CString showMsg;
	
	
	while(pos)
	{
		CString strKey;
		USERLISTITEM* pAlmItem = NULL;
		CMapUserList.GetNextAssoc(pos, strKey, (LPVOID&)pAlmItem);
		
		while(pAlmItem != NULL)
		{
			
			CString myUser_numStr;
			myUser_numStr.Format("%s", pAlmItem->User_num);	
			
			strTemp.Format("myUser_numStr=%s, pAlmItem->room_id=%d,pAlmItem->table_id=%d ", myUser_numStr, pAlmItem->room_id, pAlmItem->table_id);
			//	theApp.WriteLog(strFile, strTemp);
			int myLength = lengInt-sizeof(PACK_VIDEO);	
			//必须同房间,同TABLE, 不传给本人
			if(	 m_room_id == ((pAlmItem->room_id)) &&  m_table_id == (pAlmItem->table_id)  && atoi(pAlmItem->User_num) != m_userID)
			{				
				
				strTemp.Format("找到一条同桌记录,myUser_numStr=%s", myUser_numStr);
				//	theApp.WriteLog(strFile, strTemp);
				
				//	SendOnePlayerInfo(  (char *) pAlmItem, ipStr, portInt);
				int myNetStatus = GetOnePlayerNetLinkStatus(myUser_numStr);

		strTemp.Format("myNetStatus=%d,myUser_numStr=%s", myNetStatus, myUser_numStr);
		theApp.WriteLog(strFile, strTemp);

				switch (myNetStatus)
				{
					//
				case	NET_LINK_TYPE_NO:
					{
						//发送注册信息
						GetOnePlayerNetLinkMode(pAlmItem->ipAdd,  pAlmItem->recVideoPort[0],myUser_numStr);
		strTemp.Format("发送注册信息");
		theApp.WriteLog(strFile, strTemp);

					}
					break; 
					
				case NET_LINK_TYPE_DIRECT:
				case NET_LINK_TYPE_P2P:
					{
						SendVideoDataToClient(  buf,   lengInt, pAlmItem->ipAdd,  pAlmItem->recVideoPort[0] );	
				strTemp.Format("P2P Send Msg ip=%s, port=%d,av lengInt=%d, allNum=%d",  pAlmItem->ipAdd,  pAlmItem->recVideoPort[0], myLength, sendAVDataLength);
				theApp.WriteLog(strFile, strTemp);			
					}
					break; 
				case NET_LINK_TYPE_SERVER:
					{
						//发送到服务器
						int serverVideoPort=SERVER_RECV_VIDEO_PORT;			
						
						//修改目标IP,PORT
						
						((PACK_VIDEO*)buf)->toPortInt = pAlmItem->recVideoPort[0];
						
						CopyMemory( ((PACK_VIDEO*)buf)->toIPStr, pAlmItem->ipAdd, sizeof(pAlmItem->ipAdd));
						
						
						SendVideoDataToClient(  buf,   lengInt,(LPSTR)(PCSTR)avServerIPStr,  serverVideoPort);	
						
				strTemp.Format("Send Msg ip=%s, port=%d,av lengInt=%d, allNum=%d, 目标IP=%s,端口=%d", avServerIPStr,  serverVideoPort, myLength, sendAVDataLength,
					((PACK_VIDEO*)buf)->toIPStr,
					((PACK_VIDEO*)buf)->toPortInt
					);
				theApp.WriteLog(strFile, strTemp);	
				
					}
					break;  	
				}//End switch
				//rlen+sizeof(PACK_VIDEO)
				

				sendAVDataLength +=myLength;
				if (myLength>sendMaxLength)
				{
					sendMaxLength=myLength;
					strTemp.Format("sendMaxLength=%d", sendMaxLength);
					theApp.WriteLog(strFile, strTemp);
					
				}
//				strTemp.Format("Send Msg ip=%s, port=%d,av lengInt=%d, allNum=%d",  pAlmItem->ipAdd,  pAlmItem->recVideoPort[0], myLength, sendAVDataLength);
//				theApp.WriteLog(strFile, strTemp);		
			//	sendAVDataLength +=(lengInt-sizeof(PACK_VIDEO));
			}//End if
			pAlmItem = pAlmItem->pNext;
			num ++;
		}//End while(pAlmItem != NULL)
	}	//End while(pos)
	
	
}

/*------------------------------------------------------------------------------*/
/*
//视频解码
*/
void CFlyMsgManager::OnDecodeVideoData(PACK_VIDEO *ppv, int len)
{	
	
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnDecodeVideoData.log",strTime);
	
	if(m_CodecMgr.DecodeVideoData(((char*)ppv)+sizeof(PACK_VIDEO),ppv->data_size,m_VideoRemote,0,0))
	{
		m_nFrameCount++;
		
		m_nCurVid=ppv->id;
		int showVideoInt = ppv->video_show_position;
		int sendPlayerID = ppv->sendPlayerID ;//为什么不直接使用DESK ID
		
		int deskInt= ppv->deskID ;
		//DisplayRemoteVedio(deskInt);
		DisplayRemoteVedioNew(deskInt);
		
	}
	//统计收到的数据
	m_uDataRcv+=sizeof(PACK_VIDEO)+ppv->data_size;
}

/*------------------------------------------------------------------------------*/
//音频解码，并将音频数据发送到想要数据的窗口
void CFlyMsgManager::OnDecodeAudioData(PACK_AUDIO *ppa, int len)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnDecodeAudioData.log",strTime);

	int showVideoInt = ppa->video_show_position;
	
	if(m_CodecMgr.DecodeAudioData((char*)ppa->data,SIZE_AUDIO_PACKED,m_AudioRemote,0))
	{	
		//copy to 		
		//		CopyMemory ( m_VoiceRec,m_AudioRemote, SIZE_AUDIO_FRAME);
		
		//为了避免延迟过长，当累积的缓冲超过六块时抛弃即将加入的缓冲
		if(m_iAudioBuf<6)
		{
			//录音
			if (m_bRecordVoice)
			{
				//打开文件,把数据写入,解压的数据
				//doctor	destFile->Write( m_AudioRemote, SIZE_AUDIO_FRAME);
				//doctor	TRACE("Write num %d\r\n", SIZE_AUDIO_FRAME);	
				
			}//end (m_bRecordVoice)
			
			m_iAudioBuf++;
			m_AudioPlay.Play(m_AudioRemote,SIZE_AUDIO_FRAME);
			
		/*	//发送记录
			//发送消息，刷新聊天信息
			if(m_hwndChatRoomWnd)
			{
				SendMessage(m_hwndChatRoomWnd,FLY_CMD_REFRUSH_SOUND_CTRL,showVideoInt, 0);
				
			}
			*/
			
			
			// Play the audio
			//			play->PostThreadMessage(WM_PLAYSOUND_PLAYBLOCK,SIZE_AUDIO_FRAME,(LPARAM)m_AudioRemote);
			
		}
		/*Doctor 
		if(m_hwndRemoteAudioRcv)
			SendMessage(m_hwndRemoteAudioRcv,IOM_AUDIO,1,(LPARAM)this);
		*/
		
		
		
	}
	
	m_uDataRcv+=sizeof(PACK_AUDIO);
	delete []ppa;
	
}
/*
//心跳线程
*/
DWORD WINAPI CFlyMsgManager::NetHeartSendSockThreadProc(LPVOID lpParameter)
{
	CFlyMsgManager* pMgr=(CFlyMsgManager*)lpParameter;	
	while(1)
	{
		pMgr->SendHeartNetMsg();
		Sleep(1000*30);//30s
	}//End while(1)

	return 0;
}
/*
//Socket线程，负责接收数据,视频数据
*/
DWORD WINAPI CFlyMsgManager::VideoRecvSockThreadProc(LPVOID lpParameter)
{
	
	int udpID = UDP_SOCKET_TYPE_RECV_VIDEO;//视频
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	//	strFile.Format("log\\%sMsgJS.log",strTime);
	strFile.Format("log\\%sVideoRecvSockThreadProc.log",strTime);
	CFlyMsgManager* pMgr=(CFlyMsgManager*)lpParameter;
	
	strTemp.Format("VideoRecvSockThreadProc()");	
	//写入日志
	theApp.WriteLog(strFile, strTemp);
	
	//主动发送消息,打开端口
	char mybuf[50] = {0};
	int lengInt=sizeof(mybuf);
	int serverPortInt = SERVER_SEND_VIDEO_PORT;	
	pMgr->SendUDPTestMsgToServer(mybuf,  lengInt,  serverPortInt,  udpID);


	//	char buf[4096];
	char buf[MAX_UDP_SIZE+1];
	
	char *pszRemoteSockAddr;
	char myArray[200];
	pszRemoteSockAddr= myArray;
	
	int uPort;
	
	int iLen=0;
	//	while(!pMgr->m_StopRecv)
	while(!g_VideoRecvSockThreadProc)
	{
		iLen=FlyNetRecvDataFromIpAndPort( buf,MAX_UDP_SIZE/*4096*/, pszRemoteSockAddr,  &uPort, udpID);//端口不起作用
		
		strTemp.Format("iLen=%d", iLen);	
		//写入日志
		//	theApp.WriteLog(strFile, strTemp);
		
		if(iLen>0)
		{
			//			TRACE("FlyNetRecvFromIp=%d\r\n",iLen );	
			switch(*((short*)buf))//check the flag
			{
				//控制命令
			case FLAG_CMD:
				{
	strTemp.Format("FLAG_CMD  控制命令 ");	
	//写入日志
	theApp.WriteLog(strFile, strTemp);

					PACK_CMD *ppc =	(PACK_CMD*)buf;
					CString  fromUserNO,toUserNO ;
					int avType;
					int poInt;
					
					
					CString showMsg;
					showMsg.Format("%s", (char*)buf +sizeof(PACK_CMD) );
					
					
					CString msg = showMsg;
					CString findStr = "*";
					CArray<CString,CString> *myArray;
					myArray =new CArray<CString,CString>;
					//	showMsg.Format("*%d*%d",  m_userID, touserID 	);
					pMgr->myCUtilitFun.UnPackListStr( msg,  findStr, myArray );
					fromUserNO = myArray->GetAt(0);//FROM
					toUserNO== myArray->GetAt(1);//TO
					
					//
					switch(	ppc->cmd )
					{
						//网络测试包	 OK
					case 	CMD_SEND_NET_TEST	:
						{
	strTemp.Format("CMD_SEND_NET_TEST  控制命令 ");	
	//写入日志
	theApp.WriteLog(strFile, strTemp);

							//1.获取
							//2.回复
							char *pszRemoteSockAddr;
							int portInt;
							int udpID=UDP_SOCKET_TYPE_RECV_VIDEO;
							int touserID=atoi(fromUserNO);
							int messageInt=CMD_SEND_NET_TEST_BAK;//回复
							
							CString clientIPStr;
							//	int portInt;
							
							CString strKey;
							strKey.Format("%d",  touserID);
							USERLISTITEM myItem;
							myItem = pMgr->myPlayerDataManager.GetOnePlayerInfo( strKey);
							
							clientIPStr = myItem.ipAdd ;
							portInt = myItem.recVideoPort[0];
							
							//	char *pszRemoteSockAddr;			
							pszRemoteSockAddr = (LPSTR)(PCSTR)	clientIPStr ;
							
							pMgr->SendUdpNetTestMsg( pszRemoteSockAddr, portInt,   udpID,   touserID,    messageInt);

	strTemp.Format("SendUdpNetTestMsg( pszRemoteSockAddr=%s, portInt=%d,   udpID=%d,   touserID=%d,    messageInt=%d)",
		pszRemoteSockAddr, 
		portInt,   
		udpID,   
		touserID,    
		messageInt
		);	
	//写入日志
	theApp.WriteLog(strFile, strTemp);

							
						}
						break;
						
						//网络测试回复包 OK
					case 	CMD_SEND_NET_TEST_BAK	:
						{
							//1.获取 哪个用户回复的?
							//2.设置状态
							CString userStr = fromUserNO;
							int netStatus = NET_LINK_TYPE_DIRECT;
							pMgr->myPlayerDataManager.SetOnePlayerNetLinkStatus(  userStr,   netStatus);
							
						}
						break;
					case CMD_SEND_NET_P2PTRANS:
						//P2P消息		
					case 	CMD_SEND_NET_P2PSOMEONEWANTTOCALLYOU	:
						{

	strTemp.Format("CMD_SEND_NET_P2PTRANS  控制命令 ");	
	//写入日志
	theApp.WriteLog(strFile, strTemp);

							//1.获取  哪个用户回复
							//2.回复
							//1.获取
							//2.回复
							char *pszRemoteSockAddr;
							int portInt;
							int udpID=UDP_SOCKET_TYPE_RECV_VIDEO;
							int touserID=atoi(fromUserNO);
							int messageInt= CMD_SEND_NET_P2PTRASH;//回复
							
							CString clientIPStr;
							//	int portInt;
							
							CString strKey;
							strKey.Format("%d",  touserID);
							USERLISTITEM myItem;
							myItem = pMgr->myPlayerDataManager.GetOnePlayerInfo( strKey);
							
							clientIPStr = myItem.ipAdd ;
							portInt = myItem.recVideoPort[0] ;
							
							//	char *pszRemoteSockAddr;			
							pszRemoteSockAddr = (LPSTR)(PCSTR)	clientIPStr ;
							
							pMgr->SendUdpNetP2PLinkMsg( pszRemoteSockAddr, portInt,   udpID,   touserID,    messageInt);
							
							
							
						}
						break;
						//P2P回复消息
					case 	CMD_SEND_NET_P2PTRASH	:
						{
							//1.获取
							//2.设置状态
							CString userStr = fromUserNO;
							int netStatus = NET_LINK_TYPE_P2P;
							pMgr->myPlayerDataManager.SetOnePlayerNetLinkStatus(  userStr,   netStatus);
							
						}
						break;
						
					}//End switch(	ppc->cmd )
					
					delete myArray;
					
				}
				break;
				//MP3数据
			case FLAG_MP3:
				{
					
				}
				break;
				
			case FLAG_VIDEO:
				{
					
					strTemp.Format("FLAG_VIDEO");	
					//写入日志
					//	theApp.WriteLog(strFile, strTemp);
					
					
					//视频数据,先要检查编码器和播放是否正常
					//视频输出正常，视频编码正常
					if( pMgr->m_ds.bVideoOut&&
					pMgr->m_ds.bVideoCodec/*&&
					((PACK_VIDEO*)buf)->session== m_session*/)
					{
						
						pMgr->OnDecodeVideoData((PACK_VIDEO*)buf,iLen);
						
						
						int getInt = iLen-sizeof(PACK_VIDEO);
						getAVDataLength +=getInt;
						if (getInt>getMaxLength )
						{
							getMaxLength=getInt;
							strTemp.Format("getMaxLength=%d", getMaxLength);	
							//写入日志
							theApp.WriteLog(strFile, strTemp);
						}
						
						strTemp.Format("no decode get video data %d,allInt=%d", getInt, getAVDataLength);	
						//写入日志
						theApp.WriteLog(strFile, strTemp);
					}
					
				}
				
				break;
			}
		}
		else
		{
			//	if(! m_Socket.IsSocket())
			if(!FlyNetIsSocket(udpID))
				break;
		}//the socket should be closed,that is m_Socket have been
		//destroyed,so break the loop and end the thread
		
		//然后   
		/*
		if(WaitForSingleObject(eventKill,0)==WAIT_OBJECT_0)   
		{   
		//退出处理 
		//退出处理   
		CString strFile,strTemp;
		CTime tmCur = CTime::GetCurrentTime();
		CString strTime = tmCur.Format("%m%d");
		strFile.Format("log\\%s OutSys.log",strTime);
		
		  strTemp.Format("out SockThreadProc()");
		  theApp.WriteLog(strFile, strTemp);
		  
			return   0;   
			}   
		*/
	}//End while(!pMgr->m_StopRecv)
	
	strTemp.Format("End g_VideoRecvSockThreadProc");	
	//写入日志
	theApp.WriteLog(strFile, strTemp);
	g_VideoRecvSockThreadProc = false;//停止视频线程
	return 0;
}
//无用!!!!!!!!!
//特别注意参数个数(原版本参数多)
//显示视频
//void CFlyMsgManager::WndProc(WPARAM wParam, LPARAM lParam)
LRESULT CFlyMsgManager::WndProc(WPARAM wParam, LPARAM lParam)
{
/*
CString strFile,strTemp;
CTime tmCur = CTime::GetCurrentTime();
CString strTime = tmCur.Format("%m%d");
strFile.Format("log\\%sAV2.log",strTime);

  strTemp.Format("WndProc()");
  theApp.WriteLog(strFile, strTemp);
  //showHwd
		
		  
			HWND m_hWnd = showHwd->GetSafeHwnd() ;//GetSafeHwnd();//doctor
			strTemp.Format("WndProc()1");
			theApp.WriteLog(strFile, strTemp);
			
			  HDC hdc=	showHwd->GetDC()->m_hDC;//	::GetDC(m_hWnd);
			  strTemp.Format("WndProc()2");
			  theApp.WriteLog(strFile, strTemp);
			  
				
				  HDC hMemDC=CreateCompatibleDC(hdc);
				  
					strTemp.Format("WndProc()3");
					theApp.WriteLog(strFile, strTemp);
					
					  HBITMAP hob=(HBITMAP)SelectObject(hMemDC,m_hBmp);
					  //	RECT rc;
					  RECT rcSmall;
					  
						strTemp.Format("WndProc()4");
						theApp.WriteLog(strFile, strTemp);
						
						  //	GetClientRect(&rc);
						  
							strTemp.Format("WndProc()");
							theApp.WriteLog(strFile, strTemp);
							
							  //获得长，宽数据
							  //	int screen_w = rc.right;
							  //	int full_screen_h =  rc.bottom; //分2区
							  //	int screen_h1 = full_screen_h / 20 ; //头区
							  
								BOOL flag =	(BOOL)wParam;
								
								  
									//	rc.left= 200;//10 ;
									//	rc.right=rc.left+ VIDEO_SHOW_W;
									int showPostionInt =	(int)wParam;//Add by 2006
									
									  
										int sizeInt = 1;
										int w;
										int h;
										
										  
											{
											
											  rcSmall.left= 100;//SMALL_VIDEO_LEFT;//10 ;
											  rcSmall.top= 20;//SMALL_VIDEO_TOP;//screen_h1;
											  
												w = 250;
												h= 200;
												}
												
												  
													//	::GetClientRect(m_hWnd, &rc);
													strTemp.Format("WndProc()2");
													//	theApp.WriteLog(strFile, strTemp);
													
													  if(hob)
													  {		
													  
														strTemp.Format("if(hob)");
														theApp.WriteLog(strFile, strTemp);
														
														  UINT len;
														  char* p;
														  
															
															  p= GetRemoteVideo(len);//!!
															  
																int dataLength = (int)p;
																
																  strTemp.Format("dataLength=%d", dataLength);
																  theApp.WriteLog(strFile, strTemp);
																  
																	
																	  BITMAPINFOHEADER* pbih=&GetBitampInfo()->bmiHeader;
																	  
																		HDRAWDIB h=DrawDibOpen();
																		
																		  
																			
																			  
																				
																				  
																					
																					  int x1,x2;
																					  int y1,y2;
																					  
																						x1 = 40;
																						x2 = pbih->biWidth-2*x1;
																						y1 = 60/4*3;
																						y2 = pbih->biHeight-y1;
																						h=DrawDibOpen();
																						
																						  DrawDibDraw(h,hMemDC, 	0, 	0, 260, 200 ,pbih,p,x1,y1, x2,y2,DDF_NOTKEYFRAME);
																						  
																							DrawDibClose(h);
																							
																							  //BitBlt(hdc, rcSmall.left, rcSmall.top, 260 , 200,hMemDC,0,0,SRCCOPY);
																							  BitBlt(hdc, rcSmall.left, rcSmall.top, 260 , 200,hMemDC,0,0,SRCCOPY);
																							  
																								
																								  
																									SelectObject(hMemDC,hob);
																									}//END if(hob)
																									::DeleteDC(hMemDC);
																									::ReleaseDC(m_hWnd,hdc);
																									
																									  if(!::IsWindowVisible(m_hWnd))
																									  ::ShowWindow(m_hWnd,SW_SHOW);	
*/
return 1;
}
/*
功能:
输入:
返回:
*/
char* CFlyMsgManager::GetRemoteVideo(UINT &len)
{
	len=m_CodecMgr.m_BmpU.bmiHeader.biSizeImage;
	return m_VideoRemote;
}
/*
//得到原始视频格式
*/
BITMAPINFO* CFlyMsgManager::GetBitampInfo()
{
	return &m_CodecMgr.m_BmpU;
}
/*
//功能:设置句柄,刷新RECT
//输入:
//输出:
*/
void CFlyMsgManager::SetRemoteShow(HWND myhRemoteWnd, int deskInt)
{
	if (deskInt < REMOTE_VIDEO_MAX_NUM)
	{
		hRemoteWnd[deskInt] = myhRemoteWnd;
		::GetClientRect( hRemoteWnd[deskInt], &remoteRect[deskInt]);
		
		HDC hdc=::GetDC(hRemoteWnd[deskInt]);
		m_hBmp[deskInt]=CreateCompatibleBitmap( hdc ,remoteRect[deskInt].Width(),remoteRect[deskInt].Height());

		m_VideoWindow[deskInt].SetHwnd( myhRemoteWnd );
		
	}//End if (deskInt < REMOTE_VIDEO_MAX_NUM)
}
/*
//功能:显示视频
//输入:
//输出:
*/
void CFlyMsgManager::DisplayRemoteVedioNew(int deskInt)
{
	if ( deskInt > REMOTE_VIDEO_MAX_NUM)
		return;
			
	UINT len;
	char* p;

	p= GetRemoteVideo(len);

	m_VideoWindow[deskInt].OutVideo( p, len );


}
/*
//功能:显示视频
//输入:
//输出:
*/
void CFlyMsgManager::DisplayRemoteVedio(int deskInt)
{
	
	if ( deskInt > REMOTE_VIDEO_MAX_NUM)
		return;
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sOnDecodeVideoData.log",strTime);
	
	HWND myhwnd= hRemoteWnd[deskInt];//::GetActiveWindow();//myCwnd->GetSafeHwnd();//::GetForegroundWindow();
	//	HWND   myhwnd   =   ::FindWindow("testDLL",NULL);  
	
	//	::GetClientRect( myhwnd, &rc);
	
	HDC hdc=::GetDC(myhwnd);
	
	
	
	strTemp.Format("FindWindow(deskInt=%d,NULL)", deskInt);
	theApp.WriteLog(strFile, strTemp);
	
	HDC hMemDC=CreateCompatibleDC(hdc);
	HBITMAP hob=(HBITMAP)SelectObject(hMemDC,m_hBmp[deskInt]);
	
	
	
	if(hob)
	{			
		UINT len;
		char* p;
		
		p= GetRemoteVideo(len);
		
		int dataLength = (int)p;
		
		BITMAPINFOHEADER* pbih=& GetBitampInfo()->bmiHeader;
		
		HDRAWDIB h=DrawDibOpen();
		
		DrawDibDraw(h,hMemDC, 0, 0  ,remoteRect[deskInt].right  - remoteRect[deskInt].left ,remoteRect[deskInt].bottom-remoteRect[deskInt].top ,pbih,p,0,0,pbih->biWidth,pbih->biHeight,DDF_NOTKEYFRAME);
		
		DrawDibClose(h);
		
		BitBlt(hdc,remoteRect[deskInt].left , remoteRect[deskInt].top,remoteRect[deskInt].right  - remoteRect[deskInt].left ,remoteRect[deskInt].bottom-remoteRect[deskInt].top,hMemDC,0,0,SRCCOPY);
		SelectObject(hMemDC,hob);
		
		
		//end
		
	}
	::DeleteDC(hMemDC);
	::ReleaseDC(myhwnd,hdc);
	
	if(!::IsWindowVisible(myhwnd))
		::ShowWindow(myhwnd,SW_SHOW);
	
}

/*
//功能:查询某用户网络连接方式
//输入:对方IP,端口,对方USER
//输出:
*/
void CFlyMsgManager::GetOnePlayerNetLinkMode(char *pszRemoteSockAddr,int portInt,CString userStr)
{
	//1.连续发送5个包
	int udpID = UDP_SOCKET_TYPE_RECV_VIDEO;
	int userID=atoi( userStr);
	
	//1S 发送 5次
	//直连
	for (int i=0; i < MAX_NET_TEST_NUMBER; i++)
	{
		//CMD_SEND_NET_TEST
		SendUdpNetTestMsg( pszRemoteSockAddr,  portInt,   udpID,   userID);
		Sleep(200);
	}//End for (int i=0; i < MAX_NET_TEST_NUMBER; i++)
	
	int myNetStatus = GetOnePlayerNetLinkStatus(userStr);
	
	//表示直连成功
	if (myNetStatus ==NET_LINK_TYPE_DIRECT)
	{
		
		return;
	}
	
	//连续发送P2P请求5个包
	
	//1S 发送 5次
	//向服务器发送请求
	for ( i=0; i < MAX_NET_TEST_NUMBER; i++)
	{
		
		SendUdpNetP2PMsg( (LPSTR)(PCSTR)avServerIPStr,  udpID,   m_userID,   atoi(userStr));
		
		Sleep(200);
	}//End for (int i=0; i < MAX_NET_TEST_NUMBER; i++)
	
	
	myNetStatus = GetOnePlayerNetLinkStatus(userStr);
	if (myNetStatus ==NET_LINK_TYPE_P2P)
	{
		
		
	}
	//设置状态值
	else
	{
		int netStatus = NET_LINK_TYPE_SERVER;
		myPlayerDataManager.SetOnePlayerNetLinkStatus( userStr,  netStatus);
	}
	
}

/*
//功能:获取某用户网络状态
//输入:
//输出:
*/
int CFlyMsgManager::GetOnePlayerNetLinkStatus(CString userStr)
{
	int resultInt = 0;

	CString strKey;
	strKey = userStr;
	USERLISTITEM myItem;
	myItem = myPlayerDataManager.GetOnePlayerInfo( strKey);
	
	resultInt = myItem.linkTypeInt ;

	
	return resultInt;
}
/*
//功能:目前所有用户的网络状态
//输入:
//输出:
*/
void CFlyMsgManager::GetAllPlayerNetLinkMode()
{
	InitAllPlayerNetLinkMode();

	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sSendAVData.log",strTime);
	
	strTemp.Format("SendVideoDataToAll");
	//theApp.WriteLog(strFile, strTemp);
	
	char *pszRemoteSockAddr;
	int portInt;
	
	//循环
	POSITION pos = CMapUserList.GetStartPosition();
	int num = 0;
	CString showMsg;
	
	
	while(pos)
	{
		CString strKey;
		USERLISTITEM* pAlmItem = NULL;
		CMapUserList.GetNextAssoc(pos, strKey, (LPVOID&)pAlmItem);
		
		while(pAlmItem != NULL)
		{
			
			CString myUser_numStr;
			myUser_numStr.Format("%s", pAlmItem->User_num);	
			
			strTemp.Format("myUser_numStr=%s, pAlmItem->room_id=%d,pAlmItem->table_id=%d ", myUser_numStr, pAlmItem->room_id, pAlmItem->table_id);
			//	theApp.WriteLog(strFile, strTemp);
			pAlmItem->linkTypeInt = NET_LINK_TYPE_NO;
			//必须同房间,同TABLE
			if(	 m_room_id == ((pAlmItem->room_id)) &&  m_table_id == (pAlmItem->table_id) &&  atoi(pAlmItem->User_num) != m_userID)
			{				
				
			
						//发送注册信息
						GetOnePlayerNetLinkMode(pAlmItem->ipAdd,  pAlmItem->recVideoPort[0],myUser_numStr);
			
			}
			pAlmItem = pAlmItem->pNext;
			num ++;
		}//End while(pAlmItem != NULL)
	}	//End while(pos)	
}

//功能:INIT所有用户的网络状态
//输入:
//输出:
void CFlyMsgManager::InitAllPlayerNetLinkMode()
{

}
/*
//功能:发送网络心跳
//输入:
//输出:
*/
void CFlyMsgManager::SendHeartNetMsg()
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sSendHeartNetMsg.log",strTime);
	//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO	
	
	int avType;
	int poInt=0;
	
	char *pszRemoteSockAddr;
	int udpID;
	
	pszRemoteSockAddr = (LPSTR)(PCSTR)avServerIPStr;

	//保证控制命令间OK
	avType = DATA_TYPE_CMD;
	udpID = UDP_SOCKET_TYPE_RECV_CMD;//控制命令	
	SendUdpRegMsg( pszRemoteSockAddr, m_userID,   avType, poInt, udpID);

	strTemp.Format("控制命令");
	theApp.WriteLog(strFile, strTemp);

	//控制命令
	char mybuf[50] = {0};
	int lengInt = sizeof(mybuf);
	int serverPortInt = SERVER_SEND_CMD_PORT;	
	SendUDPTestMsgToServer(mybuf,  lengInt,  serverPortInt,  UDP_SOCKET_TYPE_RECV_CMD);
	strTemp.Format("控制命令 SERVER_SEND_CMD_PORT");
	theApp.WriteLog(strFile, strTemp);


	//保证视频接收OK
	serverPortInt = SERVER_SEND_VIDEO_PORT;	
	SendUDPTestMsgToServer(mybuf,  lengInt,  serverPortInt,  UDP_SOCKET_TYPE_RECV_VIDEO);
	strTemp.Format("控制命令 SERVER_SEND_VIDEO_PORT");
	theApp.WriteLog(strFile, strTemp);

}

/*------------------------------------------------------------------------------*/
//音频输出线程，包括音频解码
//声音解压线程
DWORD WINAPI CFlyMsgManager::AudioOutThreadProc(LPVOID lpParameter)
{
	//写日志
	
	CFlyMsgManager* pMgr=(CFlyMsgManager*)lpParameter;
	MSG msg;
	while(GetMessage(&msg,0,0,0))
	{
		switch(msg.message)
		{
		case IOM_AUDIO:
			pMgr->OnDecodeAudioData((PACK_AUDIO*)msg.lParam,(int)msg.wParam);
			break;
		case WOM_DONE:
			{
				WAVEHDR* pwh=(WAVEHDR*)msg.lParam;
				waveOutUnprepareHeader((HWAVEOUT)msg.wParam,pwh,sizeof(WAVEHDR));
				pMgr->m_iAudioBuf--;
				delete []pwh->lpData;//删除Play调用时分配的内存
				delete pwh;
			}
			break;
		}
	}
	return msg.wParam;
}

//声音输入线程，包括声音的压缩
//声音采集和压缩线程
DWORD WINAPI CFlyMsgManager::AudioInThreadProc(LPVOID lpParameter)
{
	//写日志
	
	CFlyMsgManager* pMgr=(CFlyMsgManager*)lpParameter;
	MSG msg;
	
	while(GetMessage(&msg,0,0,0))
	{
		switch(msg.message)
		{
		case WIM_DATA:
			{
				WAVEHDR* pWH=(WAVEHDR*)msg.lParam;
				waveInUnprepareHeader((HWAVEIN)msg.wParam,pWH,sizeof(WAVEHDR));
				
				if(pWH->dwBytesRecorded!=SIZE_AUDIO_FRAME)
					break;//it's not full recorded,i think the wave recorder has ben

				//在M上才发言
			//	if ( pMgr->mySelfStatus == USER_STATUS_TYPE_SEND_AV)
				{
					
					CopyMemory(pMgr->m_AudioLocal,pWH->lpData,SIZE_AUDIO_FRAME);
					pMgr->OnEncodeAudioData(pMgr->m_AudioLocal,SIZE_AUDIO_FRAME);
				}

				
				waveInPrepareHeader((HWAVEIN)msg.wParam,pWH,sizeof(WAVEHDR));
				waveInAddBuffer((HWAVEIN)msg.wParam,pWH,sizeof(WAVEHDR));
			}
			break;
		}
	}	
	return msg.wParam;
}

/*------------------------------------------------------------------------------*/
//
BOOL CFlyMsgManager::InitAudioCodec()
{
	
	BOOL bRet=FALSE;
	
	if(!m_CodecMgr.InitCodecA())
		goto RET;
	//启动丢包率定时器	
	//doctor	SetTimer(m_hwndMainWnd,100,1000,DropRateCounter);
	bRet=TRUE;
RET:
	if(!bRet)
	{
		/*Doctor 
		PostMessage(m_hwndMainWnd,IOM_NOTIFY,MAKEWPARAM(IOMN_ERR,IOME_AUDIO_CODEC),(LPARAM)this);
		DestroyAudioCodec();
		*/
	}
	return bRet;
}

/*------------------------------------------------------------------------------*/
//
void CFlyMsgManager::DestroyAudioCodec()
{	
	//关闭丢包率定时器
	//doctor	KillTimer(m_hwndMainWnd,100);
	m_CodecMgr.DestroyCodecA();
}

/*------------------------------------------------------------------------------*/
//初始化播放设备
BOOL CFlyMsgManager::InitAudioPlay()
{
	BOOL bRet=FALSE;
	DestroyAudioPlay();
	m_iAudioBuf=0;

	
	m_hAudioOut=CreateThread(0,0,AudioOutThreadProc,this,0,&m_dwAudioOutId);
	if(!m_hAudioOut)
		goto RET;
	
	if(!m_AudioPlay.Create(0,m_dwAudioOutId,(DWORD)this,CALLBACK_THREAD))
		goto RET;
	bRet=TRUE;
	
	
RET:
	if(!bRet)
	{/*Doctor 
		//向主窗口发送出错消息
		PostMessage(m_hwndMainWnd,IOM_NOTIFY,MAKEWPARAM(IOMN_ERR,IOME_AUDIO_OUT),(LPARAM)this);
		DestroyAudioPlay();
		*/
	}
	
	return bRet;
}

/*------------------------------------------------------------------------------*/
//
BOOL CFlyMsgManager::DestroyAudioPlay()
{
	
	m_AudioPlay.Destroy();
	if(m_hAudioOut)
	{
		int t=50;
		DWORD ExitCode;
		BOOL bEnd=FALSE;
		PostThreadMessage(m_dwAudioOutId,WM_QUIT,0,0);
		while(t)
		{
			
			GetExitCodeThread(m_hAudioOut,&ExitCode);
			if(ExitCode!= STILL_ACTIVE)
			{
				bEnd=TRUE;
				break;
			}
			else
				Sleep(10);
			t--;
		}
		if(!bEnd)
			TerminateThread(m_hAudioOut,0);
		m_hAudioOut=0;
		
	}
	return TRUE;
}

//启动发送语音
BOOL CFlyMsgManager::EnableAudio(BOOL b)
{
	//启动语音
	if (b)
	{
		if (!m_ds.bAudioIn)
		{
			if(InitAudioRec())
				m_ds.bAudioIn=true;
		}
		
		return true;
	}
	return true;
	
}

/*------------------------------------------------------------------------------*/
//初始化音频输入，当初始化完成，录音就开始了
BOOL CFlyMsgManager::InitAudioRec()
{
	DestroyAudioRec();
	BOOL bRet=FALSE;
	

	//创建录音线程
    m_hAudioIn=CreateThread(0,0,AudioInThreadProc,this,0,&m_dwAudioInId);
	if(!m_hAudioIn)
		goto RET;
	if(!m_AudioRec.Create(0,m_dwAudioInId,(DWORD)this,CALLBACK_THREAD,SIZE_AUDIO_FRAME))
		goto RET;
	//开始录音
	if(!m_AudioRec.Start())
		goto RET;
	
	bRet=TRUE;
RET:
	if(!bRet)
	{
		/*Doctor 
		//如果失败，就向主窗口发送出错消息
		PostMessage(m_hwndMainWnd,IOM_NOTIFY,MAKEWPARAM(IOMN_ERR,IOME_AUDIO_IN),(LPARAM)this);
		DestroyAudioRec();
		*/
	}
	
	return bRet;
	
}

/*------------------------------------------------------------------------------*/
//该死的waveIn函数，结束录音时老是出现死锁（XP下还是会出现这个问题，也不知道怎么解决，苦啊）
BOOL CFlyMsgManager::DestroyAudioRec()
{
	
	m_AudioRec.Stop();
    m_AudioRec.Destroy();
	if(m_hAudioIn)
	{
		int t=50;
		DWORD ExitCode;
		BOOL bEnd=FALSE;
		//向录音线程发送退出消息，并等待线程结束
		PostThreadMessage(m_dwAudioInId,WM_QUIT,0,0);
		while(t)
		{
			
			GetExitCodeThread(m_hAudioIn,&ExitCode);
			if(ExitCode!= STILL_ACTIVE)
			{
				bEnd=TRUE;
				break;
			}
			else
				Sleep(10);
			t--;
		}
		if(!bEnd)
			TerminateThread(m_hAudioIn,0);
		m_hAudioIn=0;
		
	}	
	
	return TRUE;
	
}

//Socket线程，负责接收数据,音频数据
DWORD WINAPI CFlyMsgManager::SockAudioThreadProc(LPVOID lpParameter)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sSockAudioThreadProc.log",strTime);

	int udpID = UDP_SOCKET_TYPE_RECV_AUDIO;//音频
	CFlyMsgManager* pMgr=(CFlyMsgManager*)lpParameter;
	
	//	char buf[4096];
//	char buf[10000];

	//	char buf[4096];
	char buf[MAX_UDP_SIZE+1];
	
	char *pszRemoteSockAddr;
	char myArray[200];
	pszRemoteSockAddr= myArray;

	int uPort;

	int iLen=0;
	while(1)
	{
	//	iLen = FlyNetRecvFromIp ( buf, MAX_UDP_SIZE /*4096*/, "", 0, udpID);
		iLen=FlyNetRecvDataFromIpAndPort( buf,MAX_UDP_SIZE/*4096*/, pszRemoteSockAddr,  &uPort, udpID);//端口不起作用
	
		if(iLen>0)
		{
			
	strTemp.Format("get msg %d, type=%d", iLen, *((short*)buf));
	theApp.WriteLog(strFile, strTemp);

			switch(*((short*)buf))//check the flag
			{
/*
				//文件信息,纯文件数据
			case FLAG_FILE:
				{
					char *pv;
					int len;
					pv=(char *)( (PACK_FILE *)buf)->data ;
					len = ( (PACK_FILE *)buf)->data_size;
					
					if ( 2 != pMgr->fileIsOpen)
					{
						pMgr->OnSaveFileData(   pv,  len);
					}
					
					if (len < SIZE_FILE_PACKED )
					{
						pMgr->fileIsOpen = 2;
						if ( 2 == pMgr->fileIsOpen)
						{
							pMgr->myGetFile.Close();
						}
					}
					
					
				}
				break;
				*/
				
			case FLAG_AUDIO:
				{

	strTemp.Format("FLAG_AUDIO");
	theApp.WriteLog(strFile, strTemp);

					
					if( pMgr->m_ds.bAudioOut&&
						pMgr->m_ds.bAudioCodec&&
						pMgr->m_hAudioOut)
					{
						char* p=new char[iLen];
						if(p)
						{
							CopyMemory(p,buf,iLen);
							
							if(!PostThreadMessage( pMgr->m_dwAudioOutId,IOM_AUDIO,iLen,(LPARAM)p))
								delete []p;
							
							
						}
					}
					
				}
				break;
				
			}
		}
		else
		{
			//	if(! m_Socket.IsSocket())
			if(!FlyNetIsSocket(udpID))
				break;
		}//the socket should be closed,that is m_Socket have been
		//destroyed,so break the loop and end the thread
	}
	return 0;
}

//发送语音视频到所有人员
//	//audio=0
void CFlyMsgManager::SendAVDataToOneGroup(const char * buf, int lengInt,int opType)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sSendAVDataToOneGroup.log",strTime);
	
	strTemp.Format("SendAVDataToOneGroup");
	theApp.WriteLog(strFile, strTemp);
	
	char *pszRemoteSockAddr;
	int portInt;
	
	//循环
	POSITION pos = CMapUserList.GetStartPosition();
	int num = 0;
	CString showMsg;
	
	
	while(pos)
	{
		CString strKey;
		USERLISTITEM* pAlmItem = NULL;
		CMapUserList.GetNextAssoc(pos, strKey, (LPVOID&)pAlmItem);
		
		while(pAlmItem != NULL)
		{
			
			CString myUser_numStr;
			myUser_numStr.Format("%s", pAlmItem->User_num);	
			
			strTemp.Format("myUser_numStr=%s, pAlmItem->room_id=%d,pAlmItem->table_id=%d ", myUser_numStr, pAlmItem->room_id, pAlmItem->table_id);
			//	theApp.WriteLog(strFile, strTemp);
			int myLength = lengInt-sizeof(PACK_VIDEO);	
			//必须同房间,同TABLE, 不传给本人
			if(	 m_room_id == ((pAlmItem->room_id)) &&  m_table_id == (pAlmItem->table_id)  && atoi(pAlmItem->User_num) != m_userID)
			{				
				
				strTemp.Format("找到一条同桌记录,myUser_numStr=%s", myUser_numStr);
				theApp.WriteLog(strFile, strTemp);
				
				//	SendOnePlayerInfo(  (char *) pAlmItem, ipStr, portInt);
				int myNetStatus = GetOnePlayerNetLinkStatus(myUser_numStr);

				strTemp.Format("myNetStatus=%d,myUser_numStr=%s", myNetStatus, myUser_numStr);
				theApp.WriteLog(strFile, strTemp);

				switch (myNetStatus)
				{
					//
				case	NET_LINK_TYPE_NO:
					{
						//发送注册信息
						GetOnePlayerNetLinkMode(pAlmItem->ipAdd,  pAlmItem->recAudioPort[0],myUser_numStr);
						strTemp.Format("发送注册信息");
						theApp.WriteLog(strFile, strTemp);

					}
					break; 
					
				case NET_LINK_TYPE_DIRECT:
				case NET_LINK_TYPE_P2P:
					{
						SendAudioDataToClient((char *)  buf,   lengInt, pAlmItem->ipAdd,  pAlmItem->recAudioPort[0] );	
						strTemp.Format("P2P Send Msg ip=%s, port=%d",  pAlmItem->ipAdd,  pAlmItem->recAudioPort[0]);
						theApp.WriteLog(strFile, strTemp);			
					}
					break; 
				case NET_LINK_TYPE_SERVER:
					{
						//发送到服务器
						int serverVideoPort=UDP_SOCKET_RECV_AUDIO_PORT;			
						
						//修改目标IP,PORT
						
						((PACK_VIDEO*)buf)->toPortInt = pAlmItem->recAudioPort[0];
						
						CopyMemory( ((PACK_VIDEO*)buf)->toIPStr, pAlmItem->ipAdd, sizeof(pAlmItem->ipAdd));
						
						
						SendAudioDataToClient((char *)  buf,   lengInt,(LPSTR)(PCSTR)avServerIPStr,  serverVideoPort);	
						
						strTemp.Format("Send Msg ip=%s, port=%d,av lengInt=%d, allNum=%d, 目标IP=%s,端口=%d", avServerIPStr,  serverVideoPort, myLength, sendAVDataLength,
							((PACK_VIDEO*)buf)->toIPStr,
							((PACK_VIDEO*)buf)->toPortInt
							);
						theApp.WriteLog(strFile, strTemp);	
				
					}
					break;  	
				}//End switch
				//rlen+sizeof(PACK_VIDEO)
				

				sendAVDataLength +=myLength;
				if (myLength>sendMaxLength)
				{
					sendMaxLength=myLength;
					strTemp.Format("sendMaxLength=%d", sendMaxLength);
					theApp.WriteLog(strFile, strTemp);
					
				}
//				strTemp.Format("Send Msg ip=%s, port=%d,av lengInt=%d, allNum=%d",  pAlmItem->ipAdd,  pAlmItem->recVideoPort[0], myLength, sendAVDataLength);
//				theApp.WriteLog(strFile, strTemp);		
			//	sendAVDataLength +=(lengInt-sizeof(PACK_VIDEO));
			}//End if
			pAlmItem = pAlmItem->pNext;
			num ++;
		}//End while(pAlmItem != NULL)
	}	//End while(pos)
}