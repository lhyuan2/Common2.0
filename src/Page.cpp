
#include "stdafx.h"

#include <Page.h>

#include <util.h>

#include "MainWnd.h"


//CPage

IMPLEMENT_DYNAMIC(CPage, CPropertyPage);

CPage::CPage(IModuleApp& Module, UINT nIDDlgRes, const CString& cstrTitle)
	: m_Module(Module)
	, m_cstrTitle(cstrTitle)
{
	m_Module.ActivateResource();

	CPropertyPage::CommonConstruct(MAKEINTRESOURCE(nIDDlgRes), 0);
}

BEGIN_MESSAGE_MAP(CPage, CPropertyPage)
END_MESSAGE_MAP()

void CPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BOOL CPage::Active()
{
	if (!::IsWindowVisible(this->m_hWnd))
	{
		ENSURE_RETURN_EX(CMainApp::GetMainWnd()->ActivePage(*this), FALSE);
	}

	(void)this->SetFocus();

	return TRUE;
}

BOOL CPage::SetTitle(const CString& cstrTitle)
{
	return CMainApp::GetMainWnd()->SetPageTitle(*this, cstrTitle);
}

int CPage::MsgBox(const CString& cstrText, UINT uType)
{
	return CMainApp::GetMainWnd()->MsgBox(cstrText, m_cstrTitle, uType);
}

BOOL CPage::OnSetActive()
{
	(void)this->Active();
	
	OnActive(TRUE);

	return __super::OnSetActive();
}

BOOL CPage::OnKillActive()
{
	OnActive(FALSE);

	return __super::OnKillActive();
}

void CPage::OnActive(BOOL bActive)
{
	//do nothing
}

BOOL CPage::PreTranslateMessage(MSG* pMsg)
{
	static BOOL bDragable = FALSE;
	static CPoint ptLButtonDown(0, 0);

	if (pMsg->hwnd != m_hWnd && !m_setDragableCtrls.empty() && util::ContainerFind(m_setDragableCtrls, pMsg->hwnd))
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			bDragable = TRUE;
			ptLButtonDown = CPoint(pMsg->lParam);

			break;
		case WM_LBUTTONUP:
			bDragable = FALSE;

			break;
		case WM_RBUTTONDOWN:
			bDragable = FALSE;
			
			break;
		case WM_MOUSEMOVE:
			if (bDragable && (MK_LBUTTON & GET_FLAGS_LPARAM(pMsg->wParam)))
			{
				CPoint point(pMsg->lParam);
				if (abs(point.x - ptLButtonDown.x) > 1 || abs(point.y - ptLButtonDown.y) > 1)
				{
					bDragable = FALSE;
					
					LPVOID pDragData = NULL;
					if (GetCtrlDragData(CWnd::FromHandle(pMsg->hwnd), point, pDragData))
					{
						(void)::SetFocus(pMsg->hwnd);

						(void)CDragDropMgr::DoDrag(pDragData);

						return TRUE;
					}
				}
			}

			break;
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CPage::Async(const function<void()>& cb, UINT uMsDelay)
{
	m_AsyncCB = cb;

	thread thr([&]() {
		if (0 != uMsDelay)
		{
			::Sleep(uMsDelay);
		}

		this->PostMessage(WM_USER);
	});
	thr.detach();
}

BOOL CPage::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_USER == message)
	{
		if (m_AsyncCB)
		{
			m_AsyncCB();
		}

		return TRUE;
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}
