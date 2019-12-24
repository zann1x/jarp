#pragma once

#include <volk.h>

#include <vector>

namespace jarp {

	class VulkanDescriptorSetLayout;
	class VulkanDescriptorPool;

	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet();
		~VulkanDescriptorSet();

		void CreateDescriptorSets(const VulkanDescriptorSetLayout& descriptorSetLayout, const VulkanDescriptorPool& descriptorPool, const size_t amount, const VkDeviceSize size, const std::vector<VkBuffer>& buffers, VkSampler sampler, VkImageView imageView);

		inline const VkDescriptorSet& At(size_t index) const { return m_DescriptorSets[index]; }

	private:
		std::vector<VkDescriptorSet> m_DescriptorSets;
	};

}
