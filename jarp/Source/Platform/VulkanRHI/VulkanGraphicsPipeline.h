#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanRenderPass;
class VulkanShader;

class Model;

/*
Depends on:
- Device
- Renderpass
- Shader
*/
class VulkanGraphicsPipeline
{
public:
	VulkanGraphicsPipeline(VulkanDevice& OutDevice, VulkanRenderPass& OutRenderPass, VulkanShader& OutShader);
	~VulkanGraphicsPipeline();

	void CreateGraphicsPipeline(const VkPipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo, const VkExtent2D SwapchainExtent);
	void Destroy();

	inline const VkPipeline& GetHandle() const { return Pipeline; }
	inline const VkPipelineLayout& GetLayoutHandle() const { return PipelineLayout; }

private:
	VulkanDevice& Device;
	VulkanRenderPass& RenderPass;
	VulkanShader& Shader;

	VkPipelineLayout PipelineLayout;
	VkPipeline Pipeline;

	std::vector<VkDynamicState> DynamicStates;
};
