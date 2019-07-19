#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanDebug
{
	extern std::vector<const char*> ValidationLayers;

	void SetupDebugCallback(const VkInstance& Instance);
	void DestroyDebugCallback(const VkInstance& Instance);
}
