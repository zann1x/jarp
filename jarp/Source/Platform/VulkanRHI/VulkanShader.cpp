#include "VulkanShader.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

#include "jarp/Utils.hpp"

namespace jarp {

	VulkanShader::VulkanShader()
	{
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::CreateShaderModule(const VkShaderStageFlagBits shaderStage, const std::string& filename)
	{
		auto shaderCode = Utils::ReadFile(filename);

		if (shaderCode.size() % 4 != 0)
			throw std::runtime_error("SPIR-V shader code must be a multiple of 4!");

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = shaderCode.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule;
		VK_ASSERT(vkCreateShaderModule(VulkanRendererAPI::s_Device->GetInstanceHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule));
		m_ShaderModules.insert({ shaderStage, shaderModule });

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.pNext = nullptr;
		pipelineShaderStageCreateInfo.flags = 0;
		pipelineShaderStageCreateInfo.stage = shaderStage;
		pipelineShaderStageCreateInfo.module = shaderModule;
		pipelineShaderStageCreateInfo.pName = "main";
		pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;

		m_ShaderStageCreateInfos.push_back(pipelineShaderStageCreateInfo);
	}

	void VulkanShader::Destroy()
	{
		m_ShaderStageCreateInfos.clear();
		for (auto shaderModule : m_ShaderModules)
			vkDestroyShaderModule(VulkanRendererAPI::s_Device->GetInstanceHandle(), shaderModule.second, nullptr);
	}

	void VulkanShader::AddDescriptorSetLayout(const VulkanDescriptorSetLayout& DescriptorSetLayout)
	{
		m_DescriptorSetLayouts.push_back(DescriptorSetLayout);
	}

}
