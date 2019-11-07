#include "jarppch.h"
#include "VulkanSemaphore.h"

#include "Platform/VulkanRHI/VulkanDevice.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

namespace jarp {

	VulkanSemaphore::VulkanSemaphore()
		: m_Semaphore(VK_NULL_HANDLE)
	{
	}

	VulkanSemaphore::~VulkanSemaphore()
	{
	}

	void VulkanSemaphore::CreateSemaphore()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.flags = 0;
		semaphoreCreateInfo.pNext = nullptr;

		VK_ASSERT(vkCreateSemaphore(VulkanRendererAPI::s_Device->GetInstanceHandle(), &semaphoreCreateInfo, nullptr, &m_Semaphore));
	}

	void VulkanSemaphore::Destroy()
	{
		vkDestroySemaphore(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_Semaphore, nullptr);
	}

}
