#include "jarppch.h"
#include "VulkanGraphicsPipeline.h"

#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanSwapchain.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice& OutDevice, VulkanRenderPass& OutRenderPass, VulkanShader& OutShader)
		: Device(OutDevice), RenderPass(OutRenderPass), Shader(OutShader)
	{
		DynamicStates = { };
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
	}

	void VulkanGraphicsPipeline::CreateGraphicsPipeline(const VkPipelineVertexInputStateCreateInfo& PipelineVertexInputStateCreateInfo, const VkExtent2D SwapchainExtent)
	{
		VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo = {};
		PipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		PipelineInputAssemblyStateCreateInfo.pNext = nullptr;
		PipelineInputAssemblyStateCreateInfo.flags = 0;
		PipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		PipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilCreateInfo = {};
		PipelineDepthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		PipelineDepthStencilCreateInfo.pNext = nullptr;
		PipelineDepthStencilCreateInfo.flags = 0;
		PipelineDepthStencilCreateInfo.depthTestEnable = VK_TRUE;
		PipelineDepthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		PipelineDepthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		PipelineDepthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		PipelineDepthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		PipelineDepthStencilCreateInfo.front = {};
		PipelineDepthStencilCreateInfo.back = {};
		PipelineDepthStencilCreateInfo.minDepthBounds = 0.0f;
		PipelineDepthStencilCreateInfo.maxDepthBounds = 1.0f;

		VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo = {};
		PipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		PipelineRasterizationStateCreateInfo.pNext = nullptr;
		PipelineRasterizationStateCreateInfo.flags = 0;
		PipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		PipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		PipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		PipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		PipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		PipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		PipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		PipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		PipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		PipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo = {};
		PipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		PipelineMultisampleStateCreateInfo.pNext = nullptr;
		PipelineMultisampleStateCreateInfo.flags = 0;
		PipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		PipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		PipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
		PipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
		PipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		PipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState = {};
		PipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
		PipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		PipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		PipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		PipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		PipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		PipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		PipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo = {};
		PipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		PipelineColorBlendStateCreateInfo.pNext = nullptr;
		PipelineColorBlendStateCreateInfo.flags = 0;
		PipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		PipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
		PipelineColorBlendStateCreateInfo.attachmentCount = 1;
		PipelineColorBlendStateCreateInfo.pAttachments = &PipelineColorBlendAttachmentState;
		PipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		PipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		PipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		PipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkViewport Viewport = {};
		Viewport.x = 0.0f;
		Viewport.y = static_cast<float>(SwapchainExtent.height);
		Viewport.width = static_cast<float>(SwapchainExtent.width);
		Viewport.height = -static_cast<float>(SwapchainExtent.height);
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;

		VkRect2D Scissor = {};
		Scissor.offset = { 0, 0 };
		Scissor.extent = SwapchainExtent;

		VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo = {};
		PipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		PipelineViewportStateCreateInfo.pNext = nullptr;
		PipelineViewportStateCreateInfo.flags = 0;
		PipelineViewportStateCreateInfo.viewportCount = 1;
		PipelineViewportStateCreateInfo.pViewports = &Viewport;
		PipelineViewportStateCreateInfo.scissorCount = 1;
		PipelineViewportStateCreateInfo.pScissors = &Scissor;

		VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo = {};
		PipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		PipelineDynamicStateCreateInfo.pNext = nullptr;
		PipelineDynamicStateCreateInfo.flags = 0;
		PipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
		PipelineDynamicStateCreateInfo.pDynamicStates = DynamicStates.data();

		VkDescriptorSetLayout DescriptorSetLayouts[] = { Shader.GetSetLayoutHandles().data()->GetHandle() };

		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
		PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutCreateInfo.pNext = nullptr;
		PipelineLayoutCreateInfo.flags = 0;
		PipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(Shader.GetSetLayoutHandles().size());
		PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts;
		PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		VK_ASSERT(vkCreatePipelineLayout(Device.GetInstanceHandle(), &PipelineLayoutCreateInfo, nullptr, &PipelineLayout));

		VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
		GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		GraphicsPipelineCreateInfo.pNext = nullptr;
		GraphicsPipelineCreateInfo.flags = 0;
		GraphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(Shader.GetShaderStageCreateInfos().size());
		GraphicsPipelineCreateInfo.pStages = Shader.GetShaderStageCreateInfos().data();
		GraphicsPipelineCreateInfo.pVertexInputState = &PipelineVertexInputStateCreateInfo;
		GraphicsPipelineCreateInfo.pInputAssemblyState = &PipelineInputAssemblyStateCreateInfo;
		GraphicsPipelineCreateInfo.pTessellationState = nullptr;
		GraphicsPipelineCreateInfo.pViewportState = &PipelineViewportStateCreateInfo;
		GraphicsPipelineCreateInfo.pRasterizationState = &PipelineRasterizationStateCreateInfo;
		GraphicsPipelineCreateInfo.pMultisampleState = &PipelineMultisampleStateCreateInfo;
		GraphicsPipelineCreateInfo.pDepthStencilState = &PipelineDepthStencilCreateInfo;
		GraphicsPipelineCreateInfo.pColorBlendState = &PipelineColorBlendStateCreateInfo;
		GraphicsPipelineCreateInfo.pDynamicState = &PipelineDynamicStateCreateInfo;
		GraphicsPipelineCreateInfo.layout = PipelineLayout;
		GraphicsPipelineCreateInfo.renderPass = RenderPass.GetHandle();
		GraphicsPipelineCreateInfo.subpass = 0;
		GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		GraphicsPipelineCreateInfo.basePipelineIndex = -1;

		VK_ASSERT(vkCreateGraphicsPipelines(Device.GetInstanceHandle(), VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &Pipeline));
	}

	void VulkanGraphicsPipeline::Destroy()
	{
		vkDestroyPipeline(Device.GetInstanceHandle(), Pipeline, nullptr);
		vkDestroyPipelineLayout(Device.GetInstanceHandle(), PipelineLayout, nullptr);
	}

}
