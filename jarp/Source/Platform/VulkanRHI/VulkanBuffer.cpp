#include "VulkanBuffer.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanBuffer::VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
		: m_Size(size), m_Usage(usage)
	{
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (m_Buffer != VK_NULL_HANDLE)
			Destroy();
	}

	void VulkanBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VkMemoryPropertyFlags memoryProperties, VkDeviceMemory& deviceMemory)
	{
		// Create the buffer
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0; // needed if the buffer is shared between multiple queues
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		VK_ASSERT(vkCreateBuffer(VulkanRendererAPI::s_Device->GetInstanceHandle(), &bufferCreateInfo, nullptr, &buffer));

		// Allocate the buffer's memory
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(VulkanRendererAPI::s_Device->GetInstanceHandle(), buffer, &memoryRequirements);
		uint32_t memoryTypeIndex = VulkanRendererAPI::s_Device->GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryProperties);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		VK_ASSERT(vkAllocateMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), &memoryAllocateInfo, nullptr, &deviceMemory));
	}

	void VulkanBuffer::CreateBuffer(VkMemoryPropertyFlags memoryProperties)
	{
		// Create the buffer
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = m_Size;
		bufferCreateInfo.usage = m_Usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0; // needed if the buffer is shared between multiple queues
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		VK_ASSERT(vkCreateBuffer(VulkanRendererAPI::s_Device->GetInstanceHandle(), &bufferCreateInfo, nullptr, &m_Buffer));

		// Allocate the buffer's memory
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Buffer, &memoryRequirements);
		uint32_t memoryTypeIndex = VulkanRendererAPI::s_Device->GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryProperties);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		VK_ASSERT(vkAllocateMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), &memoryAllocateInfo, nullptr, &m_BufferMemory));

		VK_ASSERT(vkBindBufferMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Buffer, m_BufferMemory, 0));
	}

	void VulkanBuffer::Destroy()
	{
		vkFreeMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_BufferMemory, nullptr);
		vkDestroyBuffer(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Buffer, nullptr);
		m_Buffer = VK_NULL_HANDLE;
	}

}
