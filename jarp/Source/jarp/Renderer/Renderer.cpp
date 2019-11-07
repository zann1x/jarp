#include "jarppch.h"
#include "Renderer.h"

#include "jarp/Core.h"

#include "Platform/VulkanRHI/VulkanRendererAPI.h"

namespace jarp {

	std::shared_ptr<RendererAPI> Renderer::s_RendererAPI;

	void Renderer::Init()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
			case RendererAPI::API::DirectX:
			case RendererAPI::API::OpenGL:
				JARP_CORE_ASSERT(false, "Only Renderer API Vulkan is supported");
				break;
			case RendererAPI::API::Vulkan:
				s_RendererAPI = std::make_shared<VulkanRendererAPI>();
				break;
		}

		s_RendererAPI->Init();
	}

	void Renderer::Render(uint32_t deltaTime)
	{
		s_RendererAPI->Render(deltaTime);
	}

	void Renderer::Shutdown()
	{
		s_RendererAPI->Shutdown();
	}

}
