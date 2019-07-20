#include "jarppch.h"
#include "VulkanFence.h"

#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanFence::VulkanFence(VulkanDevice& Device)
		: Device(Device)
	{
	}

	VulkanFence::~VulkanFence()
	{
	}

	void VulkanFence::CreateFence()
	{
		VkFenceCreateInfo FenceCreateInfo = {};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = nullptr;
		FenceCreateInfo.flags = 0;

		VK_ASSERT(vkCreateFence(Device.GetInstanceHandle(), &FenceCreateInfo, nullptr, &Fence));
	}

	void VulkanFence::Destroy()
	{
		vkDestroyFence(Device.GetInstanceHandle(), Fence, nullptr);
	}

}
