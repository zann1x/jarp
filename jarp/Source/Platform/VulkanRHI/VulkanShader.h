#pragma once

#include <volk.h>

#include "VulkanDescriptorSetLayout.h"

namespace jarp {

	class VulkanShader
	{
	public:
		VulkanShader();
		~VulkanShader();

		void CreateShaderModule(const VkShaderStageFlagBits ShaderStage, const std::string& Filename);
		void Destroy();

		inline const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageCreateInfos() const { return ShaderStageCreateInfos; }
		inline const std::vector<VulkanDescriptorSetLayout>& GetSetLayoutHandles() const { return DescriptorSetLayouts; }

		void AddDescriptorSetLayout(const VulkanDescriptorSetLayout& DescriptorSetLayout);

	private:
		std::vector<VulkanDescriptorSetLayout> DescriptorSetLayouts;

		std::map< VkShaderStageFlagBits, VkShaderModule> ShaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStageCreateInfos;
	};

}
