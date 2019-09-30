#include "jarppch.h"
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

	void VulkanShader::CreateShaderModule(const VkShaderStageFlagBits ShaderStage, const std::string& Filename)
	{
		auto ShaderCode = Utils::ReadFile(Filename);

		if (ShaderCode.size() % 4 != 0)
			throw std::runtime_error("SPIR-V shader code must be a multiple of 4!");

		VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
		ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ShaderModuleCreateInfo.pNext = nullptr;
		ShaderModuleCreateInfo.flags = 0;
		ShaderModuleCreateInfo.codeSize = ShaderCode.size();
		ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(ShaderCode.data());

		VkShaderModule ShaderModule;
		VK_ASSERT(vkCreateShaderModule(VulkanRendererAPI::pDevice->GetInstanceHandle(), &ShaderModuleCreateInfo, nullptr, &ShaderModule));
		ShaderModules.insert({ ShaderStage, ShaderModule });

		VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo = {};
		PipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		PipelineShaderStageCreateInfo.pNext = nullptr;
		PipelineShaderStageCreateInfo.flags = 0;
		PipelineShaderStageCreateInfo.stage = ShaderStage;
		PipelineShaderStageCreateInfo.module = ShaderModule;
		PipelineShaderStageCreateInfo.pName = "main";
		PipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;

		ShaderStageCreateInfos.push_back(PipelineShaderStageCreateInfo);
	}

	void VulkanShader::Destroy()
	{
		ShaderStageCreateInfos.clear();
		for (auto ShaderModule : ShaderModules)
			vkDestroyShaderModule(VulkanRendererAPI::pDevice->GetInstanceHandle(), ShaderModule.second, nullptr);
	}

	void VulkanShader::AddDescriptorSetLayout(const VulkanDescriptorSetLayout& DescriptorSetLayout)
	{
		DescriptorSetLayouts.push_back(DescriptorSetLayout);
	}

}
