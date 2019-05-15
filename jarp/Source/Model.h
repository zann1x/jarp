#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>

class VulkanShader;

struct SVertex
{
	glm::vec3 Position;
	glm::vec3 Color;

	bool operator==(const SVertex& Other) const
	{
		return Position == Other.Position && Color == Other.Color;
	}
};

namespace std {
	template<>
	struct hash<SVertex>
	{
		size_t operator()(SVertex const& Vertex) const
		{
			return ((hash<glm::vec3>()(Vertex.Position) ^
				(hash<glm::vec3>()(Vertex.Color) << 1)));
		}
	};
}

class Model
{
public:
	Model(VulkanShader& Shader);
	~Model();

	inline const VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo() const { return PipelineVertexInputStateCreateInfo; };
	inline const std::vector<SVertex>& GetVertices() const { return Vertices; }
	inline const std::vector<uint32_t>& GetIndices() const { return Indices; }
	inline const VkDeviceSize GetVerticesDeviceSize() const { return sizeof(Vertices[0]) * Vertices.size(); }
	inline const VkDeviceSize GetIndicesDeviceSize() const { return sizeof(Indices[0]) * Indices.size(); }

	void LoadModel(const std::string& FileName);

private:
	VulkanShader& Shader;

	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescriptions;
	VkVertexInputBindingDescription VertexInputBindingDescription;
	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo;
};
