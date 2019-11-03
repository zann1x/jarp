#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandBuffer;

	class VulkanImage
	{
	public:
		VulkanImage();
		~VulkanImage();

		void CreateImage(uint32_t Width, uint32_t Height, VkFormat Format, VkImageTiling ImageTiling, VkImageUsageFlags ImageUsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags);
		void Destroy();

		void TransitionImageLayout(VulkanCommandBuffer& CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout);

		inline const VkImage GetHandle() const { return Image; }

	private:
		VkImage Image;
		VkDeviceMemory DeviceMemory;
		VkFormat Format;
	};

}
