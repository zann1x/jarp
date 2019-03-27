#include "VulkanCommandPool.h"

#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanUtils.hpp"

VulkanCommandPool::VulkanCommandPool(VulkanDevice& OutDevice)
	: Device(OutDevice)
{
}

VulkanCommandPool::~VulkanCommandPool()
{
}

void VulkanCommandPool::CreateCommandPool()
{
	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.pNext = nullptr;
	CommandPoolCreateInfo.flags = 0;
	CommandPoolCreateInfo.queueFamilyIndex = Device.GetGraphicsQueue().GetFamilyIndex();

	VK_ASSERT(vkCreateCommandPool(Device.GetInstanceHandle(), &CommandPoolCreateInfo, nullptr, &CommandPool));
}

void VulkanCommandPool::Destroy()
{
	// Also frees all command buffers created from this pool
	vkDestroyCommandPool(Device.GetInstanceHandle(), CommandPool, nullptr);
}
