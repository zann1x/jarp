#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

/*
Depends on:
- Device
*/
class VulkanDescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout(VulkanDevice& OutDevice);
	~VulkanDescriptorSetLayout();

	void CreateDescriptorSetLayout(uint32_t Binding, VkDescriptorType DescriptorType, VkShaderStageFlags StageFlags);
	void Destroy();

	inline const VkDescriptorSetLayout& GetHandle() const { return DescriptorSetLayout; }

private:
	VulkanDevice& Device;

	VkDescriptorSetLayout DescriptorSetLayout;
};

