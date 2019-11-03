#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool();
		~VulkanCommandPool();

		void CreateCommandPool(const VkCommandPoolCreateFlags Flags = 0);
		void Destroy();

		inline VkCommandPool GetHandle() { return CommandPool; };

	private:
		VkCommandPool CommandPool;
	};

}
