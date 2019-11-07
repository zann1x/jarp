#include "jarppch.h"
#include "VulkanFence.h"

#include "Platform/VulkanRHI/VulkanDevice.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

namespace jarp {

	VulkanFence::VulkanFence()
		: m_Fence(VK_NULL_HANDLE)
	{
	}

	VulkanFence::~VulkanFence()
	{
	}

	void VulkanFence::CreateFence()
	{
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = 0;

		VK_ASSERT(vkCreateFence(VulkanRendererAPI::s_Device->GetInstanceHandle(), &fenceCreateInfo, nullptr, &m_Fence));
	}

	void VulkanFence::Destroy()
	{
		vkDestroyFence(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Fence, nullptr);
	}

}
