#pragma once

#include "def.h"

#include "App.h"

#include "DockView.h"

#include "Page.h"

#include <afxcmn.h>

//CMainWnd

class CDockView;

class CPage;

struct tagMainWndInfo
{
	wstring strText;
	HICON hIcon = NULL;
	HMENU hMenu = NULL;

	BOOL bSizeable = FALSE;

	UINT uWidth = 0;
	UINT uHeight = 0;

	UINT uMinWidth = 0;
	UINT uMinHeight = 0;
};

class COMMON_EXT_CLASS CMainWnd: public CWnd
{
protected:
	typedef vector<CDockView*> TD_DockViewVector;

public:
	CMainWnd()
	{
	}

	virtual ~CMainWnd()
	{
	}
	
	DECLARE_MESSAGE_MAP()

private:
	vector<int> m_vecStatusPartWidth;

	CStatusBarCtrl m_ctlStatusBar;

	CRect m_rtBlankArea;

	TD_DockViewVector m_vctDockViews;
	
	tagMainWndInfo m_WndInfo;

public:
	virtual BOOL Create(tagMainWndInfo& MainWndInfo);

	virtual void Show();

	BOOL CreateStatusBar(UINT nParts, ...);
	
	BOOL SetStatusText(UINT nPart, const CString& cstrText);

	BOOL AddDockView(CPage& Page, ST_ViewStyle nStyle, UINT nDockSize
		, UINT uOffset = 0, UINT uTabFontSize=0, UINT uTabHeight = 0);

	BOOL AddUndockView(CPage& Page, const CRect& rtPos);

	BOOL AddPage(CPage& Page, ST_ViewStyle nStyle);

	BOOL ActivePage(CPage& Page);

	BOOL SetPageTitle(CPage& Page, const CString& cstrTitle);
	
	int MsgBox(const CString& cstrText, const CString& cstrTitle=L"", UINT uType=MB_OK);
	
private:
	void OnSize();

	CDockView* GetDockView(const CPoint& ptPos);

	BOOL HandleResizeViewMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void ResizeView(CDockView &wndTargetView, CPoint &ptPos);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

// CWndRedrawGuide

class COMMON_EXT_CLASS CRedrawGuide
{
public:
	CRedrawGuide(CWnd& wndCtrl);

	~CRedrawGuide();

	void Unlock();

private:
	CWnd& m_wndCtrl;
};

class COMMON_EXT_CLASS CMenuGuide : public CMenu
{
public:
	CMenuGuide(CPage& Page, UINT nIDMenu);

private:
	CPage& m_Page;
	UINT m_nIDMenu;

	std::map<UINT, std::pair<BOOL, CString>> m_mapMenuItemInfos;

public:
	void EnableItem(UINT nIDItem, BOOL bEnable);

	void SetItemText(UINT nIDItem, const CString& cstrText);

	BOOL Popup();
};
