#pragma once

#include "jarp/Renderer/RendererAPI.h"

namespace jarp {

	class TempVulkanApplication;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapchain;

	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void Render(uint32_t deltaTime) override;
		virtual void Shutdown() override;

	public:
		static std::shared_ptr<VulkanInstance> s_Instance;
		static std::shared_ptr<VulkanDevice> s_Device;
		static std::shared_ptr<VulkanSwapchain> s_Swapchain;

	private:
		static TempVulkanApplication s_VulkanRenderer;
	};

}
