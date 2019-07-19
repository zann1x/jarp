#pragma once

#define VK_ASSERT(val)\
	if (val != VK_SUCCESS)\
	{\
		__debugbreak();\
	}
