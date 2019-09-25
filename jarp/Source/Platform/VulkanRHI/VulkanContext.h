#pragma once
#include "VulkanInstance.h"
#include "VulkanDevice.h"

namespace jarp {

	class VulkanRendererAPI
	{
	public:
		static std::unique_ptr<VulkanInstance> pInstance;
		static std::unique_ptr<VulkanDevice> pDevice;
	};

}
