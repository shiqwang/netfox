#ifndef GAME_FRAME_DLG_HEAD_FILE
#define GAME_FRAME_DLG_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "GameFrameView.h"

#include "LabelEx.h"
#include "DlgDTSound.h"

#include "../../Include/FlyGameAVClientDllDefine.h"
#include "../../Include/FlyGamePublicDefine.h"
#include "../../Include/UDPSendMsgDefine.h"

//宏定义
#define CONTROL_SPACE						2							//控件间距
#define SPLIT_WIDE							0							//拆分条宽度
#define TITLE_HIGHT							26							//标题栏高度
#define BUTTON_WIDE							8							//隐藏按钮宽度
#define BUTTON_HIGHT						80							//隐藏按钮高度
#define TALK_HIGHT							53							//聊天区域高度
#define PIC_BT_WIDE							20							//位图按钮宽度
#define PIC_BT_HIGHT						20							//位图按钮高度
#define BT_BUTTON_WIDE						55							//按钮宽度
#define BT_BUTTON_HIGHT						20							//按钮高度

//宏定义
#define CONTROL_SPACE						2							//控件间距
#define SPLIT_WIDE							0							//拆分条宽度
#define BUTTON_WIDE							7//8							//隐藏按钮宽度
#define BUTTON_HIGHT						82//80							//隐藏按钮高度
#define BT_BUTTON_WIDE						63							//按钮宽度
#define BT_BUTTON_HIGHT						39							//按钮高度
#define BUTTON_MORE_WIDE					0							//查找附加按钮
#define BUTTON_Y_POS						10							//按钮 Y 坐标
#define BUTTON_START_POS					160							//按钮起始点
#define BUTTON_END_POS						115							//按钮终止点
#define ROOM_BAR_HEIGHT						8							//房间框架高度
#define BOTTOM_HIDE_WIDTH					80							//隐藏按钮宽度							
#define BOTTOM_HIDE_HEIGHT					8							//隐藏按钮高度
#define CHAR_HEIGHT							24							//聊天高度

//框架大小 
#define ROOM_FRAME_TOP						82 //(62+20)//56							//框架上高
#define ROOM_FRAME_LEFT						3							//框架左宽
#define ROOM_FRAME_RIGHT					4							//框架右宽
#define ROOM_FRAME_BOTTOM					12							//框架下高

#define INFO_WIDTH							275							//右边信息栏宽度

#define EX_TOOL_EXE  "FlyFileDownLoadClient.exe"
#define TOOLS_LABLE_USE_MAX_NUM  6
#define TOOLS_LABLE_MYSELF_USE_MAX_NUM  4
#define DJ_LABLE_USE_MAX_NUM  4

#define TOOLS_LABLE_NUM    4  //4个工具栏
#define DJ_LABLE_NUM    2  //4个工具栏



enum LABLE_ID_TYPE{LABLE_ID_TYPE_TOOLS,LABLE_ID_TYPE_TOOLS_1,LABLE_ID_TYPE_TOOLS_2,LABLE_ID_TYPE_TOOLS_3,LABLE_ID_TYPE_DJ_1,LABLE_ID_TYPE_DJ_2,LABLE_ID_TYPE_MYSELF_TOOLS,LABLE_ID_TYPE_MAX};
//1.送礼物；2。加为好友；3。关开麦；4。关开视频；5。缩放视频；6。更多的；
enum TOOLS_OP_TYPE{TOOLS_OP_TYPE_GIFT,TOOLS_OP_TYPE_ADD_FRIEND,TOOLS_OP_TYPE_MIC,TOOLS_OP_TYPE_VIDEO,TOOLS_OP_TYPE_ZOOM_VIDEO,TOOLS_OP_TYPE_MORE,TOOLS_OP_TYPE_MAX};

//语音,视频数据类
enum DATA_TYPE{DATA_TYPE_CMD,DATA_TYPE_AUDIO, DATA_TYPE_VIDEO};
//定义用户状态
typedef struct _USER_STATUS_ITEM
{
int userId;
int micStatus;//MIC
int videoStatus;//视频

}
USER_STATUS_ITEM;

//类说明
class CGameFrameControl;
class CClientKernelSink;

#define MAX_STRING_SIZE 128


class CINIFile  
{
  CString lpFileName ;
  DWORD mMaxSize ;
public:
	static bool IsFileExist(CString fileName)
	{
		WIN32_FIND_DATA  d;
		HANDLE  hd= FindFirstFile(fileName,&d);
		if(hd != INVALID_HANDLE_VALUE)
			return true;

		return false;
	}
	static void CopyFileTo(CString destFileName,CString srcFileName)
	{
		bool bb=IsFileExist(srcFileName);
		if(!bb)return;
		CopyFile( srcFileName, destFileName, FALSE);

	}
		
	static CString GetWinSysPath()
	{
		char lpBuffer[MAX_STRING_SIZE];
		GetSystemDirectory( lpBuffer, MAX_STRING_SIZE);
		CString ss=lpBuffer;
		ss += "\\";
		return ss;

	}
		
	static CString GetAppPath()
	{
		char lpBuffer[MAX_STRING_SIZE];
		GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
		CString ss=lpBuffer;
		ss += "\\";
		return ss;
	}

	

	int GetKeyVal(CString secName,CString keyName,int lpDefault)
	{
		return (int) GetPrivateProfileInt(
					secName,// points to section name
					keyName,// points to key name
					lpDefault,       // return value if key name not found
					lpFileName//LPCTSTR lpFileName  // initialization file name
					);

	}
	

	CString GetKeyVal(CString secName,CString keyName,LPCTSTR lpDefault)
	{ 
		char* re = new char[mMaxSize + 1];
		GetPrivateProfileString(
								secName,// points to section name
								keyName,// points to key name
								lpDefault,// points to default string
								re,// points to destination buffer
								mMaxSize,// size of destination buffer
								lpFileName // points to initialization filename
							);
		CString ss=re;
		delete []re;
		return ss;
	}
		
	void SetKeyValString(CString secName,CString keyName,CString Val)
	{ 

		WritePrivateProfileString(
								secName,// pointer to section name
								keyName,// pointer to key name
								Val,// pointer to string to add
								lpFileName// pointer to initialization filename
								);

	}

		
	CINIFile(CString FileName,int maxsize=MAX_STRING_SIZE)
	{
		lpFileName=FileName;
		mMaxSize = maxsize;

	}

	~CINIFile()	{}
	void SetINIFileName(CString fileName){lpFileName=fileName;}
	
};

//////////////////////////////////////////////////////////////////////////
class CGlobal
{
public :
	CGlobal()
	{
		CString s=CINIFile::GetAppPath ();
	    CINIFile f( s + "GamePlaceSetup.ini");

		m_CenterServerIPAddr=f.GetKeyVal ("System","CenterServerIPAddr","127.0.0.1");////客户端当前版本系列号

	//	m_CenterServerPort=CENTER_SERVER_PORT;
		//CString showMsg;
		//showMsg.Format("m_CenterServerPort=%d, m_CenterServerIPAddr=%s", m_CenterServerPort, m_CenterServerIPAddr);
		//AfxMessageBox(showMsg);
		m_HelpUrlAddr=f.GetKeyVal ("System","HelpUrlAddr","www.cctvdodo.com");//

		VirtualFaceWebAddr=f.GetKeyVal ("System","VirtualFaceWebAddr","www.cctvdodo.com");

		m_AdUrl = f.GetKeyVal ("System","AdUrl","www.cctvdodo.com");//

	}
	virtual ~CGlobal(){}

//	CenterServerMsg m_CenterServerPara;
	CString m_CenterServerIPAddr;
	CString m_HelpUrlAddr;
	CString m_MainHomeUrl;
	UINT m_CenterServerPort;

	CString VirtualFaceWebAddr;
	CString m_AdUrl;//广告


};


static CGlobal g_global;

static CGlobal& Glb(){return g_global;}
//游戏主窗口
class GAME_FRAME_CLASS CGameFrameDlg : public CSkinDialog, public ISplitterSink
{
	//友元定义
	friend class CClientKernelSink;
	friend class CGameFrameControl;
	friend class CDlgDTSound;

public:
	void PlayMP3Sound(CString fileStr);
private:
	int showDlgX,showDlgY;
	//处理动态语音
//	bool DealwithDTSoundMsg(MSG_GR_RS_NormalTalk * pNormalTalk);

	CString GetPlaySoundFileStr(CString inStr, CString *charStr);

	CDlgDTSound* pOneUserInfoDlg;
	//游戏变量
protected:
	bool							m_bHideList;						//隐藏列表
	//隐藏列表按钮
	void OnBnClickedHideList();

	//场景语音
	void OnBnClickedYuYin();

	//功能：初始化工具栏
	void InitToolsBar();


	//功能：初始化myself工具栏
	void InitMyselfToolsBar();

	//功能：初始化DJ栏
	void InitDJBar();

	//功能：处理用户操作
	void DealwithOp(int lableID, int opInt);

	//功能：处理myself tool用户操作
	void DealwithMyselfToolsOp( int opInt);	

	//功能：处理tool用户操作
	void DealwithToolsOp(int lableID, int opInt);

	//功能：处理DJ用户操作
	void DealwithDJOp(int lableID, int opInt);
	//功能：获取用户ID通过LABE
	int GetOneUserIdFromLableID(int lableID);

//1.送礼物

			void OpenGiftExe(int opUserInt );

//2。加为好友

		
			void AddOneFriend(int opUserInt );
		
		
		 
//3。关开麦

		
			void OpOneMic(int opUserInt, int opType=0 );
		
		
		 
//4。关开视频

		
			void OpOneVideo(int opUserInt, int opType=0 );
		
		
		 
//5。缩放视频
void OpZoomVideo(int opUserInt, int opType=0);
		
		 
//6。更多的

		
			void ShowOneMoreMenu(int opUserInt , int opType=0);

	USER_STATUS_ITEM  myUserStatusList[TOOLS_LABLE_NUM];
	USER_STATUS_ITEM  mySelfStatusList;

	//功能：init
	void InitUserStatus();
LRESULT MyWndProc(WPARAM wParam, LPARAM lParam);

	int isHaveSetupAV;//视频组件是否调用
//设置自己的位置
void SetMyselfAVClientPostion();
    HINSTANCE myhinstDLL ; //视频
//功能：调用视频DLL
//输入：
//返回：
	void SetupVideoDLL();

//功能：释放视频DLL
//输入：
//返回：
	void FreeVideoDLL();

//视频操作接口	
//功能:控制本地发送
//输入:语音视频类型;开关类型
//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
void FlyOpNetSendMode(int avType, int opType);

void FlyGetOneTablePlayerList();

void FlyAVClientSendUnRegMsg();

void FlyAVClientSendRegMsg();

void FlySetAVClientPostionValue( 
												  int roomID, 
												  int tableID,
												  int deskID
												  );

void  SetOneRemoteShowHWND( HWND myhRemoteWnd, int deskInt);
public:
	CSkinButton				m_BtHideList;					//隐藏左列表
	CSkinButton			    m_BtYuYin;//语音按钮
	CSkinButton							m_btQuitGame;					//退出游戏
	CSkinButton							m_btGameOption;					//游戏设置
	CSkinButton							m_btGameRule;					//游戏规则

	CStatic	m_showSelfVideo;//显示本地视频

	CStatic	m_showRemoteVideo;

	CStatic	m_showRemoteVideo1;
	CStatic	m_showRemoteVideo2;

	CStatic	m_showRemoteVideo3;

	CLabelEx     m_MyselfOpKuang;
	CLabelEx     m_OpKuang;
	CLabelEx     m_OpKuang1;
	CLabelEx     m_OpKuang2;
	CLabelEx     m_OpKuang3;
//CLabelEx     m_OpKuang, m_OpKuang1, m_OpKuang2, m_OpKuang3;

	//界面辅助
private:
	bool								m_bReset;						//是否重置
	bool								m_bRectify;						//调整标志
	bool								m_bInitDialog;					//初始标志

	//配置变量
private:
	bool								m_bAllowSound;					//允许声音

	//控件变量
protected:
	CSkinSplitter						m_VorSplitter;					//拆分控件
	CGameFrameView						* m_pGameFrameView;				//视图指针
	CGameFrameControl					* m_pGameFrameControl;			//控制视图

	//内核变量
private:
	CClientKernelSink					* m_pKernelSink;				//钩子接口
	CUserFaceResHelper					m_UserFaceResHelper;			//头像组件
	CClientKernelHelper					m_ClientKernelHelper;			//游戏逻辑
	CMessageProxyHelper					m_MessageProxyHelper;			//聊天代理

	//函数定义
public:
	//构造函数
	CGameFrameDlg(CGameFrameView * pGameFrameView);
	//析构函数
	virtual ~CGameFrameDlg();

	//拆分条接口
public:
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame()=NULL;
	//重置框架
	virtual void ResetGameFrame()=NULL;
	//游戏设置
	virtual void OnGameOptionSet()=NULL;
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)=NULL;
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)=NULL;
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)=NULL;
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize)=NULL;

	//特殊继承
private:
	//调整界面
	virtual void RectifyControl(int nWidth, int nHeight);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);

	//功能函数
public:
	//发送函数
	bool SendData(WORD wSubCmdID);
	//发送函数
	bool SendData(WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送准备
	bool SendUserReady(void * pBuffer, WORD wDataSize);
	//设置定时器
	bool SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse);
	//删除定时器
	bool KillGameTimer(UINT nTimerID);
	//播放声音
	bool PlayGameSound(LPCTSTR pszSoundName);
	//播放声音
	bool PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName);

	//消息函数
public:
	//插入图片
	bool InsertImage(CBitmap * pBitmap);
	//插入名字
	bool InsertUserName(LPCTSTR pszUserName);
	//系统消息
	bool InsertSystemString(LPCTSTR pszString);
	//特色消息
	bool InsertShtickString(LPCTSTR pszString, COLORREF crColor, bool bNewLine);
	//普通消息
	bool InsertGeneralString(LPCTSTR pszString, COLORREF crColor, bool bNewLine);

	//辅助函数
public:
	//激活界面
	void ActiveGameFrame();
	//切换椅子
	WORD SwitchViewChairID(WORD wChairID);

	//声音控制
public:
	//是否启用
	bool IsEnableSound();
	//启用声音
	void EnableSound(bool bEnable);

	//信息函数
public:
	//旁观模式
	bool IsLookonMode();
	//允许旁观
	bool IsAllowLookon();
	//自己位置
	WORD GetMeChairID();
	//时间位置
	WORD GetTimeChairID();
	//获取用户
	const tagUserData * GetUserData(WORD wChairID);
	//内核接口
	void * GetClientKernel(const IID & Guid, DWORD dwQueryVer);

	//游戏状态
public:
	//游戏状态
	BYTE GetGameStatus();
	//游戏状态
	void SetGameStatus(BYTE bGameStatus);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定消息
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//消息映射
private:
//发送动态语音
LRESULT OnSendDTSound(WPARAM wparam, LPARAM lparam);
	//位置改变
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//位置改变
	afx_msg void OnWindowPosChanging(WINDOWPOS * lpwndpos);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//游戏配置
	afx_msg void OnBnClickedOption();
	//游戏规则
	afx_msg void OnBnClickedRule();
	//退出按钮
	afx_msg void OnBnClickedQuitGame();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif