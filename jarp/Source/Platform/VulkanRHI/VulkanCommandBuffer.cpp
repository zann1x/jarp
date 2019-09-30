#include "jarppch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& OutCommandPool)
		: CommandPool(OutCommandPool), CommandBuffer(VK_NULL_HANDLE)
	{
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.pNext = nullptr;
		CommandBufferAllocateInfo.commandPool = CommandPool.GetHandle();
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;

		VK_ASSERT(vkAllocateCommandBuffers(VulkanRendererAPI::pDevice->GetInstanceHandle(), &CommandBufferAllocateInfo, &CommandBuffer));
	}

	void VulkanCommandBuffer::Destroy()
	{
		vkFreeCommandBuffers(VulkanRendererAPI::pDevice->GetInstanceHandle(), CommandPool.GetHandle(), 1, &CommandBuffer);
	}

	void VulkanCommandBuffer::BeginOneTimeSubmitCommand()
	{
		CreateCommandBuffer();

		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.pNext = nullptr;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CommandBufferBeginInfo.pInheritanceInfo = nullptr;

		VK_ASSERT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));
	}

	void VulkanCommandBuffer::EndOneTimeSubmitCommand()
	{
		VK_ASSERT(vkEndCommandBuffer(CommandBuffer));

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.pNext = nullptr;
		SubmitInfo.waitSemaphoreCount = 0;
		SubmitInfo.pWaitSemaphores = nullptr;
		SubmitInfo.pWaitDstStageMask = nullptr;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffer;
		SubmitInfo.signalSemaphoreCount = 0;
		SubmitInfo.pSignalSemaphores = nullptr;

		VulkanRendererAPI::pDevice->GetGraphicsQueue().QueueSubmitAndWait({ CommandBuffer }, 0, {}, {}, VK_NULL_HANDLE, {});
		VulkanRendererAPI::pDevice->GetGraphicsQueue().WaitUntilIdle();

		Destroy();
	}

	void VulkanCommandBuffer::CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
	{
		BeginOneTimeSubmitCommand();

		VkBufferCopy BufferCopy = {};
		BufferCopy.srcOffset = 0;
		BufferCopy.dstOffset = 0;
		BufferCopy.size = Size;

		vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &BufferCopy);

		EndOneTimeSubmitCommand();
	}

}
