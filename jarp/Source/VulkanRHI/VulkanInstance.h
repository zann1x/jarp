#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class CrossPlatformWindow;

class VulkanInstance
{
public:
	VulkanInstance(CrossPlatformWindow& Window);
	~VulkanInstance();

	void CreateInstance();
	void Destroy();

	inline const VkInstance& GetHandle() const { return Instance; }

private:
	VkInstance Instance;

	std::vector<const char*> InstanceLayers;
	std::vector<const char*> InstanceExtensions;
};
