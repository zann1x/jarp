#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool();
		~VulkanCommandPool();

		void CreateCommandPool(const VkCommandPoolCreateFlags flags = 0);
		void Destroy();

		inline VkCommandPool GetHandle() const { return commandPool; };

	private:
		VkCommandPool commandPool;
	};

}
