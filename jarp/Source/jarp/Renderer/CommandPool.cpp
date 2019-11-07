#include "jarppch.h"
#include "CommandPool.h"

#include "jarp/Core.h"
#include "jarp/Renderer/Renderer.h"
#include "Platform/VulkanRHI/VulkanCommandPool.h"

namespace jarp {

	CommandPool* CommandPool::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
		case RendererAPI::API::DirectX:
		case RendererAPI::API::OpenGL:
			JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
		case RendererAPI::API::Vulkan:
			return new VulkanCommandPool();
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
