#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapchain;

/*
Depends on:
- Device
- SwapchainKHR
*/
class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanDevice& OutDevice, VulkanSwapchain& OutSwapchain);
	~VulkanRenderPass();

	void CreateRenderPass();
	void Destroy();

	inline const VkRenderPass& GetHandle() const { return RenderPass; }

private:
	VulkanDevice& Device;
	VulkanSwapchain& Swapchain;

	VkRenderPass RenderPass;
};
