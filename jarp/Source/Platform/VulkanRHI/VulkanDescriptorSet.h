#pragma once

#include <vulkan/vulkan.h>

class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanDevice;

/*
Depends on:
- Device
- SwapchainKHR
- DescriptorPool
- DescriptorSetLayout
*/
class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet(VulkanDevice& OutDevice);
	~VulkanDescriptorSet();

	void CreateDescriptorSets(const VulkanDescriptorSetLayout& DescriptorSetLayout, const VulkanDescriptorPool& DescriptorPool, const size_t Amount, const VkDeviceSize Size, const std::vector<VkBuffer>& Buffers, VkSampler Sampler, VkImageView ImageView);

	inline const VkDescriptorSet& At(size_t Index) const { return DescriptorSets[Index]; }

private:
	VulkanDevice& Device;

	std::vector<VkDescriptorSet> DescriptorSets;
};
