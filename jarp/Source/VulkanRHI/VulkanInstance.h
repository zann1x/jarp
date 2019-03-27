#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	void CreateInstance();
	void Destroy();

	inline const VkInstance& GetHandle() const { return Instance; }

private:
	VkInstance Instance;

	std::vector<const char*> InstanceLayers;
	std::vector<const char*> InstanceExtensions;
};
