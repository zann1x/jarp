#pragma once

#include <volk.h>

#include "VulkanCommandBuffer.h"
#include "VulkanRendererAPI.h"

namespace jarp {

	class VulkanBuffer
	{
	public:
		VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
		~VulkanBuffer();

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VkMemoryPropertyFlags memoryProperties, VkDeviceMemory& deviceMemory);
		void CreateBuffer(VkMemoryPropertyFlags memoryProperties);
		void Destroy();

		inline const VkBuffer& GetHandle() const { return m_Buffer; }
		inline const VkDeviceMemory& GetMemoryHandle() const { return m_BufferMemory; }

		template <typename T>
		void UploadBuffer(VulkanCommandBuffer& commandBuffer, const std::vector<T>& data)
		{
			VulkanBuffer stagingBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
			stagingBuffer.CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* rawData;
			vkMapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), stagingBuffer.GetMemoryHandle(), 0, m_Size, 0, &rawData);
			memcpy(rawData, data.data(), static_cast<size_t>(m_Size));
			vkUnmapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), stagingBuffer.GetMemoryHandle());

			commandBuffer.CopyBuffer(stagingBuffer.GetHandle(), m_Buffer, m_Size);
		}

	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;

		VkDeviceSize m_Size;
		VkBufferUsageFlags m_Usage;
	};

}
