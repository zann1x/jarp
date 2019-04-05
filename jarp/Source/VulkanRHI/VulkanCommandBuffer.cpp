#include "VulkanCommandBuffer.h"

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanUtils.hpp"

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice& OutDevice, VulkanCommandPool& OutCommandPool)
	: Device(OutDevice), CommandPool(OutCommandPool)
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

	VK_ASSERT(vkAllocateCommandBuffers(Device.GetInstanceHandle(), &CommandBufferAllocateInfo, &CommandBuffer));
}

void VulkanCommandBuffer::Destroy()
{
	vkFreeCommandBuffers(Device.GetInstanceHandle(), CommandPool.GetHandle(), 1, &CommandBuffer);
}

void VulkanCommandBuffer::CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
{
	CreateCommandBuffer();

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;

	// START OF RECORD //
	VK_ASSERT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

	VkBufferCopy BufferCopy = {};
	BufferCopy.srcOffset = 0;
	BufferCopy.dstOffset = 0;
	BufferCopy.size = Size;

	vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &BufferCopy);

	VK_ASSERT(vkEndCommandBuffer(CommandBuffer));
	// END OF RECORD //

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

	Device.GetGraphicsQueue().QueueSubmit({ CommandBuffer }, 0, {}, {});
	Device.GetGraphicsQueue().WaitUntilIdle();

	Destroy();
}
