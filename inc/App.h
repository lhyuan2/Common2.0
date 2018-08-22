
#pragma once

#include "def.h"


// Hotkey
enum ST_HotkeyFlag
{
	HKF_Null
	, HKF_Control
	, HKF_Alt
	, HKF_Shift
};

struct tagHotkeyInfo
{
	tagHotkeyInfo(UINT nKey, ST_HotkeyFlag nHotkeyFlag = HKF_Null, BOOL bGlobal = TRUE, UINT nIDMenuItem = 0)
		: bHandling(FALSE)
	{
		this->nKey = nKey;

		switch (nHotkeyFlag)
		{
		case HKF_Control:
			this->nFlag = MOD_CONTROL;
			break;
		case HKF_Alt:
			this->nFlag = MOD_ALT;
			break;
		case HKF_Shift:
			this->nFlag = MOD_SHIFT;
			break;
		default:
			this->nFlag = 0;
		}

		this->bGlobal = bGlobal;
		//if (bGlobal)
		{
			nID = (UINT)MAKELPARAM(nFlag, nKey);
		}

		this->nIDMenuItem = nIDMenuItem;
	}

	UINT nKey;
	UINT nFlag;
	BOOL bGlobal;
	UINT nIDMenuItem;

	UINT nID;

	BOOL bHandling;

	bool operator ==(const tagHotkeyInfo &HotkeyInfo)
	{
		if (nKey == HotkeyInfo.nKey && nFlag == HotkeyInfo.nFlag && bGlobal == HotkeyInfo.bGlobal)
		{
			return true;
		}

		return false;
	}

	bool operator ==(UINT nID)
	{
		return nID == this->nID;
	}
};

enum ST_ResourceType
{
	RCT_Icon
	, RCT_Menu
};

class CMainWnd;

// IModuleApp
class COMMON_EXT_CLASS IModuleApp: public CWinApp
{
public:
	IModuleApp();

	virtual ~IModuleApp();

public:
	void ActivateResource();
	
	HANDLE GetResource(ST_ResourceType nRCType, UINT nID);
	
	virtual BOOL OnReady(CMainWnd *pMainWnd);

	virtual BOOL OnQuit();

	virtual BOOL OnCommand(UINT nID);

	virtual LRESULT OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnHotkey(const tagHotkeyInfo& HotkeyInfo);
};

class CMainWnd;

class CResourceLock
{
public:
	CResourceLock(IModuleApp *pModule)
		: m_hPreInstance(NULL)
	{
		if (NULL != pModule)
		{
			m_hPreInstance = AfxGetResourceHandle();
			pModule->ActivateResource();
		}
	}

	~CResourceLock()
	{
		if (NULL != m_hPreInstance)
		{
			AfxSetResourceHandle(m_hPreInstance);
		}
	}

private:
	HINSTANCE m_hPreInstance;
};

struct MainWndInfo;

//CMainApp
typedef vector<IModuleApp*> ModuleVector;

class COMMON_EXT_CLASS CMainApp: public IModuleApp
{
public:
	CMainApp();

	virtual ~CMainApp();

private:
	static ModuleVector m_vctModules;

	static map<UINT, LPVOID> m_mapInterfaces;

	static std::map<char, LPVOID> m_mapHotkeyInfos;

	static std::vector<tagHotkeyInfo> m_vctHotkeyInfos;

protected:
	virtual BOOL InitInstance();
	
	virtual BOOL Run();
	
	virtual BOOL OnInitMainWnd(CMainWnd *&pMainWnd, MainWndInfo *pWndInfo)=0;
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	static BOOL HandleHotkey(LPARAM lParam);
	static BOOL HandleHotkey(tagHotkeyInfo &HotkeyInfo);

	static BOOL HandleCommand(UINT nID);

public:
	static wstring GetAppPath();

	static BOOL Quit();

	static void DoEvents();

	static BOOL AddModule(IModuleApp *pModule);

	static LRESULT SendMessage(UINT nMsg, WPARAM wParam=NULL, LPARAM lParam=NULL);
	static void SendMessageEx(UINT nMsg, WPARAM wParam=NULL, LPARAM lParam=NULL);
	
	static BOOL RegisterInterface(UINT nIndex, LPVOID lpInterface);
	static LPVOID GetInterface(UINT nIndex);

	static BOOL RegHotkey(const tagHotkeyInfo &HotkeyInfo);
};
