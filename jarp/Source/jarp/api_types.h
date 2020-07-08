#ifndef API_TYPES_H
#define API_TYPES_H

#ifdef JARP_PLATFORM_WIN32
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

#endif
