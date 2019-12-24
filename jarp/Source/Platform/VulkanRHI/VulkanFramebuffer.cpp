#include "VulkanFramebuffer.h"

#include "VulkanRendererAPI.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanFramebuffer::VulkanFramebuffer(VulkanRenderPass& renderPass)
		: m_RenderPass(renderPass), m_Framebuffer(VK_NULL_HANDLE)
	{
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::CreateFramebuffer(const std::vector<VkImageView> attachments, const VkExtent2D& extent)
	{
		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = m_RenderPass.GetHandle();
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = extent.width;
		framebufferCreateInfo.height = extent.height;
		framebufferCreateInfo.layers = 1;

		VK_ASSERT(vkCreateFramebuffer(VulkanRendererAPI::s_Device->GetInstanceHandle(), &framebufferCreateInfo, nullptr, &m_Framebuffer));
	}

	void VulkanFramebuffer::Destroy()
	{
		vkDestroyFramebuffer(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Framebuffer, nullptr);
	}

}
