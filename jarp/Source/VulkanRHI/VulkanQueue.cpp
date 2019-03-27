#include "VulkanQueue.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

VulkanQueue::VulkanQueue(VulkanDevice& Device, uint32_t QueueFamilyIndex, uint32_t QueueIndex)
	: Device(Device), QueueFamilyIndex(QueueFamilyIndex), QueueIndex(QueueIndex)
{
	vkGetDeviceQueue(Device.GetInstanceHandle(), QueueFamilyIndex, QueueIndex, &Queue);
}

VulkanQueue::~VulkanQueue()
{
}

void VulkanQueue::QueueSubmit(const std::vector<VkCommandBuffer>& CommandBuffers, const VkPipelineStageFlags WaitDstStageMask, const std::vector<VkSemaphore>& WaitSemaphores, const std::vector<VkSemaphore>& SignalSemaphores) const
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

	VK_ASSERT(vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE));
}

void VulkanQueue::WaitUntilIdle() const
{
	// TODO change to fences ?
	VK_ASSERT(vkQueueWaitIdle(Queue));
}
