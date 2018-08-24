#pragma once

#include "def.h"

#include <afxole.h>


class CDropTarget
{
public:
	virtual DROPEFFECT OnDragOver(CWnd *pWnd, LPVOID pDragData, DWORD dwKeyState, CPoint point, BOOL bFirstTime=FALSE) = 0;

	virtual BOOL OnDrop(CWnd *pWnd, LPVOID pDragData, DROPEFFECT dropEffect, CPoint point) = 0;

	virtual void OnDragLeave(CWnd *pWnd, LPVOID pDragData) = 0;
};

class __CommonPrjExt CDragDropMgr : private COleDropTarget
{
public:
	CDragDropMgr()
	{
	}

private:
	static LPVOID m_pCurrDragData;
	static std::map<CWnd*, CDropTarget*> m_mapDropTargets;

public:
	static BOOL DoDrag(LPVOID pDragData=NULL);

	static BOOL RegDropTarget(CDropTarget& DropTarget, CWnd& Wnd);

public:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
};
