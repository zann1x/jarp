#include "jarppch.h"
#include "VulkanSemaphore.h"

#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

VulkanSemaphore::VulkanSemaphore(VulkanDevice& Device)
	: Device(Device)
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

	VK_ASSERT(vkCreateSemaphore(Device.GetInstanceHandle(), &SemaphoreCreateInfo, nullptr, &Semaphore));
}

void VulkanSemaphore::Destroy()
{
	vkDestroySemaphore(Device.GetInstanceHandle(), Semaphore, nullptr);
}
