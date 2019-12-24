#pragma once

#include <volk.h>

#include <vector>

namespace jarp {

	class VulkanQueue
	{
	public:
		VulkanQueue(uint32_t queueFamilyIndex, uint32_t queueIndex = 0);
		~VulkanQueue();

		inline const VkQueue& GetHandle() const { return m_Queue; }
		inline uint32_t GetFamilyIndex() const { return m_QueueFamilyIndex; }

		void QueueSubmitAndWait(const std::vector<VkCommandBuffer>& commandBuffers, const VkPipelineStageFlags waitDstStageMask, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence submitFence, const std::vector<VkFence>& fences) const;
		VkResult QueuePresent(const VkSwapchainKHR swapchain, std::vector<uint32_t> activeImageIndices, const std::vector<VkSemaphore>& waitSemaphores) const;
		void WaitUntilIdle() const;

	private:
		VkQueue m_Queue;
		uint32_t m_QueueFamilyIndex;
		uint32_t m_QueueIndex;
	};

}
