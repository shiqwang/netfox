// DlgDTSound.cpp : 实现文件
//

#include "stdafx.h"
//#include "JDFace.h"
#include "DlgDTSound.h"


// CDlgDTSound 对话框

IMPLEMENT_DYNAMIC(CDlgDTSound, CSkinDialogEx)
/*
CDlgDTSound::CDlgDTSound(CWnd* pParent )
	: CDialog(CDlgDTSound::IDD, pParent)
{
}


CDlgDTSound::CDlgDTSound(CGameFrameDlg * pGameFrameDlg) : CDialog(IDD_DLG_DT_SOUND), m_pGameFrameDlg(pGameFrameDlg)
{
		return;
}
*/
//构造函数
CDlgDTSound::CDlgDTSound(CWnd * pParentWnd) : CSkinDialogEx(IDD_DLG_DT_SOUND)
{
	m_pParentWnd=NULL;
//	m_dwTargetUserID=0L;
	m_pParentWnd=pParentWnd;
//	m_pTargetUserItem=NULL;
}

CDlgDTSound::~CDlgDTSound()
{
}

void CDlgDTSound::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SOUND, m_mySoundList);
}


BEGIN_MESSAGE_MAP(CDlgDTSound, CSkinDialogEx)
		ON_WM_PAINT()
//ON_BN_CLICKED(ID_BUT_DT0, OnDTButClick2)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_SOUND, OnDblclkListSound)
ON_CONTROL_RANGE(BN_CLICKED,   ID_BUT_DT0,   ID_BUT_DT19,   OnDTButClick) 
//ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgDTSound 消息处理程序
BOOL CDlgDTSound::OnInitDialog()
{
//	CDialog::OnInitDialog();
	__super::OnInitDialog();	
	CString iniFileNameStr;
	iniFileNameStr =".\\Scenesound_boy.ini";
	myCFlySoundText.ReadDataFromIni(iniFileNameStr);

	m_bmp.LoadBitmap(IDB_PIC_DT_SOUND);
	
    BITMAP bm;
    m_bmp.GetBitmap(&bm);
	
    CRect rtWindow;
    GetWindowRect(&rtWindow);
    rtWindow.right = rtWindow.left+bm.bmWidth;
    rtWindow.bottom =rtWindow.top +bm.bmHeight;
    MoveWindow(&rtWindow);

	
	CreateAllButtons();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CDlgDTSound::OnDTButClick2()
{
OnDTButClick(ID_BUT_DT0);
}
void CDlgDTSound::OnDTButClick(UINT   nID)
{
	
	CString showMsg;
	
	
	CRect crButton; 
	dtButList[nID-ID_BUT_DT0]->GetWindowRect(crButton); 
	currentTypeInt = nID-ID_BUT_DT0;	
    CRect crLocation(crButton); 
	dtButList[currentTypeInt]->GetParent()->ScreenToClient(crLocation); 
	
	showMsg.Format("id=%d, x=%d, y=%d",    nID , crLocation.right, crLocation.top );
//	MessageBox(showMsg);
	
	InsertOneTypeSoundList(currentTypeInt);
}

//插入语音资料
void CDlgDTSound::InsertOneTypeSoundList(int kindInt)
{
	
/*
使用的是InsertItem和setItemText

  其中InsertItem用于向列表控件插入一行，并设置第零列的值，如果有多列，该行其他列的值要使用SetItemText来设置数据。
	
	*/
	m_mySoundList.DeleteAllItems();
	int InsertItem(int nItem, LPCTSTR lpszItem);
	int useTextInt = myCFlySoundText.mySceneItemList[kindInt].useTextInt;
	CString showMsg;
	
	for (int i=0; i <useTextInt ; i++)
	{
		showMsg.Format("%s",			myCFlySoundText.mySceneItemList[kindInt].TextList[i].showInfo);
		
		
		m_mySoundList.InsertItem(i, showMsg);
	}
}

//建立所有的动态控件
void CDlgDTSound::CreateAllButtons()
{
	minDTButID = ID_BUT_DT0;//动态按钮的开始值
	DTButAllNum=myCFlySoundText.useSceneInt;//动态按钮个数
	
	//	int picID = IDB_BITMAP_DT0;
	CString butStr;
	int butBaseX = 9;
	int butBaseY = 42;
	int butX = butBaseX;
	int butY=20;
	int butW=45;
	int butH=20;
	int butXBlank = 40;
	int butYBlank = 20;
	
	for (int i=0; i <DTButAllNum && i<DT_BUT_MAX_NUM; i++)
	{
		butStr.Format("%s", myCFlySoundText.mySceneItemList[i].TypeName);
		butY =(butH+butYBlank)*i +butBaseY ;
		//dtButList[i] =  new CButton;
		dtButList[i] =  new CSkinButton;
		dtButList[i]->Create( butStr,WS_CHILD,CRect(butX,butY,butX+butW,butY+butH),this,minDTButID+i);
		//	dtButList[i]->LoadBitmapFromResource4(picID+i, true);
		dtButList[i]->ShowWindow(true);	
	}//End for (int i=0; i <DTButAllNum && i<DT_BUT_MAX_NUM; i++)
	
	int x,y,w,h;
	x = 65;
	y=butBaseY;
	w=180;
	h=310;
	m_mySoundList.MoveWindow(CRect(x, y,x+w,y+h));
}
void CDlgDTSound::OnDblclkListSound(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	AfxMessageBox("222");
	//m_mySoundList
	//m_mySoundList.getseld
	//GetItemData(int nItem) 
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	int nItem = pListNotify->iItem;
	if ((nItem!=-1))
	{
	//	CGameRoomEx * pGameRoom=(CGameRoomEx *)m_pParentWnd;
	//	UserItemStruct * pUserItem=(UserItemStruct *)m_UserList.GetItemData(pListNotify->iItem);
	//	if (pUserItem==NULL) return;
	//	pGameRoom->OpenUserMenu(pUserItem);

	//	int getSelInt = m_mySoundList.GetItemData(nItem);
		CString showMsg;
			//获得场景名称
		SOUND_TEXT_ITEM  mySoundItemPtr;
				myCFlySoundText.GetOneSoundDataItem(currentTypeInt,nItem,&mySoundItemPtr );

		showMsg.Format(" nItem=%d  type=%s,name=%s",  nItem, myCFlySoundText.GetOneSceneName(currentTypeInt), mySoundItemPtr.showInfo);
		//AfxMessageBox(showMsg);
		currentSoundInt= nItem;

		GetParent()->SendMessage(WM_PLAY_DT_SOUND, currentTypeInt, currentSoundInt);

	}
	else
	{
		currentSoundInt=-1;
	}


	*pResult = 0;
}

void CDlgDTSound::OnPaint()
{
if (IsIconic())
	{
		CPaintDC dc(this);
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
//		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting
		
		CDC picDC;	
		picDC.CreateCompatibleDC (&dc);
		
		CBitmap *pOldBmp;
		pOldBmp = picDC.SelectObject (&m_bmp);
		
		BITMAP bm;
		m_bmp.GetBitmap(&bm);
		
		
		dc.BitBlt (0,0,bm.bmWidth ,bm.bmHeight,&picDC,0,0,SRCCOPY);
		dc.SelectObject(pOldBmp);  
		
		CDialog::OnPaint();
	} 


}
/*
void CDlgDTSound::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect myRect( 235, 4, 235+20, 4+20);
	if ( myRect.PtInRect(point))
	ShowWindow(false);
}
*/