#include "VulkanFramebuffer.h"

#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.hpp"

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice& OutDevice, VulkanRenderPass& OutRenderPass)
	: Device(OutDevice), RenderPass(OutRenderPass)
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{
}

void VulkanFramebuffer::CreateFramebuffer(const VkImageView& ImageView, const VkExtent2D& Extent)
{
	VkFramebufferCreateInfo FramebufferCreateInfo = {};
	FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferCreateInfo.pNext = nullptr;
	FramebufferCreateInfo.flags = 0;
	FramebufferCreateInfo.renderPass = RenderPass.GetHandle();
	FramebufferCreateInfo.attachmentCount = 1;
	FramebufferCreateInfo.pAttachments = &ImageView;
	FramebufferCreateInfo.width = Extent.width;
	FramebufferCreateInfo.height = Extent.height;
	FramebufferCreateInfo.layers = 1;

	VK_ASSERT(vkCreateFramebuffer(Device.GetInstanceHandle(), &FramebufferCreateInfo, nullptr, &Framebuffer));
}

void VulkanFramebuffer::Destroy()
{
	vkDestroyFramebuffer(Device.GetInstanceHandle(), Framebuffer, nullptr);
}
