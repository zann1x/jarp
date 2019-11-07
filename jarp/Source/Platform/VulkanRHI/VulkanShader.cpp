#include "jarppch.h"
#include "VulkanShader.h"

#include "jarp/Utils.hpp"

#include "Platform/VulkanRHI/VulkanCommandBuffer.h"
#include "Platform/VulkanRHI/VulkanDescriptorSetLayout.h"
#include "Platform/VulkanRHI/VulkanDevice.h"
#include "Platform/VulkanRHI/VulkanGraphicsPipeline.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"
#include "Platform/VulkanRHI/VulkanSwapchain.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

namespace jarp {

	VulkanShader::VulkanShader()
	{
		m_DescriptorPool.CreateDescriptorPool();
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::AddModule(Type shaderType, const char* filename)
	{
		m_ShaderModuleInfos[shaderType] = filename;

		VkShaderStageFlagBits shaderStageFlag;
		switch (shaderType)
		{
			case Type::Fragment:	shaderStageFlag = VK_SHADER_STAGE_FRAGMENT_BIT; break;
			case Type::Vertex:		shaderStageFlag = VK_SHADER_STAGE_VERTEX_BIT; break;
		}

		// TODO: cross-compile shader code -> shader file should be a glsl shader in the future

		auto shaderCode = Utils::ReadFile(filename);
		if (shaderCode.size() % 4 != 0)
			throw std::runtime_error("Size of the SPIR-V shader code must be a multiple of 4!");

		CreateShaderModule(shaderStageFlag, shaderCode);
	}

	void VulkanShader::AddLayout(const ShaderLayout& layout)
	{
		VulkanDescriptorSetLayout* descriptorSetLayout = new VulkanDescriptorSetLayout();
		for (ShaderLayoutElement element : layout.m_Elements)
		{
			VkDescriptorType descriptorType;
			VkShaderStageFlags shaderStageFlag;
			switch (element.LayoutType)
			{
				case ShaderLayout::Type::Sampler:	descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
				case ShaderLayout::Type::Uniform:	descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
			}
			switch (element.ShaderType)
			{
				case Type::Fragment:	shaderStageFlag = VK_SHADER_STAGE_FRAGMENT_BIT; break;
				case Type::Vertex:		shaderStageFlag = VK_SHADER_STAGE_VERTEX_BIT; break;
			}

			descriptorSetLayout->AddLayout(element.Binding, descriptorType, shaderStageFlag);
		}
		descriptorSetLayout->CreateDescriptorSetLayout();
		m_DescriptorSetLayouts.push_back(descriptorSetLayout);
	}

	void VulkanShader::Bind(uint32_t currentRenderIndex, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<Pipeline>& pipeline)
	{
		vkCmdBindDescriptorSets(std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, std::dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline)->GetLayoutHandle(), 0, 1, &m_DescriptorSet.At(currentRenderIndex), 0, nullptr);
	}

	void VulkanShader::CreateShaderModule(const VkShaderStageFlagBits shaderStage, const std::vector<char>& shaderCode)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = shaderCode.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule;
		VK_ASSERT(vkCreateShaderModule(VulkanRendererAPI::s_Device->GetInstanceHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule));
		m_ShaderModules[shaderStage] = shaderModule;

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
		for (auto layout : m_DescriptorSetLayouts)
		{
			layout->Destroy();
			delete layout;
		}
		m_DescriptorPool.Destroy();
	}

	void VulkanShader::UploadShaderData(std::vector<VkBuffer>& buffer, const VkSampler sampler, const VkImageView imageView)
	{
		m_DescriptorSet.UpdateDescriptorSets(GetDescriptorSetLayouts(), m_DescriptorPool, VulkanRendererAPI::s_Swapchain->GetImages().size(), sizeof(SUniformBufferObject), buffer, sampler, imageView);
	}

}
