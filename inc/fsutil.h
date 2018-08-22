
#pragma once

#include "def.h"

#include "util.h"

//fsutil

class COMMON_EXT_CLASS fsutil
{
public:
	static void SplitPath(const wstring& strPath, wstring *pstrDir, wstring *pstrFile);

	static wstring GetFileName(const wstring& strPath);

	static void GetFileName(const wstring& strPath, wstring *pstrTitle, wstring *pstrExtName);

	static wstring GetFileTitle(const wstring& strPath);

	static wstring GetFileExtName(const wstring& strPath);

	static wstring GetParentPath(const wstring& strPath);

	static BOOL CheckSubPath(const wstring& strSubPath, const wstring& strPath);

	static wstring GetRelativePath(const wstring& strPath, const wstring strBasePath);

	static void GetSysDrivers(std::list<wstring>& lstDrivers);
	
	static BOOL DeletePath(const wstring& strPath, CWnd *pwndParent=NULL, const wstring& strTitle=L"");

	static BOOL CopyFile(const wstring& strSrcFile, const wstring& strSnkFile);

	static void ExplorePath(const list<wstring>& lstPath);

	static BOOL CreateDir(const wstring& strDir);

	static void FindFile(const wstring& strPath, std::list<wstring>& lstResult);

	static BOOL ExistsFile(const wstring& strFile);
	static BOOL CheckPath(const wstring& strDir);

	// 获取文件夹图标
	static HICON getFolderIcon();
	// 获取文件图标
	static HICON getFileIcon(const std::wstring& extention);
};


//CPath
class CPath;
typedef ptrlist<CPath*> TD_PathList;

class COMMON_EXT_CLASS CPath
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
	CPath();

	CPath(const wstring& strName, bool bDir, CPath *pParentPath);
	
	CPath(const wstring& strName, const TD_PathList& lstSubPath);
	
	virtual ~CPath();

public:
	bool m_bDir;

	wstring m_strName;

	CPath *m_pParentPath;

protected:
	TD_PathList *m_plstSubPath;

protected:
	virtual CPath *NewSubPath(CFileFind &FindFile, CPath *pParentPath) = 0; 
	//{
	//	return new CPath(FindFile, pParentPath);
	//}

public:
	virtual wstring GetPath();

	void GetSubPath(TD_PathList& lstSubPath, BOOL bFindFile=TRUE, BOOL bSort=FALSE);
	
	void GetSubPath(TD_PathList *plstSubDir, TD_PathList *plstSubFile=NULL);
	
	CPath *GetSubPath(wstring strSubPath, bool bDir);
	
	void ClearSubPath();

	void RemoveSubPath(const TD_PathList& lstDeletePaths);

	BOOL FindFile();

	BOOL ExistsFile();
};

//CCFolderDlg

class COMMON_EXT_CLASS CFolderDlg
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


class COMMON_EXT_CLASS CFileDlg
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
