#include "VulkanFence.h"

#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

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
