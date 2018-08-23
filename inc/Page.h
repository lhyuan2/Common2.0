
#pragma once

#include "def.h"

#include "DragDrop.h"

#include "App.h"


class IModuleApp;

class CMainWnd;

class COMMON_EXT_CLASS CPage: public CPropertyPage
{
	DECLARE_DYNAMIC(CPage);

public:
	CPage(IModuleApp *pModule, UINT nIDDlgRes, const CString& cstrTitle=L"");

	virtual ~CPage() {}

	DECLARE_MESSAGE_MAP();

	virtual void DoDataExchange(CDataExchange* pDX);

public:
	IModuleApp *m_pModule;

public:
	CString m_cstrTitle;

	std::set<HWND> m_setDragableCtrls;

public:
	BOOL Active();

	BOOL SetTitle(const CString& cstrTitle);

	int MsgBox(const CString& cstrText, UINT uType=MB_OK);

protected:
	BOOL OnSetActive();
	BOOL OnKillActive();

	virtual void OnActive(BOOL bActive);

	BOOL RegDragableCtrl(CWnd *pwndCtrl)
	{
		ASSERT_RETURN_EX(pwndCtrl->GetSafeHwnd(), FALSE);

		m_setDragableCtrls.insert(pwndCtrl->GetSafeHwnd());
		
		return TRUE;
	}

	virtual BOOL GetCtrlDragData(CWnd *pwndCtrl, const CPoint& point, LPVOID& pDragData)
	{
		return FALSE;
	}
	
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
