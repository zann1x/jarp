#pragma once

#include <volk.h>

namespace jarp {

	class VulkanDevice;

	class VulkanQueue
	{
	public:
		VulkanQueue(VulkanDevice& Device, uint32_t QueueFamilyIndex, uint32_t QueueIndex = 0);
		~VulkanQueue();

		inline const VkQueue& GetHandle() const { return Queue; }
		inline uint32_t GetFamilyIndex() const { return QueueFamilyIndex; }

		void QueueSubmitAndWait(const std::vector<VkCommandBuffer>& CommandBuffers, const VkPipelineStageFlags WaitDstStageMask, const std::vector<VkSemaphore>& WaitSemaphores, const std::vector<VkSemaphore>& SignalSemaphores, VkFence SubmitFence, const std::vector<VkFence>& Fences) const;
		VkResult QueuePresent(const VkSwapchainKHR Swapchain, std::vector<uint32_t> ActiveImageIndices, const std::vector<VkSemaphore>& WaitSemaphores) const;
		void WaitUntilIdle() const;

	private:
		VulkanDevice& Device;

		VkQueue Queue;
		uint32_t QueueFamilyIndex;
		uint32_t QueueIndex;
	};

}
