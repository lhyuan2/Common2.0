
#pragma once

#include <def.h>

#include <afxcmn.h>

#include "util.h"

#include "FontGuide.h"

using TD_ICONLIST = list<HICON>;

class COMMON_EXT_CLASS CBaseTree: public CTreeCtrl
{
public:
	CBaseTree();

	virtual ~CBaseTree();

	DECLARE_MESSAGE_MAP()

private:
	CImageList m_ImageList;

	CFontGuide m_fontGuide;

public:
	BOOL InitCtrl(CBitmap *pBitmap = NULL, ULONG uFontSize = 0);
	BOOL InitCtrl(const TD_ICONLIST& lstIcon, UINT uIconSize, ULONG uFontSize=0);

	void GetAllItems(std::list<HTREEITEM>& lstItems);
	
protected:
	virtual HTREEITEM InsertItem(HTREEITEM hParentItem, LPCTSTR lpszItem, DWORD_PTR dwData, int nImage=0);

	void GetChildItems(HTREEITEM hItem, std::list<HTREEITEM>& lstItems);

public:
	virtual void PreSubclassWindow() override;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		
	BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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
	
	virtual void OnNMNotify(NMHDR* pNMHDR, BOOL* pResult)
	{
	}
};


//CObjectTree

class CTreeObject;

typedef ptrlist<CTreeObject*> TD_TreeObjectList;

class COMMON_EXT_CLASS CTreeObject
{
public:
	CTreeObject()
	{
		m_hTreeItem = NULL;
	}

public:
	HTREEITEM m_hTreeItem;

	virtual CString GetTreeText()
	{
		return _T("");
	};

	virtual int GetTreeImage()
	{
		return 0;
	}

	virtual void GetTreeChilds(TD_TreeObjectList& lstChilds)
	{
		//do nothing
	}
};


class COMMON_EXT_CLASS CObjectTree: public CBaseTree
{
public:
	CObjectTree();

	virtual ~CObjectTree();

public:
	void SetRootObject(CTreeObject& Object);

	virtual HTREEITEM InsertObject(CTreeObject& Object, CTreeObject *pParentObject=NULL);
	HTREEITEM InsertObjectEx(CTreeObject& Object, CTreeObject *pParentObject=NULL);

	CTreeObject *GetSelectedObject();

	CTreeObject *GetItemObject(HTREEITEM hItem);

	CTreeObject *GetParentObject(CTreeObject& Object);
	
	void GetAllObjects(TD_TreeObjectList& lstObjects);
};


enum ST_CheckState
{
	CS_Nocheck = 0
	, CS_Unchecked
	, CS_Checked
	, CS_Grayed
};

// CCheckObjectTree
class COMMON_EXT_CLASS CCheckObjectTree: public CObjectTree
{
public:
	CCheckObjectTree();
	virtual ~CCheckObjectTree();

	DECLARE_MESSAGE_MAP()

public:
	BOOL InitCtrl();

protected:
	CImageList m_StateImageList;

public:
	HTREEITEM InsertObject(CTreeObject& Object, CTreeObject *pParentObject=NULL);

	ST_CheckState GetItemCheckState(HTREEITEM hItem);

	void SetItemCheckState(HTREEITEM hItem, ST_CheckState nState);
	
	void GetAllObjects(TD_TreeObjectList& lstObjects);

	void GetAllObjects(TD_TreeObjectList& lstObjects, ST_CheckState nType);
	
	void GetCheckedObjects(TD_TreeObjectList& lstObjects);

private:
	void SetChildItemsImageState(HTREEITEM hItem);
	void SetParentItemsImageState(HTREEITEM hItem);

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


class COMMON_EXT_CLASS CListObject
{
public:
	virtual int GetListImage()
	{
		return 0;
	}
	
	virtual void GetListText(std::list<CString>& lstTexts)
	{
	};

	virtual CString GetRenameText()
	{
		std::list<CString> lstTexts;
		GetListText(lstTexts);
		ENSURE_RETURN_EX(!lstTexts.empty(), _T(""));

		return lstTexts.front();
	};

	virtual void OnListItemRename(const wstring& strNewName, CWnd& wndListCtrl)
	{
	}
};

typedef ptrlist<CListObject*> TD_ListObjectList;

// CObjectList

enum ST_ListViewType
{
	LVT_Tile = LV_VIEW_TILE,
	LVT_Icon = LVS_ICON,
	LVT_SmallIcon = LVS_SMALLICON,
	LVT_List = LVS_LIST,
	LVT_Report = LVS_REPORT
};

class COMMON_EXT_CLASS CObjectList : public CListCtrl
{
public:
	typedef std::list<std::pair<CString, int>> ColumnList;

public:
	CObjectList(bool bChangeView=false)
		: m_bChangeView(bChangeView)
	{
	}

	virtual ~CObjectList()
	{
		(void)m_ImageList.DeleteImageList();
		(void)m_ImageListSmall.DeleteImageList();		
	}

private:
	UINT m_nColumnCount = 1;

	CImageList m_ImageList;
	CImageList m_ImageListSmall;

	bool m_bChangeView = false;

	CFontGuide m_fontGuide;

public:
	BOOL InitCtrl(UINT uFontSize, const ColumnList &lstColumns = ColumnList());

	BOOL InitImage(const TD_ICONLIST& lstIcon, UINT uSize, UINT uSmallSize=0);
	BOOL InitImage(CBitmap& Bitmap);

	void SetTileSize(UINT cx, UINT cy);

	void SetView(ST_ListViewType nViewType);
	ST_ListViewType GetView();
	
	void SetObjects(const TD_ListObjectList& lstObjects, int nPos=0);

	int InsertObject(CListObject& Object, int nItem=-1);
	
	void UpdateObjects();
	void UpdateObject(CListObject& Object);
	void UpdateItem(UINT uItem);

	void DeleteObjects(const TD_ListObjectList& lstDeleteObjects);

	BOOL DeleteObject(CListObject& Object);

public:
	void SetItemObject(int nItem, CListObject& Object);
	CListObject *GetItemObject(int nItem);
	int GetObjectItem(CListObject& Object);
	void GetAllObjects(TD_ListObjectList& lstListObjects);

	int GetSingleSelectedItem();
	CListObject *GetSingleSelectedObject();

	void GetMultiSelectedItems(std::list<int>& lstItems);
	void GetMultiSelectedObjects(std::map<int, CListObject*>& mapObjects);
	void GetMultiSelectedObjects(TD_ListObjectList& lstObjects);

	BOOL SelectFirstItem();
	void SelectItem(int nItem, BOOL bSetFocus=TRUE);
	void SelectObject(CListObject& Object, BOOL bSetFocus=TRUE);
	void SelectItems(int nItem, int nSelectCount);
	void SelectAllItems();
	void DeselectAllItems();

public:
	virtual void PreSubclassWindow() override;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	
	virtual void OnNMNotify(NMHDR* pNMHDR, BOOL* pResult);

	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	//void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

private:
	void ChangeListCtrlView(short zDelta=0);
};
