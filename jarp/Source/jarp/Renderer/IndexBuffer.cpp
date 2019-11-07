#include "jarppch.h"
#include "IndexBuffer.h"

#include "jarp/Core.h"
#include "jarp/Renderer/Renderer.h"
#include "Platform/VulkanRHI/VulkanBuffer.h"

namespace jarp {

	IndexBuffer* IndexBuffer::Create(const std::vector<uint32_t>& indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
		case RendererAPI::API::DirectX:
		case RendererAPI::API::OpenGL:
			JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
		case RendererAPI::API::Vulkan:
			return new VulkanIndexBuffer(indices, size);
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
