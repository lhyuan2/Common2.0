
#pragma once

#include <def.h>

#include <afxcmn.h>

#include "util.h"

#include "Guide.h"

#include <atlimage.h>

#define __ColorBlack RGB(0,0,0)

using TD_IconVec = vector<HICON>;

enum class E_ImglstType
{
	ILT_Both = -1
	, ILT_Normal = 0
	, ILT_Small = LVSIL_SMALL
};

class __CommonPrjExt CImglst : public CImageList
{
public:
	CImglst()
	{
	}

private:
	CRect m_rcPos;

	CDC m_CompDC;
	CBitmap m_CompBitmap;

public:
	BOOL Init(UINT cx, UINT cy);

	BOOL Init(const CSize& size, const TD_IconVec& vecIcons = {});

	BOOL Init(CBitmap& bitmap);

	void SetFile(const wstring& strFile, LPCRECT prcMargin = NULL, int iPosReplace = -1);

	void SetImg(CImage *pImg, LPCRECT prcMargin = NULL, int iPosReplace = -1);
	
	void SetBitmap(CBitmap& bitmap, int iPosReplace = -1);

	void SetIcon(HICON hIcon, int iPosReplace = -1);

	void SetToListCtrl(CListCtrl &wndListCtrl, E_ImglstType eImglstType)
	{
		if (E_ImglstType::ILT_Both == eImglstType)
		{
			(void)wndListCtrl.SetImageList(this, (int)E_ImglstType::ILT_Normal);
			(void)wndListCtrl.SetImageList(this, (int)E_ImglstType::ILT_Small);
		}
		else
		{
			(void)wndListCtrl.SetImageList(this, (int)eImglstType);
		}
	}

	void SetToTreeCtrl(CTreeCtrl &wndTreeCtrl)
	{
		(void)wndTreeCtrl.SetImageList(this, TVSIL_NORMAL);
	}
};

class __CommonPrjExt CBaseTree: public CTreeCtrl
{
public:
	CBaseTree();

	virtual ~CBaseTree();

	CImglst m_Imglst;

	DECLARE_MESSAGE_MAP()

private:
	CFontGuide m_fontGuide;

public:
	BOOL InitImglst(const CSize& size, const TD_IconVec& vecIcons = {});

	BOOL InitImglst(CBitmap& Bitmap);

	void SetImageList(CImglst& imglst)
	{
		(void)__super::SetImageList(&imglst, TVSIL_NORMAL);
	}

	void SetFont(ULONG uFontSize);

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
	virtual void GetListDisplay(list<wstring>& lstTexts, int& iImage)
	{
	};

	virtual CString GetRenameText()
	{
		return L"";
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
		(void)m_Imglst.DeleteImageList();
		(void)m_ImglstSmall.DeleteImageList();		
	}

	CImglst m_Imglst;
	CImglst m_ImglstSmall;

	bool m_bDblClick = false;

private:
	CFontGuide m_fontGuide;

	UINT m_nColumnCount = 1;

	E_ListViewType m_eViewType = (E_ListViewType)-1;

	bool m_bAutoChange = false;
	CB_ListViewChanged m_cbViewChanged;

	CString m_cstrRenameText;

public:
	BOOL InitCtrl(UINT uFontSize, const TD_ListColumn &lstColumns = TD_ListColumn());

	BOOL InitImglst(const CSize& size, const CSize *pszSmall = NULL, const TD_IconVec& vecIcons = {});
	BOOL InitImglst(CBitmap& Bitmap, CBitmap *pBitmapSmall=NULL);

	void SetImageList(CImglst *pImglst, CImglst *pImglstSmall = NULL);

	void SetViewAutoChange(const CB_ListViewChanged& cb=NULL)
	{
		m_bAutoChange = true;
		m_cbViewChanged = cb;
	}

	void SetTileSize(ULONG cx, ULONG cy);

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
