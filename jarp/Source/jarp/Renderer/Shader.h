#pragma once

#include "jarp/Renderer/CommandBuffer.h"
#include "jarp/Renderer/Pipeline.h"

#include <glm/glm.hpp>

namespace jarp {

	struct SUniformBufferObject
	{
		alignas(16) glm::mat4 Model;
		alignas(16) glm::mat4 View;
		alignas(16) glm::mat4 Projection;
		alignas(16) glm::vec3 LightPosition;
	};

	struct ShaderLayoutElement;

	class ShaderLayout
	{
	public:
		enum class Type
		{
			Sampler,
			Uniform
		};

	public:
		ShaderLayout(const std::initializer_list<ShaderLayoutElement>& elements)
			: m_Elements(elements)
		{
		}

		virtual ~ShaderLayout() = default;

	public:
		std::vector<ShaderLayoutElement> m_Elements;
	};

	class Shader
	{
	public:
		enum class Type
		{
			Fragment,
			Vertex
		};

	public:
		virtual ~Shader() = default;

		virtual void AddModule(Type shaderType, const char* filename) = 0;
		virtual void AddLayout(const ShaderLayout& layout) = 0;

		virtual void Bind(uint32_t currentRenderIndex, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<Pipeline>& pipeline) = 0;

		static Shader* Create();
	};

	struct ShaderLayoutElement
	{
		int Binding;
		ShaderLayout::Type LayoutType;
		Shader::Type ShaderType;

		ShaderLayoutElement() { }

		ShaderLayoutElement(int binding, ShaderLayout::Type layoutType, Shader::Type shaderType)
			: Binding(binding), LayoutType(layoutType), ShaderType(shaderType)
		{
		}
	};

}
