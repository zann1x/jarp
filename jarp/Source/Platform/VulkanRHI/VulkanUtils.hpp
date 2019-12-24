#pragma once

#include "jarp/Log.h"

#if defined(JARP_PLATFORM_WINDOWS)
	#define VK_ASSERT(val)\
		if (val != VK_SUCCESS)\
		{\
			__debugbreak();\
		}
#elif defined(JARP_PLATFORM_LINUX)
	#define VK_ASSERT(val)\
		if (val != VK_SUCCESS)\
		{\
			JARP_CORE_ERROR("Assertion failed: {0}", val);\
		}
#endif
