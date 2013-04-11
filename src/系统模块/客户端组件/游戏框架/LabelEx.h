#if !defined(AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_)
#define AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LabelEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLabelEx window

#define MESSAGE_CHOOSE_OBJ       WM_USER+52 

#define OBJ_MAX_NUM  6 

#define COLOR_TYPE_RED RGB(255,0,0) //RGB(0x0,0xf0,0xf0)
#define COLOR_TYPE_GREEN RGB(0,255,0) //RGB(0x60,0x60,0x60)
#define COLOR_TYPE_BLUE RGB(0,0,255) //RGB(0x60,0x60,0x60)

#define OBJ_W   20 //25//
#define OBJ_H   20 //25//
#define OBJ_BLANK 0//  3//
#define OBJ_START_X 0//  43//
#define OBJ_START_Y 0//  5//

#define OBJ_NUM  6//

typedef struct _DRAW_KUANG_OBJ
{
	int objInt;//DJ编号
	char djObjName[50];
	int m_showX;//开始位置
	int m_showY;//开始位置
}
DRAW_KUANG_OBJ;

struct ToolTipInfo : public CObject
{
	UINT			nControlID;		// ID of the control
	UINT			nInfoSize;		// number of lines in the info
	CStringArray	nControlInfo;	// container of the information
	COLORREF		nTextColor;		// text color
	COLORREF		nBackColor;		// background color
};


class CLabelEx : public CStatic
{
public:
	bool isActiveBool;

	int djObjW, djObjH;//DJ 高宽
	int djObjBlank;
	int djStartX,djStartY;//起始位置
	int djObjNumber;//可用OBJ
	//设置DJ数据
	void InitDJDataList(DRAW_KUANG_OBJ *myItem, int djUseNum);

	//获取被选择的对象
	//输入:0=MOUSE MOVE, 1=DOWN
	//返回:
	void GetOpSelectObj(int x, int y, int opType=0);

	int showModeInt;//显示模式
	int labelID;
	HWND  my_hWnd;
	DRAW_KUANG_OBJ objList[OBJ_MAX_NUM];
	void InitDrawKuang();
	void InitObjListData();
	void InitMyselfObjListData();

private:
	bool  isCreatePen;
	CString objNameStr[OBJ_NUM];
	
	int maxUseObjNum;//



	void SetOneObjData(int objInt, int showX, int Y);
	
	bool IsInRect(int x,int y, int showX, int showY);
	
	int GetSelectObjID(int x, int y);
	
	//int m_showX;
	//int m_showY;
	void DrawOneKuangPic(int showX, int showY);
	
	void DrawAllKuangPic();
	

	
	int GetSelectObjInt(int x,int showX , int showY);
	//0=mousemove;1=select
	void DrawSelectOneObj(int objInt,int opType, int showX, int showY);
	
//	void DrawAllObj(int showX, int showY);
	
	void DrawAllDJObj(int showX, int showY);

	void DrawAllKuangObj();
	
	CPen		pen0,pen1;
	CPen		pen2;//move
	
	CDC *mdc;
	CRect rect;
	CBitmap   *bgbmp;

// Construction
public:
	CLabelEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabelEx)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableHighLight(BOOL bHighLight);
	void SetClickedLabelBitmap(UINT ID);
	void SetMouseOverLabelBitmap(UINT ID);
	void SetClickedBGBitmap(UINT ID);
	void SetLabelBitmap(UINT ID);
	void SetMouseOverBGBitmap(UINT ID);
	void SetBGBitmap(UINT ID);
	void RedrawWindow();
	void EnableAutoUnderLine(BOOL bAutoUnderLine);
	void SetBkColor(COLORREF crBkColor);
	void SetBorder(BOOL bBorder,COLORREF crBorder);
	void SetUnderLine(BOOL bUnderLine,COLORREF crUnderLine);
	void SetTextColor(COLORREF crText);
	virtual ~CLabelEx();

	// Generated message map functions
protected:
	HCURSOR m_hHandCur;
	HCURSOR m_hXCur;

	CFont m_Font;
	BOOL m_bBorder;
	BOOL m_bClicked;
	BOOL m_bOver;
	BOOL m_bUnderLine,m_bBackUnderLine;
	BOOL m_bAutoUnderLine;
	BOOL m_bHighLight;
	CBitmap m_BGBm,m_MouseOverBGBm,m_ClickedBGBm;
	CBitmap m_LabelBm,m_MouseOverLabelBm,m_ClickedLabelBm;
	COLORREF m_crText,m_crBorder,m_crBG,m_crUnderLine;
	COLORREF m_crBackText;
	//{{AFX_MSG(CLabelEx)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABELEX_H__C9BE230C_B8E9_48DE_8022_F3D4F5260B65__INCLUDED_)
