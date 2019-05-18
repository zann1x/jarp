#include "Texture.h"

#include "VulkanRHI/VulkanBuffer.h"
#include "VulkanRHI/VulkanCommandBuffer.h"
#include "VulkanRHI/VulkanDevice.h"
#include "VulkanRHI/VulkanUtils.hpp"

#include <vector>

Texture::Texture(VulkanDevice& Device)
	: Device(Device)
{
}

Texture::~Texture()
{
}

void Texture::Load(VulkanCommandBuffer& CommandBuffer, const std::string& FileName)
{
	// Load image from disk
	int Width, Height, Channels;
	stbi_uc* Pixels = stbi_load(FileName.c_str(), &Width, &Height, &Channels, STBI_rgb_alpha);
	VkDeviceSize ImageSize = Width * Height * 4;
	if (!Pixels)
		throw std::runtime_error("Failed to load texture image");

	VulkanBuffer StagingBuffer(Device, ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	StagingBuffer.CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	void* RawData;
	vkMapMemory(Device.GetInstanceHandle(), StagingBuffer.GetMemoryHandle(), 0, ImageSize, 0, &RawData);
	memcpy(RawData, Pixels, static_cast<size_t>(ImageSize));
	vkUnmapMemory(Device.GetInstanceHandle(), StagingBuffer.GetMemoryHandle());

	stbi_image_free(Pixels);

	// Create vulkan image for loaded texture
	CreateImage(Width, Height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Initial transfer
	TransitionImageLayout(CommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy buffer to image
	CommandBuffer.BeginOneTimeSubmitCommand();
	
	VkBufferImageCopy BufferImageCopy = {};
	BufferImageCopy.bufferOffset = 0;
	BufferImageCopy.bufferRowLength = 0;
	BufferImageCopy.bufferImageHeight = 0;
	BufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	BufferImageCopy.imageSubresource.mipLevel = 0;
	BufferImageCopy.imageSubresource.baseArrayLayer = 0;
	BufferImageCopy.imageSubresource.layerCount = 1;
	BufferImageCopy.imageOffset = { 0, 0, 0 };
	BufferImageCopy.imageExtent = { static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1 };

	vkCmdCopyBufferToImage(CommandBuffer.GetHandle(), StagingBuffer.GetHandle(), Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy);

	CommandBuffer.EndOneTimeSubmitCommand();

	// Finish transfer
	TransitionImageLayout(CommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.Destroy();

	VkImageViewCreateInfo ImageViewCreateInfo = {};
	ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewCreateInfo.pNext = nullptr;
	ImageViewCreateInfo.flags = 0;
	ImageViewCreateInfo.image = Image;
	ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	ImageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ImageViewCreateInfo.subresourceRange.levelCount = 1;
	ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ImageViewCreateInfo.subresourceRange.layerCount = 1;

	VK_ASSERT(vkCreateImageView(Device.GetInstanceHandle(), &ImageViewCreateInfo, nullptr, &ImageView));

	VkSamplerCreateInfo SamplerCreateInfo = {};
	SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	SamplerCreateInfo.pNext = nullptr;
	SamplerCreateInfo.flags = 0;
	SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.mipLodBias = 0.0f;
	SamplerCreateInfo.anisotropyEnable = VK_TRUE; // Also needs to be enabled in the physical device features
	SamplerCreateInfo.maxAnisotropy = 16;
	SamplerCreateInfo.compareEnable = VK_FALSE;
	SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerCreateInfo.minLod = 0.0f;
	SamplerCreateInfo.maxLod = 0.0f;
	SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VK_ASSERT(vkCreateSampler(Device.GetInstanceHandle(), &SamplerCreateInfo, nullptr, &Sampler));
}

void Texture::Destroy()
{
	vkDestroySampler(Device.GetInstanceHandle(), Sampler, nullptr),
	vkDestroyImageView(Device.GetInstanceHandle(), ImageView, nullptr);
	vkFreeMemory(Device.GetInstanceHandle(), DeviceMemory, nullptr);
	vkDestroyImage(Device.GetInstanceHandle(), Image, nullptr);
}

void Texture::CreateImage(uint32_t Width, uint32_t Height, VkFormat Format, VkImageTiling ImageTiling, VkImageUsageFlags ImageUsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags)
{
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

	VK_ASSERT(vkCreateImage(Device.GetInstanceHandle(), &ImageCreateInfo, nullptr, &Image));

	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(Device.GetInstanceHandle(), Image, &MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = nullptr;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = Device.GetMemoryTypeIndex(MemoryRequirements.memoryTypeBits, MemoryPropertyFlags);

	VK_ASSERT(vkAllocateMemory(Device.GetInstanceHandle(), &MemoryAllocateInfo, nullptr, &DeviceMemory));

	VK_ASSERT(vkBindImageMemory(Device.GetInstanceHandle(), Image, DeviceMemory, 0));
}

void Texture::TransitionImageLayout(VulkanCommandBuffer& CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout)
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
	else
	{
		throw std::runtime_error("Unsupported layout transition");
	}

	ImageMemoryBarrier.oldLayout = OldLayout;
	ImageMemoryBarrier.newLayout = NewLayout;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = Image;
	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(CommandBuffer.GetHandle(), SourceStage, DestinationStage, 0, 0, nullptr, 0, nullptr, 1, &ImageMemoryBarrier);

	CommandBuffer.EndOneTimeSubmitCommand();
}
