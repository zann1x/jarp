#pragma once

#include <volk.h>

namespace jarp {

	namespace VulkanDebug
	{
		extern std::vector<const char*> ValidationLayers;

		void SetupDebugCallback(const VkInstance& Instance);
		void DestroyDebugCallback(const VkInstance& Instance);
	}

}
