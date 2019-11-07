#include "jarppch.h"
#include "VulkanRendererAPI.h"

#include "jarp/Application.h"
#include "Platform/VulkanRHI/TempVulkanApplication.h"
#include "Platform/VulkanRHI/VulkanInstance.h"
#include "Platform/VulkanRHI/VulkanDevice.h"
#include "Platform/VulkanRHI/VulkanSwapchain.h"

namespace jarp {

	TempVulkanApplication VulkanRendererAPI::s_VulkanRenderer;
	std::shared_ptr<VulkanInstance> VulkanRendererAPI::s_Instance;
	std::shared_ptr<VulkanDevice> VulkanRendererAPI::s_Device;
	std::shared_ptr<VulkanSwapchain> VulkanRendererAPI::s_Swapchain;

	void VulkanRendererAPI::Init()
	{
		s_Instance = std::make_shared<VulkanInstance>();
		s_Instance->CreateInstance();

		s_Device = std::make_shared<VulkanDevice>();
		s_Device->CreateLogicalDevice();

		s_Swapchain = std::make_shared<VulkanSwapchain>(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), false);

		s_VulkanRenderer.StartVulkan();
	}

	void VulkanRendererAPI::Render(uint32_t deltaTime)
	{
		s_VulkanRenderer.Render(deltaTime);
	}

	void VulkanRendererAPI::Shutdown()
	{
		s_VulkanRenderer.ShutdownVulkan();

		s_Swapchain->Destroy();
		s_Device->Destroy();
		s_Instance->Destroy();
	}

}
