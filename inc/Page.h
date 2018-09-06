
#pragma once

#include "def.h"

#include "DragDrop.h"

#include "App.h"


class IModuleApp;

class CMainWnd;

using CB_Async = function<void()>;
using CB_AsyncLoop = function<bool()>;

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
	set<HWND> m_setDragableCtrls;

	CB_Async m_cbAsync;

	CB_AsyncLoop m_cbAsyncLoop;

	void _AsyncLoop(UINT uDelayTime);

public:
	BOOL Active();

	BOOL SetTitle(const CString& cstrTitle);

	int MsgBox(const CString& cstrText, UINT uType=MB_OK);

	void Async(const CB_Async& cb, UINT uDelayTime=0);
	void AsyncLoop(const CB_AsyncLoop& cb, UINT uDelayTime);

protected:
	BOOL OnSetActive();
	BOOL OnKillActive();

	virtual void OnActive(BOOL bActive);

	BOOL RegDragableCtrl(CWnd& wndCtrl)
	{
		__AssertReturn(wndCtrl.GetSafeHwnd(), FALSE);

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
