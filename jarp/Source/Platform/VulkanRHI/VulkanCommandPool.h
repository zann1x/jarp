#pragma once

#include "jarp/Renderer/CommandPool.h"

#include <volk.h>

namespace jarp {

	class VulkanCommandPool : public CommandPool
	{
	public:
		VulkanCommandPool(const VkCommandPoolCreateFlags flags = 0);
		~VulkanCommandPool();

		void CreateCommandPool(const VkCommandPoolCreateFlags flags);
		void Destroy();

		inline VkCommandPool GetHandle() const { return m_commandPool; };

	private:
		VkCommandPool m_commandPool;
	};

}
