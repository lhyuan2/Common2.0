
#pragma once

#include <img.h>

#include <Guide.h>

enum class E_ListViewType
{
	LVT_Tile = LV_VIEW_TILE,
	LVT_Icon = LVS_ICON,
	LVT_SmallIcon = LVS_SMALLICON,
	LVT_List = LVS_LIST,
	LVT_Report = LVS_REPORT
};

class __CommonPrjExt CListObject
{
public:
	virtual void GenListItem(class CObjectList& wndList, vector<wstring>& vecText, int& iImage)
	{
	}

	virtual CString GetRenameText()
	{
		return L"";
	}

	virtual bool OnListItemRename(const wstring& strNewName)
	{
		return false;
	}
};

typedef ptrlist<CListObject*> TD_ListObjectList;

// CObjectList

struct __CommonPrjExt tagListColumn
{
	CString cstrText;
	UINT uWidth;
	UINT uFlag = LVCFMT_LEFT;
};

typedef list<tagListColumn> TD_ListColumn;

using CB_LVCostomDraw = function<void(class CObjectList& wndList, NMLVCUSTOMDRAW& lvcd, bool& bSkipDefault)>;

class CHeaderCtrlEx : public CHeaderCtrl
{
	DECLARE_MESSAGE_MAP()

public:
	CHeaderCtrlEx(UINT uHeight)
		: m_uHeight(uHeight)
	{
	}

private:
	UINT m_uHeight = 0;

	LRESULT OnLayout(WPARAM wParam, LPARAM lParam);
};

class __CommonPrjExt CObjectList : public CListCtrl
{
public:
	using CB_ListViewChanged = function<void(E_ListViewType)>;

	struct tagListPara
	{
		UINT uFontSize = 0;
		UINT uFontSizeHeader = 0;

		E_ListViewType eViewType = (E_ListViewType)-1;

		TD_ListColumn lstColumns;

		set<UINT> setUnderlineColumns;

		UINT uItemHeight = 0;

		UINT uTileWidth = 0;
		UINT uTileHeight = 0;

		CB_LVCostomDraw cbCustomDraw;
		CB_ListViewChanged cbViewChanged;
		CB_TrackMouseEvent cbMouseEvent;
	};

	CObjectList(UINT uHeaderHeight = 0)
		: m_wndHeader(uHeaderHeight)
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
	CHeaderCtrlEx m_wndHeader;

	tagListPara m_para;

	CFontGuide m_fontGuide;

	UINT m_nColumnCount = 1;

	CCompatableFont m_fontUnderline;
	
	bool m_bCusomDrawNotify = false;
	bool m_bAutoChange = false;
	int m_iTrackMouseFlag = -1;
	
	CString m_cstrRenameText;

public:
	BOOL InitCtrl(const tagListPara& para);

	BOOL InitCtrl(UINT uFontSize, const TD_ListColumn &lstColumns = TD_ListColumn());

	BOOL InitImglst(const CSize& size, const CSize *pszSmall = NULL, const TD_IconVec& vecIcons = {});
	BOOL InitImglst(CBitmap& Bitmap, CBitmap *pBitmapSmall=NULL);

	void SetImageList(CImglst *pImglst, CImglst *pImglstSmall = NULL);

	BOOL SetItemHeight(UINT uItemHeight);

	BOOL SetUnderlineColumn(const set<UINT>& setColumns);

	void SetTileSize(ULONG cx, ULONG cy);

	void SetCusomDrawNotify(const CB_LVCostomDraw& cbCustomDraw = NULL)
	{
		m_bCusomDrawNotify = true;
		m_para.cbCustomDraw = cbCustomDraw;
	}

	void SetViewAutoChange(const CB_ListViewChanged& cb = NULL)
	{
		m_bAutoChange = true;
		m_para.cbViewChanged = cb;
	}

	void SetTrackMouse(const CB_TrackMouseEvent& cbMouseEvent=NULL);

	void SetView(E_ListViewType eViewType, bool bArrange=false);
	E_ListViewType GetView();
	
	void SetColumnText(UINT uColumn, const wstring& strText);

	void SetObjects(const TD_ListObjectList& lstObjects, int nPos=0);

	int InsertObject(CListObject& Object, int nItem=-1);
	
	void UpdateObject(CListObject& Object);

	void UpdateItem(UINT uItem);
	void UpdateItem(UINT uItem, CListObject& Object, const list<UINT>& lstColumn);

	void UpdateItems();
	void UpdateItems(const list<UINT>& lstColumn);

	void DeleteObjects(const TD_ListObjectList& lstDeleteObjects);

	BOOL DeleteObject(const CListObject *pObject);

	void SetItemObject(int nItem, CListObject& Object);

	void SetItemImage(int nItem, int iImage);

	CListObject *GetItemObject(int nItem);
	int GetObjectItem(const CListObject *pObject);
	void GetAllObjects(TD_ListObjectList& lstListObjects);

	int GetSingleSelectedItem();
	CListObject *GetSingleSelectedObject();

	void GetMultiSelectedItems(list<UINT>& lstItems);
	void GetMultiSelectedObjects(map<int, CListObject*>& mapObjects);
	void GetMultiSelectedObjects(TD_ListObjectList& lstObjects);

	BOOL SelectFirstItem();
	void SelectItem(int nItem, BOOL bSetFocus=TRUE);
	void SelectObject(const CListObject *pObject, BOOL bSetFocus=TRUE);
	void SelectItems(int nItem, int nSelectCount);
	void SelectAllItems();
	void DeselectAllItems();

	UINT GetHeaderHeight();

protected:
	virtual void GenListItem(CListObject& Object, vector<wstring>& vecText, int& iImage);

	virtual void PreSubclassWindow() override;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	virtual void OnTrackMouseEvent(E_TrackMouseEvent eMouseEvent, const CPoint& point);

	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	virtual BOOL handleNMNotify(NMHDR& NMHDR, LRESULT* pResult);

	void ChangeListCtrlView(short zDelta=0);

	virtual void OnCustomDraw(NMLVCUSTOMDRAW& lvcd, bool& bSkipDefault);
};
