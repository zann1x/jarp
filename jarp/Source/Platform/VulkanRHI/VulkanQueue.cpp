#include "VulkanQueue.h"

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanUtils.hpp"
#include "VulkanRendererAPI.h"

namespace jarp {

	VulkanQueue::VulkanQueue(uint32_t queueFamilyIndex, uint32_t queueIndex)
		: m_QueueFamilyIndex(queueFamilyIndex), m_QueueIndex(queueIndex)
	{
		vkGetDeviceQueue(VulkanRendererAPI::s_Device->GetInstanceHandle(), queueFamilyIndex, queueIndex, &m_Queue);
	}

	VulkanQueue::~VulkanQueue()
	{
	}

	void VulkanQueue::QueueSubmitAndWait(const std::vector<VkCommandBuffer>& commandBuffers, const VkPipelineStageFlags waitDstStageMask, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence submitFence, const std::vector<VkFence>& waitFences) const
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = &waitDstStageMask;
		submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		VK_ASSERT(vkQueueSubmit(m_Queue, 1, &submitInfo, submitFence));
		if (!waitFences.empty())
		{
			VK_ASSERT(vkWaitForFences(VulkanRendererAPI::s_Device->GetInstanceHandle(), static_cast<uint32_t>(waitFences.size()), waitFences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max()));
			VK_ASSERT(vkResetFences(VulkanRendererAPI::s_Device->GetInstanceHandle(), static_cast<uint32_t>(waitFences.size()), waitFences.data()));
		}
	}

	VkResult VulkanQueue::QueuePresent(const VkSwapchainKHR swapchain, std::vector<uint32_t> activeImageIndices, const std::vector<VkSemaphore>& waitSemaphores) const
	{
		VkPresentInfoKHR presentInfoKHR = {};
		presentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfoKHR.pNext = nullptr;
		presentInfoKHR.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
		presentInfoKHR.pWaitSemaphores = waitSemaphores.data();
		presentInfoKHR.swapchainCount = 1; // Would be > 1 if there were multiple images being rendered simumltaneously in a multi-window setup
		presentInfoKHR.pSwapchains = &swapchain;
		presentInfoKHR.pImageIndices = activeImageIndices.data();
		presentInfoKHR.pResults = nullptr;

		return vkQueuePresentKHR(m_Queue, &presentInfoKHR);
	}

	void VulkanQueue::WaitUntilIdle() const
	{
		VK_ASSERT(vkQueueWaitIdle(m_Queue));
	}

}
