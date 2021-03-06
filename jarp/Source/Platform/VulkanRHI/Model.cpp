#include "jarppch.h"
#include "Model.h"

#include <tiny_obj_loader.h>
#include <unordered_map>

namespace jarp {

	Model::Model()
	{
		//Vertices = {
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
		//Indices = {
		//	0, 1, 2, 2, 3, 0,
		//	4, 5, 6, 6, 7, 4
		//};

		VertexInputBindingDescription = {};
		VertexInputBindingDescription.binding = 0;
		VertexInputBindingDescription.stride = sizeof(SVertex);
		VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VertexInputAttributeDescriptions[0] = {};
		VertexInputAttributeDescriptions[0].location = 0;
		VertexInputAttributeDescriptions[0].binding = 0;
		VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		VertexInputAttributeDescriptions[0].offset = offsetof(SVertex, Position);
		VertexInputAttributeDescriptions[1] = {};
		VertexInputAttributeDescriptions[1].location = 1;
		VertexInputAttributeDescriptions[1].binding = 0;
		VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		VertexInputAttributeDescriptions[1].offset = offsetof(SVertex, Normal);
		VertexInputAttributeDescriptions[2] = {};
		VertexInputAttributeDescriptions[2].location = 2;
		VertexInputAttributeDescriptions[2].binding = 0;
		VertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		VertexInputAttributeDescriptions[2].offset = offsetof(SVertex, Color);
		VertexInputAttributeDescriptions[3] = {};
		VertexInputAttributeDescriptions[3].location = 3;
		VertexInputAttributeDescriptions[3].binding = 0;
		VertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		VertexInputAttributeDescriptions[3].offset = offsetof(SVertex, TextureCoordinate);

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

	void Model::Load(const std::string& ObjectFile)
	{
		tinyobj::attrib_t Attrib;
		std::vector<tinyobj::shape_t> Shapes;
		std::vector<tinyobj::material_t> materials;
		std::string Warn, Err;

		if (!tinyobj::LoadObj(&Attrib, &Shapes, &materials, &Warn, &Err, ObjectFile.c_str()))
		{
			throw std::runtime_error(Warn + Err);
		}

		std::unordered_map<SVertex, uint32_t> UniqueVertices = {};

		for (const auto& Shape : Shapes)
		{
			for (const auto& Index : Shape.mesh.indices)
			{
				SVertex Vertex = {};

				Vertex.Position = {
					Attrib.vertices[3 * Index.vertex_index + 0],
					Attrib.vertices[3 * Index.vertex_index + 1],
					Attrib.vertices[3 * Index.vertex_index + 2]
				};

				Attrib.normals.empty()
					? Vertex.Normal = glm::vec3(0.0f, 0.0f, 1.0f)
					: Vertex.Normal = {
						Attrib.normals[3 * Index.normal_index + 0],
						Attrib.normals[3 * Index.normal_index + 1],
						Attrib.normals[3 * Index.normal_index + 2]
				};

				Attrib.colors.empty()
					? Vertex.Color = glm::vec3(1.0f, 0.0f, 0.86f)
					: Vertex.Color = {
						Attrib.colors[3 * Index.vertex_index + 0],
						Attrib.colors[3 * Index.vertex_index + 1],
						Attrib.colors[3 * Index.vertex_index + 2]
				};

				Attrib.texcoords.empty()
					? Vertex.TextureCoordinate = glm::vec2(0.0f, 0.0f)
					: Vertex.TextureCoordinate = {
						Attrib.texcoords[2 * Index.texcoord_index + 0],
						1.0f - Attrib.texcoords[2 * Index.texcoord_index + 1]
				};

				if (UniqueVertices.count(Vertex) == 0)
				{
					// Save unique vertices with an index to the position at which they are found in the file
					UniqueVertices[Vertex] = static_cast<uint32_t>(Vertices.size());
					Vertices.push_back(Vertex);
				}

				Indices.push_back(UniqueVertices[Vertex]);
			}
		}

		// TODO: optimize mesh for more efficient GPU rendering ?
	}

}
