
#include "StdAfx.h"

#include <PathCtrl.h>

#include "Resource.h"

#include <util.h>


//CPathList

void CPathList::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	(void)ModifyStyle(LVS_ALIGNLEFT, LVS_AUTOARRANGE);
}

BOOL CPathList::InitCtrl(UINT uFontSize)
{
	return __super::InitCtrl(uFontSize, m_lstColumns);
}

BOOL CPathList::InitCtrl(UINT uFontSize, const CSize& szImglst, const CSize *pszSmallImglst, const TD_IconVec& vecIcons)
{
	__EnsureReturn(__super::InitCtrl(uFontSize, m_lstColumns), FALSE);

	__EnsureReturn(__super::InitImglst(szImglst, pszSmallImglst, vecIcons), FALSE);

	return TRUE;
}

BOOL CPathList::InitCtrlEx(UINT uFontSize)
{
	__EnsureReturn(__super::InitCtrl(uFontSize, m_lstColumns), FALSE);

	CBitmap Bitmap;
	HBITMAP hBitmap = ::LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_PATHCTRL_NORMAL));
	__AssertReturn(hBitmap, FALSE);
	Bitmap.Attach(hBitmap);

	__EnsureReturn(__super::InitImglst(Bitmap, &Bitmap), FALSE);

	(void)Bitmap.DeleteObject();

	return TRUE;
}

void CPathList::SetPath(CPathObject* pPath)
{
	if (pPath)
	{
		TD_PathList lstSubPaths;
		pPath->GetSubPath(lstSubPaths);

		this->SetRedraw(FALSE);
		DeleteAllItems();

		TD_ListObjectList lstObjects;
		lstObjects.Insert(TD_PathObjectList(lstSubPaths));
		__super::SetObjects(lstObjects);

		this->SetRedraw(TRUE);
	}
	else
	{
		DeleteAllItems();
	}
}

BOOL CPathList::IsFileItem(int nItem)
{
	__EnsureReturn(0 <= nItem, FALSE);

	CPathObject *pPath = (CPathObject*)GetItemObject(nItem);
	__EnsureReturn(pPath, FALSE);
		
	return !pPath->m_bDir;
}

void CPathList::GetAllPathObjects(TD_PathObjectList& lstPathObjects)
{
	TD_ListObjectList lstListObjects;
	__super::GetAllObjects(lstListObjects);

	lstPathObjects.Insert(lstListObjects);
}

void CPathList::GetAllPathObjects(TD_PathObjectList& lstPathObjects, bool bDir)
{
	TD_PathObjectList result;
	GetAllPathObjects(result);

	for (TD_PathObjectList::iterator itrPathObject = result.begin()
		; itrPathObject != result.end(); ++itrPathObject)
	{
		if ((*itrPathObject)->m_bDir == bDir)
		{
			lstPathObjects.push_back(*itrPathObject);
		}
	}
}
