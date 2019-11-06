#pragma once

#include "jarp/Renderer/CommandBuffer.h"
#include "Platform/VulkanRHI/VulkanCommandPool.h"

#include <volk.h>

namespace jarp {

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(const std::shared_ptr<VulkanCommandPool>& commandPool);
		~VulkanCommandBuffer();

		void CreateCommandBuffer();
		void Destroy();

		inline const VkCommandBuffer& GetHandle() const { return m_CommandBuffer; }

		void BeginOneTimeSubmitCommand();
		void EndOneTimeSubmitCommand();

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	private:
		std::shared_ptr<VulkanCommandPool> m_CommandPool;

		VkCommandBuffer m_CommandBuffer;
	};

}
