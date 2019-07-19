#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class WindowsWindow;

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	void CreateInstance(const WindowsWindow& Window);
	void Destroy();

	inline const VkInstance& GetHandle() const { return Instance; }

private:
	VkInstance Instance;

	std::vector<const char*> InstanceLayers;
	std::vector<const char*> InstanceExtensions;
};
