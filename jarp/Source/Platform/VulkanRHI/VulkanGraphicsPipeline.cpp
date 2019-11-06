#include "jarppch.h"
#include "VulkanGraphicsPipeline.h"

#include "VulkanRendererAPI.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanSwapchain.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanRenderPass& renderPass, VulkanShader& shader)
		: m_RenderPass(renderPass), m_Shader(shader)
	{
		m_DynamicStates = { };
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
	}

	void VulkanGraphicsPipeline::CreateGraphicsPipeline(const VkPipelineVertexInputStateCreateInfo& pipelineVertexInputStateCreateInfo, const VkExtent2D swapchainExtent)
	{
		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
		pipelineInputAssemblyStateCreateInfo.flags = 0;
		pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilCreateInfo = {};
		pipelineDepthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilCreateInfo.pNext = nullptr;
		pipelineDepthStencilCreateInfo.flags = 0;
		pipelineDepthStencilCreateInfo.depthTestEnable = VK_TRUE;
		pipelineDepthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		pipelineDepthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		pipelineDepthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		pipelineDepthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		pipelineDepthStencilCreateInfo.front = {};
		pipelineDepthStencilCreateInfo.back = {};
		pipelineDepthStencilCreateInfo.minDepthBounds = 0.0f;
		pipelineDepthStencilCreateInfo.maxDepthBounds = 1.0f;

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.pNext = nullptr;
		pipelineRasterizationStateCreateInfo.flags = 0;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.pNext = nullptr;
		pipelineMultisampleStateCreateInfo.flags = 0;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
		pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
		pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
		pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
		pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.pNext = nullptr;
		pipelineColorBlendStateCreateInfo.flags = 0;
		pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
		pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
		pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(swapchainExtent.height);
		viewport.width = static_cast<float>(swapchainExtent.width);
		viewport.height = -static_cast<float>(swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.pNext = nullptr;
		pipelineViewportStateCreateInfo.flags = 0;
		pipelineViewportStateCreateInfo.viewportCount = 1;
		pipelineViewportStateCreateInfo.pViewports = &viewport;
		pipelineViewportStateCreateInfo.scissorCount = 1;
		pipelineViewportStateCreateInfo.pScissors = &scissor;

		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.pNext = nullptr;
		pipelineDynamicStateCreateInfo.flags = 0;
		pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
		pipelineDynamicStateCreateInfo.pDynamicStates = m_DynamicStates.data();

		VkDescriptorSetLayout descriptorSetLayouts[] = { m_Shader.GetSetLayoutHandles().data()->GetHandle() };

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(m_Shader.GetSetLayoutHandles().size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		VK_ASSERT(vkCreatePipelineLayout(VulkanRendererAPI::s_Device->GetInstanceHandle(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(m_Shader.GetShaderStageCreateInfos().size());
		graphicsPipelineCreateInfo.pStages = m_Shader.GetShaderStageCreateInfos().data();
		graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = m_PipelineLayout;
		graphicsPipelineCreateInfo.renderPass = m_RenderPass.GetHandle();
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		VK_ASSERT(vkCreateGraphicsPipelines(VulkanRendererAPI::s_Device->GetInstanceHandle(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_Pipeline));
	}

	void VulkanGraphicsPipeline::Destroy()
	{
		vkDestroyPipeline(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_PipelineLayout, nullptr);
	}

}
