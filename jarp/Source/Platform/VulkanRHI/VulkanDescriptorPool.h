#pragma once

#include <volk.h>

namespace jarp {

	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool();
		~VulkanDescriptorPool();

		void CreateDescriptorPool();
		void Destroy();

		inline const VkDescriptorPool& GetHandle() const { return m_DescriptorPool; }

	private:
		VkDescriptorPool m_DescriptorPool;
	};

}
