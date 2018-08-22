
#include "stdafx.h"

#include <Page.h>

#include <util.h>

#include "MainWnd.h"


//CPage

IMPLEMENT_DYNAMIC(CPage, CPropertyPage);

CPage::CPage(IModuleApp *pModule, UINT nIDDlgRes, const CString& cstrTitle)
	: m_pModule(pModule)
	, m_cstrTitle(cstrTitle)
{
	m_pModule->ActivateResource();

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
		ENSURE_RETURN_EX(((CMainWnd*)AfxGetMainWnd())->ActivePage(this), FALSE);
	}

	(void)this->SetFocus();

	return TRUE;
}

BOOL CPage::SetTitle(const CString& cstrTitle)
{
	return ((CMainWnd*)AfxGetMainWnd())->SetPageTitle(this, cstrTitle);
}

int CPage::MsgBox(const wstring& strText, UINT uType)
{
	CMainWnd *pMainWnd = (CMainWnd*)AfxGetMainWnd();
	ASSERT_RETURN_EX(pMainWnd, -1);

	return pMainWnd->MsgBox(strText, (LPCWSTR)m_cstrTitle, uType); 
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
