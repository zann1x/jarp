#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapchain;

/*
Depends on:
- Device
- SwapchainKHR
*/
class VulkanDescriptorPool
{
public:
	VulkanDescriptorPool(VulkanDevice& OutDevice, VulkanSwapchain& OutSwapchain);
	~VulkanDescriptorPool();

	void CreateDescriptorPool();
	void Destroy();

	inline const VkDescriptorPool& GetHandle() const { return DescriptorPool; }

private:
	VulkanDevice& Device;
	VulkanSwapchain& Swapchain;

	VkDescriptorPool DescriptorPool;
};
