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
			case Renderer::API::None:
			case Renderer::API::DirectX:
			case Renderer::API::OpenGL:
				JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
			case Renderer::API::Vulkan:
				return new VulkanVertexBuffer(vertices, size);
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
