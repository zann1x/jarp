#pragma once

// Include generally useful standard headers for more types compliant with C99
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#if JARP_PLATFORM_WIN32
	#ifdef __cplusplus
		#define API extern "C"
	#else
		#define API __declspec(dllexport)
	#endif
#elif JARP_PLATFORM_LINUX
	#define API
#else
	#define API
#endif
