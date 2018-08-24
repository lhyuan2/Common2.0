
#pragma once

#include <def.h>


// CLogList

class __CommonPrjExt CLogList : public CListCtrl
{
	DECLARE_DYNAMIC(CLogList)

public:
	CLogList();
	virtual ~CLogList();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void InitCtrl(const std::vector<std::pair<CString, LONG>>& vctColumns);

	void AddLog(const std::vector<CString>& vctLogTexts);

	void ClearLogs();
};
