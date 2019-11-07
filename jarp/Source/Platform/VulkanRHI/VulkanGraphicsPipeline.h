#pragma once

#include <volk.h>

#include "jarp/Renderer/Pipeline.h"

namespace jarp {

	class VulkanRenderPass;
	class VulkanShader;

	class VulkanGraphicsPipeline : public Pipeline
	{
	public:
		VulkanGraphicsPipeline(VulkanRenderPass& renderPass, VulkanShader& shader);
		virtual ~VulkanGraphicsPipeline();

		void CreateGraphicsPipeline(const VkPipelineVertexInputStateCreateInfo& pipelineVertexInputStateCreateInfo, const VkExtent2D swapchainExtent);
		void Destroy();

		inline const VkPipeline& GetHandle() const { return m_Pipeline; }
		inline const VkPipelineLayout& GetLayoutHandle() const { return m_PipelineLayout; }

	private:
		VulkanRenderPass& m_RenderPass;
		VulkanShader& m_Shader;

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;

		std::vector<VkDynamicState> m_DynamicStates;
	};

}
