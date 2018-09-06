
#pragma once

#include <def.h>

#include <afxcmn.h>

#include "util.h"

#include "FontGuide.h"

#define __ColorBlack RGB(0,0,0)

using TD_ICONLIST = list<HICON>;

class CImageListEx : public CImageList
{
public:
	CImageListEx()
	{
	}

public:
	BOOL CreateEx(CBitmap& bitmap)
	{
		BITMAP bmp;
		(void)bitmap.GetBitmap(&bmp);
		__AssertReturn(Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32, 0, 0), FALSE);
		(void)Add(&bitmap, __ColorBlack);

		return TRUE;
	}

	BOOL CreateEx(UINT cx, UINT cy, const TD_ICONLIST& lstIcon)
	{
		__AssertReturn(Create(cx, cy, ILC_COLOR32, 0, lstIcon.size()), FALSE);
		for (auto hIcon : lstIcon)
		{
			(void)Add(hIcon);
		}

		return TRUE;
	}

	void AddBitmap(CBitmap& bitmap)
	{
		(void)Add(&bitmap, RGB(0, 0, 0));
	}

	void ReplaceBitmap(UINT uPos, CBitmap& bitmap)
	{
		(void)__super::Replace(uPos, &bitmap, NULL);
	}
};

class __CommonPrjExt CBaseTree: public CTreeCtrl
{
public:
	CBaseTree();

	virtual ~CBaseTree();

	CImageListEx m_ImageList;

	DECLARE_MESSAGE_MAP()

private:
	CFontGuide m_fontGuide;

public:
	BOOL InitCtrl(CBitmap *pBitmap = NULL, ULONG uFontSize = 0);
	BOOL InitCtrl(const TD_ICONLIST& lstIcon, UINT uIconSize, ULONG uFontSize=0);

	void GetAllItems(list<HTREEITEM>& lstItems);
	
protected:
	virtual HTREEITEM InsertItem(HTREEITEM hParentItem, LPCTSTR lpszItem, DWORD_PTR dwData, int nImage=0);

	void GetChildItems(HTREEITEM hItem, list<HTREEITEM>& lstItems);

public:
	virtual void PreSubclassWindow() override;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

protected:
	virtual BOOL handleNMNotify(NMHDR& NMHDR) { return FALSE; }
};


//CObjectTree

class CTreeObject;

typedef ptrlist<CTreeObject*> TD_TreeObjectList;

class __CommonPrjExt CTreeObject
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


class __CommonPrjExt CObjectTree: public CBaseTree
{
public:
	CObjectTree();

	virtual ~CObjectTree();

public:
	void SetRootObject(CTreeObject& Object);

	virtual HTREEITEM InsertObject(CTreeObject& Object, CTreeObject *pParentObject=NULL);
	HTREEITEM InsertObjectEx(CTreeObject& Object, CTreeObject *pParentObject=NULL);

	void UpdateImage(CTreeObject& Object);

	CTreeObject *GetSelectedObject();

	CTreeObject *GetItemObject(HTREEITEM hItem);

	CTreeObject *GetParentObject(CTreeObject& Object);
	
	void GetAllObjects(TD_TreeObjectList& lstObjects);

private:
	virtual BOOL handleNMNotify(NMHDR& NMHDR) override;
};


enum E_CheckState
{
	CS_Nocheck = 0
	, CS_Unchecked
	, CS_Checked
	, CS_Grayed
};

// CCheckObjectTree
class __CommonPrjExt CCheckObjectTree: public CObjectTree
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

	E_CheckState GetItemCheckState(HTREEITEM hItem);

	void SetItemCheckState(HTREEITEM hItem, E_CheckState eCheckState);
	
	void GetAllObjects(TD_TreeObjectList& lstObjects);

	void GetAllObjects(TD_TreeObjectList& lstObjects, E_CheckState eCheckState);
	
	void GetCheckedObjects(TD_TreeObjectList& lstObjects);

private:
	void SetChildItemsImageState(HTREEITEM hItem);
	void SetParentItemsImageState(HTREEITEM hItem);

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


class __CommonPrjExt CListObject
{
public:
	virtual int GetListImage()
	{
		return 0;
	}
	
	virtual void GetListText(list<wstring>& lstTexts)
	{
	};

	virtual CString GetRenameText()
	{
		list<wstring> lstTexts;
		GetListText(lstTexts);
		__EnsureReturn(!lstTexts.empty(), _T(""));

		return lstTexts.front().c_str();
	};

	virtual bool OnListItemRename(const wstring& strNewName)
	{
		return false;
	}
};

typedef ptrlist<CListObject*> TD_ListObjectList;

// CObjectList

enum E_ListViewType
{
	LVT_Tile = LV_VIEW_TILE,
	LVT_Icon = LVS_ICON,
	LVT_SmallIcon = LVS_SMALLICON,
	LVT_List = LVS_LIST,
	LVT_Report = LVS_REPORT
};

struct __CommonPrjExt tagListColumn
{
	CString cstrText;
	UINT uWidth;
	UINT uFlag = LVCFMT_LEFT;
};

class __CommonPrjExt CObjectList : public CListCtrl
{
public:
	typedef list<tagListColumn> TD_ListColumn;

	using CB_ListViewChanged = function<void(E_ListViewType)>;

	CObjectList()
	{
	}

	virtual ~CObjectList()
	{
		(void)m_ImageList.DeleteImageList();
		(void)m_ImageListSmall.DeleteImageList();		
	}

	CImageListEx m_ImageList;
	CImageListEx m_ImageListSmall;

private:
	CFontGuide m_fontGuide;

	UINT m_nColumnCount = 1;

	E_ListViewType m_eViewType = (E_ListViewType)-1;

	bool m_bAutoChange = false;
	CB_ListViewChanged m_cbViewChanged;

public:
	BOOL InitCtrl(UINT uFontSize, const TD_ListColumn &lstColumns = TD_ListColumn());

	BOOL InitImage(const TD_ICONLIST& lstIcon, UINT uSize, UINT uSmallSize=0);
	BOOL InitImage(CBitmap& Bitmap, CBitmap *pBitmapSmall=NULL);

	void SetViewAutoChange(const CB_ListViewChanged& cb=NULL)
	{
		m_bAutoChange = true;
		m_cbViewChanged = cb;
	}

	void SetTileSize(UINT cx, UINT cy);

	void SetView(E_ListViewType eViewType, bool bArrange=false);
	E_ListViewType GetView();
	
	void SetObjects(const TD_ListObjectList& lstObjects, int nPos=0);

	int InsertObject(CListObject& Object, int nItem=-1);
	
	void UpdateObjects();
	void UpdateObject(CListObject& Object);
	void UpdateItem(UINT uItem);

	void DeleteObjects(const TD_ListObjectList& lstDeleteObjects);

	BOOL DeleteObject(const CListObject *pObject);

public:
	void SetItemObject(int nItem, CListObject& Object);
	CListObject *GetItemObject(int nItem);
	int GetObjectItem(const CListObject *pObject);
	void GetAllObjects(TD_ListObjectList& lstListObjects);

	int GetSingleSelectedItem();
	CListObject *GetSingleSelectedObject();

	void GetMultiSelectedItems(list<int>& lstItems);
	void GetMultiSelectedObjects(map<int, CListObject*>& mapObjects);
	void GetMultiSelectedObjects(TD_ListObjectList& lstObjects);

	BOOL SelectFirstItem();
	void SelectItem(int nItem, BOOL bSetFocus=TRUE);
	void SelectObject(const CListObject *pObject, BOOL bSetFocus=TRUE);
	void SelectItems(int nItem, int nSelectCount);
	void SelectAllItems();
	void DeselectAllItems();

protected:
	virtual void PreSubclassWindow() override;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
	
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

protected:
	virtual BOOL handleNMNotify(NMHDR& NMHDR);

	void ChangeListCtrlView(short zDelta=0);
};
