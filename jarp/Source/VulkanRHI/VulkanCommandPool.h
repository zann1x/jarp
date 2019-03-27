#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

/*
Depends on:
- Device
*/
class VulkanCommandPool
{
public:
	VulkanCommandPool(VulkanDevice& OutDevice);
	~VulkanCommandPool();

	void CreateCommandPool();
	void Destroy();

	inline VkCommandPool GetHandle() { return CommandPool; };

private:
	VulkanDevice& Device;
	VkCommandPool CommandPool;
};
