#include "jarppch.h"
#include "VulkanCommandPool.h"

#include "VulkanQueue.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanCommandPool::VulkanCommandPool()
		: CommandPool(VK_NULL_HANDLE)
	{
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
	}

	void VulkanCommandPool::CreateCommandPool(const VkCommandPoolCreateFlags Flags)
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = nullptr;
		CommandPoolCreateInfo.flags = Flags;
		CommandPoolCreateInfo.queueFamilyIndex = VulkanRendererAPI::pDevice->GetGraphicsQueue().GetFamilyIndex();

		VK_ASSERT(vkCreateCommandPool(VulkanRendererAPI::pDevice->GetInstanceHandle(), &CommandPoolCreateInfo, nullptr, &CommandPool));
	}

	void VulkanCommandPool::Destroy()
	{
		// Also frees all command buffers created from this pool
		vkDestroyCommandPool(VulkanRendererAPI::pDevice->GetInstanceHandle(), CommandPool, nullptr);
	}

}
