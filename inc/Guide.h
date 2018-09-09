#pragma once

#include <def.h>

#include <Page.h>

// CWndRedrawGuide

class __CommonPrjExt CRedrawLockGuide
{
public:
	CRedrawLockGuide(CWnd& wndCtrl);

	~CRedrawLockGuide();

	void Unlock();

private:
	CWnd& m_wndCtrl;
};

class __CommonPrjExt CMenuGuide : public CMenu
{
public:
	CMenuGuide(CPage& Page, UINT nIDMenu);

private:
	CPage& m_Page;
	UINT m_uIDMenu;

	map<UINT, pair<BOOL, CString>> m_mapMenuItemInfos;

public:
	void EnableItem(UINT nIDItem, BOOL bEnable);

	void SetItemText(UINT nIDItem, const CString& cstrText);

	BOOL Popup();
};

class __CommonPrjExt CFontGuide
{
public:
	CFontGuide() {};

private:
	CFont m_font;

public:
	bool setFontSize(CWnd& wnd, ULONG uFontSize);
};
