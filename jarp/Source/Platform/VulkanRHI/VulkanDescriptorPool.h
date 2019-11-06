#pragma once

#include <volk.h>

namespace jarp {

	class VulkanSwapchain;

	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VulkanSwapchain& swapchain);
		~VulkanDescriptorPool();

		void CreateDescriptorPool();
		void Destroy();

		inline const VkDescriptorPool& GetHandle() const { return m_DescriptorPool; }

	private:
		VulkanSwapchain& m_Swapchain;

		VkDescriptorPool m_DescriptorPool;
	};

}
