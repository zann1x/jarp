#pragma once

#include <volk.h>

#include "VulkanCommandBuffer.h"
#include "VulkanRendererAPI.h"

#include "jarp/Renderer/IndexBuffer.h"
#include "jarp/Renderer/VertexBuffer.h"

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

		void UploadBuffer(VulkanCommandBuffer& commandBuffer, const void* data);

	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;

		VkDeviceSize m_Size;
		VkBufferUsageFlags m_Usage;
	};

	///////////////////////////////////////////////////////////////////
	// Vertex Buffer //////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const std::vector<SVertex>& vertices, uint32_t size);
		virtual ~VulkanVertexBuffer();

		void Destroy();

		virtual void Bind(const std::shared_ptr<CommandBuffer> commandBuffer) override;

		inline const VkBuffer& GetHandle() const { return m_Buffer->GetHandle(); }

	private:
		std::shared_ptr<VulkanBuffer> m_Buffer;
	};

	///////////////////////////////////////////////////////////////////
	// Index Buffer ///////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const std::vector<uint32_t>& indices, uint32_t size);
		virtual ~VulkanIndexBuffer();

		void Destroy();

		virtual void Bind(const std::shared_ptr<CommandBuffer> commandBuffer) override;

		inline const VkBuffer& GetHandle() const { return m_Buffer->GetHandle(); }

	private:
		std::shared_ptr<VulkanBuffer> m_Buffer;
	};

}
