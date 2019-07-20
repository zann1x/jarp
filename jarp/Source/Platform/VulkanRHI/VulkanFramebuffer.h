#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

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
		VulkanFramebuffer(VulkanDevice& Device, VulkanRenderPass& RenderPass);
		~VulkanFramebuffer();

		void CreateFramebuffer(const std::vector<VkImageView> Attachments, const VkExtent2D& Extent);
		void Destroy();

		inline const VkFramebuffer& GetHandle() const { return Framebuffer; }

	private:
		VulkanDevice& Device;
		VulkanRenderPass& RenderPass;

		VkFramebuffer Framebuffer;
	};

}
