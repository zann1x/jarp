#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanCommandBuffer.h"

class VulkanDevice;

/*
Depends on:
- Device
*/
class VulkanBuffer
{
public:
	VulkanBuffer(VulkanDevice& Device);
	~VulkanBuffer();

	void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkBuffer& Buffer, VkMemoryPropertyFlags MemoryProperties, VkDeviceMemory& DeviceMemory);
	void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags MemoryProperties);
	void Destroy();

	inline const VkBuffer& GetHandle() const { return Buffer; }
	inline const VkDeviceMemory& GetMemoryHandle() const { return BufferMemory; }

	template <typename T>
	void UploadBuffer(class VulkanCommandBuffer& CommandBuffer, const std::vector<T>& Data)
	{
		VulkanBuffer StagingBuffer(Device);
		StagingBuffer.CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* RawData;
		vkMapMemory(Device.GetInstanceHandle(), StagingBuffer.GetMemoryHandle(), 0, Size, 0, &RawData);
		memcpy(RawData, Data.data(), static_cast<size_t>(Size));
		vkUnmapMemory(Device.GetInstanceHandle(), StagingBuffer.GetMemoryHandle());

		Bind();
		Bind(StagingBuffer.GetHandle(), StagingBuffer.GetMemoryHandle());
		CommandBuffer.CopyBuffer(StagingBuffer.GetHandle(), Buffer, Size);
	}

	void Bind();
	void Bind(const VkBuffer& Buffer, const VkDeviceMemory& DeviceMemory);

private:
	VulkanDevice& Device;

	VkBuffer Buffer;
	VkDeviceMemory BufferMemory;

	VkDeviceSize Size;
	VkBufferUsageFlags Usage;
};

