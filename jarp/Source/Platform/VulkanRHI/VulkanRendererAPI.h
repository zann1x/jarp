#pragma once

#include "Platform/VulkanRHI/VulkanInstance.h"
#include "Platform/VulkanRHI/VulkanDevice.h"
#include "Platform/VulkanRHI/VulkanSwapchain.h"

namespace jarp {

	struct VulkanRendererAPI
	{
		static std::unique_ptr<VulkanInstance> s_Instance;
		static std::unique_ptr<VulkanDevice> s_Device;
		static std::unique_ptr<VulkanSwapchain> s_Swapchain;
	};

}
