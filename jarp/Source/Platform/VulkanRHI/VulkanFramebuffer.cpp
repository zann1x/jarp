#include "VulkanFramebuffer.h"

#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.hpp"

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice& Device, VulkanRenderPass& RenderPass)
	: Device(Device), RenderPass(RenderPass)
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

	VK_ASSERT(vkCreateFramebuffer(Device.GetInstanceHandle(), &FramebufferCreateInfo, nullptr, &Framebuffer));
}

void VulkanFramebuffer::Destroy()
{
	vkDestroyFramebuffer(Device.GetInstanceHandle(), Framebuffer, nullptr);
}
