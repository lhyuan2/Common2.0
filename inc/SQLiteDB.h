#pragma once

#include "IDB.h"

#include <def.h>

class CSQLiteDB;
class COMMON_EXT_CLASS CSQLiteDBResult : public IDBResult
{
friend CSQLiteDB;
public:
	CSQLiteDBResult();

	virtual ~CSQLiteDBResult();

private:
	UINT m_nColumnCount;
	UINT m_nRowCount;

	char ** m_pData;

public:
	UINT GetColumnCount();

	UINT GetRowCount();

	BOOL GetData(UINT nRow, UINT nColumn, int& nValue);

	BOOL GetData(UINT nRow, UINT nColumn, double& dbValue);

	BOOL _GetData(UINT nRow, UINT nColumn, string& strValue);
	BOOL GetData(UINT nRow, UINT nColumn, std::wstring& strValue);
};


class COMMON_EXT_CLASS CSQLiteDB : public IDB
{
public:
	CSQLiteDB(const string& strDBPath);

	virtual ~CSQLiteDB();

private:
	string m_strDBPath;
	
	HANDLE m_hDB;

public:
	int GetStatus();

	BOOL Connect();

	BOOL Disconnect();

	BOOL Execute(const std::wstring& strSql, string& strError);

	IDBResult* Query(const std::wstring& strSql, string& strError);
};
