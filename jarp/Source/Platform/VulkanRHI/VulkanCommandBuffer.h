#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandPool;

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(VulkanCommandPool& commandPool);
		~VulkanCommandBuffer();

		void CreateCommandBuffer();
		void Destroy();

		inline const VkCommandBuffer& GetHandle() const { return m_CommandBuffer; }

		void BeginOneTimeSubmitCommand();
		void EndOneTimeSubmitCommand();

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	private:
		VulkanCommandPool& m_CommandPool;

		VkCommandBuffer m_CommandBuffer;
	};

}
