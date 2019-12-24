#pragma once

#include "VulkanInstance.h"
#include "VulkanDevice.h"

#include <memory>

namespace jarp {

	struct VulkanRendererAPI
	{
		static std::unique_ptr<VulkanInstance> s_Instance;
		static std::unique_ptr<VulkanDevice> s_Device;
	};

}
