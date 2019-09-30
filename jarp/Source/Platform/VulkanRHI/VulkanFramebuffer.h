#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanRenderPass;

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(VulkanRenderPass& RenderPass);
		~VulkanFramebuffer();

		void CreateFramebuffer(const std::vector<VkImageView> Attachments, const VkExtent2D& Extent);
		void Destroy();

		inline const VkFramebuffer& GetHandle() const { return Framebuffer; }

	private:
		VulkanRenderPass& RenderPass;

		VkFramebuffer Framebuffer;
	};

}
