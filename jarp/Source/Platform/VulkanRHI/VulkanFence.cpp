#include "jarppch.h"
#include "VulkanFence.h"

#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanFence::VulkanFence()
		: Fence(VK_NULL_HANDLE)
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

		VK_ASSERT(vkCreateFence(VulkanRendererAPI::pDevice->GetInstanceHandle(), &FenceCreateInfo, nullptr, &Fence));
	}

	void VulkanFence::Destroy()
	{
		vkDestroyFence(VulkanRendererAPI::pDevice->GetInstanceHandle(), Fence, nullptr);
	}

}
