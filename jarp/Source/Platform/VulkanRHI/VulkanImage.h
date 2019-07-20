#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanCommandBuffer;
	class VulkanDevice;

	class VulkanImage
	{
	public:
		VulkanImage(VulkanDevice& Device);
		~VulkanImage();

		void CreateImage(uint32_t Width, uint32_t Height, VkFormat Format, VkImageTiling ImageTiling, VkImageUsageFlags ImageUsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags);
		void Destroy();

		void TransitionImageLayout(VulkanCommandBuffer& CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout);

		inline const VkImage GetHandle() const { return Image; }

	private:
		VulkanDevice& Device;

		VkImage Image;
		VkDeviceMemory DeviceMemory;
		VkFormat Format;
	};

}
