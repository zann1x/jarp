#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanRenderPass;

/*
Depends on:
- Device
- SwapchainKHR
- RenderPass
*/
class VulkanFramebuffer
{
public:
	VulkanFramebuffer(VulkanDevice& OutDevice, VulkanRenderPass& OutRenderPass);
	~VulkanFramebuffer();

	void CreateFramebuffer(const VkImageView& ImageView, const VkExtent2D& Extent);
	void Destroy();

	inline const VkFramebuffer& GetHandle() const { return Framebuffer; }

private:
	VulkanDevice& Device;
	VulkanRenderPass& RenderPass;

	VkFramebuffer Framebuffer;
};
