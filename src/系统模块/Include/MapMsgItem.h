// MapMsgItem.h: interface for the CMapMsgItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPMSGITEM_H__1B16D5B3_04FE_4979_B4C6_F21E5A1EB457__INCLUDED_)
#define AFX_MAPMSGITEM_H__1B16D5B3_04FE_4979_B4C6_F21E5A1EB457__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//×ö³ÉÄ£°å
template <class T>
class CMapMsgItem : public CMapStringToPtr
{
public:
	void RemoveAllEx();
//	CMapMsgItem(int i);
		CMapMsgItem();
	virtual ~CMapMsgItem();

    void SetAtEx( LPCTSTR key, T * pnewVal);
    BOOL RemoveKeyEx(LPCTSTR key);

};
/*
template <class T>
CMapMsgItem<T>::CMapMsgItem(int i)
{

}
*/
template <class T>
CMapMsgItem<T>::CMapMsgItem()
{

}
template <class T>
CMapMsgItem<T>::~CMapMsgItem()
{
    RemoveAllEx();
}

template <class T>
void CMapMsgItem<T>::SetAtEx(LPCTSTR key, T * pnewVal)
{
    T *pItem = NULL;
    if(Lookup(key, (LPVOID&)pItem))
    {
        SetAt(key, (LPVOID)pnewVal);
        pnewVal->pNext = pItem;
    }
    else
    {
        SetAt(key, (LPVOID)pnewVal);
    }
}

template <class T>
BOOL CMapMsgItem<T>::RemoveKeyEx(LPCTSTR key)
{
    T *pAlmItem = NULL;
    T* pDelItem = NULL;
    if(Lookup(key, (LPVOID&)pAlmItem))
    {
        while(pAlmItem != NULL)
        {
            pDelItem = pAlmItem;
            pAlmItem = pDelItem->pNext;
            delete pDelItem;
            pDelItem = NULL;
        }
        RemoveKey(key);
        return TRUE;
    }
    return FALSE;
}

template <class T>
void CMapMsgItem<T>::RemoveAllEx()
{
    try
    {
        POSITION pos = GetStartPosition();
        while(pos)
        {
            CString strKey;
            T* pItem = NULL;
            T* pDelItem = NULL;
            GetNextAssoc(pos, strKey, (LPVOID&)pItem);
            while(pItem != NULL)
            {
                pDelItem = pItem;
                pItem = pDelItem->pNext;
                delete pDelItem;
                pDelItem = NULL;
            }
        }
        RemoveAll();
    }
    catch(...)
    {
    }
}

#endif // !defined(AFX_MAPMSGITEM_H__1B16D5B3_04FE_4979_B4C6_F21E5A1EB457__INCLUDED_)
