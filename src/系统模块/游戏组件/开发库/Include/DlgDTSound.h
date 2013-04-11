#pragma once
#include "resource.h"
#include "FlySoundText.h"
#include "GameFrameDlg.h"
// CDlgDTSound 对话框
#define DT_BUT_MAX_NUM	20 //最多可以建立的个数

class CDlgDTSound : public CDialog
{
	DECLARE_DYNAMIC(CDlgDTSound)
public:
		CFlySoundText myCFlySoundText;
	CGameFrameDlg						* m_pGameFrameDlg;				//对话框
private:
	CBitmap  m_bmp;
	//获得播放声音
	CString GetPlaySoundFileStr(CString inStr, CString *charStr);

	int currentTypeInt;//当前选择的类型
	int currentSoundInt;//当前歌曲ID


	int minDTButID;//动态按钮的开始值
	int DTButAllNum;//动态按钮个数
//	CButton * dtButList[DT_BUT_MAX_NUM];//动态按钮数组
	CSkinButton * dtButList[DT_BUT_MAX_NUM];//动态按钮数组
	//建立所有的动态控件
	void CreateAllButtons();

	//插入语音资料
	void InsertOneTypeSoundList(int kindInt);
	CListCtrl	m_mySoundList;
public:
	//构造函数
	CDlgDTSound(CGameFrameDlg * pGameFrameDlg); 
//	CDlgDTSound(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDTSound();

// 对话框数据
//	enum { IDD = IDD_DLG_DT_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg	void OnDTButClick(UINT   nID);
	afx_msg void OnDTButClick2();
	afx_msg void OnDblclkListSound(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};
