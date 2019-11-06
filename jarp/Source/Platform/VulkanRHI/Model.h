#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <volk.h>
#include <glm/gtx/hash.hpp>

#include "jarp/Renderer/VertexComponent.h"

namespace jarp {

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
