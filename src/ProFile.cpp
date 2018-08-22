
#include "stdafx.h"

#include <ProFile.h>

#include <util.h>


#define MAX_BUFFER 1024

CProFile::CProFile(const string& strIniPath)
	: m_strIniPath(strIniPath)
{
}

CProFile::~CProFile()
{
}

BOOL CProFile::ReadString(const string& strSection, const string& strKey, string& strValue)
{
	CHAR pszValue[MAX_BUFFER];
	ASSERT_RETURN_EX(0 != ::GetPrivateProfileStringA(strSection.c_str(), strKey.c_str(), strValue.c_str()
		, pszValue, MAX_BUFFER, m_strIniPath.c_str())
	, FALSE);
	
	strValue = pszValue;

	return TRUE;
}

BOOL CProFile::ReadInt(const string& strSection, const string& strKey, int& nValue)
{
	string strValue = std::to_string(nValue);

	ENSURE_RETURN_EX(this->ReadString(strSection, strKey, strValue), FALSE);

	nValue = util::StrToInt(strValue);
	
	return TRUE;
}

double CProFile::ReadDouble(const string& strSection, const string& strKey, double& dblValue)
{
	string strValue = util::DblToStr(dblValue);

	ENSURE_RETURN_EX(this->ReadString(strSection, strKey, strValue), FALSE);

	dblValue = util::StrToDbl(strValue);
	
	return TRUE;
}

BOOL CProFile::WriteString(const string& strSection, const string& strKey, const string& strValue)
{
	return ::WritePrivateProfileStringA(strSection.c_str(), strKey.c_str()
		, strValue.c_str(), m_strIniPath.c_str());
}

BOOL CProFile::WriteInt(const string& strSection, const string& strKey, const int nValue)
{
	return this->WriteString(strSection, strKey, std::to_string(nValue));
}

BOOL CProFile::WriteDouble(const string& strSection, const string& strKey, const double dblValue)
{
	return this->WriteString(strSection, strKey, util::DblToStr(dblValue));
}
