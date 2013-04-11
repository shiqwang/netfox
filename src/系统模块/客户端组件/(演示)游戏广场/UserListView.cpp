#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "UserListView.h"

//静态变量
CImageList		CUserListView::m_StatusImage;						//状态位图

//排序信息结构
struct tagListSortInfo
{
	int						nItemIndex;							//排序子项
	BYTE					cbAscendSort;						//升序标志
	WORD					cbFieldKind;						//字段类型
	CUserListView			* pRoomListCtrl;					//列表指针
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUserListView, CSkinListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserListView::CUserListView()
{
	m_uImageSpace=0;
	m_wColumnCount=0;
	m_cbAscendSort=FALSE;
	memset(m_wDataDescribe,0,sizeof(m_wDataDescribe));
}

//析构函数
CUserListView::~CUserListView()
{
}

//初始化列表
bool CUserListView::InitUserListView()
{
	//加载背景图
	if (m_StatusImage.GetSafeHandle()==NULL)
	{
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_USER_STATUS);
		Image.GetBitmap(&ImageInfo);
		m_StatusImage.Create(IMGAE_SPACE,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);
		m_StatusImage.Add(&Image,RGB(255,0,255));
	}
	SetImageList(&m_StatusImage,LVSIL_SMALL);

	//设置颜色
	SetTextColor(RGB(52,87,130));
	SetBkColor(RGB(217,242,255));
	SetTextBkColor(RGB(217,242,255));

	return true;
}

//设置列信息
bool CUserListView::SetColumnDescribe(tagColumnItem ColumnItem[], WORD wColumnCount)
{
	//效验状态
	ASSERT(ColumnItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;

	//删除旧信息
	m_wColumnCount=0;
	WORD wTempCount=m_SkinHeadCtrl.GetItemCount();
	for (WORD i=0;i<wTempCount;i++) DeleteColumn(0);

	//调整参数
	wColumnCount=__min(MAX_COLUMN-1,wColumnCount);

	//插入新信息
	SetRedraw(FALSE);
	for (WORD i=0;i<wColumnCount;i++)
	{
		m_wDataDescribe[i]=ColumnItem[i].wDataDescribe;
		if (m_wColumnCount==0) InsertColumn(m_wColumnCount++,ColumnItem[i].szColumnName,LVCFMT_LEFT,ColumnItem[i].wColumnWidth+m_uImageSpace);
		else InsertColumn(m_wColumnCount++,ColumnItem[i].szColumnName,LVCFMT_LEFT,ColumnItem[i].wColumnWidth);
	}
	SetRedraw(TRUE);

	return true;
}

//加入用户
bool CUserListView::InsertUserItem(IUserItem * pIUserItem)
{
	//效验状态
	ASSERT(pIUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem!=-1)
	{
		UpdateUserItem(pIUserItem);
		return true;
	}

	//写入用户
	if (m_wColumnCount>0) 
	{
		int nImageStation=GetImageStation(pIUserItem);
		int nInsertStation=GetInsertStation(pIUserItem);
		iItem=InsertItem(LVIF_IMAGE|LVIF_TEXT|LVIF_PARAM,nInsertStation,GetDisplayText(pIUserItem,0),
			0,0,nImageStation,(LPARAM)pIUserItem);
	}
	for (WORD i=1;i<m_wColumnCount;i++)
	{
		SetItem(iItem,i,LVIF_TEXT,GetDisplayText(pIUserItem,i),0,0,0,0);
	}
	
	return true;
}

//更新用户
bool CUserListView::UpdateUserItem(IUserItem * pIUserItem)
{
	//效验状态
	ASSERT(pIUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem==-1) return false;

	//更新用户
	if (m_wColumnCount>0)
	{
		int nImageStation=GetImageStation(pIUserItem);
		SetItem(iItem,0,LVIF_IMAGE|LVIF_TEXT,GetDisplayText(pIUserItem,0),nImageStation,0,0,0);
	}
	for (WORD i=1;i<m_wColumnCount;i++) SetItemText(iItem,i,GetDisplayText(pIUserItem,i));

	return true;
}

//删除用户
bool CUserListView::DeleteUserItem(IUserItem * pIUserItem)
{
	//效验状态
	ASSERT(pIUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem!=-1)
	{
		DeleteItem(iItem);
		return true;
	}

	return false;
}

//获取位图
int CUserListView::GetImageStation(IUserItem * pIUserItem)
{
	//变量定义
	tagUserData * pUserData=pIUserItem->GetUserData();
	BYTE cbUserStatus=pUserData->cbUserStatus;

	//权限对比
	int nRightExcursion=0;
	if (CUserRight::IsMatchUser(pUserData->dwUserRight)==true) nRightExcursion=12;
	else if (pUserData->cbCompanion==enCompanion_Friend) nRightExcursion=2;
	else if (pUserData->dwMasterRight!=0) nRightExcursion=4;
	else nRightExcursion=pUserData->cbMember;

	//计算偏移
	return nRightExcursion*6+(cbUserStatus>=US_FREE?(cbUserStatus-US_FREE):0);
}

//获取插入
int CUserListView::GetInsertStation(IUserItem * pIUserItem)
{
	return GetItemCount();
}

//获取字符
LPCTSTR CUserListView::GetDisplayText(IUserItem * pIUserItem, WORD wColumnPos)
{
	//效验参数
	ASSERT(pIUserItem!=NULL);
	if (wColumnPos>=m_wColumnCount) return NULL;

	//变量定义
	static TCHAR szBuffer[512];
	tagUserData * pUserData=pIUserItem->GetUserData();

	//生成显示
	switch (m_wDataDescribe[wColumnPos])
	{
	case DTP_USER_ID:			//用户 ID
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pIUserItem->GetUserData()->dwUserID);
			return szBuffer;
		}
	case DTP_USER_ACCOUNTS:		//用户名字
		{
			return pIUserItem->GetUserData()->szName;
		}
	case DTP_USER_TABLE:		//游戏桌号
		{
			WORD wTableID=pUserData->wTableID;
			if (wTableID!=INVALID_TABLE) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%3d"),wTableID+1);
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_USER_GOLD:			//用户金币
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lGold);
			return szBuffer;
		}
	case DTP_USER_SCORE:		//用户积分
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lScore);
			return szBuffer;
		}
	case DTP_USER_EXPERIENCE:	//用户经验
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lExperience);
			return szBuffer;
		}
	case DTP_USER_WIN:			//胜局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lWinCount);
			return szBuffer;
		}
	case DTP_USER_LOST:			//输局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lLostCount);
			return szBuffer;
		}
	case DTP_USER_DRAW:			//和局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lDrawCount);
			return szBuffer;
		}
	case DTP_USER_FLEE:			//逃局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lFleeCount);
			return szBuffer;
		}
	case DTP_USER_PLAY_COUNT:	//总局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pIUserItem->GetUserPlayCount());
			return szBuffer;
		}
	case DTP_WIN_RATE:			//用户胜率
		{
			if (pUserData->lWinCount>0)
			{
				LONG lAllCount=pIUserItem->GetUserPlayCount();
				double dWinRate=(double)(pUserData->lWinCount*100)/(double)lAllCount;
				if (dWinRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dWinRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_LOST_RATE:			//用户输率
		{
			if (pUserData->lLostCount>0)
			{
				LONG lAllCount=pIUserItem->GetUserPlayCount();
				double dLostRate=(double)(pUserData->lLostCount*100)/(double)lAllCount;
				if (dLostRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dLostRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_DRAW_RATE:			//用户和率
		{
			if (pUserData->lDrawCount>0)
			{
				LONG lAllCount=pIUserItem->GetUserPlayCount();
				double dDrawRate=(double)(pUserData->lDrawCount*100)/(double)lAllCount;
				if (dDrawRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dDrawRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_FLEE_RATE:			//用户逃率
		{
			if (pUserData->lFleeCount>0)
			{
				LONG lAllCount=pIUserItem->GetUserPlayCount();
				double dFleeRate=(double)(pUserData->lFleeCount*100)/(double)lAllCount;
				if (dFleeRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dFleeRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_USER_PING:			//网络延时
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),pUserData->wNetDelay);
			return szBuffer;
		}
	case DTP_GAME_LEVEL:		//游戏等级
		{
			return g_GlobalUnits.m_GameRankManager->GetGameRank(m_wKindID,pIUserItem->GetUserData()->lScore,m_wGameGenre);
		}
	}

	//默认显示
	szBuffer[0]=0;

	return szBuffer;
}

//绘画函数
void CUserListView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	int iItem=lpDrawItemStruct->itemID;
	int iCount=GetHeaderCtrl()->GetItemCount();
	int iBeen=rcItem.left;
	TCHAR szBuffer[1024];
	for (int i=0;i<iCount;i++)
	{
		int iWidth=GetColumnWidth(i);
		GetSubItemRect(iItem,i,LVIR_LABEL,rcItem);
		pDC->FillSolidRect(&rcItem,GetBkColor());
		GetItemText(iItem,i,szBuffer,sizeof(szBuffer));
		pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcItem,DT_LEFT|DT_END_ELLIPSIS);

/*		pDC->FillSolidRect(iBeen,rcItem.top,iWidth,rcItem.Height(),crColor[i]);
		GetItemText(iItem,i,szBuffer,sizeof(szBuffer));
		CRect rcRect;
		rcRect.SetRect(iBeen,rcItem.top,iWidth,rcItem.Height());
		pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcRect,DT_LEFT|DT_END_ELLIPSIS);
		pDC->
		iBeen+=iWidth;
*/
	}

/*	if (lpDrawItemStruct->itemState==0)
	{
		CDC BitmapDC;
		BitmapDC.CreateCompatibleDC(pDC);
		BitmapDC.SelectObject(&m_Bitmap);
		pDC->BitBlt(rcItem.left,rcItem.top,rcItem.Width(),rcItem.Height(),&BitmapDC,0,0,SRCCOPY);
		
	}
	else pDC->FillSolidRect(&rcItem,RGB(255,255,255)); 
*/
}

//////////////////////////////////////////////////////////////////////////

//点击列消息
void CUserListView::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	if (m_wColumnCount!=0)
	{
		//构造数据
		tagListSortInfo SortInfo;
		int iSubItem=pNMListView->iSubItem;
		SortInfo.cbAscendSort=m_cbAscendSort;
		SortInfo.pRoomListCtrl=this;
		SortInfo.nItemIndex=iSubItem;
		SortInfo.cbFieldKind=m_wDataDescribe[iSubItem];

		//排列列表
		SortItems(SortFun,(LPARAM)&SortInfo);
		m_cbAscendSort=!m_cbAscendSort;
	}

	return;
}

//排列函数
int CALLBACK CUserListView::SortFun(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//定义变量
	IUserItem * pUserItem1=(IUserItem *)lParam1;
	IUserItem * pUserItem2=(IUserItem *)lParam2;
	tagUserData * pUserData1=pUserItem1->GetUserData();
	tagUserData * pUserData2=pUserItem2->GetUserData();
    tagListSortInfo * pSortInfo=(tagListSortInfo *)lParamList;
	CUserListView * pListCtrl=pSortInfo->pRoomListCtrl;
	ASSERT(pUserItem1!=NULL);
	ASSERT(pUserItem2!=NULL);
	ASSERT(pSortInfo!=NULL);

	//自己放置顶
	tagGlobalUserData & GlobalInfo=g_GlobalUnits.GetGolbalUserData();
	if (pUserData1->dwUserID==GlobalInfo.dwUserID) return -1;
	if (pUserData2->dwUserID==GlobalInfo.dwUserID) return 1;

	//对比数据
	switch (pSortInfo->cbFieldKind)
	{
	case DTP_USER_ID:
		{
			int iCompRes=pUserData1->dwUserID>pUserData2->dwUserID?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_ACCOUNTS:
		{
			//好友放置顶
			BYTE cbConnection1=pUserData1->cbCompanion;
			BYTE cbConnection2=pUserData2->cbCompanion;
			if (((cbConnection1==enCompanion_Friend)&&(cbConnection2!=enCompanion_Friend))
				||((cbConnection1!=enCompanion_Friend)&&(cbConnection2==enCompanion_Friend)))
			{
				if (cbConnection1==enCompanion_Friend) return -1;
				else return 1;
			}
			
			//会员置顶
			if (pUserData1->cbMember!=pUserData2->cbMember)
			{
				if (pUserData1->cbMember>pUserData2->cbMember) return -1;
				else return 1;
			}

			int iCompRes=strcmp(pUserData1->szName,pUserData2->szName);
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_SCORE:
		{
			int iCompRes=pUserData1->lScore>pUserData2->lScore?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_GAME_LEVEL:
		{
			int iCompRes=pUserData1->lScore>pUserData2->lScore?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_GOLD:
		{
			int iCompRes=pUserData1->lGold>pUserData2->lGold?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_TABLE:
		{
			int iCompRes=pUserData1->wTableID>pUserData2->wTableID?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_EXPERIENCE:
		{
			int iCompRes=pUserData1->lExperience>pUserData2->lExperience?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_GROUP_NAME:
		{
			int iCompRes=strcmp(pUserData1->szGroupName,pUserData2->szGroupName);
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_WIN:
		{
			int iCompRes=pUserData1->lWinCount>pUserData2->lWinCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_LOST:
		{
			int iCompRes=pUserData1->lLostCount>pUserData2->lLostCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_DRAW:
		{
			int iCompRes=pUserData1->lDrawCount>pUserData2->lDrawCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_FLEE:
		{
			int iCompRes=pUserData1->lFleeCount>pUserData2->lFleeCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_WIN_RATE:
		{
			long lAllCount1=pUserData1->lWinCount+pUserData1->lLostCount;
			long lAllCount2=pUserData2->lWinCount+pUserData2->lLostCount;
			double dWinRate1=0.0,dWinRate2=0.0;
			if (lAllCount1!=0) dWinRate1=(double)(pUserData1->lWinCount)/(double)(lAllCount1);
			if (lAllCount2!=0) dWinRate2=(double)(pUserData2->lWinCount)/(double)(lAllCount2);
			int iCompRes=dWinRate1>dWinRate2?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
			break;
		}
	case DTP_FLEE_RATE:
		{
			long lAllCount1=pUserItem1->GetUserPlayCount();
			long lAllCount2=pUserItem2->GetUserPlayCount();
			double dFleeRate1=0.0,dFleeRate2=0.0;
			if (lAllCount1!=0) dFleeRate1=(double)(pUserData1->lFleeCount)/(double)(lAllCount1);
			if (lAllCount2!=0) dFleeRate2=(double)(pUserData2->lFleeCount)/(double)(lAllCount2);
			int iCompRes=dFleeRate1>dFleeRate2?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_PING:
		{
			int iCompRes=pUserData1->wNetDelay>pUserData2->wNetDelay?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_PLAY_COUNT:
		{
			long lAllCount1=pUserItem1->GetUserPlayCount();
			long lAllCount2=pUserItem2->GetUserPlayCount();
			int iCompRes=lAllCount1>lAllCount2?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	}

	return 0;
}
