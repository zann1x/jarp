#include "jarppch.h"
#include "VulkanBuffer.h"

#include "Platform/VulkanRHI/CommandBufferPool.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

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

	void VulkanBuffer::UploadBuffer(VulkanCommandBuffer& commandBuffer, const void* data)
	{
		VulkanBuffer stagingBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		stagingBuffer.CreateBuffer(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* rawData;
		vkMapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), stagingBuffer.GetMemoryHandle(), 0, m_Size, 0, &rawData);
		memcpy(rawData, data, static_cast<size_t>(m_Size));
		vkUnmapMemory(VulkanRendererAPI::s_Device->GetInstanceHandle(), stagingBuffer.GetMemoryHandle());

		commandBuffer.CopyBuffer(stagingBuffer.GetHandle(), m_Buffer, m_Size);
	}

	///////////////////////////////////////////////////////////////////
	// Vertex Buffer //////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<SVertex>& vertices, uint32_t size)
	{
		VkDeviceSize deviceSize = sizeof(vertices[0]) * size;
		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		m_Buffer = std::make_shared<VulkanBuffer>(deviceSize, usageFlags);
		m_Buffer->CreateBuffer(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_Buffer->UploadBuffer(*CommandBufferPool::Get()->GetFreeBuffer(), vertices.data());
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
	}

	void VulkanVertexBuffer::Destroy()
	{
		m_Buffer->Destroy();
	}

	void VulkanVertexBuffer::Bind(const std::shared_ptr<CommandBuffer> commandBuffer)
	{
		VkBuffer vertexBufferHandles[] = { m_Buffer->GetHandle() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetHandle(), 0, 1, vertexBufferHandles, offsets);
	}

	///////////////////////////////////////////////////////////////////
	// Index Buffer ///////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(const std::vector<uint32_t>& indices, uint32_t size)
	{
		VkDeviceSize deviceSize = sizeof(indices[0]) * size;
		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		m_Buffer = std::make_shared<VulkanBuffer>(deviceSize, usageFlags);
		m_Buffer->CreateBuffer(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_Buffer->UploadBuffer(*CommandBufferPool::Get()->GetFreeBuffer(), indices.data());
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
	}

	void VulkanIndexBuffer::Destroy()
	{
		m_Buffer->Destroy();
	}

	void VulkanIndexBuffer::Bind(const std::shared_ptr<CommandBuffer> commandBuffer)
	{
		vkCmdBindIndexBuffer(std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetHandle(), m_Buffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
	}

}
