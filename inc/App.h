
#pragma once

#include "def.h"


// Hotkey
enum class ST_HotkeyFlag
{
	HKF_Null = 0

	, HKF_Alt = MOD_ALT
	, HKF_Control = MOD_CONTROL
	, HKF_Shift = MOD_SHIFT

	, HKF_AltControl = MOD_ALT | MOD_CONTROL
	, HKF_AltShift = MOD_ALT | MOD_SHIFT
	, HKF_ControlShift = MOD_CONTROL | MOD_SHIFT
	, HKF_AltControlShift = MOD_ALT | MOD_CONTROL | MOD_SHIFT
};

struct tagHotkeyInfo
{
	tagHotkeyInfo(UINT t_nKey, ST_HotkeyFlag eHotkeyFlag = ST_HotkeyFlag::HKF_Null, bool t_bGlobal=FALSE, UINT t_nIDMenuItem = 0)
		: nKey(t_nKey)
		, nFlag((UINT)eHotkeyFlag)
		, lParam(MAKELPARAM(nFlag, nKey))
		, bGlobal(t_bGlobal)
		, nIDMenuItem(t_nIDMenuItem)
	{
	}

	UINT nKey = 0;
	UINT nFlag = 0;
	LPARAM lParam = 0;

	bool bGlobal = false;

	UINT nIDMenuItem = 0;

	bool bHandling = false;

	bool operator ==(const tagHotkeyInfo &HotkeyInfo)
	{
		return (lParam == HotkeyInfo.lParam && bGlobal == HotkeyInfo.bGlobal);
	}
};

enum ST_ResourceType
{
	RCT_Icon
	, RCT_Menu
};

class CMainWnd;

// IModuleApp
class __CommonPrjExt IModuleApp: public CWinApp
{
public:
	IModuleApp();

	virtual ~IModuleApp();

public:
	void ActivateResource();
	
	HANDLE GetResource(ST_ResourceType nRCType, UINT nID);
	
	virtual BOOL OnReady(CMainWnd& MainWnd) { return TRUE; }

	virtual BOOL OnQuit();

	virtual BOOL OnCommand(UINT nID);

	virtual LRESULT OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnHotkey(const tagHotkeyInfo& HotkeyInfo);
};

class CModuleApp : public IModuleApp {};

class CMainWnd;

class CResourceLock
{
public:
	CResourceLock(IModuleApp& Module)
	{
		m_hPreInstance = AfxGetResourceHandle();
		Module.ActivateResource();
	}

	~CResourceLock()
	{
		if (NULL != m_hPreInstance)
		{
			AfxSetResourceHandle(m_hPreInstance);
		}
	}

private:
	HINSTANCE m_hPreInstance = NULL;
};

struct tagMainWndInfo;

//CMainApp
typedef vector<IModuleApp*> ModuleVector;

class __CommonPrjExt CMainApp: public IModuleApp
{
public:
	CMainApp();

	virtual ~CMainApp();

	static CMainWnd* GetMainWnd()
	{
		return (CMainWnd*)AfxGetMainWnd();
	}

private:
	static ModuleVector m_vctModules;

	static map<UINT, LPVOID> m_mapInterfaces;

	static std::map<char, LPVOID> m_mapHotkeyInfos;

	static std::vector<tagHotkeyInfo> m_vctHotkeyInfos;

protected:
	virtual BOOL InitInstance();
	
	virtual CMainWnd* OnInitMainWnd(tagMainWndInfo& MainWndInfo)=0;
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	static bool HandleHotkey(LPARAM lParam, bool bGlobal);
	static bool HandleHotkey(tagHotkeyInfo &HotkeyInfo);

	static BOOL HandleCommand(UINT nID);

public:
	static wstring GetAppPath();

	static BOOL Quit();

	static void DoEvents();

	static BOOL AddModule(IModuleApp& Module);

	static LRESULT SendMessage(UINT nMsg, WPARAM wParam=NULL, LPARAM lParam=NULL);
	static void SendMessageEx(UINT nMsg, WPARAM wParam=NULL, LPARAM lParam=NULL);
	
	static BOOL RegisterInterface(UINT nIndex, LPVOID lpInterface);
	static LPVOID GetInterface(UINT nIndex);

	static BOOL RegHotkey(const tagHotkeyInfo &HotkeyInfo);
};
