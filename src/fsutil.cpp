﻿
#include "stdafx.h"

#include <fsutil.h>

#include <util.h>

// fsutil

void fsutil::SplitPath(const wstring& strPath, wstring *pstrDir, wstring *pstrFile)
{
	int iPos = -1;
	auto pos = strPath.find_last_of('/');
	if (wstring::npos != pos)
	{
		iPos = pos;
	}
	pos = strPath.find_last_of('\\');
	if (wstring::npos != pos && (int)pos > iPos)
	{
		iPos = pos;
	}

	if (iPos >= 0)
	{
		if (NULL != pstrDir)
		{
			*pstrDir = strPath.substr(0, iPos);
		}

		if (NULL != pstrFile)
		{
			*pstrFile = strPath.substr(iPos + 1);
		}
	}
	else
	{
		if (NULL != pstrFile)
		{
			*pstrFile = strPath;
		}
	}
}

wstring fsutil::GetFileName(const wstring& strPath)
{
	wstring strFileName;
	SplitPath(strPath, NULL, &strFileName);

	return strFileName;
}

void fsutil::GetFileName(const wstring& strPath, wstring *pstrTitle, wstring *pstrExtName)
{
	wstring strName = GetFileName(strPath);

	auto pos = strName.find_last_of(L'.');
	if (wstring::npos != pos)
	{
		if (NULL != pstrExtName)
		{
			*pstrExtName = strName.substr(pos);
		}

		strName = strName.substr(0, pos);
	}

	if (NULL != pstrTitle)
	{
		*pstrTitle = strName;
	}
}

wstring fsutil::GetFileTitle(const wstring& strPath)
{
	wstring strTitle;
	GetFileName(strPath, &strTitle, NULL);
	return strTitle;
}

wstring fsutil::GetFileExtName(const wstring& strPath)
{
	wstring strExtName;
	GetFileName(strPath, NULL, &strExtName);
	return strExtName;
}

wstring fsutil::GetParentPath(const wstring& strPath)
{
	__AssertReturn(!strPath.empty(), L"");

	wstring strNewPath = strPath;
	if ('\\' == strNewPath.back() || '/' == strNewPath.back())
	{
		strNewPath.pop_back();
	}

	int nPos = (int)strNewPath.rfind('\\');
	__EnsureReturn(0 <= nPos, L"");

	return strNewPath.substr(0, nPos);
}

bool fsutil::CheckSubPath(const wstring& strPath, const wstring& strSubPath)
{
	auto size = strPath.size();
	__EnsureReturn(size > 0, FALSE);
	__EnsureReturn(size < strSubPath.size(), FALSE);

	__EnsureReturn(0 == wcsncmp(strPath.c_str(), strSubPath.c_str(), size), FALSE);

	__EnsureReturn(__BackSlant == *strPath.rbegin() || __BackSlant == strSubPath[size], FALSE);

	return TRUE;
}

wstring fsutil::GetOppPath(const wstring& strPath, const wstring strBasePath)
{
	if (strBasePath.empty())
	{
		return strPath;
	}
	
	if (!CheckSubPath(strBasePath, strPath))
	{
		return L"";
	}

	return strPath.substr(strBasePath.size());
}

void fsutil::GetSysDrivers(list<wstring>& lstDrivers)
{
	#define MAX_DRIVE (_MAX_DRIVE + 1)

	TCHAR pszBuffer[256] = {0};

	UINT nCount = ::GetLogicalDriveStrings(0, NULL);

	(void)GetLogicalDriveStrings(nCount, pszBuffer);

	wstring strDriver;
	int nDriveType = 0;

	for(UINT i=0; i<nCount/MAX_DRIVE; ++i)
	{
		strDriver = pszBuffer + i*MAX_DRIVE;

		nDriveType = ::GetDriveType(strDriver.c_str());
		if (DRIVE_FIXED == nDriveType || DRIVE_REMOVABLE == nDriveType)
		{
			lstDrivers.push_back(strDriver.substr(0,2));
		}
	}
}

bool fsutil::DeletePath(const wstring& strPath, CWnd *pwndParent, const wstring& strTitle)
{
	SHFILEOPSTRUCT FileOp;
	ZeroMemory(&FileOp, sizeof(FileOp));
		
	FileOp.fFlags = FOF_NOCONFIRMATION;

	FileOp.hwnd = pwndParent->GetSafeHwnd();

	if (!strTitle.empty())
	{
		FileOp.lpszProgressTitle = strTitle.c_str();
	}

	FileOp.pFrom = strPath.c_str();

	FileOp.wFunc = FO_DELETE;

	int nResult = SHFileOperation(&FileOp);
	if (ERROR_SUCCESS == nResult)
	{
		return TRUE;
	}

	if (ERROR_FILE_NOT_FOUND == nResult || ERROR_FILE_NOT_FOUND == ::GetLastError())
	{
		return TRUE;
	}

	return FALSE;
}
#include <fstream>
bool fsutil::CopyFile(const wstring& strSrcFile, const wstring& strSnkFile)
{
#define MAX_BUFFER 1024
	char lpBuffer[MAX_BUFFER];

	size_t nReadedSize = 0;

	ifstream srcStream;
	ofstream snkStream;

	try
	{
		srcStream.open(strSrcFile.c_str(), ios::in | ios::binary);

		//pfSrc = fopen(strSrcFile.c_str(), "r");
		//bResult = srcfile.Open(util::StrToWStr(strSrcFile), CFile::modeRead| CFile::shareDenyNone);
	}
	catch (...)
	{
	}
	
	__EnsureReturn(srcStream, FALSE);

	try
	{
		snkStream.open(strSnkFile.c_str(), ios::out | ios::binary | ios::trunc);
		//bResult = snkfile.Open(util::StrToWStr(strSnkFile), CFile::modeCreate| CFile::modeWrite);
	}
	catch (...)
	{
	}

	if (!snkStream)
	{
		srcStream.close();
		//(void)fclose(pfSrc);

		//srcfile.Close();
		return FALSE;
	}

	bool bResult = TRUE;
	try
	{
		while (true)
		{
			srcStream.read(lpBuffer, MAX_BUFFER);
			nReadedSize = (size_t)srcStream.gcount();
				
			if (nReadedSize)
			{
				snkStream.write(lpBuffer, nReadedSize);
			}

			//nReadedSize = fread(lpBuffer, 1, MAX_BUFFER, pfSrc)
			//fwrite(lpBuffer, nReadedSize, 1, pfSnk);

			//snkfile.Write(lpBuffer, nReadedSize);

			if (srcStream.gcount() < MAX_BUFFER)
			{
				break;
			}
		}
	}
	catch (...)
	{
		bResult = FALSE;
	}

	srcStream.close();
	snkStream.close();
	//fclose(pfSrc);
	//fclose(pfSnk);

	//srcfile.Close();
	//snkfile.Close();
	
	return bResult;
}

void fsutil::ExplorePath(const list<wstring>& lstPath)
{
	wstring strExplore;
	for (auto& strPath : lstPath)
	{
		if (!ExistsPath(strPath))
		{
			continue;
		}

		if (!strExplore.empty())
		{
			strExplore.append(L",");
		}

		strExplore.append(L'\"' + strPath + L'\"');
	}
	if (strExplore.empty())
	{
		return;
	}

	(void)::ShellExecute(NULL, L"open", L"explorer.exe", (L"/select," + strExplore).c_str(), NULL, SW_MAXIMIZE);
}

bool fsutil::CreateDir(const wstring& strDir)
{
	if (::CreateDirectory(strDir.c_str(), NULL) || ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		return TRUE;
	}
	
	if (!CreateDir(GetParentPath(strDir)))
	{
		return FALSE;
	}

	return CreateDir(strDir);
}

void fsutil::FindFile(const wstring& strPath, list<wstring>& lstResult)
{
	CFileFind fileFind;
	bool bExists = fileFind.FindFile(strPath.c_str());
	while (bExists)
	{
		bExists = fileFind.FindNextFile();

		if (fileFind.IsDots() || fileFind.IsSystem())
		{
			continue;
		}
		
		lstResult.push_back((LPCTSTR)fileFind.GetFilePath());
	}
}

bool fsutil::ExistsFile(const wstring& strFile)
{
	WIN32_FIND_DATA ffd;
	return (INVALID_HANDLE_VALUE != FindFirstFile(strFile.c_str(), &ffd));
}

bool fsutil::ExistsPath(const wstring& strDir)
{
	return (-1 != ::GetFileAttributes(strDir.c_str()));
}

// 获取文件夹类型
static wstring getFolderType()
{
	SHFILEINFO info;
	if (SHGetFileInfo(L"folder",
		FILE_ATTRIBUTE_DIRECTORY,
		&info,
		sizeof(info),
		SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
	{
		return info.szTypeName;
	}

	return L"";
}

// 获取文件类型
static wstring getFileType(const wstring& extention)
{
	if (!extention.empty())
	{
		SHFILEINFO info;
		if (SHGetFileInfo(extention.c_str(),
			FILE_ATTRIBUTE_NORMAL,
			&info,
			sizeof(info),
			SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
		{
			return info.szTypeName;
		}
	}

	return L"";
}

// 获取文件夹图标
HICON fsutil::getFolderIcon()
{
	SHFILEINFO info;
	if (SHGetFileInfo(L"folder",
		FILE_ATTRIBUTE_DIRECTORY,
		&info,
		sizeof(info),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
	{
		return info.hIcon;
	}

	return NULL;
}

// 获取文件图标
HICON fsutil::getFileIcon(const wstring& extention)
{
	if (!extention.empty())
	{
		SHFILEINFO info;
		if (SHGetFileInfo(extention.c_str(),
			FILE_ATTRIBUTE_NORMAL,
			&info,
			sizeof(info),
			SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
		{
			return info.hIcon;
		}
	}

	return NULL;
}

//CFolderDlg

CString CFolderDlg::Show(CWnd& WndOwner, LPCTSTR lpszInitDir, LPCTSTR lpszTitle, LPCTSTR lpszMessage
							, LPCTSTR lpszOKButton, LPCTSTR lpszCancelButton, int nWidth, int nHeight)
{
	if (lpszInitDir)
	{
		m_cstrInitDir = lpszInitDir;
	}

	if (lpszTitle)
	{
		m_cstrTitle = lpszTitle;
	}

	if (lpszMessage)
	{
		m_cstrMessage = lpszMessage;
	}

	if (lpszOKButton)
	{
		m_cstrOKButton = lpszOKButton;
	}

	if (lpszCancelButton)
	{
		m_cstrCancelButton = lpszCancelButton;
	}

	if (nWidth)
	{
		m_nWidth = nWidth;
	}

	if (nHeight)
	{
		m_nHeight = nHeight;
	}

	//(void)::EnableWindow(WndOwner.m_hWnd, FALSE);

	BROWSEINFO browseInfo;
	::ZeroMemory(&browseInfo, sizeof browseInfo);
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_STATUSTEXT | BIF_BROWSEFORCOMPUTER;

	browseInfo.hwndOwner = WndOwner.m_hWnd;
	browseInfo.lpfn = BrowseFolderCallBack;
	browseInfo.lParam = (LPARAM)this;

	LPITEMIDLIST lpItemIDList = SHBrowseForFolder(&browseInfo);

	(void)::EnableWindow(WndOwner.m_hWnd, TRUE);

	if(lpItemIDList)
	{
		TCHAR pszPath[512];
		if (SHGetPathFromIDList(lpItemIDList, pszPath))
		{
			m_cstrInitDir = pszPath;
			m_cstrInitDir.Trim('\\');

			return m_cstrInitDir;
		}
	}

	::DoEvents();

	return (LPCTSTR)NULL;
}

int CFolderDlg::BrowseFolderCallBack(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpParam)
{
	CWnd *pWnd = CWnd::FromHandle(hWnd);
	CWnd *pWndOkButton = pWnd->GetDlgItem(IDOK);
	CWnd *pWndCancelButton = pWnd->GetDlgItem(IDCANCEL);

	CFolderDlg* pInstance = (CFolderDlg*)lpParam;

	switch (uMsg)
	{
		case BFFM_INITIALIZED:
			{
				if (!pInstance->m_cstrInitDir.IsEmpty())
				{
					(void)::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)pInstance->m_cstrInitDir);
				}

				if (!pInstance->m_cstrTitle.IsEmpty())
				{
					pWnd->SetWindowText(pInstance->m_cstrTitle);
				}

				CString cstrMessage = pInstance->m_cstrMessage;
				cstrMessage += _T("\n\n");
				cstrMessage += pInstance->m_cstrInitDir;
				(void)pWnd->SendMessage(BFFM_SETSTATUSTEXT, 1, (LPARAM)(LPCTSTR)cstrMessage);

				if (!pInstance->m_cstrOKButton.IsEmpty())
				{
					pWndOkButton->SetWindowText(pInstance->m_cstrOKButton);
				}
				if (!pInstance->m_cstrCancelButton.IsEmpty())
				{
					pWndCancelButton->SetWindowText(pInstance->m_cstrCancelButton);
				}


				if (0 == pInstance->m_nWidth || 0 == pInstance->m_nHeight)
				{
					break;
				}

				CRect rcPreClient(0,0,0,0);
				pWnd->GetClientRect(&rcPreClient);

				pWnd->MoveWindow(0, 0, pInstance->m_nWidth, pInstance->m_nHeight, FALSE);
				pWnd->CenterWindow();

				CRect rcClient(0,0,0,0);
				pWnd->GetClientRect(&rcClient);
				
				int nWidthOff = rcClient.Width() - rcPreClient.Width();
				int nHeightOff = rcClient.Height() - rcPreClient.Height();


				HWND hWndStatic = NULL;
				CRect rcStatic(0,0,0,0);
				while (TRUE)
				{
					hWndStatic = ::FindWindowEx(hWnd, hWndStatic, L"Static", NULL);
					if (!hWndStatic)
					{
						break;
					}


					(void)::GetWindowRect(hWndStatic, &rcStatic);
					pWnd->ScreenToClient(&rcStatic);
					(void)::MoveWindow(hWndStatic, rcStatic.left, rcStatic.left
						, rcStatic.Width() + nWidthOff, rcStatic.bottom - rcStatic.left, FALSE);
				}


				HWND hWndTreeCtrl = ::FindWindowEx(hWnd, NULL, L"SysTreeView32", NULL);
				CWnd *pWndTreeCtrl = CWnd::FromHandle(hWndTreeCtrl);
				if (!pWndTreeCtrl)
				{
					ASSERT(FALSE);
					break;
				}

				CRect rcTreeCtrl(0,0,0,0);
				pWndTreeCtrl->GetWindowRect(&rcTreeCtrl);
				pWnd->ScreenToClient(&rcTreeCtrl);
				pWndTreeCtrl->MoveWindow(rcTreeCtrl.left, rcTreeCtrl.top
					, rcTreeCtrl.Width() + nWidthOff, rcTreeCtrl.Height() + nHeightOff, FALSE);


				CRect rcOkButton(0,0,0,0);
				pWndOkButton->GetWindowRect(&rcOkButton);
				pWnd->ScreenToClient(&rcOkButton);
				pWndOkButton->MoveWindow(rcOkButton.left + nWidthOff, rcOkButton.top + nHeightOff
					, rcOkButton.Width() , rcOkButton.Height(), FALSE);

				CRect rcCancelButton(0,0,0,0);
				pWndCancelButton->GetWindowRect(&rcCancelButton);
				pWnd->ScreenToClient(&rcCancelButton);
				pWndCancelButton->MoveWindow(rcCancelButton.left + nWidthOff, rcCancelButton.top + nHeightOff
					, rcCancelButton.Width(), rcCancelButton.Height(), FALSE);

				break;
			}
		case BFFM_SELCHANGED:
			{
				CString cstrMessage = pInstance->m_cstrMessage;

				TCHAR pszPath[512];
				if (SHGetPathFromIDList((LPITEMIDLIST)lParam, pszPath))
				{
					if (!fsutil::ExistsPath(pszPath))
					{
						pWndOkButton->EnableWindow(FALSE);
					}
					else
					{
						pWndOkButton->EnableWindow();
						cstrMessage = cstrMessage + _T("\n\n") + pszPath;
					}
				}

				(void)pWnd->SendMessage(BFFM_SETSTATUSTEXT, 0, (LPARAM)(LPCTSTR)cstrMessage);

				break;
			}
	}

	return 0;
}


CFileDlg::CFileDlg(const wstring& strTitle, const wstring& strFilter
	, const wstring& strInitialDir, const wstring& strFileName, CWnd *pWndOwner)
	: m_strTitle(strTitle)
	, m_strInitialDir(strInitialDir)
{
	::ZeroMemory(&m_ofn, sizeof(m_ofn));
	m_ofn.lStructSize = sizeof(m_ofn);

	m_ofn.hInstance = AfxGetInstanceHandle();
	
	m_ofn.lpstrTitle = m_strTitle.c_str();

	if (NULL == pWndOwner)
	{
		pWndOwner = AfxGetMainWnd();
	}
	m_ofn.hwndOwner = pWndOwner->GetSafeHwnd();
	
	::ZeroMemory(m_lpstrFilter, sizeof m_lpstrFilter);
	m_ofn.lpstrFilter = m_lpstrFilter;
	if (!strFilter.empty())
	{
		::lstrcat(m_lpstrFilter, strFilter.c_str());

		LPTSTR pch = m_lpstrFilter;

		while (TRUE)
		{
			pch = wcschr(pch, '|');
			if (!pch)
			{
				break;
			}

			*pch = '\0';

			pch++;
		}
	}

	::ZeroMemory(m_lpstrFileName, sizeof(m_lpstrFileName));
	m_ofn.lpstrFile = m_lpstrFileName;
	m_ofn.nMaxFile = sizeof(m_lpstrFileName)/sizeof(m_lpstrFileName[0]);
	if (!strFileName.empty())
	{
		::wcscpy_s(m_lpstrFileName, m_ofn.nMaxFile, strFileName.c_str());
	}

	m_ofn.lpstrInitialDir = m_strInitialDir.c_str();
	
	m_ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志如果是多选要加上OFN_ALLOWMULTISELECT
}

wstring CFileDlg::Show()
{
	bool bResult = ::GetOpenFileName(&m_ofn);

	(void)::EnableWindow(m_ofn.hwndOwner, TRUE);

	(void)::SetFocus(m_ofn.hwndOwner);

	__EnsureReturn(bResult, L"");
	
	return m_lpstrFileName;
}

void CFileDlg::ShowMultiSel(list<wstring>& lstFiles)
{
	m_ofn.Flags |= OFN_ALLOWMULTISELECT;

	bool bResult = ::GetOpenFileName(&m_ofn);

	(void)::EnableWindow(m_ofn.hwndOwner, TRUE);

	(void)::SetFocus(m_ofn.hwndOwner);

	__Ensure(bResult);

	for (TCHAR *p = m_lpstrFileName; ; p++)
	{
		if ('\0' == *p)
		{
			if ('\0' == *(p + 1))
			{
				break;
			}

			lstFiles.push_back(p+1);
		}
	}
	if (lstFiles.empty())
	{
		lstFiles.push_back(m_lpstrFileName);
	}
	else
	{
		wstring strDir = m_lpstrFileName;
		for (list<wstring>::iterator itrFile = lstFiles.begin()++; itrFile!=lstFiles.end(); itrFile++)
		{
			*itrFile = strDir + __BackSlant + *itrFile;
		}
	}
}

wstring CPath::GetPath()
{
	if (m_pParentPath)
	{
		return m_pParentPath->GetPath() + __BackSlant + m_strName;
	}

	return m_strName;
}

void CPath::GetSubPath(TD_PathList& lstSubPath, bool bFindFile, bool bSort)
{
	__Ensure(m_bDir);

	if (bFindFile)
	{
		__Ensure(FindFile() && m_plstSubPath);
	}

	if (NULL != m_plstSubPath)
	{
		if (bSort)
		{
			m_plstSubPath->sort(tagPathSortor());
		}

		lstSubPath.Insert(*m_plstSubPath);
	}
}

void CPath::GetSubPath(TD_PathList *plstSubDir, TD_PathList *plstSubFile)
{
	if (!plstSubDir && !plstSubFile)
	{
		return;
	}

	TD_PathList lstSubPath;
	this->GetSubPath(lstSubPath);

	for (TD_PathList::iterator itSubPath = lstSubPath.begin()
		; itSubPath != lstSubPath.end(); ++itSubPath)
	{
		if ((*itSubPath)->m_bDir)
		{
			if (plstSubDir)
			{
				plstSubDir->push_back(*itSubPath);
			}
		}
		else
		{
			if (plstSubFile)
			{
				plstSubFile->push_back(*itSubPath);
			}
		}
	}
}
	
CPath *CPath::GetSubPath(wstring strSubPath, bool bDir)
{
	list<wstring> lstSubDirs;
	while (!strSubPath.empty())
	{
		wstring strName;
		wstring strExtName;
		fsutil::GetFileName(strSubPath, &strName, &strExtName);

		strName += strExtName;
		if (strName.empty())
		{
			break;
		}
		
		lstSubDirs.push_back(strName);

		strSubPath = fsutil::GetParentPath(strSubPath);
	}
	
	CPath *pPath = this;

	while (!lstSubDirs.empty() && NULL != pPath)
	{
		wstring strName = lstSubDirs.back();
		lstSubDirs.pop_back();

		TD_PathList lstSubPath;
		pPath->GetSubPath(lstSubPath, TRUE);
		
		pPath = NULL;

		for (TD_PathList::iterator itSubPath = lstSubPath.begin()
			; itSubPath != lstSubPath.end(); ++itSubPath)
		{
			if (lstSubDirs.empty())
			{
				if ((*itSubPath)->m_bDir != bDir)
				{
					continue;
				}
			}
			else
			{
				if (!(*itSubPath)->m_bDir)
				{
					continue;
				}
			}

			if (util::StrCompareIgnoreCase((*itSubPath)->m_strName, strName))
			{
				pPath = *itSubPath;
				break;
			}
		}
	}

	return pPath;
}

void CPath::ClearSubPath()
{
	if (NULL != m_plstSubPath)
	{
		for (TD_PathList::iterator itSubPath = m_plstSubPath->begin()
			; itSubPath != m_plstSubPath->end(); ++itSubPath)
		{
			delete *itSubPath;
		}

		m_plstSubPath->clear();
		delete m_plstSubPath;
		m_plstSubPath = NULL;
	}
}

void CPath::RemoveSubPath(const TD_PathList& lstDeletePaths)
{
	__Ensure(m_plstSubPath);

	for (TD_PathList::iterator itSubPath = m_plstSubPath->begin()
		; itSubPath != m_plstSubPath->end(); )
	{
		if (util::ContainerFind(lstDeletePaths, *itSubPath))
		{
			delete *itSubPath;
			itSubPath = m_plstSubPath->erase(itSubPath);
			continue;
		}			

		itSubPath++;
	}
}

bool CPath::FindFile()
{
	if (NULL != m_plstSubPath)
	{
		return true;
	}

	CString cstrFind = this->GetPath().c_str();
	cstrFind.Append(L"\\*");
	CFileFind fileFind;
	bool bExists = fileFind.FindFile(cstrFind);
	if (!bExists)
	{
		return false;
	}
	
	m_plstSubPath = new TD_PathList();
	
	CPath *pSubPath = NULL;

	while (bExists)
	{
		bExists = fileFind.FindNextFile();

		if (fileFind.IsDots() || fileFind.IsSystem())
		{
			continue;
		}

		pSubPath = NewSubPath(fileFind, this);
		if (pSubPath)
		{
			m_plstSubPath->push_back(pSubPath);
		}
	}

	return true;
}
