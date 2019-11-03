#pragma once

#include <volk.h>

namespace jarp {

	class VulkanSwapchain;

	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VulkanSwapchain& OutSwapchain);
		~VulkanDescriptorPool();

		void CreateDescriptorPool();
		void Destroy();

		inline const VkDescriptorPool& GetHandle() const { return DescriptorPool; }

	private:
		VulkanSwapchain& Swapchain;

		VkDescriptorPool DescriptorPool;
	};

}
