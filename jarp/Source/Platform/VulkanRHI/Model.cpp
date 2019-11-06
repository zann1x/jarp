#include "jarppch.h"
#include "Model.h"

#include <tiny_obj_loader.h>
#include <unordered_map>

namespace jarp {

	Model::Model()
	{
		//m_Vertices = {
		//	{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } }, // 0
		//	{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } }, // 1
		//	{ {  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }, // 2
		//	{ { -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }, // 3

		//	{{ -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
		//	{{  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
		//	{{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},
		//	{{ -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }}
		//};
		//
		//m_Indices = {
		//	0, 1, 2, 2, 3, 0,
		//	4, 5, 6, 6, 7, 4
		//};

		m_VertexInputBindingDescription = {};
		m_VertexInputBindingDescription.binding = 0;
		m_VertexInputBindingDescription.stride = sizeof(SVertex);
		m_VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		m_VertexInputAttributeDescriptions[0] = {};
		m_VertexInputAttributeDescriptions[0].location = 0;
		m_VertexInputAttributeDescriptions[0].binding = 0;
		m_VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_VertexInputAttributeDescriptions[0].offset = offsetof(SVertex, Position);
		m_VertexInputAttributeDescriptions[1] = {};
		m_VertexInputAttributeDescriptions[1].location = 1;
		m_VertexInputAttributeDescriptions[1].binding = 0;
		m_VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_VertexInputAttributeDescriptions[1].offset = offsetof(SVertex, Normal);
		m_VertexInputAttributeDescriptions[2] = {};
		m_VertexInputAttributeDescriptions[2].location = 2;
		m_VertexInputAttributeDescriptions[2].binding = 0;
		m_VertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_VertexInputAttributeDescriptions[2].offset = offsetof(SVertex, Color);
		m_VertexInputAttributeDescriptions[3] = {};
		m_VertexInputAttributeDescriptions[3].location = 3;
		m_VertexInputAttributeDescriptions[3].binding = 0;
		m_VertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		m_VertexInputAttributeDescriptions[3].offset = offsetof(SVertex, TextureCoordinate);

		m_PipelineVertexInputStateCreateInfo = {};
		m_PipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_PipelineVertexInputStateCreateInfo.pNext = nullptr;
		m_PipelineVertexInputStateCreateInfo.flags = 0;
		m_PipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		m_PipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &m_VertexInputBindingDescription;
		m_PipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexInputAttributeDescriptions.size());
		m_PipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = m_VertexInputAttributeDescriptions.data();
	}

	Model::~Model()
	{
	}

	void Model::Load(const std::string& objectFile)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objectFile.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<SVertex, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				SVertex vertex = {};

				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				attrib.normals.empty()
					? vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f)
					: vertex.Normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
				};

				attrib.colors.empty()
					? vertex.Color = glm::vec3(1.0f, 0.0f, 0.86f)
					: vertex.Color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
				};

				attrib.texcoords.empty()
					? vertex.TextureCoordinate = glm::vec2(0.0f, 0.0f)
					: vertex.TextureCoordinate = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (uniqueVertices.count(vertex) == 0)
				{
					// Save unique vertices with an index to the position at which they are found in the file
					uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
					m_Vertices.push_back(vertex);
				}

				m_Indices.push_back(uniqueVertices[vertex]);
			}
		}

		// TODO: optimize mesh for more efficient GPU rendering ?
	}

}
