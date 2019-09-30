#include "jarppch.h"
#include "VulkanFramebuffer.h"

#include "VulkanRendererAPI.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanFramebuffer::VulkanFramebuffer(VulkanRenderPass& RenderPass)
		: RenderPass(RenderPass), Framebuffer(VK_NULL_HANDLE)
	{
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::CreateFramebuffer(const std::vector<VkImageView> Attachments, const VkExtent2D& Extent)
	{
		VkFramebufferCreateInfo FramebufferCreateInfo = {};
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.pNext = nullptr;
		FramebufferCreateInfo.flags = 0;
		FramebufferCreateInfo.renderPass = RenderPass.GetHandle();
		FramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
		FramebufferCreateInfo.pAttachments = Attachments.data();
		FramebufferCreateInfo.width = Extent.width;
		FramebufferCreateInfo.height = Extent.height;
		FramebufferCreateInfo.layers = 1;

		VK_ASSERT(vkCreateFramebuffer(VulkanRendererAPI::pDevice->GetInstanceHandle(), &FramebufferCreateInfo, nullptr, &Framebuffer));
	}

	void VulkanFramebuffer::Destroy()
	{
		vkDestroyFramebuffer(VulkanRendererAPI::pDevice->GetInstanceHandle(), Framebuffer, nullptr);
	}

}
