#include "jarppch.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

VulkanBuffer::VulkanBuffer(VulkanDevice& Device, VkDeviceSize Size, VkBufferUsageFlags Usage)
	: Device(Device), Size(Size), Usage(Usage)
{
}

VulkanBuffer::~VulkanBuffer()
{
	if (Buffer != VK_NULL_HANDLE)
		Destroy();
}

void VulkanBuffer::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkBuffer& Buffer, VkMemoryPropertyFlags MemoryProperties, VkDeviceMemory& DeviceMemory)
{
	// Create the buffer
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = nullptr;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = Usage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0; // needed if the buffer is shared between multiple queues
	BufferCreateInfo.pQueueFamilyIndices = nullptr;

	VK_ASSERT(vkCreateBuffer(Device.GetInstanceHandle(), &BufferCreateInfo, nullptr, &Buffer));

	// Allocate the buffer's memory
	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(Device.GetInstanceHandle(), Buffer, &MemoryRequirements);
	uint32_t MemoryTypeIndex = Device.GetMemoryTypeIndex(MemoryRequirements.memoryTypeBits, MemoryProperties);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = nullptr;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = MemoryTypeIndex;

	VK_ASSERT(vkAllocateMemory(Device.GetInstanceHandle(), &MemoryAllocateInfo, nullptr, &DeviceMemory));
}

void VulkanBuffer::CreateBuffer(VkMemoryPropertyFlags MemoryProperties)
{
	// Create the buffer
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = nullptr;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = Usage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0; // needed if the buffer is shared between multiple queues
	BufferCreateInfo.pQueueFamilyIndices = nullptr;

	VK_ASSERT(vkCreateBuffer(Device.GetInstanceHandle(), &BufferCreateInfo, nullptr, &Buffer));

	// Allocate the buffer's memory
	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(Device.GetInstanceHandle(), Buffer, &MemoryRequirements);
	uint32_t MemoryTypeIndex = Device.GetMemoryTypeIndex(MemoryRequirements.memoryTypeBits, MemoryProperties);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = nullptr;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = MemoryTypeIndex;

	VK_ASSERT(vkAllocateMemory(Device.GetInstanceHandle(), &MemoryAllocateInfo, nullptr, &BufferMemory));

	VK_ASSERT(vkBindBufferMemory(Device.GetInstanceHandle(), Buffer, BufferMemory, 0));
}

void VulkanBuffer::Destroy()
{
	vkFreeMemory(Device.GetInstanceHandle(), BufferMemory, nullptr);
	vkDestroyBuffer(Device.GetInstanceHandle(), Buffer, nullptr);
	Buffer = VK_NULL_HANDLE;
}
