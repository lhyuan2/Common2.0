
#include "stdafx.h"

#include <util.h>

BOOL util::StrCompare(const wstring& str1, const wstring& str2)
{
	return 0 == _wcsicmp(str1.c_str(), str2.c_str());
}

int util::StrFind(const wstring& str1, const wstring& str2)
{
	ENSURE_RETURN_EX(str1.size() >= str2.size(), -1);
	wstring::size_type pos = StrLowerCase(str1).find(StrLowerCase(str2));
	if (wstring::npos == pos)
	{
		return -1;
	}
	return pos;
}

wstring util::StrLowerCase(const wstring& str)
{
	wstring strTemp = str;
	
	(void)::_wcslwr_s((TCHAR *)strTemp.c_str(), strTemp.size() + 1);

	return strTemp;
}

string util::WStrToStr(const wstring&str, UINT CodePage)
{
	if (str.empty())
	{
		return "";
	}

	int buffSize = WideCharToMultiByte(CodePage,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	char* pBuff = new char[buffSize + 1];
	memset((void*)pBuff, 0, sizeof(pBuff[0]) * (buffSize + 1));
	::WideCharToMultiByte(CodePage,
		0,
		str.c_str(),
		-1,
		pBuff,
		buffSize,
		NULL,
		NULL);
	string strRet = pBuff;
	delete[] pBuff;
	return strRet;
}

wstring util::StrToWStr(const string&str, UINT CodePage)
{
	if (str.empty())
	{
		return L"";
	}

	int buffSize = ::MultiByteToWideChar(CodePage,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t * pBuff;
	pBuff = new wchar_t[buffSize + 1];
	memset(pBuff, 0, sizeof(pBuff[0]) * (buffSize + 1));
	::MultiByteToWideChar(CodePage,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pBuff,
		buffSize);
	wstring strRet = (wchar_t*)pBuff;
	delete pBuff;
	return strRet;
}

int util::StrToInt(string x)
{
	return ::atoi(x.c_str());
}

string util::DblToStr(double x)
{
	char lpBuffer[16];
	sprintf_s(lpBuffer, "%f", x);
	
	return string(lpBuffer);		
}

double util::StrToDbl(const string& x)
{
	return ::atof(x.c_str());
}
