
#include "stdafx.h"

#include <SQLiteDB.h>

#include <util.h>

#include "sqlite/sqlite3.h"


//CSQLiteDBResult

CSQLiteDBResult::CSQLiteDBResult()
{
	m_nColumnCount = 0;
	m_nRowCount = 0;

	m_pData = NULL;
}

CSQLiteDBResult::~CSQLiteDBResult()
{
	ENSURE_RETURN(this);

	if (m_pData)
	{
		sqlite3_free_table(m_pData);
	}

	delete this;
}

UINT CSQLiteDBResult::GetColumnCount()
{
	ASSERT_RETURN_EX(this, 0);

	return m_nColumnCount;
}

UINT CSQLiteDBResult::GetRowCount()
{
	ASSERT_RETURN_EX(this, 0);

	return m_nRowCount;
}

BOOL CSQLiteDBResult::GetData(UINT nRow, UINT nColumn, int& nValue)
{
	string strValue;
	ENSURE_RETURN_EX(_GetData(nRow, nColumn, strValue), FALSE);

	nValue = util::StrToInt(strValue.c_str());

	return TRUE;
}

BOOL CSQLiteDBResult::GetData(UINT nRow, UINT nColumn, double& dbValue)
{
	string strValue;
	ENSURE_RETURN_EX(_GetData(nRow, nColumn, strValue), FALSE);

	dbValue = util::StrToDbl(strValue.c_str());

	return TRUE;
}

BOOL CSQLiteDBResult::_GetData(UINT nRow, UINT nColumn, string& strValue)
{
	ASSERT_RETURN_EX(this, NULL);
	ASSERT_RETURN_EX(m_nRowCount && m_nColumnCount, NULL);
	ASSERT_RETURN_EX(nRow < m_nRowCount && nColumn < m_nColumnCount, NULL);
	ASSERT_RETURN_EX(m_pData, FALSE);

	char* pszValue = m_pData[(nRow + 1) * m_nColumnCount + nColumn];
	if (NULL != pszValue)
	{
		strValue = pszValue;
	}

	return TRUE;
}

BOOL CSQLiteDBResult::GetData(UINT nRow, UINT nColumn, std::wstring& strValue)
{
	string strData;
	ENSURE_RETURN_EX(_GetData(nRow, nColumn, strData), FALSE);
	strValue = util::StrToWStr(strData, CP_UTF8);
	return TRUE;
}


//CSQLiteDB

CSQLiteDB::CSQLiteDB(const string& strDBPath)
	: m_strDBPath(strDBPath)
{
}

CSQLiteDB::~CSQLiteDB()
{
	Disconnect();
}

int CSQLiteDB::GetStatus()
{
	return (NULL != m_hDB);
}

BOOL CSQLiteDB::Connect()
{
	ASSERT_RETURN_EX(!m_hDB, FALSE);

	ASSERT_RETURN_EX(SQLITE_OK == sqlite3_open(m_strDBPath.c_str(), (sqlite3**)&m_hDB), FALSE);
	ASSERT_RETURN_EX(m_hDB, FALSE);

	return TRUE;
}

BOOL CSQLiteDB::Disconnect()
{
	ENSURE_RETURN_EX(m_hDB, FALSE);

	ENSURE_RETURN_EX(SQLITE_OK == sqlite3_close((sqlite3*)m_hDB), FALSE);

	m_hDB = NULL;

	return TRUE;
}

BOOL CSQLiteDB::Execute(const std::wstring& strSql, string& strError)
{
	ASSERT_RETURN_EX(m_hDB, FALSE);
	
	char *pszError = NULL;
	if (SQLITE_OK != sqlite3_exec((sqlite3*)m_hDB, util::WStrToStr(strSql, CP_UTF8).c_str(), 0, 0, &pszError))
	{
		strError = pszError;
		return FALSE;
	}

	return TRUE;
}

IDBResult* CSQLiteDB::Query(const std::wstring& strSql, string& strError)
{
	ASSERT_RETURN_EX(m_hDB, NULL);

	char ** pData = NULL;

	int nColumnCount = 0;
	int nRowCount = 0;

	char *pszError = NULL;
	int nResult = sqlite3_get_table((sqlite3*)m_hDB, util::WStrToStr(strSql, CP_UTF8).c_str(), &pData
		, &nRowCount, &nColumnCount, &pszError);

	if (pszError)
	{
		strError = pszError;
	}

	ASSERT_RETURN_EX(SQLITE_OK == nResult && pData, NULL);
	
	CSQLiteDBResult* pSQLiteDBResult = new CSQLiteDBResult;

	pSQLiteDBResult->m_nColumnCount = (UINT)nColumnCount;
	pSQLiteDBResult->m_nRowCount = (UINT)nRowCount;

	pSQLiteDBResult->m_pData = pData;

	return pSQLiteDBResult;
}
