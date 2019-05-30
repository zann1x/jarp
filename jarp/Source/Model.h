#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <array>
#include <vector>

class VulkanCommandBuffer;
class VulkanDevice;
class VulkanShader;

struct SVertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TextureCoordinate;
	glm::vec3 Normal;

	bool operator==(const SVertex& Other) const
	{
		return Position == Other.Position && Color == Other.Color && TextureCoordinate == Other.TextureCoordinate && Normal == Other.Normal;
	}
};

namespace std {
	template<>
	struct hash<SVertex>
	{
		size_t operator()(SVertex const& Vertex) const
		{
			size_t h1 = hash<glm::vec3>()(Vertex.Position);
			size_t h2 = hash<glm::vec3>()(Vertex.Color);
			size_t h3 = hash<glm::vec2>()(Vertex.TextureCoordinate);
			size_t h4 = hash<glm::vec3>()(Vertex.Normal);

			return (((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) ^ (h4 >> 1);
			//return ((((h1 ^ (h2 << 1)) >> 1) ^ h3) << 1) ^ h4;
		}
	};
}

class Model
{
public:
	Model(VulkanDevice& Device);
	~Model();

	inline const VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo() const { return PipelineVertexInputStateCreateInfo; };
	inline const std::vector<SVertex>& GetVertices() const { return Vertices; }
	inline const std::vector<uint32_t>& GetIndices() const { return Indices; }
	inline const VkDeviceSize GetVerticesDeviceSize() const { return sizeof(Vertices[0]) * Vertices.size(); }
	inline const VkDeviceSize GetIndicesDeviceSize() const { return sizeof(Indices[0]) * Indices.size(); }

	void Load(VulkanCommandBuffer& CommandBuffer, const std::string& ObjectFile);

private:
	VulkanDevice& Device;

	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::array<VkVertexInputAttributeDescription, 4> VertexInputAttributeDescriptions;
	VkVertexInputBindingDescription VertexInputBindingDescription;
	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo;
};
