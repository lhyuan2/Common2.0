
#pragma once

#include <string>
#include <sstream>

#include <list>
#include <set>
#include <vector>
#include <map>

#include <algorithm>

using namespace std;


#define ENSURE_RETURN(x) \
	if (!(x)) \
	{ \
		return; \
	}

#define ASSERT_RETURN(x) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		return; \
	}

#define ENSURE_RETURN_EX(x, y) \
	if (!(x)) \
	{ \
		return y; \
	}

#define ASSERT_RETURN_EX(x, y) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		return y; \
	}


#define ENSURE_CONTINUE(x) \
	if (!(x)) \
	{ \
		continue; \
	}

#define ASSERT_CONTINUE(x) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		continue; \
	}

#define ENSURE_BREAK(x) \
	if (!(x)) \
	{ \
		break; \
	}

#define ASSERT_BREAK(x) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		break; \
	}


#include <stdlib.h>

#define DECLARE_SINGLETON(_CLASS) \
	public: \
		_CLASS(); \
\
		~_CLASS(); \
\
		static _CLASS *Instance() \
		{ \
			if (m_pInstance == NULL) \
			{ \
				m_pInstance = new _CLASS; \
				atexit(DeleteInstance); \
			} \
\
			return m_pInstance; \
		} \
\
		static void DeleteInstance() \
		{ \
			if (!m_pInstance) \
			{ \
				delete m_pInstance; \
				m_pInstance = NULL; \
			} \
		} \
\
	private: \
		static _CLASS *m_pInstance;

#define IMPLEMENT_SINGLETON(_CLASS) \
_CLASS *_CLASS::m_pInstance = NULL;


#ifndef COMMON_EXT_CLASS
	#ifdef _COMMON
		#define COMMON_EXT_CLASS __declspec(dllexport)
	#else
		#define COMMON_EXT_CLASS __declspec(dllimport)
	#endif
#endif
