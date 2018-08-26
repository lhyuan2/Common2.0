
#pragma once

#include <def.h>

#include <fsutil.h>

#include <ObjectCtrl.h>

class CPathObject;
typedef ptrlist<CPathObject*> TD_PathObjectList;

class __CommonPrjExt CPathObject: public CPath, public CListObject
{
public:
	CPathObject()
	{
	}

	CPathObject(const wstring& strDir)
		: CPath(strDir)
	{
	}

	CPathObject(const wstring& strName, const TD_PathObjectList& lstSubPathObjects)
		: CPath(strName, TD_PathList(lstSubPathObjects))
	{
	}

	CPathObject(CFileFind &FindFile, CPath *pParentPath)
		: CPath(FindFile, pParentPath)
	{
	}

	virtual ~CPathObject()
	{
	}

protected:
	virtual CPath *NewSubPath(CFileFind &FindFile, CPath *pParentPath)
	{
		return new CPathObject(FindFile, pParentPath);
	}

public:
	CString GetFileSize()
	{
		if (m_bDir)
		{
			return L"";
		}

		LONG nK = (LONG)(m_uFileSize / 1000);

		int nM = nK / 1000;
		nK %= 1000;

		CString strFileSize;
		strFileSize.Format(_T("%3d,%d"), nM, nK);
		
		return strFileSize;
	}

	CString GetFileModifyTime()
	{
		if (m_bDir)
		{
			return L"";
		}

		return CTime(m_modifyTime).Format(_T("%y-%m-%d %H:%M"));
	}

	void GetListText(std::list<CString>& lstTitles)
	{
		lstTitles.push_back(m_strName.c_str());
		
		lstTitles.push_back(GetFileSize());
		
		lstTitles.push_back(GetFileModifyTime());
	}
};


class CDirObject;

typedef ptrlist<CDirObject*> TD_DirObjectList;

class CDirObject : public CPathObject, public CTreeObject
{
public:
	CDirObject(const wstring& strDir=L"")
		: CPathObject(strDir)
	{
	}
	
	CDirObject(const wstring& strName, const TD_DirObjectList& lstSubDirObjects)
		: CPathObject(strName, TD_PathList(lstSubDirObjects))
	{
	}

	CDirObject(CFileFind &FindFile, CPath *pParentPath)
		: CPathObject(FindFile, pParentPath)
	{
	}

	virtual ~CDirObject()
	{
	}

protected:
	virtual CPath *NewSubPath(CFileFind &FindFile, CPath *pParentPath)
	{
		ENSURE_RETURN_EX(FindFile.IsDirectory(), NULL);

		return new CDirObject(FindFile, pParentPath);
	}

public:
	CString GetTreeText()
	{
		return m_strName.c_str();
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds)
	{
		TD_PathList lstSubPaths;
		this->GetSubPath(lstSubPaths);
		
		TD_DirObjectList lstDirObjects(lstSubPaths);
		lstChilds.Insert(lstDirObjects);
	}
};

template <typename T>
class CDirTree: public T
{
	struct tagDirSortor
	{
		bool operator () (CPath *pPath1, CPath *pPath2)
		{
			return 0 >= StrCmpA(pPath1->m_strName.c_str(), pPath2->m_strName.c_str());
		}
	};
	
public:
	CDirTree()
		: m_pRootDir(NULL)
	{
	}

	virtual	~CDirTree(void)
	{
	}

private:
	CDirObject *m_pRootDir;
	 
public:
	void SetRootDir(CDirObject *pRootDir, BOOL bShowRoot)
	{
		(void)DeleteAllItems();

		m_pRootDir = pRootDir;
		m_pRootDir->FindFile();
	
		if (bShowRoot)
		{
			(void)InsertObject(*m_pRootDir);
			InsertChilds(m_pRootDir);

			(void)__super::SelectItem(m_pRootDir->m_hTreeItem);
			(void)__super::Expand(m_pRootDir->m_hTreeItem, TVE_EXPAND);
		}
		else
		{
			InsertChilds(m_pRootDir);
			InsertChildsEx(m_pRootDir);
		}
	}

	void InsertChilds(CDirObject *pDirObject)
	{
		TD_PathList lstSubDirs;
		pDirObject->GetSubPath(lstSubDirs, TRUE, FALSE);
	
		for (TD_PathList::iterator itSubDir = lstSubDirs.begin()
			; itSubDir != lstSubDirs.end(); ++itSubDir)
		{
			(void)InsertObject(*(CDirObject*)*itSubDir, pDirObject);
		}
	}
	
	virtual void OnNMNotify(NMHDR* pNMHDR, BOOL* pResult)
	{
		if (TVN_ITEMEXPANDING == pNMHDR->code)
		{
			*pResult = 0;
			LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
			if (TVE_EXPAND == pNMTreeView->action)
			{
				CWaitCursor WaitCursor;

				CDirObject* pDirObject = (CDirObject*)__super::GetItemObject(pNMTreeView->itemNew.hItem);
				ASSERT_RETURN(pDirObject);
				InsertChildsEx(pDirObject);
			}
		}
	}

private:
	void InsertChildsEx(CDirObject *pDirObject)
	{
		TD_PathList lstSubDirs;
		pDirObject->GetSubPath(lstSubDirs, TRUE, FALSE);

		for (TD_PathList::iterator itSubDir = lstSubDirs.begin()
			; itSubDir != lstSubDirs.end(); ++itSubDir)
		{
			InsertChilds((CDirObject*)*itSubDir);
		}
	}
};


class __CommonPrjExt CPathList: public CObjectList
{
public:
	CPathList(const ColumnList& lstColumns, bool bChangeView = true)
		: CObjectList(bChangeView)
	{
		if (!lstColumns.empty())
		{
			m_lstColumns = lstColumns;
		}
	}

private:
	ColumnList m_lstColumns = {
		{_T("名称"), 400}
		, {_T("大小"), 80}
		, {_T("修改日期"), 150}
	};
	
private:
	virtual void PreSubclassWindow() override;

public:
	BOOL InitCtrl(UINT uFontSize, const TD_ICONLIST& lstIcon, UINT uSize, UINT uSmallSize=0);

	BOOL InitCtrl(UINT uFontSize = 0, CBitmap *pBitmap = NULL, CBitmap *pBitmapSmall = NULL);

	void SetPath(CPathObject* pPath);

	BOOL IsFileItem(int nItem);
	
	void GetAllPathObjects(TD_PathObjectList& lstPathObjects);

	void GetAllPathObjects(TD_PathObjectList& lstPathObjects, bool bDir);
};
