#pragma once

#include <volk.h>

namespace jarp {

	class VulkanRenderPass;

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(VulkanRenderPass& renderPass);
		~VulkanFramebuffer();

		void CreateFramebuffer(const std::vector<VkImageView> attachments, const VkExtent2D& extent);
		void Destroy();

		inline const VkFramebuffer& GetHandle() const { return m_Framebuffer; }

	private:
		VulkanRenderPass& m_RenderPass;

		VkFramebuffer m_Framebuffer;
	};

}
