
#pragma once

#include "def.h"

#include "DragDrop.h"

#include "App.h"


class IModuleApp;

class CMainWnd;

class __CommonPrjExt CPage: public CPropertyPage
{
	DECLARE_DYNAMIC(CPage);

public:
	CPage(IModuleApp& Module, UINT nIDDlgRes, const CString& cstrTitle=L"");

	virtual ~CPage() {}

	DECLARE_MESSAGE_MAP();

	virtual void DoDataExchange(CDataExchange* pDX);

public:
	IModuleApp& m_Module;

	CString m_cstrTitle;

private:
	std::set<HWND> m_setDragableCtrls;

	function<void()> m_AsyncCB;

public:
	BOOL Active();

	BOOL SetTitle(const CString& cstrTitle);

	int MsgBox(const CString& cstrText, UINT uType=MB_OK);

	void Async(const function<void()>& cb, UINT uMsDelay = 0);

protected:
	BOOL OnSetActive();
	BOOL OnKillActive();

	virtual void OnActive(BOOL bActive);

	BOOL RegDragableCtrl(CWnd& wndCtrl)
	{
		ASSERT_RETURN_EX(wndCtrl.GetSafeHwnd(), FALSE);

		m_setDragableCtrls.insert(wndCtrl.GetSafeHwnd());
		
		return TRUE;
	}

	virtual BOOL GetCtrlDragData(CWnd *pwndCtrl, const CPoint& point, LPVOID& pDragData)
	{
		return FALSE;
	}
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
};
