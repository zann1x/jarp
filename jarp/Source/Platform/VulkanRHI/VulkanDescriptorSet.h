#pragma once

#include <volk.h>

namespace jarp {

	class VulkanDescriptorSetLayout;
	class VulkanDescriptorPool;

	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet();
		~VulkanDescriptorSet();

		void CreateDescriptorSets(const VulkanDescriptorSetLayout& DescriptorSetLayout, const VulkanDescriptorPool& DescriptorPool, const size_t Amount, const VkDeviceSize Size, const std::vector<VkBuffer>& Buffers, VkSampler Sampler, VkImageView ImageView);

		inline const VkDescriptorSet& At(size_t Index) const { return DescriptorSets[Index]; }

	private:
		std::vector<VkDescriptorSet> DescriptorSets;
	};

}
