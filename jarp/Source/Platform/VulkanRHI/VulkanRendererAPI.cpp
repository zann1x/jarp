#include "jarppch.h"
#include "VulkanContext.h"

namespace jarp {

	std::unique_ptr<VulkanInstance> VulkanRendererAPI::pInstance;
	std::unique_ptr<VulkanDevice> VulkanRendererAPI::pDevice;

}
