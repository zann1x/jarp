#pragma once

#include <volk.h>

#include "VulkanDescriptorSetLayout.h"

#include <map>
#include <string>
#include <vector>

namespace jarp {

	class VulkanShader
	{
	public:
		VulkanShader();
		~VulkanShader();

		void CreateShaderModule(const VkShaderStageFlagBits shaderStage, const std::string& filename);
		void Destroy();

		inline const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageCreateInfos() const { return m_ShaderStageCreateInfos; }
		inline const std::vector<VulkanDescriptorSetLayout>& GetSetLayoutHandles() const { return m_DescriptorSetLayouts; }

		void AddDescriptorSetLayout(const VulkanDescriptorSetLayout& descriptorSetLayout);

	private:
		std::vector<VulkanDescriptorSetLayout> m_DescriptorSetLayouts;

		std::map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;
	};

}
