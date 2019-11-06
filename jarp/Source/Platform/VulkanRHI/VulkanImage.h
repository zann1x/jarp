#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandBuffer;

	class VulkanImage
	{
	public:
		VulkanImage();
		~VulkanImage();

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags);
		void Destroy();

		void TransitionImageLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

		inline const VkImage GetHandle() const { return m_Image; }

	private:
		VkImage m_Image;
		VkDeviceMemory m_DeviceMemory;
		VkFormat m_Format;
	};

}
