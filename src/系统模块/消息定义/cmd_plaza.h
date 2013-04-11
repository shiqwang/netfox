#ifndef CMD_PLAZA_HEAD_FILE
#define CMD_PLAZA_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//广场版本
#define VER_PLAZA_LOW					2								//广场版本
#define VER_PLAZA_HIGH					1								//广场版本
#define VER_PLAZA_FRAME					MAKELONG(VER_PLAZA_LOW,VER_PLAZA_HIGH)

//////////////////////////////////////////////////////////////////////////
//登录错误标识

#define ERC_GP_LOGON_SUCCESS			0								//登陆成功
#define ERC_GP_ACCOUNTS_NOT_EXIST		1								//帐号不存在
#define ERC_GP_LONG_NULLITY				2								//禁止登录
#define ERC_GP_PASSWORD_ERCOR			3								//密码错误

//////////////////////////////////////////////////////////////////////////
//登录命令码

#define MDM_GP_LOGON					1								//广场登录

#define SUB_GP_LOGON_ACCOUNTS			1								//帐号登录
#define SUB_GP_LOGON_USERID				2								//I D 登录
#define SUB_GP_REGISTER_ACCOUNTS		3								//注册帐号

#define SUB_GP_LOGON_SUCCESS			100								//登陆成功
#define SUB_GP_LOGON_ERROR				101								//登陆失败
#define SUB_GP_LOGON_FINISH				102								//登陆完成

//帐号登录
struct CMD_GP_LogonByAccounts
{
	DWORD								dwPlazaVersion;					//广场版本
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	TCHAR								szRealityPass[PASS_LEN];		//真实密码
};

//I D 登录
struct CMD_GP_LogonByUserID
{
	DWORD								dwPlazaVersion;					//广场版本
	DWORD								dwUserID;						//用户 I D
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//注册帐号
struct CMD_GP_RegisterAccounts
{
	WORD								wFaceID;						//头像标识
	DWORD								dwPlazaVersion;					//广场版本
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//登陆成功
struct CMD_GP_LogonSuccess
{
	BYTE								cbGender;						//用户性别
	BYTE								cbMember;						//会员等级
	WORD								wFaceID;						//头像索引
	DWORD								dwUserID;						//用户 I D
	DWORD								dwGroupID;						//社团索引
	DWORD								dwExperience;					//用户经验
	DWORD								dwUserRight;					//用户等级
	DWORD								dwMasterRight;					//管理权限
};

//登陆失败
struct CMD_GP_LogonError
{
	LONG								lErrorCode;						//错误代码
	TCHAR								szErrorDescribe[128];			//错误消息
};

//////////////////////////////////////////////////////////////////////////
//游戏列表命令码

#define MDM_GP_SERVER_LIST				2								//列表信息

#define SUB_GP_LIST_TYPE				100								//类型列表
#define SUB_GP_LIST_KIND				101								//种类列表
#define SUB_GP_LIST_STATION				102								//站点列表
#define SUB_GP_LIST_SERVER				103								//房间列表
#define SUB_GP_LIST_FINISH				104								//发送完成
#define SUB_GP_LIST_CONFIG				105								//列表配置

#define SUB_GP_LIST_PROCESS				106								//进程列表

//列表配置
struct CMD_GP_ListConfig
{
	BYTE								bShowOnLineCount;				//显示人数
};

//////////////////////////////////////////////////////////////////////////
//系统命令码

#define MDM_GP_SYSTEM					3								//系统命令

#define SUB_GP_VERSION					100								//版本通知
#define SUB_SP_SYSTEM_MSG				101								//系统消息

//版本通知
struct CMD_GP_Version
{
	BYTE								bNewVersion;					//更新版本
	BYTE								bAllowConnect;					//允许连接
};

//////////////////////////////////////////////////////////////////////////

#endif