#include "jarppch.h"
#include "CommandBuffer.h"

#include "jarp/Core.h"
#include "jarp/Renderer/Renderer.h"
#include "Platform/VulkanRHI/VulkanCommandBuffer.h"

namespace jarp {

	CommandBuffer* CommandBuffer::Create(const std::shared_ptr<CommandPool>& commandPool)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
		case RendererAPI::API::DirectX:
		case RendererAPI::API::OpenGL:
			JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
		case RendererAPI::API::Vulkan:
			return new VulkanCommandBuffer(std::dynamic_pointer_cast<VulkanCommandPool>(commandPool));
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
