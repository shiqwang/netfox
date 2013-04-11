#include "Stdafx.h"
#include "SkinEdit.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define DEF_FOCUSTX_COLOR			RGB(255,0,0)							//默认颜色
#define DEF_FOCUSBK_COLOR			RGB(255,255,0)							//默认颜色
#define DEF_NOFOCUSTX_COLOR			RGB(10,10,10)							//默认颜色
#define DEF_NOFOCUSBK_COLOR			RGB(255,255,255)						//默认颜色
#define DEF_DISFOCUSTK_COLOR		RGB(50,50,50)							//默认颜色
#define DEF_DISFOCUSBK_COLOR		RGB(125,125,125)						//默认颜色

#define DEF_FOCUSTX_COLOR_EX		RGB(255,0,0)							//默认颜色
#define DEF_FOCUSBK_COLOR_EX		RGB(255,255,0)							//默认颜色
#define DEF_NOFOCUSTX_COLOR_EX		RGB(10,10,10)							//默认颜色
#define DEF_NOFOCUSBK_COLOR_EX		RGB(255,255,255)						//默认颜色
#define DEF_DISFOCUSTK_COLOR_EX		RGB(50,50,50)							//默认颜色
#define DEF_DISFOCUSBK_COLOR_EX		RGB(125,125,125)						//默认颜色

//静态变量
CSkinEditAttribute					CSkinEdit::m_SkinAttribute;				//界面属性

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSkinEdit, CEdit)

BEGIN_MESSAGE_MAP(CSkinEdit, CEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEditAttribute::CSkinEditAttribute() : CSkinAttribute(TEXT("CSkinEdit"))
{
	//初始化变量
	m_crFocusTX=DEF_FOCUSTX_COLOR;
	m_crFocusBK=DEF_FOCUSBK_COLOR;
	m_crNoFocusTX=DEF_NOFOCUSTX_COLOR;
	m_crNoFocusBK=DEF_NOFOCUSBK_COLOR;
	m_crDisFocusTK=DEF_DISFOCUSTK_COLOR;
	m_crDisFocusBK=DEF_DISFOCUSBK_COLOR;

	//注册属性
	RegisterAttribute(SKIN_EDIT_CRFOCUSTX,&m_crFocusTX,false);
	RegisterAttribute(SKIN_EDIT_CRFOCUSBK,&m_crFocusBK,false);
	RegisterAttribute(SKIN_EDIT_CRNOFOCUSTX,&m_crNoFocusTX,false);
	RegisterAttribute(SKIN_EDIT_CRNOFOCUSBK,&m_crNoFocusBK,false);
	RegisterAttribute(SKIN_EDIT_CRDISFOCUSTX,&m_crDisFocusTK,false);
	RegisterAttribute(SKIN_EDIT_CRDISFOCUSBK,&m_crDisFocusBK,false);

	return;
}

//析构函数
CSkinEditAttribute::~CSkinEditAttribute() 
{
}

//加载配置
bool __cdecl CSkinEditAttribute::LoadSkinOption()
{
	//读取字体颜色
	m_crFocusTX=ReadColor(SKIN_EDIT_CRFOCUSTX,DEF_FOCUSTX_COLOR);
	m_crFocusBK=ReadColor(SKIN_EDIT_CRFOCUSBK,DEF_FOCUSBK_COLOR);
	m_crNoFocusTX=ReadColor(SKIN_EDIT_CRNOFOCUSTX,DEF_NOFOCUSTX_COLOR);
	m_crNoFocusBK=ReadColor(SKIN_EDIT_CRNOFOCUSBK,DEF_NOFOCUSBK_COLOR);
	m_crDisFocusTK=ReadColor(SKIN_EDIT_CRDISFOCUSTX,DEF_DISFOCUSTK_COLOR);
	m_crDisFocusBK=ReadColor(SKIN_EDIT_CRDISFOCUSBK,DEF_DISFOCUSBK_COLOR);

	//建立对象
	m_brFocus.DeleteObject();
	m_brNoFocus.DeleteObject();
	m_brDisbale.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	return true;
}

//保存配置
bool __cdecl CSkinEditAttribute::SaveSkinOption()
{
	WriteColor(SKIN_EDIT_CRFOCUSTX,m_crFocusTX);
	WriteColor(SKIN_EDIT_CRFOCUSBK,m_crFocusBK);
	WriteColor(SKIN_EDIT_CRNOFOCUSTX,m_crNoFocusTX);
	WriteColor(SKIN_EDIT_CRNOFOCUSBK,m_crNoFocusBK);
	WriteColor(SKIN_EDIT_CRDISFOCUSTX,m_crDisFocusTK);
	WriteColor(SKIN_EDIT_CRDISFOCUSBK,m_crDisFocusBK);
	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEdit::CSkinEdit()
{
	m_bFocus=false;
}

//析构函数
CSkinEdit::~CSkinEdit()
{
}

//绘画控件
HBRUSH CSkinEdit::CtlColor(CDC * pDC, UINT nCtlColor)
{
	pDC->SetBkMode(OPAQUE);
	if (IsWindowEnabled())
	{
		if (m_bFocus) 
		{
			pDC->SetTextColor(m_SkinAttribute.m_crFocusTX);
			pDC->SetBkColor(m_SkinAttribute.m_crFocusBK);
			return m_SkinAttribute.m_brFocus;
		}
		pDC->SetTextColor(m_SkinAttribute.m_crNoFocusTX);
		pDC->SetBkColor(m_SkinAttribute.m_crNoFocusBK);
		return m_SkinAttribute.m_brNoFocus;
	}
	pDC->SetTextColor(m_SkinAttribute.m_crDisFocusBK);
	pDC->SetBkColor(m_SkinAttribute.m_crDisFocusBK);
	return m_SkinAttribute.m_brDisbale;
}

//得到焦点
void CSkinEdit::OnSetFocus(CWnd * pOldWnd)
{
	m_bFocus=true;
	Invalidate(TRUE);
	__super::OnSetFocus(pOldWnd);
}

//失去焦点
void CSkinEdit::OnKillFocus(CWnd * pNewWnd)
{
	m_bFocus=false;
	Invalidate(TRUE);
	__super::OnKillFocus(pNewWnd);
}

//////////////////////////////////////////////////////////////////////////
