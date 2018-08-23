
#include "StdAfx.h"

#include <ObjectCtrl.h>

#include "Resource.h"



// CBaseTree

CBaseTree::CBaseTree()
{
}

CBaseTree::~CBaseTree()
{
	(void)m_ImageList.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CBaseTree, CTreeCtrl)
END_MESSAGE_MAP()

// CBaseTree 消息处理程序

BOOL CBaseTree::_InitCtrl(ULONG uFontSize)
{
	ASSERT_RETURN_EX(m_hWnd, FALSE);

	(void)__super::ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS);

	if (0 < uFontSize)
	{
		m_fontGuide.setFontSize(*this, uFontSize);
	}

	return TRUE;
}

BOOL CBaseTree::InitCtrl(CBitmap *pBitmap, ULONG uFontSize)
{
	ASSERT_RETURN_EX(_InitCtrl(uFontSize), FALSE);

	if (pBitmap)
	{
		BITMAP bmp;
		(void)pBitmap->GetBitmap(&bmp);
		ASSERT_RETURN_EX(m_ImageList.Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32, 0, 0), FALSE);
		(void)m_ImageList.Add(pBitmap, RGB(0, 0, 0));

		(void)__super::SetImageList(&m_ImageList, TVSIL_NORMAL);
	}

	return TRUE;
}

BOOL CBaseTree::InitCtrl(const TD_ICONLIST& lstIcon, UINT uIconSize, ULONG uFontSize)
{
	ASSERT_RETURN_EX(_InitCtrl(uFontSize), FALSE);

	ASSERT_RETURN_EX(m_ImageList.Create(uIconSize, uIconSize, ILC_COLOR32, 0, lstIcon.size()), FALSE);
	for (auto hIcon : lstIcon)
	{
		(void)m_ImageList.Add(hIcon);
	}

	(void)__super::SetImageList(&m_ImageList, TVSIL_NORMAL);

	return TRUE;
}


HTREEITEM CBaseTree::InsertItem(HTREEITEM hParentItem, LPCTSTR lpszItem, DWORD_PTR dwData, int nImage)
{
	if (!hParentItem)
	{
		hParentItem = TVI_ROOT;
	}

	HTREEITEM hItem = __super::InsertItem(lpszItem, nImage, nImage, hParentItem);
	
	(void)__super::SetItemData(hItem, dwData);

	return hItem;
}

void CBaseTree::GetAllItems(std::list<HTREEITEM>& lstItems)
{
	HTREEITEM hRootItem = this->GetRootItem();
	while (hRootItem)
	{
		lstItems.push_back(hRootItem);

		GetChildItems(hRootItem, lstItems);

		hRootItem = this->GetNextItem(hRootItem, TVGN_NEXT);
	}
}

void CBaseTree::GetChildItems(HTREEITEM hItem, std::list<HTREEITEM>& lstItems)
{
	HTREEITEM hChildItem = this->GetChildItem(hItem);
	while (hChildItem)
	{
		lstItems.push_back(hChildItem);

		GetChildItems(hChildItem, lstItems);

		hChildItem = this->GetNextItem(hChildItem, TVGN_NEXT);
	}
}

BOOL CBaseTree::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_LBUTTONDOWN == message || WM_RBUTTONDOWN == message)
	{
		CPoint ptCursor(0,0);
		(void)::GetCursorPos(&ptCursor);

		this->ScreenToClient(&ptCursor);

		HTREEITEM hItem = this->HitTest(ptCursor);
		if (hItem)
		{
			(void)this->SelectItem(hItem);
		}
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}


// CCheckObjectTree

CCheckObjectTree::CCheckObjectTree()
{
}

CCheckObjectTree::~CCheckObjectTree()
{
	(void)m_StateImageList.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CCheckObjectTree, CObjectTree)
END_MESSAGE_MAP()

// CCheckObjectTree 消息处理程序

BOOL CCheckObjectTree::InitCtrl(CBitmap *pNormalBitmap, UINT uFontSize)
{
	ENSURE_RETURN_EX(__super::InitCtrl(pNormalBitmap, uFontSize), FALSE);


	HBITMAP hStateBitmap = ::LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_CHECKTREE_STATE));
	ASSERT_RETURN_EX(hStateBitmap, FALSE);

	CBitmap StateBitmap;
	StateBitmap.Attach(hStateBitmap);

	BITMAP bmp;
	(void)StateBitmap.GetBitmap(&bmp);

	ASSERT_RETURN_EX(m_StateImageList.Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32, 0, 0), FALSE);
	(void)m_StateImageList.Add(&StateBitmap, RGB(0,0,0));
	
	(void)StateBitmap.DeleteObject();

	(void)__super::SetImageList(&m_StateImageList, TVSIL_STATE);


	(void)__super::ModifyStyle(TVS_CHECKBOXES, 0);


	return TRUE;
}

HTREEITEM CCheckObjectTree::InsertObject(CTreeObject *pObject, CTreeObject *pParentObject)
{
	HTREEITEM hItem = __super::InsertObject(pObject, pParentObject);

	(void)__super::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(CS_Unchecked), TVIS_STATEIMAGEMASK);

	return hItem;
}

void CCheckObjectTree::SetItemCheckState(HTREEITEM hItem, ST_CheckState nState)
{
	(void)__super::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);

	this->SetChildItemsImageState(hItem);

	this->SetParentItemsImageState(hItem);
}

ST_CheckState CCheckObjectTree::GetItemCheckState(HTREEITEM hItem)
{
	return (ST_CheckState)(__super::GetItemState(hItem, TVIS_STATEIMAGEMASK) >>12);
}

void CCheckObjectTree::GetAllObjects(TD_TreeObjectList& lstObjects)
{
	std::list<HTREEITEM> lstItems;
	__super::GetAllItems(lstItems);

	for (std::list<HTREEITEM>::iterator itItem = lstItems.begin()
		; itItem != lstItems.end(); ++itItem)
	{
		lstObjects.push_back(__super::GetItemObject(*itItem));
	}
}

void CCheckObjectTree::GetAllObjects(TD_TreeObjectList& lstObjects, ST_CheckState nType)
{
	TD_TreeObjectList lstItemObjects;
	__super::GetAllObjects(lstItemObjects);

	for (TD_TreeObjectList::iterator itObject = lstItemObjects.begin()
		; itObject != lstItemObjects.end(); ++itObject)
	{
		if (nType == this->GetItemCheckState((*itObject)->m_hTreeItem))
		{
			lstObjects.push_back(*itObject);
		}
	}
}

void CCheckObjectTree::GetCheckedObjects(TD_TreeObjectList& lstObjects)
{
	TD_TreeObjectList lstChekedObjects;
	this->GetAllObjects(lstChekedObjects, CS_Checked);

	HTREEITEM hParentItem = NULL;
	for (TD_TreeObjectList::iterator itObject = lstChekedObjects.begin()
		; itObject != lstChekedObjects.end(); ++itObject)
	{
		hParentItem = this->GetParentItem((*itObject)->m_hTreeItem);

		if (!util::ContainerFind(lstChekedObjects, this->GetItemObject(hParentItem)))
		{
			lstObjects.push_back(*itObject);
		}
	}
}

BOOL CCheckObjectTree::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HTREEITEM hItem = NULL;

	if (WM_KEYDOWN == message)
	{
		int nChar = LOWORD(wParam);
		if (VK_SPACE == nChar)
		{
			hItem = __super::GetSelectedItem();
		}
	}
	else if (WM_LBUTTONDOWN == message)
	{
		CPoint ptCursor(0,0);
		(void)::GetCursorPos(&ptCursor);

		__super::ScreenToClient(&ptCursor);

		UINT nFlag = 0;
		hItem = __super::HitTest(ptCursor, &nFlag);
		if (TVHT_ONITEMSTATEICON != nFlag)
		{
			hItem = NULL;
		}
	}

	if (hItem)
	{
		UINT nState = this->GetItemCheckState(hItem);
		this->SetItemCheckState(hItem, nState!=CS_Checked? CS_Checked:CS_Unchecked);
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CCheckObjectTree::SetChildItemsImageState(HTREEITEM hItem)
{
	UINT nState = __super::GetItemState(hItem, TVIS_STATEIMAGEMASK);

	HTREEITEM hChildItem = __super::GetChildItem(hItem);

	while (hChildItem)
	{
		(void)__super::SetItemState(hChildItem, nState, TVIS_STATEIMAGEMASK);

		if (__super::GetChildItem(hChildItem))
		{
			this->SetChildItemsImageState(hChildItem);
		}

		hChildItem = __super::GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CCheckObjectTree::SetParentItemsImageState(HTREEITEM hItem)
{
	HTREEITEM hParentItem = hItem;

	HTREEITEM hChildItem = NULL;
	
	UINT nChildState = 0;

	UINT nChildState2 = 0;

	
	while (hParentItem = __super::GetParentItem(hParentItem))
	{
		if (CS_Nocheck == this->GetItemCheckState(hParentItem))
		{
			continue;
		}

		hChildItem = __super::GetChildItem(hParentItem);
		ASSERT(hChildItem);

		nChildState = this->GetItemCheckState(hChildItem);

		while (hChildItem = __super::GetNextItem(hChildItem, TVGN_NEXT))
		{
			nChildState2 = this->GetItemCheckState(hChildItem);

			if (CS_Nocheck == nChildState2)
			{
				continue;
			}

			if (nChildState != nChildState2)
			{
				nChildState = CS_Grayed;
				break;
			}
		}

		(void)__super::SetItemState(hParentItem, INDEXTOSTATEIMAGEMASK(nChildState), TVIS_STATEIMAGEMASK);
	}
}


// CObjectTree
CObjectTree::CObjectTree()
{
}

CObjectTree::~CObjectTree()
{
}

void CObjectTree::SetRootObject(CTreeObject *pObject)
{
	ASSERT_RETURN(pObject);

	(void)DeleteAllItems();

	(void)InsertObjectEx(pObject);

	(void)__super::SelectItem(pObject->m_hTreeItem);
	(void)__super::EnsureVisible(pObject->m_hTreeItem);
	(void)__super::Expand(pObject->m_hTreeItem, TVE_EXPAND);
}

HTREEITEM CObjectTree::InsertObject(CTreeObject *pObject, CTreeObject *pParentObject)
{
	HTREEITEM hParentItem = TVI_ROOT;
	if (pParentObject && pParentObject->m_hTreeItem)
	{
		TVITEM item;
		ZeroMemory(&item, sizeof(item));
		item.hItem = pParentObject->m_hTreeItem;
		item.mask = TVIF_PARAM;
		(void)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
		
		if ((CTreeObject*)item.lParam == pParentObject)
		{
			hParentItem = pParentObject->m_hTreeItem;
		}
	}
	
	pObject->m_hTreeItem = __super::InsertItem(hParentItem, pObject->GetTreeText()
		, (DWORD_PTR)pObject, pObject->GetTreeImage());
	ASSERT(pObject->m_hTreeItem);

	return pObject->m_hTreeItem;
}

HTREEITEM CObjectTree::InsertObjectEx(CTreeObject *pObject, CTreeObject *pParentObject)
{
	pObject->m_hTreeItem = InsertObject(pObject, pParentObject);
	ENSURE_RETURN_EX(pObject->m_hTreeItem, NULL);

	TD_TreeObjectList lstSubObjects;
	pObject->GetTreeChilds(lstSubObjects);

	for (TD_TreeObjectList::iterator itSubObject=lstSubObjects.begin()
		; itSubObject!=lstSubObjects.end(); ++itSubObject)
	{
		(void)InsertObjectEx(*itSubObject, pObject);
	}

	return pObject->m_hTreeItem;
}

CTreeObject *CObjectTree::GetSelectedObject()
{
	HTREEITEM hItem = this->GetSelectedItem();
	if (!hItem)
	{
		return NULL;
	}

	return GetItemObject(hItem);
}

CTreeObject *CObjectTree::GetItemObject(HTREEITEM hItem)
{
	return (CTreeObject*)__super::GetItemData(hItem);
}

CTreeObject *CObjectTree::GetParentObject(CTreeObject *pTreeObject)
{
	ASSERT_RETURN_EX(pTreeObject->m_hTreeItem, NULL);
	
	HTREEITEM hItemParent = __super::GetParentItem(pTreeObject->m_hTreeItem);
	ENSURE_RETURN_EX(hItemParent, NULL);

	return this->GetItemObject(hItemParent);
}

void CObjectTree::GetAllObjects(TD_TreeObjectList& lstObjects)
{
	std::list<HTREEITEM> lstItems;
	__super::GetAllItems(lstItems);

	for (std::list<HTREEITEM>::iterator itItem = lstItems.begin()
		; itItem != lstItems.end(); ++itItem)
	{
		lstObjects.push_back(this->GetItemObject(*itItem));
	}
}


// CObjectList

BOOL CObjectList::InitCtrl(UINT uFontSize, const ColumnList &lstColumns)
{
	ASSERT_RETURN_EX(m_hWnd, FALSE);
	(void)__super::ModifyStyle(0, LVS_SHOWSELALWAYS| LVS_SHAREIMAGELISTS);

	(void)__super::SetExtendedStyle(__super::GetExtendedStyle() 
		| LVS_EX_FULLROWSELECT |LVS_EX_HEADERDRAGDROP |LVS_EX_LABELTIP);

	if (0 < uFontSize)
	{
		m_fontGuide.setFontSize(*this, uFontSize);
	}

	if (!lstColumns.empty())
	{
		m_nColumnCount = lstColumns.size();
		for (ColumnList::const_iterator itColumn = lstColumns.begin(); itColumn != lstColumns.end()
			; ++itColumn, ++m_nColumnCount)
		{
			(void)__super::InsertColumn(m_nColumnCount, itColumn->first, 0, itColumn->second);
		}
	}

	return TRUE;
}

BOOL CObjectList::InitImage(const TD_ICONLIST& lstIcon, UINT uSize, UINT uSmallSize)
{
	if (0 == uSmallSize)
	{
		uSmallSize = uSize;
	}

	ASSERT_RETURN_EX(m_ImageList.Create(uSize, uSize, ILC_COLOR32, 0, lstIcon.size()), FALSE);
	ASSERT_RETURN_EX(m_ImageListSmall.Create(uSmallSize, uSmallSize, ILC_COLOR32, 0, lstIcon.size()), FALSE);
	for (auto hIcon : lstIcon)
	{
		(void)m_ImageList.Add(hIcon);
		(void)m_ImageListSmall.Add(hIcon);
	}
	(void)__super::SetImageList(&m_ImageList, LVSIL_NORMAL);
	(void)__super::SetImageList(&m_ImageListSmall, LVSIL_SMALL);

	return TRUE;
}

BOOL CObjectList::InitImage(CBitmap& Bitmap)
{
	BITMAP bmp;
	(void)Bitmap.GetBitmap(&bmp);
	
	ASSERT_RETURN_EX(m_ImageList.Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32, 0, 0), FALSE);
	(void)m_ImageList.Add(&Bitmap, RGB(0, 0, 0));

	//ASSERT_RETURN_EX(m_ImageListSmall.Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32, 0, 0), FALSE);
	//(void)m_ImageListSmall.Add(&Bitmap, RGB(0, 0, 0));

	(void)__super::SetImageList(&m_ImageList, LVSIL_NORMAL);
	(void)__super::SetImageList(&m_ImageList, LVSIL_SMALL);

	return TRUE;
}

void CObjectList::SetTileSize(UINT cx, UINT cy)
{
	LVTILEVIEWINFO LvTileViewInfo = { sizeof(LVTILEVIEWINFO) };
	LvTileViewInfo.dwFlags = LVTVIF_FIXEDSIZE;
	LvTileViewInfo.sizeTile = { (LONG)cx, (LONG)cy };
	//LvTileViewInfo.cLines = 2;
	LvTileViewInfo.dwMask = LVTVIM_TILESIZE;// | LVTVIM_COLUMNS;
	(void)__super::SetTileViewInfo(&LvTileViewInfo);
}

void CObjectList::SetView(ST_ListViewType nViewType)
{
	__super::SetRedraw(FALSE);

	(void)__super::SetView(nViewType);
	(void)__super::Arrange(0);

	__super::SetRedraw();
}

ST_ListViewType CObjectList::GetView()
{
	return (ST_ListViewType)__super::GetView();
}

void CObjectList::SetObjects(const TD_ListObjectList& lstObjects, int nPos)
{
	if (lstObjects.empty())
	{
		if (0 == nPos)
		{
			(void)DeleteAllItems();
		}
		
		return;
	}
	
	ASSERT_RETURN(nPos <= GetItemCount());

	int nMaxItem = GetItemCount()-1;

	DeselectAllItems();

	this->SetRedraw(FALSE);
	
	int nItem = nPos;
	for (TD_ListObjectList::const_iterator itObject = lstObjects.begin()
		; itObject != lstObjects.end(); ++itObject, ++nItem)
	{
		if (nItem <= nMaxItem)
		{
			SetItemObject(nItem, *itObject);
		}
		else
		{
			(void)InsertObject(*itObject, nItem);
		}
	}
	
	for (; nMaxItem >= nItem; --nMaxItem)
	{
		(void)DeleteItem(nMaxItem);
	}

	this->SetRedraw(TRUE);
}

int CObjectList::InsertObject(CListObject *pObject, int nItem)
{
	if (-1 == nItem)
	{
		nItem = __super::GetItemCount();
	}

	nItem = __super::InsertItem(nItem, NULL);
	
	this->SetItemObject(nItem, pObject);
	
	return nItem;
}

void CObjectList::UpdateObjects()
{
	ENSURE_RETURN(m_hWnd);

	for (int nItem = 0; nItem < this->GetItemCount(); ++nItem)
	{
		SetItemObject(nItem, (CListObject*)__super::GetItemData(nItem));
	}
}

void CObjectList::UpdateObject(CListObject *pObject)
{
	ENSURE_RETURN(m_hWnd);

	int nItem = this->GetObjectItem(pObject);
	if (0 <= nItem)
	{
		this->SetItemObject(nItem, pObject);
	}
}

void CObjectList::DeleteObjects(const TD_ListObjectList& lstDeleteObjects)
{
	ENSURE_RETURN(m_hWnd);

	CListObject *pObject = NULL;

	int nItemCount = this->GetItemCount();
	for (int nItem = 0; nItem < nItemCount; )
	{
		pObject = this->GetItemObject(nItem);
		
		if (util::ContainerFind(lstDeleteObjects, pObject))
		{
			this->DeleteItem(nItem);

			nItemCount--;

			continue;
		}

		nItem++;
	}
}

BOOL CObjectList::DeleteObject(CListObject* pObject)
{
	int nItem = this->GetObjectItem(pObject);
	ENSURE_RETURN_EX(0 <= nItem, FALSE);

	return this->DeleteItem(nItem);
}

void CObjectList::SetItemObject(int nItem, CListObject *pObject)
{
	ASSERT_RETURN(SetItem(nItem, 0, LVIF_IMAGE | LVIF_PARAM, NULL
		, pObject->GetListImage(), 0, 0, (LPARAM)pObject));

	std::list<CString> lstTexts;
	pObject->GetListText(lstTexts);

	CString strText;
	std::list<CString>::iterator itSubTitle = lstTexts.begin();
	for (UINT nColumn = 0; nColumn < m_nColumnCount; ++nColumn)
	{
		if (itSubTitle != lstTexts.end())
		{
			strText = *itSubTitle;

			itSubTitle++;
		}
		else
		{
			strText = "";
		}

		(void)__super::SetItemText(nItem, nColumn, strText);
	}
}

CListObject *CObjectList::GetItemObject(int nItem)
{
	ENSURE_RETURN_EX(0 <= nItem && nItem < GetItemCount(), NULL);
	return (CListObject*)__super::GetItemData(nItem);
}

int CObjectList::GetObjectItem(CListObject* pObject)
{
	for (int nItem = 0; nItem < __super::GetItemCount(); ++nItem)
	{
		if ((CListObject*)__super::GetItemData(nItem) == pObject)
		{
			return nItem;
		}
	}

	return -1;
}

void CObjectList::GetAllObjects(TD_ListObjectList& lstListObjects)
{
	for (int nItem = 0; nItem < __super::GetItemCount(); ++nItem)
	{
		lstListObjects.push_back((CListObject*)__super::GetItemData(nItem));
	}
}

int CObjectList::GetSingleSelectedItem()
{
	POSITION lpPos = __super::GetFirstSelectedItemPosition();
	ENSURE_RETURN_EX(lpPos, -1);

	return __super::GetNextSelectedItem(lpPos);
}

CListObject *CObjectList::GetSingleSelectedObject()
{
	int nItem = this->GetSingleSelectedItem();
	ENSURE_RETURN_EX(0 <= nItem, NULL);

	return this->GetItemObject(nItem);
}

void CObjectList::GetMultiSelectedItems(std::list<int>& lstItems)
{
	int nItem = 0;

	POSITION lpPos = __super::GetFirstSelectedItemPosition();
	while (lpPos)
	{
		lstItems.push_back(__super::GetNextSelectedItem(lpPos));
	}
}

void CObjectList::GetMultiSelectedObjects(std::map<int, CListObject*>& mapObjects)
{
	std::list<int> lstItems;
	this->GetMultiSelectedItems(lstItems);

	for (std::list<int>::iterator itrItem = lstItems.begin()
		; itrItem != lstItems.end(); ++itrItem)
	{
		mapObjects[*itrItem] = this->GetItemObject(*itrItem);
	}
}

void CObjectList::GetMultiSelectedObjects(TD_ListObjectList& lstObjects)
{
	std::list<int> lstItems;
	this->GetMultiSelectedItems(lstItems);

	for (std::list<int>::iterator itrItem = lstItems.begin()
		; itrItem != lstItems.end(); ++itrItem)
	{
		lstObjects.push_back(this->GetItemObject(*itrItem));
	}
}

BOOL CObjectList::SelectFirstItem()
{
	ENSURE_RETURN_EX(0 < this->GetItemCount(), FALSE);
	
	this->SelectItem(0, TRUE);

	return TRUE;
}

void CObjectList::SelectItem(int nItem, BOOL bSetFocus)
{
	UINT nState = LVIS_SELECTED;
	if (bSetFocus)
	{
		nState |= LVIS_FOCUSED;
	}

	(void)this->SetItemState(nItem, nState, nState);

	(void)this->SetSelectionMark(nItem);
	
	(void)this->EnsureVisible(nItem, FALSE);
}

void CObjectList::SelectObject(CListObject *pObject, BOOL bSetFocus)
{
	int nItem =	this->GetObjectItem(pObject);
	if (0 <= nItem)
	{
		this->SelectItem(nItem, bSetFocus);
	}
}

void CObjectList::SelectItems(int nItem, int nSelectCount)
{
	DeselectAllItems();

	for (int nIndex = 0; nIndex < nSelectCount; ++nIndex)
	{
		SelectItem(nItem + nIndex, 0==nIndex);
	}
}

void CObjectList::SelectAllItems()
{
	this->SetRedraw(FALSE);

	int nCount = this->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		(void)__super::SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	}

	this->SetRedraw();
}

void CObjectList::DeselectAllItems()
{
	int nItem = 0;

	POSITION lpPos = __super::GetFirstSelectedItemPosition();
	while (lpPos)
	{
		nItem = __super::GetNextSelectedItem(lpPos);

		(void)__super::SetItemState(nItem, 0, LVIS_SELECTED);
	}
}

BOOL CObjectList::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (m_bChangeView)
	{
		if (WM_MOUSEWHEEL == message)
		{
			UINT nFlag = GET_FLAGS_LPARAM(wParam);
			if (nFlag & MK_CONTROL)
			{
				static DWORD dwLastTime = 0;

				DWORD dwTime = ::GetTickCount();

				if (500 < dwTime - dwLastTime)
				{
					ChangeListCtrlView();
				
					dwLastTime = dwTime;
				}

				return TRUE;
			}
		}
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CObjectList::OnNMNotify(NMHDR* pNMHDR, BOOL* pResult)
{
	switch (pNMHDR->code)
	{
	case LVN_BEGINLABELEDIT:
		{
			NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
			CListObject *pObject = this->GetItemObject(pLVDispInfo->item.iItem);
			ENSURE_RETURN(pObject);

			CEdit *pwndEdit = this->GetEditControl();
			ASSERT_RETURN(pwndEdit);

			CString cstrRenameText = pObject->GetRenameText();
			pwndEdit->SetWindowText(cstrRenameText);
			pwndEdit->SetSel(0, cstrRenameText.GetLength(), TRUE);
		}

		break;
	case LVN_ENDLABELEDIT:
		{
			NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

			int nItem = pLVDispInfo->item.iItem;
			CListObject *pObject = GetItemObject(nItem);
			ASSERT_RETURN(pObject);

			CString cstrNewName(pLVDispInfo->item.pszText);
			(void)cstrNewName.Trim();
			ENSURE_RETURN(!cstrNewName.IsEmpty());
			
			CWaitCursor WaitCursor;
			pObject->OnListItemRename((LPCTSTR)cstrNewName, *this);
		}

		break;
	case LVN_KEYDOWN:
		{
			LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

			if ('A' == pLVKeyDow->wVKey && (0x80 & ::GetKeyState(VK_CONTROL)))
			{
				
				if (0 == (this->GetStyle() & LVS_SINGLESEL))
				{
					this->DeselectAllItems();
					this->SelectAllItems();
				}
			}
		}
	}
}

BOOL CObjectList::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_NOTIFY == message)
	{
		BOOL bResult = FALSE;
		OnNMNotify((NMHDR*)lParam, &bResult);
		if (bResult)
		{
			return TRUE;
		}
	}

	return __super::OnChildNotify(message, wParam, lParam, pResult);
}

void CObjectList::ChangeListCtrlView(short zDelta)
{
	ST_ListViewType lpViewType[] = {
		LVT_Icon
		//, LVT_SmallIcon
		//, LVT_List
		, LVT_Report
		, LVT_Tile
	};

	ST_ListViewType nPreViewType = this->GetView();

	for (UINT nIndex = 0; nIndex < sizeof(lpViewType)/sizeof(ST_ListViewType); ++nIndex)
	{
		if (lpViewType[nIndex] == nPreViewType)
		{
			nIndex++;
				
			if (sizeof(lpViewType)/sizeof(ST_ListViewType) <= nIndex)
			{
				nIndex = 0;
			}

			this->SetView(lpViewType[nIndex]);
			
			break;
		}
	}

	(void)this->RedrawWindow();
}
