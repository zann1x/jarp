#include "Model.h"
#include "VulkanRHI/VulkanShader.h"

Model::Model(VulkanShader& Shader)
	: Shader(Shader)
{
	Vertices = {
		{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } }, // 0
		{ {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } }, // 1
		{ {  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }, // 2
		{ { -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } }  // 3
	};
	
	Indices = {
		0, 1, 2,
		2, 3, 0
	};

	VertexInputBindingDescription = {};
	VertexInputBindingDescription.binding = 0;
	VertexInputBindingDescription.stride = sizeof(SVertex);
	VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VertexInputAttributeDescriptions.resize(2);
	VertexInputAttributeDescriptions[0] = {};
	VertexInputAttributeDescriptions[0].location = 0;
	VertexInputAttributeDescriptions[0].binding = 0;
	VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	VertexInputAttributeDescriptions[0].offset = offsetof(SVertex, Position);
	VertexInputAttributeDescriptions[1] = {};
	VertexInputAttributeDescriptions[1].location = 1;
	VertexInputAttributeDescriptions[1].binding = 0;
	VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	VertexInputAttributeDescriptions[1].offset = offsetof(SVertex, Color);

	PipelineVertexInputStateCreateInfo = {};
	PipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	PipelineVertexInputStateCreateInfo.pNext = nullptr;
	PipelineVertexInputStateCreateInfo.flags = 0;
	PipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	PipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &VertexInputBindingDescription;
	PipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexInputAttributeDescriptions.size());
	PipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = VertexInputAttributeDescriptions.data();
}

Model::~Model()
{
}
