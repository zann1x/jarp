#include "jarppch.h"
#include "VulkanRendererAPI.h"

namespace jarp {

	std::unique_ptr<VulkanInstance> VulkanRendererAPI::s_Instance;
	std::unique_ptr<VulkanDevice> VulkanRendererAPI::s_Device;
	std::unique_ptr<VulkanSwapchain> VulkanRendererAPI::s_Swapchain;

}
