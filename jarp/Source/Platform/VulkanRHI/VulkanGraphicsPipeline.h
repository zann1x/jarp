#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanRenderPass;
	class VulkanShader;

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(VulkanRenderPass& OutRenderPass, VulkanShader& OutShader);
		~VulkanGraphicsPipeline();

		void CreateGraphicsPipeline(const VkPipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo, const VkExtent2D SwapchainExtent);
		void Destroy();

		inline const VkPipeline& GetHandle() const { return Pipeline; }
		inline const VkPipelineLayout& GetLayoutHandle() const { return PipelineLayout; }

	private:
		VulkanRenderPass& RenderPass;
		VulkanShader& Shader;

		VkPipelineLayout PipelineLayout;
		VkPipeline Pipeline;

		std::vector<VkDynamicState> DynamicStates;
	};

}
