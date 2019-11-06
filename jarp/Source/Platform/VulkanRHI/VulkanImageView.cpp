#include "jarppch.h"
#include "VulkanImageView.h"

#include "VulkanDevice.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanImageView::VulkanImageView()
		: m_ImageView(VK_NULL_HANDLE)
	{
	}

	VulkanImageView::~VulkanImageView()
	{
	}

	void VulkanImageView::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		imageViewCreateInfo.subresourceRange.aspectMask = imageAspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_ASSERT(vkCreateImageView(VulkanRendererAPI::s_Device->GetInstanceHandle(), &imageViewCreateInfo, nullptr, &m_ImageView));
	}

	void VulkanImageView::Destroy()
	{
		vkDestroyImageView(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_ImageView, nullptr);
	}

}
