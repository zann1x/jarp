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
			case Renderer::API::None:
			case Renderer::API::DirectX:
			case Renderer::API::OpenGL:
				JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
			case Renderer::API::Vulkan:
				// TODO
				//return new VulkanGraphicsPipeline();
				return nullptr;
		}

		JARP_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

}
