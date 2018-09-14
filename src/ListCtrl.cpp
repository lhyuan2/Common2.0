
#include "StdAfx.h"

#include <ListCtrl.h>

#include "Resource.h"

LRESULT CHeaderCtrlEx::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);

	if (0 != m_uHeight)
	{
		HD_LAYOUT& hdl = *(HD_LAYOUT*)lParam;
		hdl.prc->top = hdl.pwpos->cy = m_uHeight;
	}

	return lResult;
}

BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
END_MESSAGE_MAP()

// CObjectList

void CObjectList::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	(void)ModifyStyle(0, LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS);

	(void)SetExtendedStyle(__super::GetExtendedStyle()
		| LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);
}

BOOL CObjectList::InitCtrl(UINT uFontSize, const TD_ListColumn &lstColumns)
{
	if (0 < uFontSize)
	{
		(void)m_fontGuide.setFontSize(*this, uFontSize);
	}

	if (!lstColumns.empty())
	{
		m_nColumnCount = 0;
		for (auto& column : lstColumns)
		{
			(void)__super::InsertColumn(m_nColumnCount, (0 == column.uFlag) ? L' ' + column.cstrText
				: column.cstrText, column.uFlag, column.uWidth);
			
			m_nColumnCount++;
		}
	}

	m_wndHeader.SubclassWindow(CListCtrl::GetHeaderCtrl()->GetSafeHwnd());

	return TRUE;
}

BOOL CObjectList::InitImglst(const CSize& size, const CSize *pszSmall, const TD_IconVec& vecIcons)
{
	__AssertReturn(m_Imglst.Init(size, vecIcons), FALSE);
	(void)__super::SetImageList(&m_Imglst, LVSIL_NORMAL);

	if (NULL != pszSmall)
	{
		__AssertReturn(m_ImglstSmall.Init(*pszSmall, vecIcons), FALSE);
		(void)__super::SetImageList(&m_ImglstSmall, LVSIL_SMALL);
	}
	else
	{
		(void)__super::SetImageList(&m_Imglst, LVSIL_SMALL);
	}
	
	return TRUE;
}

BOOL CObjectList::InitImglst(CBitmap& Bitmap, CBitmap *pBitmapSmall)
{
	__AssertReturn(m_Imglst.Init(Bitmap), FALSE);
	(void)__super::SetImageList(&m_Imglst, LVSIL_NORMAL);

	if (NULL != pBitmapSmall)
	{
		__AssertReturn(m_ImglstSmall.Init(*pBitmapSmall), FALSE);
		(void)__super::SetImageList(&m_ImglstSmall, LVSIL_SMALL);
	}
	else
	{
		(void)__super::SetImageList(&m_Imglst, LVSIL_SMALL);
	}

	return TRUE;
}

void CObjectList::SetImageList(CImglst *pImglst, CImglst *pImglstSmall)
{
	if (NULL != pImglst)
	{
		(void)__super::SetImageList(pImglst, LVSIL_NORMAL);
	}

	if (NULL == pImglstSmall)
	{
		pImglstSmall = pImglst;
	}
	if (NULL != pImglstSmall)
	{
		(void)__super::SetImageList(pImglstSmall, LVSIL_SMALL);
	}
}

void CObjectList::SetColumnText(UINT uColumn, const wstring& strText)
{
	HDITEM hdItem;
	hdItem.mask = HDI_TEXT;
	hdItem.pszText = (LPWSTR)strText.c_str();
	m_wndHeader.SetItem(uColumn, &hdItem);
}

void CObjectList::SetItemHeight(UINT uItemHeight)
{
	if (m_ImglstSmall.Create(1, uItemHeight, ILC_COLOR8, 1, 0))
	{
		SetImageList(NULL, &m_ImglstSmall);
	}
}

bool CObjectList::SetUnderlineColumn(const set<UINT>& setColumns)
{
	m_setUnderlineColumns = setColumns;

	__AssertReturn(m_fontUnderline.create(*this, [](LOGFONT& logFont) {
		logFont.lfUnderline = 1;
	}), false);

	SetCusomDrawNotify();

	return true;
}

void CObjectList::SetTileSize(ULONG cx, ULONG cy)
{
	LVTILEVIEWINFO LvTileViewInfo = { sizeof(LVTILEVIEWINFO) };
	LvTileViewInfo.dwFlags = LVTVIF_FIXEDSIZE;
	LvTileViewInfo.sizeTile = { (LONG)cx, (LONG)cy };
	//LvTileViewInfo.cLines = 2;
	LvTileViewInfo.dwMask = LVTVIM_TILESIZE;// | LVTVIM_COLUMNS;
	(void)__super::SetTileViewInfo(&LvTileViewInfo);
}

void CObjectList::SetView(E_ListViewType eViewType, bool bArrange)
{
	this->SetRedraw(FALSE);

	m_eViewType = eViewType;
	(void)__super::SetView(eViewType);

	if (bArrange)
	{
		(void)Arrange(0);
	}

	this->SetRedraw(TRUE);
}

E_ListViewType CObjectList::GetView()
{
	if ((E_ListViewType)-1 != m_eViewType)
	{
		return m_eViewType;
	}

	m_eViewType = (E_ListViewType)__super::GetView();

	return m_eViewType;
}

void CObjectList::SetTrackMouse(const CB_TrackMouseEvent& cbMouseEvent)
{
	m_cbMouseEvent = cbMouseEvent;

	m_iTrackStatus = 0;
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
	
	__Assert(nPos <= GetItemCount());

	int nMaxItem = GetItemCount()-1;

	DeselectAllItems();

	this->SetRedraw(FALSE);
	
	int nItem = nPos;
	for (TD_ListObjectList::const_iterator itObject = lstObjects.begin()
		; itObject != lstObjects.end(); ++itObject, ++nItem)
	{
		if (nItem <= nMaxItem)
		{
			SetItemObject(nItem, **itObject);
		}
		else
		{
			(void)InsertObject(**itObject, nItem);
		}
	}
	
	for (; nMaxItem >= nItem; --nMaxItem)
	{
		(void)DeleteItem(nMaxItem);
	}

	this->SetRedraw(TRUE);
}

int CObjectList::InsertObject(CListObject& Object, int nItem)
{
	if (-1 == nItem)
	{
		nItem = __super::GetItemCount();
	}

	nItem = __super::InsertItem(nItem, NULL);
	
	this->SetItemObject(nItem, Object);
	
	return nItem;
}

void CObjectList::UpdateObjects()
{
	__Ensure(m_hWnd);

	for (int nItem = 0; nItem < this->GetItemCount(); ++nItem)
	{
		SetItemObject(nItem, *(CListObject*)__super::GetItemData(nItem));
	}
}

void CObjectList::UpdateObject(CListObject& Object)
{
	__Ensure(m_hWnd);

	int nItem = this->GetObjectItem(&Object);
	if (0 <= nItem)
	{
		this->SetItemObject(nItem, Object);
	}
}

void CObjectList::UpdateItem(UINT uItem)
{
	__Ensure(m_hWnd);

	auto pObject = this->GetItemObject(uItem);
	if (NULL != pObject)
	{
		this->SetItemObject(uItem, *pObject);
	}
}

void CObjectList::DeleteObjects(const TD_ListObjectList& lstDeleteObjects)
{
	__Ensure(m_hWnd);

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

BOOL CObjectList::DeleteObject(const CListObject *pObject)
{
	int nItem = this->GetObjectItem(pObject);
	__EnsureReturn(0 <= nItem, FALSE);

	return this->DeleteItem(nItem);
}

void CObjectList::SetItemObject(int nItem, CListObject& Object)
{
	int iImage = 0;
	vector<wstring> vecText;
	GenListItem(Object, vecText, iImage);

	__Assert(SetItem(nItem, 0, LVIF_IMAGE | LVIF_PARAM, NULL
		, iImage, 0, 0, (LPARAM)&Object));

	auto itSubTitle = vecText.begin();
	for (UINT nColumn = 0; nColumn < m_nColumnCount; ++nColumn)
	{
		wstring strText;
		if (itSubTitle != vecText.end())
		{
			strText = *itSubTitle;

			itSubTitle++;
		}

		(void)__super::SetItemText(nItem, nColumn, strText.c_str());
	}
}

void CObjectList::GenListItem(CListObject& Object, vector<wstring>& vecText, int& iImage)
{
	Object.GenListItem(*this, vecText, iImage);
}

void CObjectList::SetItemImage(int nItem, int iImage)
{
	(void)SetItem(nItem, 0, LVIF_IMAGE, NULL, iImage, 0, 0, 0);
	Update(nItem);
}

CListObject *CObjectList::GetItemObject(int nItem)
{
	__EnsureReturn(0 <= nItem && nItem < GetItemCount(), NULL);
	return (CListObject*)__super::GetItemData(nItem);
}

int CObjectList::GetObjectItem(const CListObject *pObject)
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
	__EnsureReturn(lpPos, -1);

	return __super::GetNextSelectedItem(lpPos);
}

CListObject *CObjectList::GetSingleSelectedObject()
{
	int nItem = this->GetSingleSelectedItem();
	__EnsureReturn(0 <= nItem, NULL);

	return this->GetItemObject(nItem);
}

void CObjectList::GetMultiSelectedItems(list<int>& lstItems)
{
	int nItem = 0;

	POSITION lpPos = __super::GetFirstSelectedItemPosition();
	while (lpPos)
	{
		lstItems.push_back(__super::GetNextSelectedItem(lpPos));
	}
}

void CObjectList::GetMultiSelectedObjects(map<int, CListObject*>& mapObjects)
{
	list<int> lstItems;
	this->GetMultiSelectedItems(lstItems);

	for (list<int>::iterator itrItem = lstItems.begin()
		; itrItem != lstItems.end(); ++itrItem)
	{
		mapObjects[*itrItem] = this->GetItemObject(*itrItem);
	}
}

void CObjectList::GetMultiSelectedObjects(TD_ListObjectList& lstObjects)
{
	list<int> lstItems;
	this->GetMultiSelectedItems(lstItems);

	for (list<int>::iterator itrItem = lstItems.begin()
		; itrItem != lstItems.end(); ++itrItem)
	{
		lstObjects.push_back(this->GetItemObject(*itrItem));
	}
}

BOOL CObjectList::SelectFirstItem()
{
	__EnsureReturn(0 < this->GetItemCount(), FALSE);
	
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

void CObjectList::SelectObject(const CListObject *pObject, BOOL bSetFocus)
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

	this->SetRedraw(TRUE);
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

BOOL CObjectList::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_NOTIFY == message)
	{
		NMHDR *pNMHDR = (NMHDR*)lParam;
		if (NULL != pNMHDR)
		{
			if (handleNMNotify(*pNMHDR, pResult))
			{
				return TRUE;
			}
		}
	}

	return __super::OnChildNotify(message, wParam, lParam, pResult);
}

BOOL CObjectList::handleNMNotify(NMHDR& NMHDR, LRESULT* pResult)
{
	switch (NMHDR.code)
	{
	case NM_CUSTOMDRAW:
	{
		__EnsureBreak(m_bCusomDrawNotify);

		LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(&NMHDR);

		if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}

		if (pLVCD->nmcd.dwDrawStage & CDDS_ITEM)
		{
			if (pLVCD->nmcd.dwDrawStage & CDDS_SUBITEM)
			{
				bool bSkipDefault = false;
				OnCustomDraw(*pLVCD, bSkipDefault);
				if (bSkipDefault)
				{
					*pResult = CDRF_SKIPDEFAULT;
				}
			}
		}

		return TRUE;
	}

	break;
	case LVN_BEGINLABELEDIT:
	{
		NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(&NMHDR);
		CListObject *pObject = this->GetItemObject(pLVDispInfo->item.iItem);
		__EnsureBreak(pObject);

		CEdit *pwndEdit = this->GetEditControl();
		__AssertBreak(pwndEdit);

		m_cstrRenameText = pObject->GetRenameText();
		if (m_cstrRenameText.IsEmpty())
		{
			m_cstrRenameText = pLVDispInfo->item.pszText;
			m_cstrRenameText.Trim();
		}
		pwndEdit->SetWindowText(m_cstrRenameText);
		pwndEdit->SetSel(0, m_cstrRenameText.GetLength(), TRUE);
	}

	break;
	case LVN_ENDLABELEDIT:
	{
		NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(&NMHDR);

		CString cstrNewText(pLVDispInfo->item.pszText);
		(void)cstrNewText.Trim();
		__EnsureReturn(!cstrNewText.IsEmpty(), TRUE);

		__EnsureReturn(cstrNewText != m_cstrRenameText, TRUE);

		int nItem = pLVDispInfo->item.iItem;
		CListObject *pObject = GetItemObject(nItem);
		__AssertReturn(pObject, TRUE);

		if (pObject->OnListItemRename((LPCTSTR)cstrNewText))
		{
			this->UpdateItem(pLVDispInfo->item.iItem);
		}
	}

	break;
	case LVN_KEYDOWN:
	{
		LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(&NMHDR);

		if ('A' == pLVKeyDow->wVKey && (0x80 & ::GetKeyState(VK_CONTROL)))
		{
			if (0 == (this->GetStyle() & LVS_SINGLESEL))
			{
				this->DeselectAllItems();
				this->SelectAllItems();
			}
		}

		return TRUE;
	}

	break;
	case NM_CLICK:
		m_bDblClick = false;

		break;
	case NM_DBLCLK:
		m_bDblClick = true;

		break;
	default:
		break;
	}
	
	return FALSE;
}

void CObjectList::OnCustomDraw(NMLVCUSTOMDRAW& lvcd, bool& bSkipDefault)
{
	if (m_cbCustomDraw)
	{
		m_cbCustomDraw(*this, lvcd, bSkipDefault);
		if (bSkipDefault)
		{
			return;
		}
	}

	if (!m_setUnderlineColumns.empty())
	{
		if (m_setUnderlineColumns.find(lvcd.iSubItem) != m_setUnderlineColumns.end())
		{
			::SelectObject(lvcd.nmcd.hdc, m_fontUnderline);
		}
		else
		{
			::SelectObject(lvcd.nmcd.hdc, *this->GetFont());
		}
	}
}

void CObjectList::OnTrackMouseEvent(E_TrackMouseEvent eMouseEvent, const CPoint& point)
{
	if (m_cbMouseEvent)
	{
		m_cbMouseEvent(eMouseEvent, point);
	}
}

BOOL CObjectList::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_MOUSEWHEEL == message)
	{
		if (m_bAutoChange)
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
	else if (-1 != m_iTrackStatus)
	{
		if (WM_MOUSEMOVE == message)
		{
			if (0 == m_iTrackStatus)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = HOVER_DEFAULT;
				m_iTrackStatus = ::TrackMouseEvent(&tme);
			}

			OnTrackMouseEvent(E_TrackMouseEvent::LME_MouseMove, CPoint(lParam));
		}
		else if (WM_MOUSELEAVE == message)
		{
			m_iTrackStatus = 0;

			OnTrackMouseEvent(E_TrackMouseEvent::LME_MouseLeave, CPoint(lParam));
		}
		else if (WM_MOUSEHOVER == message)
		{
			//m_iTrackStatus = 0;

			OnTrackMouseEvent(E_TrackMouseEvent::LME_MouseHover, CPoint(lParam));
		}
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CObjectList::ChangeListCtrlView(short zDelta)
{
	E_ListViewType lpViewType[] = {
		LVT_Tile
		, LVT_Report
		, LVT_List
		//, LVT_SmallIcon
		, LVT_Icon
	};

	E_ListViewType nPreViewType = this->GetView();

	for (UINT nIndex = 0; nIndex < sizeof(lpViewType) / sizeof(E_ListViewType); ++nIndex)
	{
		if (lpViewType[nIndex] == nPreViewType)
		{
			nIndex++;

			if (sizeof(lpViewType) / sizeof(E_ListViewType) <= nIndex)
			{
				nIndex = 0;
			}
			
			m_eViewType = lpViewType[nIndex];
			this->SetView(m_eViewType);

			if (m_cbViewChanged)
			{
				m_cbViewChanged(m_eViewType);
			}

			break;
		}
	}

	//(void)this->RedrawWindow();
}
