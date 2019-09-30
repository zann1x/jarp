#include "jarppch.h"
#include "VulkanImage.h"

#include "VulkanCommandBuffer.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanImage::VulkanImage()
		: Image(VK_NULL_HANDLE), DeviceMemory(VK_NULL_HANDLE), Format(VK_FORMAT_UNDEFINED)
	{
	}

	VulkanImage::~VulkanImage()
	{
	}

	void VulkanImage::CreateImage(uint32_t Width, uint32_t Height, VkFormat Format, VkImageTiling ImageTiling, VkImageUsageFlags ImageUsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags)
	{
		this->Format = Format;

		VkImageCreateInfo ImageCreateInfo = {};
		ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageCreateInfo.pNext = nullptr;
		ImageCreateInfo.flags = 0;
		ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageCreateInfo.format = Format;
		ImageCreateInfo.extent.width = Width;
		ImageCreateInfo.extent.height = Height;
		ImageCreateInfo.extent.depth = 1;
		ImageCreateInfo.mipLevels = 1;
		ImageCreateInfo.arrayLayers = 1;
		ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Change this in case of multisampling support
		ImageCreateInfo.tiling = ImageTiling;
		ImageCreateInfo.usage = ImageUsageFlags;
		ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		ImageCreateInfo.queueFamilyIndexCount = 0;
		ImageCreateInfo.pQueueFamilyIndices = nullptr;
		ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VK_ASSERT(vkCreateImage(VulkanRendererAPI::pDevice->GetInstanceHandle(), &ImageCreateInfo, nullptr, &Image));

		VkMemoryRequirements MemoryRequirements;
		vkGetImageMemoryRequirements(VulkanRendererAPI::pDevice->GetInstanceHandle(), Image, &MemoryRequirements);

		VkMemoryAllocateInfo MemoryAllocateInfo = {};
		MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		MemoryAllocateInfo.pNext = nullptr;
		MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
		MemoryAllocateInfo.memoryTypeIndex = VulkanRendererAPI::pDevice->GetMemoryTypeIndex(MemoryRequirements.memoryTypeBits, MemoryPropertyFlags);

		VK_ASSERT(vkAllocateMemory(VulkanRendererAPI::pDevice->GetInstanceHandle(), &MemoryAllocateInfo, nullptr, &DeviceMemory));

		VK_ASSERT(vkBindImageMemory(VulkanRendererAPI::pDevice->GetInstanceHandle(), Image, DeviceMemory, 0));
	}

	void VulkanImage::Destroy()
	{
		vkFreeMemory(VulkanRendererAPI::pDevice->GetInstanceHandle(), DeviceMemory, nullptr);
		vkDestroyImage(VulkanRendererAPI::pDevice->GetInstanceHandle(), Image, nullptr);
	}

	void VulkanImage::TransitionImageLayout(VulkanCommandBuffer& CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout)
	{
		CommandBuffer.BeginOneTimeSubmitCommand();

		VkImageMemoryBarrier ImageMemoryBarrier{};
		ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		ImageMemoryBarrier.pNext = nullptr;

		VkPipelineStageFlags SourceStage;
		VkPipelineStageFlags DestinationStage;
		if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			ImageMemoryBarrier.srcAccessMask = 0;
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			SourceStage = VK_PIPELINE_STAGE_HOST_BIT;
			DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			ImageMemoryBarrier.srcAccessMask = 0;
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			SourceStage = VK_PIPELINE_STAGE_HOST_BIT;
			DestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			throw std::runtime_error("Unsupported layout transition");
		}

		ImageMemoryBarrier.oldLayout = OldLayout;
		ImageMemoryBarrier.newLayout = NewLayout;
		ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageMemoryBarrier.image = Image;

		if (NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT)
			{
				ImageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		ImageMemoryBarrier.subresourceRange.levelCount = 1;
		ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		ImageMemoryBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(CommandBuffer.GetHandle(), SourceStage, DestinationStage, 0, 0, nullptr, 0, nullptr, 1, &ImageMemoryBarrier);

		CommandBuffer.EndOneTimeSubmitCommand();
	}

}
