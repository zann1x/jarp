#pragma once

#include <volk.h>

#include "jarp/Renderer/Shader.h"

#include "Platform/VulkanRHI/VulkanDescriptorPool.h"
#include "Platform/VulkanRHI/VulkanDescriptorSet.h"
#include "Platform/VulkanRHI/VulkanDescriptorSetLayout.h"

namespace jarp {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader();
		~VulkanShader();

		virtual void AddModule(Type shaderType, const char* filename) override;
		virtual void AddLayout(const ShaderLayout& layout) override;
		virtual void Bind(uint32_t currentRenderIndex, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<Pipeline>& pipeline) override;

		void CreateShaderModule(const VkShaderStageFlagBits shaderStage, const std::vector<char>& shaderCode);
		void Destroy();

		void UploadShaderData(std::vector<VkBuffer>& buffer, const VkSampler sampler, const VkImageView imageView);

		inline const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageCreateInfos() const { return m_ShaderStageCreateInfos; }
		inline const std::vector<VulkanDescriptorSetLayout*>& GetDescriptorSetLayouts() const { return m_DescriptorSetLayouts; }

	private:
		VulkanDescriptorPool m_DescriptorPool;
		std::vector<VulkanDescriptorSetLayout*> m_DescriptorSetLayouts;
		VulkanDescriptorSet m_DescriptorSet;
		std::vector<VkWriteDescriptorSet> m_WriteDescriptorSets;

		std::map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;

		std::map<Type, const char*> m_ShaderModuleInfos;
	};

}
