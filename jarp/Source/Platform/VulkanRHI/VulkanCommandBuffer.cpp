#include "VulkanCommandBuffer.h"

#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& commandPool)
		: m_CommandPool(commandPool), m_CommandBuffer(VK_NULL_HANDLE)
	{
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = m_CommandPool.GetHandle();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		VK_ASSERT(vkAllocateCommandBuffers(VulkanRendererAPI::s_Device->GetInstanceHandle(), &commandBufferAllocateInfo, &m_CommandBuffer));
	}

	void VulkanCommandBuffer::Destroy()
	{
		vkFreeCommandBuffers(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_CommandPool.GetHandle(), 1, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::BeginOneTimeSubmitCommand()
	{
		CreateCommandBuffer();

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = m_CommandPool.GetHandle();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		VK_ASSERT(vkAllocateCommandBuffers(VulkanRendererAPI::s_Device->GetInstanceHandle(), &commandBufferAllocateInfo, &m_CommandBuffer));
	}

	void VulkanCommandBuffer::EndOneTimeSubmitCommand()
	{
		VK_ASSERT(vkEndCommandBuffer(m_CommandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		VulkanRendererAPI::s_Device->GetGraphicsQueue().QueueSubmitAndWait({ m_CommandBuffer }, 0, {}, {}, VK_NULL_HANDLE, {});
		VulkanRendererAPI::s_Device->GetGraphicsQueue().WaitUntilIdle();

		Destroy();
	}

	void VulkanCommandBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		BeginOneTimeSubmitCommand();

		VkBufferCopy bufferCopy = {};
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = 0;
		bufferCopy.size = size;

		vkCmdCopyBuffer(m_CommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

		EndOneTimeSubmitCommand();
	}

}
