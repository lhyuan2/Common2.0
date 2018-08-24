
#pragma once

#include <def.h>


// CWorkThread

class CWorkThread;

struct tagWorkThreadInfo
{
	UINT nIndex;
	HANDLE hHandle;
	CWorkThread *pThread;
};

class __CommonPrjExt CWorkThread
{
public:
	CWorkThread();
	virtual ~CWorkThread();

public:
	std::list<tagWorkThreadInfo> m_lstThreadInfos;

private:
	HANDLE m_hExitEvent;

public:
	BOOL RunWorkThread(UINT nThreadCount=1);

protected:
	void Pause(BOOL bPause=TRUE);

	void SetExitSignal();
	BOOL GetExitSignal();

	void WaitForExit();

	int GetWorkThreadCount();

private:
	virtual void WorkThreadProc(tagWorkThreadInfo& ThreadInfo) = 0;

	static DWORD WINAPI ThreadProc(LPVOID lpThreadParam);
};
