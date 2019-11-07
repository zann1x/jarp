#include "jarppch.h"
#include "Pipeline.h"

#include "jarp/Core.h"
#include "jarp/Renderer/Renderer.h"
#include "Platform/VulkanRHI/VulkanGraphicsPipeline.h"

namespace jarp {

	Pipeline* Pipeline::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			case RendererAPI::API::DirectX:
			case RendererAPI::API::OpenGL:
				JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
			case RendererAPI::API::Vulkan:
				// TODO
				//return new VulkanGraphicsPipeline();
				return nullptr;
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
