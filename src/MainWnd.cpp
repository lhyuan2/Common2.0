
#include "stdafx.h"

#include <MainWnd.h>

#include <util.h>


#define __CursorName(_Style) ( \
		VS_DockLeft == __DockStyle(_Style) || VS_DockRight == __DockStyle(_Style) \
	) \
	? \
		IDC_SIZEWE \
	: \
		(\
			(VS_DockTop == __DockStyle(_Style) || VS_DockBottom == __DockStyle(_Style)) \
			? \
				IDC_SIZENS \
			: \
				IDC_ARROW \
		)

BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

BOOL CMainWnd::Create(tagMainWndInfo& MainWndInfo)
{
	m_WndInfo = MainWndInfo;

	LPCTSTR lpszClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS
		, 0, ::GetSysColorBrush(CTLCOLOR_DLG), m_WndInfo.hIcon);
	ASSERT_RETURN_EX(lpszClassName, FALSE);

	DWORD dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
	//if (0 == m_WndInfo.uWidth || 0 == m_WndInfo.uHeight)
	//{
	//	dwStyle |= WS_MAXIMIZE;
	//}
	ASSERT_RETURN_EX(
		this->CreateEx(WS_EX_OVERLAPPEDWINDOW, lpszClassName
			, m_WndInfo.strText.c_str(), dwStyle, 0, 0
			, m_WndInfo.uWidth, m_WndInfo.uHeight, NULL, m_WndInfo.hMenu)
		, FALSE);

	//if (NULL != m_WndInfo.hIcon)
	//{
	//	this->SetIcon(m_WndInfo.hIcon, TRUE);
	//	this->SetIcon(m_WndInfo.hIcon, FALSE);
	//}

	return TRUE;
}

void CMainWnd::Show()
{
	this->CenterWindow();

	if (0 == m_WndInfo.uWidth || 0 == m_WndInfo.uHeight)
	{
		(void)this->ShowWindow(SW_SHOWMAXIMIZED);
	}
	else
	{
		(void)this->ShowWindow(SW_SHOW);
	}

	if (!m_WndInfo.bSizeable)
	{
		(void)this->ModifyStyle(WS_THICKFRAME, 0);
	}
}

void CMainWnd::OnSize(UINT nType, int cx, int cy)
{
	if (SIZE_MINIMIZED == nType)
	{
		return;
	}
	else if (SIZE_RESTORED == nType)
	{
		if (!m_WndInfo.bSizeable && ::IsWindowVisible(m_hWnd))
		{
			(void)this->ShowWindow(SW_MAXIMIZE);
			return;
		}
	}

	OnSize();
}

void CMainWnd::OnSize()
{
	this->GetClientRect(&m_rtBlankArea);
	if (m_ctlStatusBar.m_hWnd)
	{
		CRect rcStatusBar(0,0,0,0);
		m_ctlStatusBar.GetWindowRect(&rcStatusBar);

		m_rtBlankArea.bottom -= rcStatusBar.Height();

		m_ctlStatusBar.MoveWindow(0, m_rtBlankArea.bottom, m_rtBlankArea.Width(), rcStatusBar.Height());
	}

	CDockView *pCenterView = NULL;
	for (TD_DockViewVector::iterator itView=m_vctDockViews.begin(); itView!=m_vctDockViews.end(); ++itView)
	{
		if (VS_DockCenter == __DockStyle((*itView)->m_nStyle))
		{
			pCenterView = *itView;

			continue;
		}

		(*itView)->Resize(m_rtBlankArea);
	}

	if (pCenterView)
	{
		pCenterView->Resize(m_rtBlankArea);
	}
}

BOOL CMainWnd::CreateStatusBar(UINT nParts, ...)
{
	if (0 == nParts)
	{
		return FALSE;
	}

	m_vecStatusPartWidth.resize(nParts);

	va_list argList;
	va_start( argList, nParts );

	int nWidth = 0;
	for (UINT nIndex = 0; nIndex<nParts; nIndex++)
	{
		nWidth += va_arg(argList, int);

		m_vecStatusPartWidth[nIndex] = nWidth;
	}

	va_end( argList );

	if (!m_ctlStatusBar.Create(WS_VISIBLE| WS_CHILD, CRect(0,0,100,100), this, 0))
	{
		return FALSE;
	}

	m_ctlStatusBar.SetParts(nParts, &m_vecStatusPartWidth[0]);

	return TRUE;
}

BOOL CMainWnd::SetStatusText(UINT nPart, const CString& cstrText)
{
	ASSERT_RETURN_EX(m_ctlStatusBar.m_hWnd, FALSE)
	
	return m_ctlStatusBar.SetText(cstrText, (int)nPart, 0);
}

BOOL CMainWnd::AddDockView(CPage& Page, ST_ViewStyle nStyle, UINT nDockSize
	, UINT uOffset, UINT uTabFontSize, UINT uTabHeight)
{
	CDockView *pView = new CDockView(*this, nStyle, nDockSize, uOffset, uTabFontSize, uTabHeight);

	ASSERT_RETURN_EX(pView->AddPage(Page), FALSE);

	m_vctDockViews.push_back(pView);

	this->OnSize();

	return TRUE;
}

BOOL CMainWnd::AddUndockView(CPage& Page, const CRect& rtPos)
{
	CDockView *pView = new CDockView(*this, VS_Undock, rtPos);

	ASSERT_RETURN_EX(pView->AddPage(Page), FALSE);

	m_vctDockViews.push_back(pView);

	this->OnSize();
	
	return TRUE;
}

BOOL CMainWnd::AddPage(CPage& Page, ST_ViewStyle nStyle)
{
	for (TD_DockViewVector::iterator itView=m_vctDockViews.begin(); itView!=m_vctDockViews.end(); ++itView)
	{
		if ((*itView)->m_nStyle & nStyle)
		{
			return (*itView)->AddPage(Page);
		}
	}

	ASSERT(FALSE);
	return FALSE;
}

BOOL CMainWnd::ActivePage(CPage& Page)
{
	for (TD_DockViewVector::iterator itView=m_vctDockViews.begin(); itView!=m_vctDockViews.end(); ++itView)
	{
		if ((*itView)->ActivePage(Page))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMainWnd::SetPageTitle(CPage& Page, const CString& cstrTitle)
{
	for (TD_DockViewVector::iterator itView=m_vctDockViews.begin(); itView!=m_vctDockViews.end(); ++itView)
	{
		if ((*itView)->SetPageTitle(Page, cstrTitle))
		{
			return TRUE;
		}
	}

	return FALSE;
}

int CMainWnd::MsgBox(const CString& cstrText, const CString& cstrTitle, UINT uType)
{
	LPCTSTR pszTitle = NULL;
	if (!cstrTitle.IsEmpty())
	{
		pszTitle = cstrTitle;
	}
	else
	{
		pszTitle = m_WndInfo.strText.c_str();
	}

	int nResult = ::MessageBox(this->GetSafeHwnd(), cstrText, pszTitle, uType);

	(void)this->EnableWindow(TRUE);

	(void)this->SetFocus();

	CMainApp::DoEvents();

	return nResult;
}

void CMainWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_WndInfo.bSizeable)
	{
		if (0 != m_WndInfo.uMinWidth && 0 != m_WndInfo.uMinHeight)
		{
			lpMMI->ptMinTrackSize = { (LONG)m_WndInfo.uMinWidth, (LONG)m_WndInfo.uMinHeight };
		}
	}
	else
	{
		if (0 != m_WndInfo.uWidth && 0 != m_WndInfo.uHeight)
		{
			lpMMI->ptMaxSize = { (LONG)m_WndInfo.uWidth, (LONG)m_WndInfo.uHeight };
		}
	}
}

CDockView* CMainWnd::GetDockView(const CPoint& ptPos)
{
	CDockView* pView = NULL;
	CRect rcPos(0,0,0,0);
	UINT nDockStyle = 0;

	for (TD_DockViewVector::iterator itView=m_vctDockViews.begin(); itView!=m_vctDockViews.end(); ++itView)
	{
		pView = *itView;

		nDockStyle = __DockStyle(pView->m_nStyle);
		if (nDockStyle && VS_DockCenter != nDockStyle)
		{
			continue;
		}

		pView->GetWindowRect(&rcPos);
		this->ScreenToClient(&rcPos);
		
		if (
			(VS_DockLeft == nDockStyle && PtInRect(&CRect(rcPos.right, rcPos.top, rcPos.right+__DXView, rcPos.bottom), ptPos))
			|| (VS_DockTop == nDockStyle && PtInRect(&CRect(rcPos.left, rcPos.bottom, rcPos.right, rcPos.bottom+__DXView), ptPos))
			|| (VS_DockRight == nDockStyle && PtInRect(&CRect(rcPos.left-__DXView, rcPos.top, rcPos.left, rcPos.bottom), ptPos))
			|| (VS_DockBottom == nDockStyle && PtInRect(&CRect(rcPos.left, rcPos.top-__DXView, rcPos.right, rcPos.top), ptPos))
			)
		{
			return pView;
		}
	}

	return NULL;
}

void CMainWnd::ResizeView(CDockView &wndTargetView, CPoint &ptPos)
{
	CRect rcPos(0,0,0,0);
	this->GetClientRect(&rcPos);

	if (!::PtInRect(&rcPos, ptPos))
	{
		return;
	}

	switch (__DockStyle(wndTargetView.m_nStyle))
	{
	case VS_DockLeft:
		wndTargetView.m_nDockSize = ptPos.x;

		break;
	case VS_DockTop:
		wndTargetView.m_nDockSize = ptPos.y;
		
		break;
	case VS_DockRight:
		wndTargetView.m_nDockSize = rcPos.right - ptPos.x;
		
		break;
	case VS_DockBottom:
		wndTargetView.m_nDockSize = rcPos.bottom - ptPos.y;
		
		break;
	default:
		break;
	}
			
	CMainApp::DoEvents();
	OnSize(0, 0, 0);
}

BOOL CMainWnd::HandleResizeViewMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	static CDockView* pTargetView = NULL;

	switch (message)
	{
	case WM_CAPTURECHANGED:
		pTargetView = NULL;
		
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();

		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
		{
			CPoint ptPos(lParam);

			if (WM_MOUSEMOVE == message && pTargetView)
			{
				ResizeView(*pTargetView, ptPos);
				break;
			}

			CDockView* pView = GetDockView(ptPos);
			ENSURE_BREAK(pView);
						
			if (pView->m_nStyle & VS_FixSize)
			{
				break;
			}

			::SetCursor(::LoadCursor(NULL, __CursorName(pView->m_nStyle)));
			if (WM_LBUTTONDOWN == message)
			{
				pTargetView = pView;

				this->SetCapture();
			}
		}

		break;
	default:
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMainWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_NCLBUTTONDBLCLK == message)
	{
		if (!m_WndInfo.bSizeable)
		{
			return true;
		}
	}

	if (HandleResizeViewMessage(message, wParam, lParam))
	{
		return TRUE;
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}


// CRedrawGuide

CRedrawGuide::CRedrawGuide(CWnd& wndCtrl)
	: m_wndCtrl(wndCtrl)
{
	m_wndCtrl.SetRedraw(FALSE);
}

CRedrawGuide::~CRedrawGuide()
{
	this->Unlock();
}

void CRedrawGuide::Unlock()
{
	m_wndCtrl.SetRedraw(TRUE);
}


CMenuGuide::CMenuGuide(CPage& Page, UINT nIDMenu)
	: m_Page(Page)
	, m_nIDMenu(nIDMenu)
{
}

void CMenuGuide::EnableItem(UINT nIDItem, BOOL bEnable)
{
	m_mapMenuItemInfos[nIDItem].first = bEnable;
}

void CMenuGuide::SetItemText(UINT nIDItem, const CString& cstrText)
{	
	m_mapMenuItemInfos[nIDItem].second = cstrText;
}

BOOL CMenuGuide::Popup()
{
	CMainApp::DoEvents();
	
	if (!*this)
	{
		CResourceLock ResourceLock(m_Page.m_Module);

		ASSERT_RETURN_EX(__super::LoadMenu(m_nIDMenu), FALSE);
	}

	CMenu *pSubMenu = __super::GetSubMenu(0);
	ASSERT_RETURN_EX(pSubMenu, FALSE);

	UINT nIDItem = 0;
	BOOL bEnable = FALSE;
	CString *pcstrText = NULL;
	for (std::map<UINT, std::pair<BOOL, CString>>::iterator itrMenuItemInfo = m_mapMenuItemInfos.begin()
		; itrMenuItemInfo != m_mapMenuItemInfos.end(); ++itrMenuItemInfo)
	{
		nIDItem = itrMenuItemInfo->first;
		bEnable = itrMenuItemInfo->second.first;
		pcstrText = &itrMenuItemInfo->second.second;
		
		if (!pcstrText->IsEmpty())
		{
			ASSERT_RETURN_EX(pSubMenu->ModifyMenu(nIDItem, MF_BYCOMMAND|MF_STRING, nIDItem, *pcstrText), FALSE);
		}
	
		(void)pSubMenu->EnableMenuItem(nIDItem, bEnable?MF_ENABLED:MF_GRAYED);
	}

	m_mapMenuItemInfos.clear();

	CPoint ptCursor(0, 0);
	(void)::GetCursorPos(&ptCursor);

	return pSubMenu->TrackPopupMenu(0, ptCursor.x, ptCursor.y, &m_Page);
}
