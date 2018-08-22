#pragma once

#include <string>
using namespace std;

interface IDBResult
{
	virtual UINT GetColumnCount() = 0;
	virtual UINT GetRowCount() = 0;

	virtual BOOL GetData(UINT nRow, UINT nColumn, int& nValue) = 0;
	virtual BOOL GetData(UINT nRow, UINT nColumn, double& dbValue) = 0;
	virtual BOOL GetData(UINT nRow, UINT nColumn, std::wstring& strValue) = 0;
};

interface IDB
{
	virtual int GetStatus() = 0;

	virtual BOOL Connect() = 0;
	virtual BOOL Disconnect() = 0;

	virtual BOOL Execute(const std::wstring& strSql, string& strError) = 0;
	virtual IDBResult* Query(const std::wstring& strSql, string& strError) = 0;
};
