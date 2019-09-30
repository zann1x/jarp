#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace jarp {

	enum class Component
	{
		VERTEX_COMPONENT_POSITION = 0x0,
		VERTEX_COMPONENT_NORMAL = 0x1,
		VERTEX_COMPONENT_COLOR = 0x2,
		VERTEX_COMPONENT_UV = 0x3
	};

	struct SVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Color;
		glm::vec2 TextureCoordinate;

		bool operator==(const SVertex& Other) const
		{
			return Position == Other.Position && Normal == Other.Normal && Color == Other.Color && TextureCoordinate == Other.TextureCoordinate;
		}
	};

	class Model
	{
	public:
		Model();
		~Model();

		inline const VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo() const { return PipelineVertexInputStateCreateInfo; };
		inline const std::vector<SVertex>& GetVertices() const { return Vertices; }
		inline const std::vector<uint32_t>& GetIndices() const { return Indices; }
		inline const VkDeviceSize GetVerticesDeviceSize() const { return sizeof(Vertices[0]) * Vertices.size(); }
		inline const VkDeviceSize GetIndicesDeviceSize() const { return sizeof(Indices[0]) * Indices.size(); }

		void Load(const std::string& ObjectFile);

	private:
		std::vector<SVertex> Vertices;
		std::vector<uint32_t> Indices;
		std::array<VkVertexInputAttributeDescription, 4> VertexInputAttributeDescriptions;
		VkVertexInputBindingDescription VertexInputBindingDescription;
		VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo;
	};

}

namespace std {
	template<>
	struct hash<jarp::SVertex>
	{
		size_t operator()(jarp::SVertex const& Vertex) const
		{
			size_t h1 = hash<glm::vec3>()(Vertex.Position);
			size_t h2 = hash<glm::vec3>()(Vertex.Normal);
			size_t h3 = hash<glm::vec3>()(Vertex.Color);
			size_t h4 = hash<glm::vec2>()(Vertex.TextureCoordinate);

			return (((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) ^ (h4 >> 1);
			//return ((((h1 ^ (h2 << 1)) >> 1) ^ h3) << 1) ^ h4;
		}
	};
}
