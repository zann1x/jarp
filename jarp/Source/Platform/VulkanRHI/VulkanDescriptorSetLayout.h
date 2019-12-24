#pragma once

#include <volk.h>

#include <vector>

namespace jarp {

	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout();
		~VulkanDescriptorSetLayout();

		void CreateDescriptorSetLayout();
		void Destroy();

		void AddLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);

		inline const VkDescriptorSetLayout& GetHandle() const { return m_DescriptorSetLayout; }

	private:
		std::vector<VkDescriptorSetLayoutBinding> m_DescriptorSetLayoutBindings;
		VkDescriptorSetLayout m_DescriptorSetLayout;
	};

}
