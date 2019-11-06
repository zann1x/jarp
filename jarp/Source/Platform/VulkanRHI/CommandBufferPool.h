#pragma once

#include "Platform/VulkanRHI/VulkanCommandBuffer.h"
#include "Platform/VulkanRHI/VulkanCommandPool.h"

#include <memory>
#include <vector>

namespace jarp {

	class CommandBufferPool
	{
	public:
		inline static CommandBufferPool* Get() { return s_Instance; }

		void Destroy()
		{
			m_TransientCommandPool->Destroy();
		}

		std::shared_ptr<VulkanCommandBuffer>& GetFreeBuffer()
		{
			// TODO: remove this temporary hack

			if (m_CommandBuffers.size() == 0)
			{
				m_TransientCommandPool = std::make_shared<VulkanCommandPool>(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
				m_CommandBuffers.push_back(std::make_shared<VulkanCommandBuffer>(m_TransientCommandPool));
			}
			return m_CommandBuffers[0];
		}

	private:
		static CommandBufferPool* s_Instance;

		std::vector<std::shared_ptr<VulkanCommandBuffer>> m_CommandBuffers;
		std::shared_ptr<VulkanCommandPool> m_TransientCommandPool;
	};

}
