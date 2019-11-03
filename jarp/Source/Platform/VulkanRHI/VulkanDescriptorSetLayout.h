#pragma once

#include <volk.h>

namespace jarp {

	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout();
		~VulkanDescriptorSetLayout();

		void CreateDescriptorSetLayout();
		void Destroy();

		void AddLayout(uint32_t Binding, VkDescriptorType DescriptorType, VkShaderStageFlags StageFlags);

		inline const VkDescriptorSetLayout& GetHandle() const { return DescriptorSetLayout; }

	private:
		std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
		VkDescriptorSetLayout DescriptorSetLayout;
	};

}
