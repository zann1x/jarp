#pragma once

#include <volk.h>

#include <vector>

namespace jarp {

	namespace VulkanDebug
	{
		extern std::vector<const char*> ValidationLayers;

		void SetupDebugCallback(const VkInstance& instance);
		void DestroyDebugCallback(const VkInstance& instance);
	}

}
