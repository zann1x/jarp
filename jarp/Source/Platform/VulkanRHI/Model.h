#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <volk.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <array>
#include <vector>

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

		bool operator==(const SVertex& other) const
		{
			return Position == other.Position && Normal == other.Normal && Color == other.Color && TextureCoordinate == other.TextureCoordinate;
		}
	};

	class Model
	{
	public:
		Model();
		~Model();

		inline const VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo() const { return m_PipelineVertexInputStateCreateInfo; };
		inline const std::vector<SVertex>& GetVertices() const { return m_Vertices; }
		inline const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		inline const VkDeviceSize GetVerticesDeviceSize() const { return sizeof(m_Vertices[0]) * m_Vertices.size(); }
		inline const VkDeviceSize GetIndicesDeviceSize() const { return sizeof(m_Indices[0]) * m_Indices.size(); }

		void Load(const std::string& objectFile);

	private:
		std::vector<SVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::array<VkVertexInputAttributeDescription, 4> m_VertexInputAttributeDescriptions;
		VkVertexInputBindingDescription m_VertexInputBindingDescription;
		VkPipelineVertexInputStateCreateInfo m_PipelineVertexInputStateCreateInfo;
	};

}

namespace std {
	template<>
	struct hash<jarp::SVertex>
	{
		size_t operator()(jarp::SVertex const& vertex) const
		{
			size_t h1 = hash<glm::vec3>()(vertex.Position);
			size_t h2 = hash<glm::vec3>()(vertex.Normal);
			size_t h3 = hash<glm::vec3>()(vertex.Color);
			size_t h4 = hash<glm::vec2>()(vertex.TextureCoordinate);

			return (((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) ^ (h4 >> 1);
			//return ((((h1 ^ (h2 << 1)) >> 1) ^ h3) << 1) ^ h4;
		}
	};
}
