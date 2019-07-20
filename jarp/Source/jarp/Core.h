#pragma once

#define JARP_CORE_ASSERT(x, ...) {\
		if (!x)\
		{\
			JARP_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__);\
			__debugbreak();\
		}\
	}
