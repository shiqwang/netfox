#ifndef DLG_LOGON_HEAD_FILE
#define DLG_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "..\..\共享组件\公共服务\HtmlBrowser.h"

//////////////////////////////////////////////////////////////////////////

//登录模式
enum enLogonMode
{
	LogonMode_Accounts,			//帐号登录
	LogonMode_UserID,			//I D 登录
};

//注册表项名
#define REG_OPTION_LOGON		TEXT("OptionLogon")
#define REG_USER_INFO			TEXT("Software\\114PKGame\\GamePlaza\\UserInfo")
#define REG_LOGON_SERVER		TEXT("Software\\114PKGame\\GamePlaza\\LogonServer")

//////////////////////////////////////////////////////////////////////////

//登陆 TAB 控件
class CTabCtrlLogon : public CTabCtrl
{
	//函数定义
public:
	//构造函数
	CTabCtrlLogon();
	//析够函数
	virtual ~CTabCtrlLogon();

	//消息函数
protected:
	//重画函数	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//用户注册
class CDlgRegister : public CSkinDialogEx
{
	friend class CDlgLogon;

	//登陆信息
public:
	WORD								m_wFaceID;
	TCHAR								m_szAccounts[NAME_LEN];			//游戏帐号
	CString								m_strPassWord;					//登录密码

	//控件变量
public:
	CImageList							m_ImageList;
	CComboBoxEx							m_FaceSelect;
	CSkinButton							m_btLogon;						//登陆按钮
	CSkinButton							m_btCancel;						//取消按钮
	CSkinHyperLink						m_LineMainPage;					//游戏主页
	CSkinHyperLink						m_LinePassWord;					//密码保护

	//函数定义
public:
	//构造函数
	CDlgRegister();
	//析构函数
	virtual ~CDlgRegister();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//登陆对话框
class CDlgLogon : public CSkinDialogEx
{
	friend class CRoomViewItem;
	friend class CPlazaViewItem;

	//登陆信息
protected:
	WORD								m_wFaceID;						//头像标识
	DWORD								m_dwUserID;						//用户 I D
	TCHAR								m_szAccounts[NAME_LEN];			//游戏帐号
	CString								m_strPassWord;					//登录密码
	enLogonMode							m_LogonMode;					//登录模式

	CHtmlBrower                         m_pRegisterBrower;


	//辅助信息
protected:
	bool								m_bRegister;					//注册标志
	bool								m_bChangePassWord;				//改变标志

	//连接信息
protected:
	CString								m_strLogonServer;				//服务器地址
	static tagProxyInfo					m_ProxyInfo;					//代理信息

	//位置变量
protected:
	int									m_nFullWidth;					//全部宽度
	int									m_nFullHeight;					//全部高度
	bool								m_bNetOption;					//网络设置
	CRect								m_rcNormalFrame;				//框架位置

	//控件变量
public:
	CSkinButton							m_btLogon;						//登陆按钮
	CSkinButton							m_btCancel;						//取消按钮
	CSkinButton							m_btDelete;						//删除按钮
	CSkinButton							m_btRegister;					//注册按钮
	CSkinButton							m_btNetOption;					//网络按钮
	CTabCtrlLogon						m_TabLogonMode;					//登陆选择
	CSkinHyperLink						m_LineMainPage;					//游戏主页
	CSkinHyperLink						m_LinePassWord;					//密码保护
	CSkinHyperLink						m_LineGetPassWord;				//取回密码
    CSkinHyperLink                      m_ApplyAccount;


	CStatic                             m_LoginServer;
	CStatic                             m_Account;
	CStatic                             m_Password;
	//CCheckBox               　　　　　　m_RememberPass;



	//函数定义
public:
	//构造函数
	CDlgLogon();
	//析构函数
	virtual ~CDlgLogon();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//获取服务器
	LPCTSTR GetLogonServer() { return m_strLogonServer; }
	//发送登录包
	bool SendLogonPacket(IClientSocket * pIClientSocke);
	//登陆成功处理
	bool OnLogonSuccess();

	//内部函数
private:
	//加载服务器
	void LoadLogonServer();
	//读取帐号
	void LoadAccountsInfo();
	//效验输入
	bool CheckLogonInput(bool bShowError);
	//设置模式
	void SetLogonMode(enLogonMode LogonMode);
    void Setup();
	//辅助函数
private:
	//更新密码
	void UpdateUserPassWord(DWORD dwUserDBID);
	//更新选择
	void UpdateUserComboBox(UINT uComboBoxID);
	//查找字符
	int ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString);

	//消息函数
public:
	//注册帐号
	afx_msg void OnRegisterAccounts();
	//删除用户
	afx_msg void OnDeleteAccounts();
	//密码改变
	afx_msg void OnEnChangePassword();
	//选择改变
	afx_msg void OnSelchangeAccounts();
	//选择改变
	afx_msg void OnSelchangeUserID();
	//网络设置
	afx_msg void OnBnClickedNetOption();
	//类型改变
	afx_msg void OnTcnSelchangeLogonType(NMHDR * pNMHDR, LRESULT * pResult);
	//退出
	afx_msg void OnExit();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif