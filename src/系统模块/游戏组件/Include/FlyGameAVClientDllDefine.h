

//功能:启动服务
//输入:
//输出:
typedef int(*StartAVClient)( 
	HWND myhWnd,

												  char * serverIP,
												  int portInt,
												  int userID, 
												  int roomID, 
												  int tableID,
												  int deskID
												  );

//功能:停止服务
//输入:
//输出:
typedef int(*StopAVClient)();

//功能:发送注册消息
//输入:
//输出:
typedef int(*AVClientSendRegMsg)();

//功能:发送取消注册消息
//输入:
//输出:
typedef int(*AVClientSendUnRegMsg)();

//功能:获取同DESK玩家列表
//输入:
//输出:
typedef int(*GetOneTablePlayerList)();


//功能:传入远程HWND
//输入:
//输出:
typedef int(*SetRemoteShowHWND)( 
												  HWND myhRemoteWnd,
												  int deskInt
												  );


//功能:控制用户列表/人数,具体信息,清理
//输入:
//输出:
typedef int(*OpPlayerList)( int opType  );

//功能:控制本地发送
//输入:语音视频类型;开关类型
//DATA_TYPE_AUDIO, DATA_TYPE_VIDEO;
//AV_OP_TYPE_CLOSE, AV_OP_TYPE_OPEN
//输出:
typedef int(*OpNetSendMode)(int avType, int opType);

//功能:设置位置参数变量
//输入:
//输出:
typedef int(*SetAVClientPostionValue)( 
												  int roomID, 
												  int tableID,
												  int deskID
												  );

