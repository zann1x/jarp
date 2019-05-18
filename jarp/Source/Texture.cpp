#include "Texture.h"

#include "VulkanRHI/VulkanBuffer.h"
#include "VulkanRHI/VulkanCommandBuffer.h"
#include "VulkanRHI/VulkanDevice.h"
#include "VulkanRHI/VulkanImage.h"
#include "VulkanRHI/VulkanImageView.h"
#include "VulkanRHI/VulkanUtils.hpp"

#include <vector>

Texture::Texture(VulkanDevice& Device)
	: Device(Device)
{
	pTextureImage = new VulkanImage(Device);
	pTextureImageView = new VulkanImageView(Device);
}

Texture::~Texture()
{
	delete pTextureImageView;
	delete pTextureImage;
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
	pTextureImage->CreateImage(Width, Height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Initial transfer
	pTextureImage->TransitionImageLayout(CommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

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

	vkCmdCopyBufferToImage(CommandBuffer.GetHandle(), StagingBuffer.GetHandle(), pTextureImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy);

	CommandBuffer.EndOneTimeSubmitCommand();

	// Final transfer for shader access
	pTextureImage->TransitionImageLayout(CommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.Destroy();

	pTextureImageView->CreateImageView(pTextureImage->GetHandle(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

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
	vkDestroySampler(Device.GetInstanceHandle(), Sampler, nullptr);
	pTextureImageView->Destroy();
	pTextureImage->Destroy();
}
