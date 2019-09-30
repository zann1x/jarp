#include "jarppch.h"
#include "VulkanSemaphore.h"

#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanSemaphore::VulkanSemaphore()
		: Semaphore(VK_NULL_HANDLE)
	{
	}

	VulkanSemaphore::~VulkanSemaphore()
	{
	}

	void VulkanSemaphore::CreateSemaphore()
	{
		VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		SemaphoreCreateInfo.flags = 0;
		SemaphoreCreateInfo.pNext = nullptr;

		VK_ASSERT(vkCreateSemaphore(VulkanRendererAPI::pDevice->GetInstanceHandle(), &SemaphoreCreateInfo, nullptr, &Semaphore));
	}

	void VulkanSemaphore::Destroy()
	{
		vkDestroySemaphore(VulkanRendererAPI::pDevice->GetInstanceHandle(), Semaphore, nullptr);
	}

}
