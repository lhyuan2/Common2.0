
#include "StdAfx.h"

#include <ObjectCtrl.h>

#include "Resource.h"

BOOL CImg::StretchBltEx(CDC& dc, const CRect& rcPos, bool bHalfToneMode, LPCRECT prcMargin)
{
	CRect rcDst(rcPos);
	if (NULL != prcMargin)
	{
		dc.FillSolidRect(rcDst, RGB(255, 255, 255));

		rcDst.left = prcMargin->left;
		rcDst.top = prcMargin->top;
		rcDst.right -= prcMargin->right;
		rcDst.bottom -= prcMargin->bottom;
	}

	int iImgWidth = this->GetWidth();
	int iImgHeight = this->GetHeight();
	float fHWRate = (float)iImgHeight / iImgWidth;

	int iDstWidth = rcDst.Width();
	int iDstHeight = rcDst.Height();

	float fNeedHWRate = (float)rcDst.Height() / rcDst.Width();

	CRect rcSrc;
	if (fHWRate > fNeedHWRate)
	{
		rcSrc.left = 0;
		rcSrc.right = iImgWidth;

		rcSrc.top = LONG(iImgHeight - iImgWidth*fNeedHWRate) / 2;
		rcSrc.bottom = iImgHeight - rcSrc.top;
	}
	else
	{
		rcSrc.top = 0;
		rcSrc.bottom = iImgHeight;

		rcSrc.left = LONG(iImgWidth - iImgHeight / fNeedHWRate) / 2;
		rcSrc.right = iImgWidth - rcSrc.left;
	}

	(void)dc.SetStretchBltMode(bHalfToneMode ? STRETCH_HALFTONE : STRETCH_DELETESCANS);

	CDC *pdcThis = CDC::FromHandle(this->GetDC());

	BOOL bRet = dc.StretchBlt(rcDst.left, rcDst.top, iDstWidth, iDstHeight
		, pdcThis, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), SRCCOPY);

	this->ReleaseDC();

	return bRet;
}

BOOL CImg::InitInnerDC(bool bHalfToneMode, UINT cx, UINT cy, LPCRECT prcMargin)
{
	m_cx = cx;
	m_cy = cy;
	m_rcDst = { 0, 0, (int)cx, (int)cy };

	m_bHalfToneMode = bHalfToneMode;

	CDC *pDC = CDC::FromHandle(::GetDC(NULL));
	__AssertReturn(pDC, FALSE);

	__AssertReturn(m_CompDC.CreateCompatibleDC(pDC), FALSE);

	__AssertReturn(m_CompBitmap.CreateCompatibleBitmap(pDC, m_rcDst.Width(), m_rcDst.Height()), FALSE);

	if (NULL != prcMargin)
	{
		m_CompDC.FillSolidRect(m_rcDst, RGB(255, 255, 255));

		m_rcDst.left = prcMargin->left;
		m_rcDst.top = prcMargin->top;
		m_rcDst.right -= prcMargin->right;
		m_rcDst.bottom -= prcMargin->bottom;
	}

	return TRUE;
}

CBitmap* CImg::LoadEx(const wstring& strFile)
{
	Destroy();
	HRESULT hr = Load(strFile.c_str());
	__EnsureReturn(S_OK == hr, NULL);

	CBitmap *pbmpPrev = (CBitmap*)m_CompDC.SelectObject(&m_CompBitmap);

	BOOL bRet = StretchBltEx(m_CompDC, m_rcDst, m_bHalfToneMode);
	if (!bRet)
	{
		return NULL;
	}

	//(void)m_CompDC.SelectObject(pbmpPrev);

	return &m_CompBitmap;
}

BOOL CImg::StretchBltEx(CDC& dc, const CRect& rcPos)
{
	return dc.StretchBlt(rcPos.left, rcPos.top, rcPos.Width(), rcPos.Height(), &m_CompDC, 0, 0, m_cx, m_cy, SRCCOPY);
}

BOOL CImg::StretchBltEx(CImg& img)
{
	return StretchBltEx(img.m_CompDC, CRect(0, 0, m_cx, m_cy));
}

BOOL CImglst::Init(UINT cx, UINT cy)
{
	__AssertReturn(Create(cx, cy, ILC_COLOR32, 0, 0), FALSE);

	m_cx = cx;
	m_cy = cy;

	CDC *pDC = CDC::FromHandle(::GetDC(NULL));
	__AssertReturn(pDC, FALSE);

	__AssertReturn(m_CompDC.CreateCompatibleDC(pDC), FALSE);

	__AssertReturn(m_CompBitmap.CreateCompatibleBitmap(pDC, cx, cy), FALSE);
	
	return TRUE;
}

BOOL CImglst::Init(const CSize& size, const TD_IconVec& vecIcons)
{
	__AssertReturn(Init(size.cx, size.cy), FALSE);

	for (auto hIcon : vecIcons)
	{
		(void)Add(hIcon);
	}

	return TRUE;
}

BOOL CImglst::Init(CBitmap& bitmap)
{
	BITMAP bmp;
	(void)bitmap.GetBitmap(&bmp);
	__AssertReturn(Init(bmp.bmHeight, bmp.bmHeight), FALSE);

	(void)Add(&bitmap, __ColorBlack);

	return TRUE;
}

BOOL CImglst::SetFile(const wstring& strFile, bool bHalfToneMode, LPCRECT prcMargin, int iPosReplace)
{
	CImg img;
	HRESULT hr = img.Load(strFile.c_str());
	__EnsureReturn(S_OK == hr, FALSE);
	{
		return NULL;
	}

	SetImg(img, bHalfToneMode, prcMargin, iPosReplace);
	
	return TRUE;
}

void CImglst::SetImg(CImg& img, bool bHalfToneMode, LPCRECT prcMargin, int iPosReplace)
{
	CBitmap *pbmpPrev = (CBitmap*)m_CompDC.SelectObject(&m_CompBitmap);

	img.StretchBltEx(m_CompDC, CRect(0, 0, m_cx, m_cy), bHalfToneMode, prcMargin);

	(void)m_CompDC.SelectObject(pbmpPrev);

	SetBitmap(m_CompBitmap, iPosReplace);
}

void CImglst::SetBitmap(CBitmap& bitmap, int iPosReplace)
{
	if (iPosReplace >= 0)
	{
		(void)__super::Replace(iPosReplace, &bitmap, NULL);
	}
	else
	{
		(void)Add(&bitmap, RGB(0, 0, 0));
	}
}

void CImglst::SetIcon(HICON hIcon, int iPosReplace)
{
	if (iPosReplace >= 0)
	{
		(void)Replace(iPosReplace, hIcon);
	}
	else
	{
		(void)Add(hIcon);
	}
}

// CBaseTree

CBaseTree::CBaseTree()
{
}

CBaseTree::~CBaseTree()
{
	(void)m_Imglst.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CBaseTree, CTreeCtrl)
END_MESSAGE_MAP()

// CBaseTree 消息处理程序

void CBaseTree::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	(void)ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS);
}

BOOL CBaseTree::InitImglst(const CSize& size, const TD_IconVec& vecIcons)
{
	__AssertReturn(m_Imglst.Init(size, vecIcons), FALSE);
	(void)__super::SetImageList(&m_Imglst, TVSIL_NORMAL);
	
	return TRUE;
}

BOOL CBaseTree::InitImglst(CBitmap& Bitmap)
{
	__AssertReturn(m_Imglst.Init(Bitmap), FALSE);
	(void)__super::SetImageList(&m_Imglst, TVSIL_NORMAL);

	return TRUE;
}

void CBaseTree::SetFont(ULONG uFontSize)
{
	if (0 < uFontSize)
	{
		(void)m_fontGuide.setFontSize(*this, uFontSize);
	}
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

void CBaseTree::GetAllItems(list<HTREEITEM>& lstItems)
{
	HTREEITEM hRootItem = this->GetRootItem();
	while (hRootItem)
	{
		lstItems.push_back(hRootItem);

		GetChildItems(hRootItem, lstItems);

		hRootItem = this->GetNextItem(hRootItem, TVGN_NEXT);
	}
}

void CBaseTree::GetChildItems(HTREEITEM hItem, list<HTREEITEM>& lstItems)
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

BOOL CBaseTree::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_NOTIFY == message)
	{
		NMHDR *pNMHDR = (NMHDR*)lParam;
		if (NULL != pNMHDR)
		{
			if (handleNMNotify(*pNMHDR))
			{
				return TRUE;
			}
		}
	}

	return __super::OnChildNotify(message, wParam, lParam, pResult);
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

BOOL CCheckObjectTree::InitCtrl()
{
	HBITMAP hStateBitmap = ::LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_CHECKTREE_STATE));
	__AssertReturn(hStateBitmap, FALSE);

	CBitmap StateBitmap;
	StateBitmap.Attach(hStateBitmap);

	BITMAP bmp;
	(void)StateBitmap.GetBitmap(&bmp);

	__AssertReturn(m_StateImageList.Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32, 0, 0), FALSE);
	(void)m_StateImageList.Add(&StateBitmap, __ColorBlack);
	
	(void)StateBitmap.DeleteObject();

	(void)CTreeCtrl::SetImageList(&m_StateImageList, TVSIL_STATE);

	(void)ModifyStyle(TVS_CHECKBOXES, 0);

	return TRUE;
}

HTREEITEM CCheckObjectTree::InsertObject(CTreeObject& Object, CTreeObject *pParentObject)
{
	HTREEITEM hItem = __super::InsertObject(Object, pParentObject);

	(void)__super::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(CS_Unchecked), TVIS_STATEIMAGEMASK);

	return hItem;
}

void CCheckObjectTree::SetItemCheckState(HTREEITEM hItem, E_CheckState eCheckState)
{
	(void)__super::SetItemState(hItem, INDEXTOSTATEIMAGEMASK(eCheckState), TVIS_STATEIMAGEMASK);

	this->SetChildItemsImageState(hItem);

	this->SetParentItemsImageState(hItem);
}

E_CheckState CCheckObjectTree::GetItemCheckState(HTREEITEM hItem)
{
	return (E_CheckState)(__super::GetItemState(hItem, TVIS_STATEIMAGEMASK) >>12);
}

void CCheckObjectTree::GetAllObjects(TD_TreeObjectList& lstObjects)
{
	list<HTREEITEM> lstItems;
	__super::GetAllItems(lstItems);

	for (list<HTREEITEM>::iterator itItem = lstItems.begin()
		; itItem != lstItems.end(); ++itItem)
	{
		lstObjects.push_back(__super::GetItemObject(*itItem));
	}
}

void CCheckObjectTree::GetAllObjects(TD_TreeObjectList& lstObjects, E_CheckState eCheckState)
{
	TD_TreeObjectList lstItemObjects;
	__super::GetAllObjects(lstItemObjects);

	for (TD_TreeObjectList::iterator itObject = lstItemObjects.begin()
		; itObject != lstItemObjects.end(); ++itObject)
	{
		if (eCheckState == this->GetItemCheckState((*itObject)->m_hTreeItem))
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

void CObjectTree::SetRootObject(CTreeObject& Object)
{
	(void)DeleteAllItems();

	(void)InsertObjectEx(Object);

	(void)__super::SelectItem(Object.m_hTreeItem);
	(void)__super::EnsureVisible(Object.m_hTreeItem);
	(void)__super::Expand(Object.m_hTreeItem, TVE_EXPAND);
}

HTREEITEM CObjectTree::InsertObject(CTreeObject& Object, CTreeObject *pParentObject)
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
	
	return Object.m_hTreeItem = __super::InsertItem(hParentItem, Object.GetTreeText()
		, (DWORD_PTR)&Object, Object.GetTreeImage());
}

HTREEITEM CObjectTree::InsertObjectEx(CTreeObject& Object, CTreeObject *pParentObject)
{
	Object.m_hTreeItem = InsertObject(Object, pParentObject);
	__EnsureReturn(Object.m_hTreeItem, NULL);

	TD_TreeObjectList lstSubObjects;
	Object.GetTreeChilds(lstSubObjects);

	for (TD_TreeObjectList::iterator itSubObject=lstSubObjects.begin()
		; itSubObject!=lstSubObjects.end(); ++itSubObject)
	{
		(void)InsertObjectEx(**itSubObject, &Object);
	}

	return Object.m_hTreeItem;
}

void CObjectTree::UpdateImage(CTreeObject& Object)
{
	int iImage = Object.GetTreeImage();
	SetItemImage(Object.m_hTreeItem, iImage, iImage);
	this->Invalidate(FALSE);
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

CTreeObject *CObjectTree::GetParentObject(CTreeObject& Object)
{
	__AssertReturn(Object.m_hTreeItem, NULL);
	
	HTREEITEM hItemParent = __super::GetParentItem(Object.m_hTreeItem);
	__EnsureReturn(hItemParent, NULL);

	return this->GetItemObject(hItemParent);
}

void CObjectTree::GetAllObjects(TD_TreeObjectList& lstObjects)
{
	list<HTREEITEM> lstItems;
	__super::GetAllItems(lstItems);

	for (list<HTREEITEM>::iterator itItem = lstItems.begin()
		; itItem != lstItems.end(); ++itItem)
	{
		lstObjects.push_back(this->GetItemObject(*itItem));
	}
}

BOOL CObjectTree::handleNMNotify(NMHDR& NMHDR)
{
	if (TVN_ENDLABELEDIT == NMHDR.code)
	{
		NMTVDISPINFO *pTVDispInfo = reinterpret_cast<NMTVDISPINFO*>(&NMHDR);

		CString cstrNewName(pTVDispInfo->item.pszText);
		(void)cstrNewName.Trim();
		__EnsureReturn(!cstrNewName.IsEmpty(), TRUE);

		CTreeObject *pObject = GetItemObject(pTVDispInfo->item.hItem);
		__AssertReturn(pObject, TRUE);

		__EnsureReturn(cstrNewName != pObject->GetTreeText(), TRUE);
	}

	return __super::handleNMNotify(NMHDR);
}

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

void CObjectList::TrackMouseEvent(const CB_ListMouseEvent& cbMouseEvent)
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
	list<wstring> lstTexts;
	Object.GetListDisplay(m_eViewType, lstTexts, iImage);

	__Assert(SetItem(nItem, 0, LVIF_IMAGE | LVIF_PARAM, NULL
		, iImage, 0, 0, (LPARAM)&Object));

	list<wstring>::iterator itSubTitle = lstTexts.begin();
	for (UINT nColumn = 0; nColumn < m_nColumnCount; ++nColumn)
	{
		wstring strText;
		if (itSubTitle != lstTexts.end())
		{
			strText = *itSubTitle;

			itSubTitle++;
		}

		(void)__super::SetItemText(nItem, nColumn, strText.c_str());
	}
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
				//   鼠标移入窗时，请求WM_MOUSEHOVER和WM_MOUSELEAVE 消息
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = HOVER_DEFAULT;
				m_iTrackStatus = ::TrackMouseEvent(&tme);
			}

			OnMouseEvent(E_ListMouseEvent::LME_MouseMove, CPoint(lParam));
		}
		else if (WM_MOUSEHOVER == message)
		{
			m_iTrackStatus = 0;

			OnMouseEvent(E_ListMouseEvent::LME_MouseHover, CPoint(lParam));
		}
		else if (WM_MOUSELEAVE == message)
		{
			m_iTrackStatus = 0;
			
			OnMouseEvent(E_ListMouseEvent::LME_MouseLeave, CPoint(lParam));
		}
	}
	
	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CObjectList::OnMouseEvent(E_ListMouseEvent eMouseEvent, const CPoint& point)
{
	if (m_cbMouseEvent)
	{
		m_cbMouseEvent(eMouseEvent, point);
	}
}

BOOL CObjectList::handleNMNotify(NMHDR& NMHDR)
{
	switch (NMHDR.code)
	{
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

BOOL CObjectList::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_NOTIFY == message)
	{
		NMHDR *pNMHDR = (NMHDR*)lParam;
		if (NULL != pNMHDR)
		{
			if (handleNMNotify(*pNMHDR))
			{
				return TRUE;
			}
		}
	}

	return __super::OnChildNotify(message, wParam, lParam, pResult);
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
