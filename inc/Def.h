
#pragma once

#include <string>
#include <sstream>

#include <list>
#include <vector>
#include <set>
#include <map>

#include <algorithm>

#include <functional>

#include <thread>
#include <future>

#include <stdlib.h>

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

#define DECLARE_SINGLETON(_CLASS) \
	public: \
		static _CLASS& inst() \
		{ \
			static _CLASS inst; \
			return inst; \
		} \
\
	private: \
		_CLASS();

#ifdef __CommonPrj
	#define __CommonPrjExt __declspec(dllexport)
#else
	#define __CommonPrjExt __declspec(dllimport)
#endif
