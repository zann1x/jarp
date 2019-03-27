#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

class VulkanShader;

class Model
{
public:
	struct SVertex
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	Model(VulkanShader& Shader);
	~Model();

	inline const VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo() const { return PipelineVertexInputStateCreateInfo; };
	inline const std::vector<SVertex>& GetVertices() const { return Vertices; }
	inline const std::vector<uint32_t>& GetIndices() const { return Indices; }

private:
	VulkanShader& Shader;

	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescriptions;
	VkVertexInputBindingDescription VertexInputBindingDescription;
	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo;
};
