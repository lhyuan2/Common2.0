
#include "stdafx.h"

#include <DockView.h>

#include <util.h>


//CDockView
CDockView::CDockView(CWnd& wndParent, ST_ViewStyle nStyle, UINT nDockSize, UINT uOffset, UINT uTabFontSize, UINT uTabHeight)
	: m_nStyle(nStyle)
	, m_nDockSize(nDockSize)
	, m_uOffset(uOffset)
	, m_uTabFontSize(uTabFontSize)
	, m_uTabHeight(uTabHeight)
{
	CPropertySheet::m_pParentWnd = &wndParent;
	
	m_rtPos.SetRect(0,0,0,0);
}

CDockView::CDockView(CWnd& wndParent, ST_ViewStyle nStyle, const CRect& rtPos)
	: m_nStyle(nStyle)
	, m_rtPos(rtPos)
{
	CPropertySheet::m_pParentWnd = &wndParent;
}

BEGIN_MESSAGE_MAP(CDockView, CPropertySheet)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CDockView::Create()
{
	DWORD dwStyle = WS_CHILD;
	if (!__TabStyle(m_nStyle))
	{
		dwStyle |= WS_BORDER;
	}

	__AssertReturn(__super::Create(m_pParentWnd, dwStyle, WS_EX_CONTROLPARENT), FALSE);

	CTabCtrl *pTabCtrl = this->GetTabControl();
	__AssertReturn(pTabCtrl, FALSE);

	if (__TabStyle(m_nStyle))
	{
		//TCS_FIXEDWIDTH
		pTabCtrl->ModifyStyle(TCS_MULTILINE, TCS_FOCUSNEVER| (VS_BottomTab == __TabStyle(m_nStyle)? TCS_BOTTOM:0));

		if (m_uTabFontSize > 0)
		{
			(void)m_fontGuide.setFontSize(*pTabCtrl, m_uTabFontSize);
		}

		if (m_uTabHeight > 0)
		{
			if (m_ImageList.Create(m_uTabHeight, m_uTabHeight, ILC_COLOR8, 1, 0))
			{
				pTabCtrl->SetImageList(&m_ImageList);
			}
		}
	}
	else
	{
		pTabCtrl->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

BOOL CDockView::AddPage(CPage& Page)
{
	__AssertReturn(!util::ContainerFind(m_vctPages, &Page), FALSE);

	m_vctPages.push_back(&Page);

	CPropertySheet::AddPage(&Page);

	if (!m_hWnd)
	{
		__AssertReturn(this->Create(), FALSE);
	}

	//pPage->MoveWindow(m_rtPos);

	if (__TabStyle(m_nStyle) && !Page.m_cstrTitle.IsEmpty())
	{
		CTabCtrl *pTabCtrl = this->GetTabControl();

		TCITEM tci = {0};
		tci.mask = TCIF_TEXT;
		tci.pszText = (LPTSTR)(LPCTSTR)Page.m_cstrTitle;
		(void)pTabCtrl->SetItem(pTabCtrl->GetItemCount()-1, &tci);
	}

	return TRUE;
}

BOOL CDockView::ActivePage(CPage& Page)
{
	int nActivePage = __super::GetActiveIndex();

	PageVector::iterator itPage = std::find(m_vctPages.begin(), m_vctPages.end(), &Page);
	__EnsureReturn(itPage != m_vctPages.end(), FALSE);

	if (itPage - m_vctPages.begin() != nActivePage)
	{
		(void)__super::SetActivePage(*itPage);
	}

	this->OnSize();

	return TRUE;
}

BOOL CDockView::SetPageTitle(CPage& Page, const CString& cstrTitle)
{
	PageVector::iterator itPage = std::find(m_vctPages.begin(), m_vctPages.end(),& Page);
	__EnsureReturn(itPage != m_vctPages.end(), FALSE);

	int nPage = (int)(itPage - m_vctPages.begin());
	
	CTabCtrl *pTabCtrl = this->GetTabControl();
	__AssertReturn(pTabCtrl->GetItemCount() > nPage, TRUE);

	Page.m_cstrTitle = cstrTitle;

	TCITEM tci = {0};
	tci.mask = TCIF_TEXT;
	tci.pszText = (LPTSTR)(LPCTSTR)Page.m_cstrTitle;
	(void)pTabCtrl->SetItem(nPage, &tci);

	return TRUE;
}

void CDockView::Resize(CRect& rcRestrict)
{
	if (!__DockStyle(m_nStyle))
	{
		m_nDockSize = 0;
	}
	//else
	//{
	//	if (0 == (m_nStyle & VS_FixSize))
	//	{
	//		m_nDockSize = max(m_nDockSize, m_nMinDockSize);
	//	}
	//}

	int nOffSize = m_nDockSize;
	if (0 == (VS_FixSize & m_nStyle))
	{
		nOffSize += __DXView;
	}

	switch (__DockStyle(m_nStyle))
	{
	case VS_DockLeft:
		m_rtPos.SetRect(0, rcRestrict.top + m_uOffset, m_nDockSize, rcRestrict.bottom);
		rcRestrict.left += nOffSize;

		break;
	case VS_DockTop:
		m_rtPos.SetRect(rcRestrict.left, 0, rcRestrict.Width(), m_nDockSize);
		rcRestrict.top += nOffSize;

		break;
	case VS_DockRight:
		m_rtPos.SetRect(rcRestrict.right - m_nDockSize, rcRestrict.top, rcRestrict.right, rcRestrict.bottom);
		rcRestrict.right -= nOffSize;

		break;
	case VS_DockBottom:
		m_rtPos.SetRect(rcRestrict.left, rcRestrict.bottom - m_nDockSize, rcRestrict.right, rcRestrict.bottom);
		rcRestrict.bottom -= nOffSize;

		break;
	case VS_DockCenter:
		m_rtPos.CopyRect(rcRestrict);

		break;
	default:
		break;
	}

	if (this->GetSafeHwnd())
	{
		(void)this->MoveWindow(&m_rtPos);

		(void)this->ShowWindow(SW_SHOW);
	}
}

void CDockView::OnSize(UINT nType, int, int)
{
	__Ensure(SIZE_MINIMIZED != nType);

	this->OnSize();
}

void CDockView::OnSize()
{
	CRect rcClient(0,0,0,0);
	this->GetClientRect(&rcClient);

	CTabCtrl* pTabCtrl = this->GetTabControl();
	__Ensure(pTabCtrl);
	
	int nPage = __super::GetActiveIndex();
	__Assert(nPage < (int)m_vctPages.size());

	CPage* pPage = m_vctPages[nPage];
	__Ensure(pPage->m_hWnd);

	if (!__TabStyle(m_nStyle))
	{
		pPage->MoveWindow(&rcClient);
	}
	else
	{
		pTabCtrl->MoveWindow(0, 0, rcClient.Width(), rcClient.Height());

		CRect rcItem(0, 0, 0, 0);
		(void)pTabCtrl->GetItemRect(0, &rcItem);
		int nItemHeight = rcItem.Height();
		nItemHeight += 2;

		CRect rcPage(1, 1, rcClient.Width()-3, rcClient.Height()-2);
		if (VS_BottomTab == __TabStyle(m_nStyle))
		{
			rcPage.bottom -= nItemHeight;
		}
		else
		{
			rcPage.top += nItemHeight;
		}

		pPage->MoveWindow(&rcPage);
	}
}
//
//if (!m_bTrackStatus)
//{
//	//   鼠标移入窗时，请求WM_MOUSEHOVER和WM_MOUSELEAVE 消息
//	TRACKMOUSEEVENT tme;
//	tme.cbSize = sizeof(tme);
//	tme.hwndTrack = m_hWnd;
//	tme.dwFlags = TME_LEAVE | TME_HOVER;
//	tme.dwHoverTime = HOVER_DEFAULT;
//	m_bTrackStatus = TrackMouseEvent(&tme);
//}

BOOL CDockView::PreTranslateMessage(MSG* pMsg)
{
	if (WM_MOUSEMOVE == pMsg->message)
	{
		auto pTabCtrl = this->GetTabControl();
		if (NULL != pTabCtrl)
		{
			POINT ptPos = pMsg->pt;
			this->ScreenToClient(&ptPos);

			tagTCHITTESTINFO htInfo;
			htInfo.pt = ptPos;
			htInfo.flags = TCHT_ONITEM;
			int iItem = pTabCtrl->HitTest(&htInfo);

			do {
				__EnsureBreak(iItem >= 0);

				__EnsureBreak(iItem != GetActiveIndex());

				__EnsureBreak(iItem < (int)m_vctPages.size());

				__EnsureBreak(m_vctPages[iItem]->m_bAutoActive);

				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				//(void)__super::SetActivePage(iItem);
			} while (0);
		}
	}

	return __super::PreTranslateMessage(pMsg);
}
