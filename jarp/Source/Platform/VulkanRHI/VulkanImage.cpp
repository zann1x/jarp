#include "jarppch.h"
#include "VulkanImage.h"

#include "Platform/VulkanRHI/VulkanDevice.h"
#include "Platform/VulkanRHI/VulkanCommandBuffer.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

namespace jarp {

	VulkanImage::VulkanImage()
		: m_Image(VK_NULL_HANDLE), m_DeviceMemory(VK_NULL_HANDLE), m_Format(VK_FORMAT_UNDEFINED)
	{
	}

	VulkanImage::~VulkanImage()
	{
	}

	void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags)
	{
		m_Format = format;

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Change this in case of multisampling support
		imageCreateInfo.tiling = imageTiling;
		imageCreateInfo.usage = imageUsageFlags;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VK_ASSERT(vkCreateImage(VulkanRendererAPI::s_Device->GetInstanceHandle(), &imageCreateInfo, nullptr, &m_Image));

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Image, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VulkanRendererAPI::s_Device->GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryPropertyFlags);

		VK_ASSERT(vkAllocateMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), &memoryAllocateInfo, nullptr, &m_DeviceMemory));

		VK_ASSERT(vkBindImageMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Image, m_DeviceMemory, 0));
	}

	void VulkanImage::Destroy()
	{
		vkFreeMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_DeviceMemory, nullptr);
		vkDestroyImage(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Image, nullptr);
	}

	void VulkanImage::TransitionImageLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		commandBuffer.BeginOneTimeSubmitCommand();

		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			throw std::runtime_error("Unsupported layout transition");
		}

		imageMemoryBarrier.oldLayout = oldLayout;
		imageMemoryBarrier.newLayout = newLayout;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = m_Image;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (m_Format == VK_FORMAT_D32_SFLOAT_S8_UINT || m_Format == VK_FORMAT_D24_UNORM_S8_UINT)
			{
				imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(commandBuffer.GetHandle(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		commandBuffer.EndOneTimeSubmitCommand();
	}

}
