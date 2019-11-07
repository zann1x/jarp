#include "jarppch.h"
#include "VertexBuffer.h"

#include "jarp/Core.h"
#include "jarp/Renderer/Renderer.h"
#include "Platform/VulkanRHI/VulkanBuffer.h"

namespace jarp {

	VertexBuffer* VertexBuffer::Create(const std::vector<SVertex>& vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			case RendererAPI::API::DirectX:
			case RendererAPI::API::OpenGL:
				JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
			case RendererAPI::API::Vulkan:
				return new VulkanVertexBuffer(vertices, size);
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
