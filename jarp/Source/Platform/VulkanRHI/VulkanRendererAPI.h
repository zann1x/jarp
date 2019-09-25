#pragma once
#include "VulkanInstance.h"
#include "VulkanDevice.h"

namespace jarp {

	struct VulkanRendererAPI
	{
		static std::unique_ptr<VulkanInstance> pInstance;
		static std::unique_ptr<VulkanDevice> pDevice;
	};

}
