#pragma once

#include <volk.h>

#include "VulkanCommandBuffer.h"
#include "VulkanRendererAPI.h"

namespace jarp {

	class VulkanBuffer
	{
	public:
		VulkanBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage);
		~VulkanBuffer();

		void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkBuffer& Buffer, VkMemoryPropertyFlags MemoryProperties, VkDeviceMemory& DeviceMemory);
		void CreateBuffer(VkMemoryPropertyFlags MemoryProperties);
		void Destroy();

		inline const VkBuffer& GetHandle() const { return Buffer; }
		inline const VkDeviceMemory& GetMemoryHandle() const { return BufferMemory; }

		template <typename T>
		void UploadBuffer(VulkanCommandBuffer& CommandBuffer, const std::vector<T>& Data)
		{
			VulkanBuffer StagingBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
			StagingBuffer.CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* RawData;
			vkMapMemory(VulkanRendererAPI::pDevice->GetInstanceHandle(), StagingBuffer.GetMemoryHandle(), 0, Size, 0, &RawData);
			memcpy(RawData, Data.data(), static_cast<size_t>(Size));
			vkUnmapMemory(VulkanRendererAPI::pDevice->GetInstanceHandle(), StagingBuffer.GetMemoryHandle());

			CommandBuffer.CopyBuffer(StagingBuffer.GetHandle(), Buffer, Size);
		}

	private:
		VkBuffer Buffer;
		VkDeviceMemory BufferMemory;

		VkDeviceSize Size;
		VkBufferUsageFlags Usage;
	};

}
