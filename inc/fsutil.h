
#pragma once

#include "def.h"

#include "util.h"

class __CommonPrjExt fsutil
{
public:
	static void SplitPath(const wstring& strPath, wstring *pstrDir, wstring *pstrFile);

	static wstring GetFileName(const wstring& strPath);

	static void GetFileName(const wstring& strPath, wstring *pstrTitle, wstring *pstrExtName);

	static wstring GetFileTitle(const wstring& strPath);

	static wstring GetFileExtName(const wstring& strPath);

	static wstring GetParentPath(const wstring& strPath);

	static BOOL CheckPathInside(const wstring& strSubPath, const wstring& strPath);

	static wstring GetRelativePath(const wstring& strPath, const wstring strBasePath);

	static void GetSysDrivers(std::list<wstring>& lstDrivers);
	
	static BOOL DeletePath(const wstring& strPath, CWnd *pwndParent=NULL, const wstring& strTitle=L"");

	static BOOL CopyFile(const wstring& strSrcFile, const wstring& strSnkFile);

	static void ExplorePath(const list<wstring>& lstPath);

	static BOOL CreateDir(const wstring& strDir);

	static void FindFile(const wstring& strPath, std::list<wstring>& lstResult);

	static BOOL ExistsFile(const wstring& strFile);
	static BOOL ExistsPath(const wstring& strDir);

	// 获取文件夹图标
	static HICON getFolderIcon();
	// 获取文件图标
	static HICON getFileIcon(const std::wstring& extention);
};


//CPath
class CPath;
typedef ptrlist<CPath*> TD_PathList;

class __CommonPrjExt CPath
{
	struct tagPathSortor
	{
		bool operator () (CPath *pPath1, CPath *pPath2)
		{
			if (pPath1->m_bDir && !pPath2->m_bDir)
			{
				return true;
			}

			if (pPath1->m_bDir == pPath2->m_bDir)
			{
				return 0 >= wcscmp(pPath1->m_strName.c_str(), pPath2->m_strName.c_str());
			}

			return false;
		}
	};

public:
	CPath()
	{
	}

	CPath(const wstring& strDir)
		: m_bDir(true)
		, m_strName(strDir)
	{
	}

	CPath(CFileFind& fileFind, CPath *pParentPath)
		: m_bDir(fileFind.IsDirectory())
		, m_strName(fileFind.GetFileName())
		, m_uFileSize(fileFind.GetLength())
		, m_pParentPath(pParentPath)
	{
		//(void)fileFind.GetCreationTime(&m_createTime);
		(void)fileFind.GetLastWriteTime(&m_modifyTime);
	}

	CPath(const wstring& strName, const TD_PathList & lstSubPath)
		: m_strName(strName)
		, m_bDir(TRUE)
	{
		m_plstSubPath = new TD_PathList();
		m_plstSubPath->Insert(lstSubPath);
	}

	virtual ~CPath()
	{
		ClearSubPath();
	}

public:
	bool m_bDir = false;

	wstring m_strName;

	ULONGLONG m_uFileSize = 0;

	FILETIME m_modifyTime = { 0,0 };

	CPath *m_pParentPath = NULL;

protected:
	TD_PathList *m_plstSubPath = NULL;

protected:
	virtual CPath *NewSubPath(CFileFind &FindFile, CPath *pParentPath) = 0; 
	//{
	//	return new CPath(FindFile, pParentPath);
	//}

public:
	UINT GetChildCount()
	{
		if (NULL == m_plstSubPath)
		{
			return 0;
		}

		return m_plstSubPath->size();
	}

	virtual wstring GetPath();

	void GetSubPath(TD_PathList& lstSubPath, BOOL bFindFile=TRUE, BOOL bSort=FALSE);
	
	void GetSubPath(TD_PathList *plstSubDir, TD_PathList *plstSubFile=NULL);
	
	CPath *GetSubPath(wstring strSubPath, bool bDir);
	
	void ClearSubPath();

	void RemoveSubPath(const TD_PathList& lstDeletePaths);

	BOOL FindFile();
};

//CCFolderDlg

class __CommonPrjExt CFolderDlg
{
public:
	CFolderDlg(LPCTSTR lpszInitDir=NULL, LPCTSTR lpszTitle=NULL, LPCTSTR lpszMessage=NULL
		, LPCTSTR lpszOKButton=NULL, LPCTSTR lpszCancelButton=NULL, int nWidth=0, int nHeight=0)
	{
		m_cstrInitDir = lpszInitDir;

		m_cstrTitle = lpszTitle;
		m_cstrMessage = lpszMessage;
		m_cstrOKButton = lpszOKButton;
		m_cstrCancelButton = lpszCancelButton;

		m_nWidth = nWidth;
		m_nHeight = nHeight;
	}

public:
	CString m_cstrInitDir;

	CString m_cstrTitle;
	CString m_cstrMessage;
	CString m_cstrOKButton;
	CString m_cstrCancelButton;

	int m_nWidth;
	int m_nHeight;

public:
	CString Show(CWnd& WndOwner, LPCTSTR lpszInitDir=NULL, LPCTSTR lpszTitle=NULL, LPCTSTR lpszMessage=NULL
		, LPCTSTR lpszOKButton=NULL, LPCTSTR lpszCancelButton=NULL, int nWidth=0, int nHeight=0);

	static int __stdcall BrowseFolderCallBack(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpParam);
};


class __CommonPrjExt CFileDlg
{
public:
	CFileDlg(const wstring& strTitle, const wstring& strFilter=L""
		, const wstring& strInitialDir=L"", const wstring& strFileName=L"", CWnd *pWndOwner=NULL);
	
private:
	OPENFILENAME m_ofn;

	TCHAR m_lpstrFilter[512];

	wstring m_strInitialDir;

	wstring m_strTitle;

	TCHAR m_lpstrFileName[4096];

public:
	wstring Show();
	void ShowMultiSel(list<wstring>& lstFiles);
};
