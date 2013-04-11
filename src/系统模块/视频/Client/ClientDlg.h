// ClientDlg.h : 头文件
//

#pragma once
#include "myfile\videoclient.h"
#include "afxwin.h"


// CClientDlg 对话框
class CClientDlg : public CDialog
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CVideoClient m_VideoClient;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonLogin();
	CComboBox m_ComboBox_Server;
	CComboBox m_ComboBox_Room;
	CComboBox m_ComboBox_Desk;
	CEdit m_Edit_Port;
	CStatic m_Static_Pic1;
	CStatic m_Static_Pic2;
	CStatic m_Static_Pic3;
	CStatic m_Static_Pic4;
	CStatic m_Static_Pic5;
	CStatic m_Static_Pic6;
	CStatic m_Static_Pic7;
	CStatic m_Static_Pic8;
	CStatic m_Static_Pic9;
	CStatic m_Static_Pic10;
	afx_msg void OnBnClickedButtonDisconnect();
	CComboBox m_ComboBox_Pos;
};
