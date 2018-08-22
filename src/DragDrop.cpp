
#include "stdafx.h"

#include <util.h>

#include <DragDrop.h>

#define __InnerClipboardFormat ((UINT)-1)

LPVOID CDragDropMgr::m_pCurrDragData = NULL;
std::map<CWnd*, CDropTarget*> CDragDropMgr::m_mapDropTargets;

BOOL CDragDropMgr::DoDrag(LPVOID pDragData)
{
	/*
	HGLOBAL hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE, sizeof(LPVOID));
	LPVOID *ppData = (LPVOID*)::GlobalLock(hGlobal);
	*ppData = pData;
	ENSURE_RETURN_EX(::GlobalUnlock(hGlobal), FALSE);

	COleDataSource DataSource;
	DataSource.CacheGlobalData(__InnerClipboardFormat, hGlobal);
	*/

	m_pCurrDragData = pDragData;
	
	DROPEFFECT dwResult = COleDataSource().DoDragDrop();

	m_pCurrDragData = NULL;

	//(void)::GlobalFree(hGlobal);

	return TRUE;
}

BOOL CDragDropMgr::RegDropTarget(CDropTarget *pDropTarget, CWnd *pWnd)
{
	ASSERT_RETURN_EX(pDropTarget && pWnd->GetSafeHwnd(), FALSE);

	if (m_mapDropTargets.find(pWnd) != m_mapDropTargets.end())
	{
		return TRUE;
	}

	ASSERT_RETURN_EX((new CDragDropMgr())->Register(pWnd), FALSE);

	m_mapDropTargets[pWnd] = pDropTarget;

	return TRUE;
}

DROPEFFECT CDragDropMgr::OnDragEnter(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	ASSERT_RETURN_EX(m_mapDropTargets[pWnd], DROPEFFECT_NONE);
		
	return m_mapDropTargets[pWnd]->OnDragOver(pWnd, m_pCurrDragData, dwKeyState, point, TRUE);
}

DROPEFFECT CDragDropMgr::OnDragOver(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	ASSERT_RETURN_EX(m_mapDropTargets[pWnd], DROPEFFECT_NONE);
	
	return m_mapDropTargets[pWnd]->OnDragOver(pWnd, m_pCurrDragData, dwKeyState, point);
}

BOOL CDragDropMgr::OnDrop(CWnd *pWnd, COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	ASSERT_RETURN_EX(m_mapDropTargets[pWnd], FALSE);

	BOOL bResult = m_mapDropTargets[pWnd]->OnDrop(pWnd, m_pCurrDragData, dropEffect, point);

	return bResult;
}

void CDragDropMgr::OnDragLeave(CWnd *pWnd)
{
	ASSERT_RETURN(m_mapDropTargets[pWnd]);

	m_mapDropTargets[pWnd]->OnDragLeave(pWnd, m_pCurrDragData);
}
