#pragma once

#include <stb_image.h>
#include <string>
#include <vulkan/vulkan.h>

class VulkanCommandBuffer;
class VulkanDevice;

class Texture
{
public:
	Texture(VulkanDevice& Device);
	~Texture();

	void Load(VulkanCommandBuffer& CommandBuffer, const std::string& FileName);
	void Destroy();

	inline const VkSampler GetSampler() const { return Sampler; }
	inline const VkImageView GetImageView() const { return ImageView; }

private:
	void CreateImage(uint32_t Width, uint32_t Height, VkFormat Format, VkImageTiling ImageTiling, VkImageUsageFlags ImageUsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags);
	void TransitionImageLayout(VulkanCommandBuffer& CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout);

private:
	VulkanDevice& Device;

	VkImage Image;
	VkImageView ImageView;
	VkDeviceMemory DeviceMemory;
	VkSampler Sampler;
};
