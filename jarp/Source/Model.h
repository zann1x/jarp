#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <array>
#include <vector>

class Texture;

class VulkanCommandBuffer;
class VulkanDevice;
class VulkanShader;

struct SVertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TextureCoordinate;

	bool operator==(const SVertex& Other) const
	{
		return Position == Other.Position && Color == Other.Color && TextureCoordinate == Other.TextureCoordinate;
	}
};

namespace std {
	template<>
	struct hash<SVertex>
	{
		size_t operator()(SVertex const& Vertex) const
		{
			return ((hash<glm::vec3>()(Vertex.Position) ^
				(hash<glm::vec3>()(Vertex.Color) << 1)) >> 1) ^
				(hash<glm::vec2>()(Vertex.TextureCoordinate) << 1);
		}
	};
}

class Model
{
public:
	Model(VulkanDevice& Device, VulkanShader& Shader);
	~Model();

	inline const VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo() const { return PipelineVertexInputStateCreateInfo; };
	inline const std::vector<SVertex>& GetVertices() const { return Vertices; }
	inline const std::vector<uint32_t>& GetIndices() const { return Indices; }
	inline const VkDeviceSize GetVerticesDeviceSize() const { return sizeof(Vertices[0]) * Vertices.size(); }
	inline const VkDeviceSize GetIndicesDeviceSize() const { return sizeof(Indices[0]) * Indices.size(); }
	inline const Texture& GetTexture() const { return *pTexture; }

	void Load(VulkanCommandBuffer& CommandBuffer, const std::string& ObjectFile, const std::string& TextureFile);

private:
	VulkanDevice& Device;
	VulkanShader& Shader;

	Texture* pTexture;

	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescriptions;
	VkVertexInputBindingDescription VertexInputBindingDescription;
	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo;
};
