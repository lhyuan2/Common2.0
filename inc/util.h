
#pragma once

#include "def.h"


class __CommonPrjExt util
{
public:
	static BOOL StrCompare(const wstring& str1, const wstring& str2);

	static int StrFind(const wstring& str, const wstring& strToFind);

	static wstring StrLowerCase(const wstring& str);

	static string WStrToStr(const wstring&str, UINT CodePage = CP_ACP);
	static wstring StrToWStr(const string&str, UINT CodePage = CP_ACP);
	
	static int StrToInt(string x);
	static string DblToStr(double x);
	static double StrToDbl(const string& x);

	static bool IsUTF8Str(const string& strText);

	template <class _C, class _V>
	static BOOL ContainerFind(_C& container, _V value)
	{
		return std::find(container.begin(), container.end(), value) != container.end();
	}

	template <class _C, class _V>
	static auto ContainerFindRef(_C& container, _V& value)
	{
		auto itr = container.begin();
		for (; itr != container.end(); itr++)
		{
			if ((void*)&*itr == &value)
			{
				break;
			}
		}
	
		return itr;
	}

	template <class _C>
	static wstring ContainerToStr(const _C& container, const wstring& strSplitor)
	{
		std::wstringstream strmResult;

		for (_C::const_iterator it = container.begin(); ; )
		{
			strmResult << *it;
			
			it++;
			ENSURE_BREAK(it != container.end());

			strmResult << strSplitor;
		}

		return strmResult.str();
	}
};


template <class _PtrType>
class ptrlist : public std::list<_PtrType>
{
public:	
	ptrlist()
	{
	}
	
	ptrlist(_PtrType ptr)
	{
		Insert(ptr);
	}

	template <typename _RefType>
	ptrlist(const std::list<_RefType*>& container)
	{
		Insert(container);
	}

	template <typename _RefType>
	ptrlist(const std::list<_RefType>& container)
	{
		Insert(container);
	}
	
public:
	void Insert(_PtrType ptr)
	{
		push_back(ptr);
	}

	template <typename _RefType>
	void Insert(const std::list<_RefType*>& container)
	{
		for (std::list<_RefType*>::const_iterator it = container.begin(); it != container.end(); ++it)
		{
			push_back((_PtrType)*it);
		}
	}
	
	template <typename _RefType>
	void Insert(const std::list<_RefType>& container)
	{
		for (std::list<_RefType>::const_iterator it = container.begin(); it != container.end(); ++it)
		{
			push_back((_PtrType)(_RefType*)&*it);
		}
	}
};
