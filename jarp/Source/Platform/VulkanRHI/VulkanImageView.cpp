#include "jarppch.h"
#include "VulkanImageView.h"

#include "VulkanDevice.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanImageView::VulkanImageView()
		: ImageView(VK_NULL_HANDLE)
	{
	}

	VulkanImageView::~VulkanImageView()
	{
	}

	void VulkanImageView::CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags ImageAspectFlags)
	{
		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.pNext = nullptr;
		ImageViewCreateInfo.flags = 0;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format = Format;
		ImageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		ImageViewCreateInfo.subresourceRange.aspectMask = ImageAspectFlags;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_ASSERT(vkCreateImageView(VulkanRendererAPI::pDevice->GetInstanceHandle(), &ImageViewCreateInfo, nullptr, &ImageView));
	}

	void VulkanImageView::Destroy()
	{
		vkDestroyImageView(VulkanRendererAPI::pDevice->GetInstanceHandle(), ImageView, nullptr);
	}

}
