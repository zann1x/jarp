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
		case Renderer::API::None:
		case Renderer::API::DirectX:
		case Renderer::API::OpenGL:
			JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
		case Renderer::API::Vulkan:
			return new VulkanShader();
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
