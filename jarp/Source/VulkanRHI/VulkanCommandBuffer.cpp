#include "VulkanCommandBuffer.h"

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
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
