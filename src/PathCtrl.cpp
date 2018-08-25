
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

BOOL CPathList::InitCtrl(UINT uFontSize, const TD_ICONLIST& lstIcon, UINT uSize, UINT uSmallSize)
{
	ENSURE_RETURN_EX(__super::InitCtrl(uFontSize, m_lstColumns), FALSE);

	ENSURE_RETURN_EX(__super::InitImage(lstIcon, uSize, uSmallSize), FALSE);

	return TRUE;
}

BOOL CPathList::InitCtrl(UINT uFontSize, CBitmap *pBitmap, CBitmap *pBitmapSmall)
{
	ENSURE_RETURN_EX(__super::InitCtrl(uFontSize, m_lstColumns), FALSE);

	CBitmap Bitmap;
	if (NULL == pBitmap)
	{
		HBITMAP hBitmap = ::LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_PATHCTRL_NORMAL));
		ASSERT_RETURN_EX(hBitmap, FALSE);
		Bitmap.Attach(hBitmap);

		pBitmap = &Bitmap;
	}
	ENSURE_RETURN_EX(__super::InitImage(*pBitmap, pBitmapSmall), FALSE);

	(void)Bitmap.DeleteObject();

	return TRUE;
}

void CPathList::SetPath(CPathObject* pPath)
{
	TD_ListObjectList lstObjects;

	if (pPath)
	{
		TD_PathList lstSubPaths;
		pPath->GetSubPath(lstSubPaths, TRUE, TRUE);
		
		lstObjects.Insert(TD_PathObjectList(lstSubPaths));
	}

	__super::SetObjects(lstObjects);
}

BOOL CPathList::IsFileItem(int nItem)
{
	ENSURE_RETURN_EX(0 <= nItem, FALSE);

	CPathObject *pPath = (CPathObject*)GetItemObject(nItem);
	ENSURE_RETURN_EX(pPath, FALSE);
		
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
