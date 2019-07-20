#pragma once

#include <vulkan/vulkan.h>

#include "VulkanDescriptorSetLayout.h"

class VulkanDevice;

/*
Depends on:
- Device
*/
class VulkanShader
{
public:
	VulkanShader(VulkanDevice& OutDevice);
	~VulkanShader();

	void CreateShaderModule(const VkShaderStageFlagBits ShaderStage, const std::string& Filename);
	void Destroy();

	inline const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageCreateInfos() const { return ShaderStageCreateInfos; }
	inline const std::vector<VulkanDescriptorSetLayout>& GetSetLayoutHandles() const { return DescriptorSetLayouts; }

	void AddDescriptorSetLayout(const VulkanDescriptorSetLayout& DescriptorSetLayout);

private:
	VulkanDevice& Device;
	std::vector<VulkanDescriptorSetLayout> DescriptorSetLayouts;

	std::map< VkShaderStageFlagBits, VkShaderModule> ShaderModules;
	std::vector<VkPipelineShaderStageCreateInfo> ShaderStageCreateInfos;
};
