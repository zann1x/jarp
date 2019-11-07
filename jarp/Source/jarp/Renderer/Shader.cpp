#include "jarppch.h"
#include "Shader.h"

#include "jarp/Core.h"
#include "jarp/Renderer/Renderer.h"
#include "Platform/VulkanRHI/VulkanShader.h"

namespace jarp {

	Shader* Shader::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
		case RendererAPI::API::DirectX:
		case RendererAPI::API::OpenGL:
			JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
		case RendererAPI::API::Vulkan:
			return new VulkanShader();
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
