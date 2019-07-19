#include "VulkanQueue.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanUtils.hpp"

VulkanQueue::VulkanQueue(VulkanDevice& Device, uint32_t QueueFamilyIndex, uint32_t QueueIndex)
	: Device(Device), QueueFamilyIndex(QueueFamilyIndex), QueueIndex(QueueIndex)
{
	vkGetDeviceQueue(Device.GetInstanceHandle(), QueueFamilyIndex, QueueIndex, &Queue);
}

VulkanQueue::~VulkanQueue()
{
}

void VulkanQueue::QueueSubmitAndWait(const std::vector<VkCommandBuffer>& CommandBuffers, const VkPipelineStageFlags WaitDstStageMask, const std::vector<VkSemaphore>& WaitSemaphores, const std::vector<VkSemaphore>& SignalSemaphores, VkFence SubmitFence, const std::vector<VkFence>& WaitFences) const
{
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = nullptr;
	SubmitInfo.waitSemaphoreCount = static_cast<uint32_t>(WaitSemaphores.size());
	SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
	SubmitInfo.pWaitDstStageMask = &WaitDstStageMask;
	SubmitInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());
	SubmitInfo.pCommandBuffers = CommandBuffers.data();
	SubmitInfo.signalSemaphoreCount = static_cast<uint32_t>(SignalSemaphores.size());
	SubmitInfo.pSignalSemaphores = SignalSemaphores.data();

	VK_ASSERT(vkQueueSubmit(Queue, 1, &SubmitInfo, SubmitFence));
	if (!WaitFences.empty())
	{
		VK_ASSERT(vkWaitForFences(Device.GetInstanceHandle(), static_cast<uint32_t>(WaitFences.size()), WaitFences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max()));
		VK_ASSERT(vkResetFences(Device.GetInstanceHandle(), static_cast<uint32_t>(WaitFences.size()), WaitFences.data()));
	}
}

VkResult VulkanQueue::QueuePresent(const VkSwapchainKHR Swapchain, std::vector<uint32_t> ActiveImageIndices, const std::vector<VkSemaphore>& WaitSemaphores) const
{
	VkPresentInfoKHR PresentInfoKHR = {};
	PresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfoKHR.pNext = nullptr;
	PresentInfoKHR.waitSemaphoreCount = static_cast<uint32_t>(WaitSemaphores.size());
	PresentInfoKHR.pWaitSemaphores = WaitSemaphores.data();
	PresentInfoKHR.swapchainCount = 1; // Would be > 1 if there were multiple images being rendered simumltaneously in a multi-window setup
	PresentInfoKHR.pSwapchains = &Swapchain;
	PresentInfoKHR.pImageIndices = ActiveImageIndices.data();
	PresentInfoKHR.pResults = nullptr;

	return vkQueuePresentKHR(Queue, &PresentInfoKHR);
}

void VulkanQueue::WaitUntilIdle() const
{
	VK_ASSERT(vkQueueWaitIdle(Queue));
}
