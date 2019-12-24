#include "Texture.h"

#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

#include <stb_image.h>

namespace jarp {

	Texture::Texture()
	{
		m_TextureImage = new VulkanImage();
		m_TextureImageView = new VulkanImageView();
	}

	Texture::~Texture()
	{
		delete m_TextureImageView;
		delete m_TextureImage;
	}

	void Texture::Load(VulkanCommandBuffer& commandBuffer, const std::string& fileName)
	{
		// Load image from disk
		int width, height, channels;
		stbi_uc* pixels = stbi_load(fileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!pixels)
			throw std::runtime_error("Failed to load texture image");
		VkDeviceSize imageSize = width * height * 4;

		VulkanBuffer stagingBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		stagingBuffer.CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* rawData;
		vkMapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), stagingBuffer.GetMemoryHandle(), 0, imageSize, 0, &rawData);
		memcpy(rawData, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), stagingBuffer.GetMemoryHandle());

		stbi_image_free(pixels);

		// Create Vulkan image for loaded texture
		m_TextureImage->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Initial transfer
		m_TextureImage->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Copy buffer to image
		commandBuffer.BeginOneTimeSubmitCommand();

		VkBufferImageCopy bufferImageCopy = {};
		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = 0;
		bufferImageCopy.bufferImageHeight = 0;
		bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferImageCopy.imageSubresource.mipLevel = 0;
		bufferImageCopy.imageSubresource.baseArrayLayer = 0;
		bufferImageCopy.imageSubresource.layerCount = 1;
		bufferImageCopy.imageOffset = { 0, 0, 0 };
		bufferImageCopy.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };

		vkCmdCopyBufferToImage(commandBuffer.GetHandle(), stagingBuffer.GetHandle(), m_TextureImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

		commandBuffer.EndOneTimeSubmitCommand();

		// Final transfer for shader access
		m_TextureImage->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		stagingBuffer.Destroy();

		m_TextureImageView->CreateImageView(m_TextureImage->GetHandle(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = nullptr;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_TRUE; // Also needs to be enabled in the physical device features
		samplerCreateInfo.maxAnisotropy = 16;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		VK_ASSERT(vkCreateSampler(VulkanRendererAPI::s_Device->GetInstanceHandle(), &samplerCreateInfo, nullptr, &m_Sampler));
	}

	void Texture::Destroy()
	{
		vkDestroySampler(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Sampler, nullptr);
		m_TextureImageView->Destroy();
		m_TextureImage->Destroy();
	}

}
