#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

VulkanBuffer::VulkanBuffer(VulkanDevice& Device)
	: Device(Device), Size(0), Usage(0)
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
	BufferCreateInfo.queueFamilyIndexCount = 0;
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

void VulkanBuffer::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags MemoryProperties)
{
	this->Size = Size;
	this->Usage = Usage;

	// Create the buffer
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = nullptr;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = Usage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0;
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
}

void VulkanBuffer::Destroy()
{
	vkFreeMemory(Device.GetInstanceHandle(), BufferMemory, nullptr);
	vkDestroyBuffer(Device.GetInstanceHandle(), Buffer, nullptr);
	Buffer = VK_NULL_HANDLE;
}

void VulkanBuffer::Bind()
{
	VK_ASSERT(vkBindBufferMemory(Device.GetInstanceHandle(), this->Buffer, this->BufferMemory, 0));
}

void VulkanBuffer::Bind(const VkBuffer& Buffer, const VkDeviceMemory& DeviceMemory)
{
	VK_ASSERT(vkBindBufferMemory(Device.GetInstanceHandle(), Buffer, DeviceMemory, 0));
}
