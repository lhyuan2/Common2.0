
#include "stdafx.h"

#include <App.h>

#include <fsutil.h>
#include <util.h>

#include "MainWnd.h"


// IModuleApp
IModuleApp::IModuleApp()
{
	CMainApp::AddModule(this);
}

IModuleApp::~IModuleApp()
{
}

void IModuleApp::ActivateResource()
{
	AfxSetResourceHandle(m_hInstance);
}

HANDLE IModuleApp::GetResource(ST_ResourceType nRCType, UINT nID)
{
	CResourceLock ResourceLock(this);

	switch (nRCType)
	{
	case RCT_Icon:
		return ::LoadIcon(m_hInstance, MAKEINTRESOURCE(nID));
	case RCT_Menu:
		return ::LoadMenu(m_hInstance, MAKEINTRESOURCE(nID));
	default:
		;
	}

	return NULL;
}

BOOL IModuleApp::OnReady(CMainWnd *pMainWnd)
{
	return TRUE;
}

BOOL IModuleApp::OnQuit()
{
	return TRUE;
}

BOOL IModuleApp::OnCommand(UINT nID)
{
	return FALSE;
}

LRESULT IModuleApp::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL IModuleApp::OnHotkey(const tagHotkeyInfo& HotkeyInfo)
{
	return FALSE;
}


// CMainApp

ModuleVector CMainApp::m_vctModules;

map<UINT, LPVOID> CMainApp::m_mapInterfaces;

std::map<char, LPVOID> CMainApp::m_mapHotkeyInfos;

std::vector<tagHotkeyInfo> CMainApp::m_vctHotkeyInfos;

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

wstring CMainApp::GetAppPath()
{
	static wstring strPath;
	if (!strPath.empty())
	{
		return strPath;
	}

	TCHAR pszPath[MAX_PATH];
	(void)::GetModuleFileName(0, pszPath, MAX_PATH);

	return fsutil::GetParentPath(pszPath);
}

BOOL CMainApp::InitInstance()
{
	ASSERT_RETURN_EX(__super::InitInstance(), FALSE);
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	(void)AfxOleInit();
	//(void)::OleInitialize(NULL);

	AfxEnableControlContainer();

	srand(GetTickCount());

	ASSERT_RETURN_EX(::SetCurrentDirectory(this->GetAppPath().c_str()), FALSE);

	return TRUE;
}

BOOL CMainApp::Run()
{
	CMainWnd *pMainWnd = NULL;
	
	MainWndInfo WndInfo;
	ZeroMemory(&WndInfo, sizeof(WndInfo));
	
	if (!OnInitMainWnd(pMainWnd, &WndInfo))
	{
		return FALSE;
	}
	
	if (NULL == pMainWnd)
	{
		 pMainWnd = new CMainWnd();
	}
	
	ENSURE_RETURN_EX(pMainWnd->Create(&WndInfo), FALSE);
	
	CMainApp::m_pMainWnd = pMainWnd;
	
	for (ModuleVector::iterator itModule=m_vctModules.begin(); itModule!=m_vctModules.end(); ++itModule)
	{
		if (!(*itModule)->OnReady(pMainWnd))
		{
			AfxPostQuitMessage(0);
			return FALSE;
		}
	}

	pMainWnd->Show();

	return __super::Run();
}

BOOL CMainApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == AfxGetMainWnd()->GetSafeHwnd())
	{
		switch (pMsg->message)
		{
		case WM_CLOSE:
			Quit();

			break;
		case WM_COMMAND:
			{				
				UINT nCode = HIWORD(pMsg->wParam);
				UINT nID = LOWORD(pMsg->wParam);

				HWND hWndCtrl = (HWND)pMsg->lParam;

				if (CN_COMMAND == nCode && !hWndCtrl)
				{
					if (HandleCommand(nID))
					{
						return TRUE;
					}
				}
			}

			break;
		case WM_HOTKEY:
			(void)HandleHotkey(pMsg->lParam);
			
			return TRUE;
		default:
			;
		}
	}
	
	if (WM_SYSKEYDOWN == pMsg->message)
	{
		for (std::vector<tagHotkeyInfo>::iterator itrHotkeyInfo = m_vctHotkeyInfos.begin()
			; itrHotkeyInfo != m_vctHotkeyInfos.end(); ++itrHotkeyInfo)
		{
			if (MOD_ALT == itrHotkeyInfo->nFlag && ::GetKeyState(itrHotkeyInfo->nKey)&0x8000)
			{
				if (HandleHotkey(itrHotkeyInfo->nID))
				{
					return TRUE;
				}
			}
		}
	}

	if (WM_KEYDOWN == pMsg->message)
	{
		UINT nKey = pMsg->wParam;
				
		if (VK_CONTROL != nKey && VK_SHIFT!= nKey)
		{
			UINT nFlag = 0;

			if(::GetKeyState(VK_CONTROL)&0x8000)
			{
				nFlag = MOD_CONTROL;
			}
			else if(::GetKeyState(VK_SHIFT)&0x8000)
			{
				nFlag = MOD_SHIFT;
			}

			if (HandleHotkey(MAKELPARAM(nFlag, nKey)))
			{
				return TRUE;
			}
		}		
	}

	return __super::PreTranslateMessage(pMsg);
}

BOOL CMainApp::HandleCommand(UINT nID)
{
	for (ModuleVector::iterator itModule=m_vctModules.begin(); itModule!=m_vctModules.end(); ++itModule)
	{
		if ((*itModule)->OnCommand(nID))
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CMainApp::HandleHotkey(LPARAM lParam)
{
	static DWORD dwLastTime = 0;
	if (500 > ::GetTickCount() - dwLastTime)
	{
		dwLastTime = ::GetTickCount();
		
		DoEvents();

		return FALSE;
	}
	dwLastTime = ::GetTickCount();

	std::vector<tagHotkeyInfo>::iterator itrHotkeyInfo = std::find(
		m_vctHotkeyInfos.begin(), m_vctHotkeyInfos.end(), (UINT)lParam);
	if (itrHotkeyInfo == m_vctHotkeyInfos.end())
	{
		return FALSE;
	}

	return HandleHotkey(*itrHotkeyInfo);
}

BOOL CMainApp::HandleHotkey(tagHotkeyInfo &HotkeyInfo)
{
	if (HotkeyInfo.bHandling)
	{
		return FALSE;
	}
	HotkeyInfo.bHandling = TRUE;

	BOOL bResult = FALSE;

	if (0 != HotkeyInfo.nIDMenuItem)
	{
		HandleCommand(HotkeyInfo.nIDMenuItem);
		bResult = TRUE;
	}
	else
	{
		for (ModuleVector::iterator itModule=m_vctModules.begin(); itModule!=m_vctModules.end(); ++itModule)
		{
			if ((*itModule)->OnHotkey(HotkeyInfo))
			{
				bResult = TRUE;
			}
		}
	}

	HotkeyInfo.bHandling = FALSE;

	return bResult;
}

BOOL CMainApp::Quit()
{
	for (ModuleVector::iterator itModule=m_vctModules.begin(); itModule!=m_vctModules.end(); ++itModule)
	{
		ENSURE_RETURN_EX((*itModule)->OnQuit(), FALSE);
	}

	for (std::vector<tagHotkeyInfo>::iterator itrHotkeyInfo = m_vctHotkeyInfos.begin()
		; itrHotkeyInfo != m_vctHotkeyInfos.end(); ++itrHotkeyInfo)
	{
		if (0 != itrHotkeyInfo->nID)
		{
			(void)::UnregisterHotKey(AfxGetMainWnd()->GetSafeHwnd(), itrHotkeyInfo->nID);
		}
	}

	AfxPostQuitMessage(0);

	return TRUE;
}

void CMainApp::DoEvents()
{
	::DoEvents();
}

BOOL CMainApp::AddModule(IModuleApp *pModule)
{
	ASSERT_RETURN_EX(pModule, FALSE);
	
	ASSERT_RETURN_EX(!util::ContainerFind(m_vctModules, pModule), FALSE);

	m_vctModules.push_back(pModule);

	return TRUE;
}

LRESULT CMainApp::SendMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWaitCursor WaitCursor;

	for (ModuleVector::iterator itModule=m_vctModules.begin(); itModule!=m_vctModules.end(); ++itModule)
	{
		LRESULT lResult = (*itModule)->OnMessage(nMsg, wParam, lParam);
		if (lResult)
		{
			return lResult;
		}
	}

	return 0;
}

void CMainApp::SendMessageEx(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWaitCursor WaitCursor;

	for (ModuleVector::iterator itModule=m_vctModules.begin(); itModule!=m_vctModules.end(); ++itModule)
	{
		(void)(*itModule)->OnMessage(nMsg, wParam, lParam);
	}
}

BOOL CMainApp::RegisterInterface(UINT nIndex, LPVOID lpInterface)
{
	if (m_mapInterfaces.find(nIndex) != m_mapInterfaces.end())
	{
		return FALSE;
	}

	m_mapInterfaces[nIndex] = lpInterface;

	return TRUE;
}

LPVOID CMainApp::GetInterface(UINT nIndex)
{
	map<UINT, LPVOID>::iterator itInterface = m_mapInterfaces.find(nIndex);

	if (itInterface == m_mapInterfaces.end())
	{
		return itInterface->second;
	}

	return NULL;
}

BOOL CMainApp::RegHotkey(const tagHotkeyInfo &HotkeyInfo)
{
	if (!util::ContainerFind(m_vctHotkeyInfos, HotkeyInfo))
	{
		if (HotkeyInfo.bGlobal)
		{
			if (!::RegisterHotKey(AfxGetMainWnd()->GetSafeHwnd(), HotkeyInfo.nID, HotkeyInfo.nFlag, HotkeyInfo.nKey))
			{
				return FALSE;
			}
		}

		m_vctHotkeyInfos.push_back(HotkeyInfo);
	}

	return TRUE;
}
