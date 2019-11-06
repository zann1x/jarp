#pragma once

#include <glm/glm.hpp>

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

}
