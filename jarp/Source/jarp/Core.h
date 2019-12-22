#pragma once

#if defined(_DEBUG)
	#if defined(JARP_PLATFORM_WINDOWS)
		#define JARP_CORE_ASSERT(x, ...) {\
			if (!x)\
			{\
				JARP_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__);\
				__debugbreak();\
			}\
		}
	#elif defined(JARP_PLATFORM_LINUX)
		#define JARP_CORE_ASSERT(x, ...) {\
			if (!x)\
			{\
				JARP_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__);\
			}\
		}
	#endif
#endif

#define JARP_BIND_FUNCTION(Func) std::bind(&Func, this, std::placeholders::_1)
