#include "jarppch.h"
#include "VulkanCommandPool.h"

#include "VulkanQueue.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanCommandPool::VulkanCommandPool(const VkCommandPoolCreateFlags flags)
		: m_commandPool(VK_NULL_HANDLE)
	{
		CreateCommandPool(flags);
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
	}

	void VulkanCommandPool::CreateCommandPool(const VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = flags;
		commandPoolCreateInfo.queueFamilyIndex = VulkanRendererAPI::s_Device->GetGraphicsQueue().GetFamilyIndex();

		VK_ASSERT(vkCreateCommandPool(VulkanRendererAPI::s_Device->GetInstanceHandle(), &commandPoolCreateInfo, nullptr, &m_commandPool));
	}

	void VulkanCommandPool::Destroy()
	{
		// Also frees all command buffers created from this pool
		vkDestroyCommandPool(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_commandPool, nullptr); 
	}

}
