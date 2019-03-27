#include "VulkanDescriptorPool.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanUtils.hpp"

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& OutDevice, VulkanSwapchain& OutSwapchain)
	: Device(OutDevice), Swapchain(OutSwapchain)
{
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
}

void VulkanDescriptorPool::CreateDescriptorPool()
{
	VkDescriptorPoolSize DescriptorPoolSize = {};
	DescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorPoolSize.descriptorCount = static_cast<uint32_t>(Swapchain.GetImages().size());

	VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
	DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	DescriptorPoolCreateInfo.pNext = nullptr;
	DescriptorPoolCreateInfo.flags = 0;
	DescriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(Swapchain.GetImages().size());
	DescriptorPoolCreateInfo.poolSizeCount = 1;
	DescriptorPoolCreateInfo.pPoolSizes = &DescriptorPoolSize;

	VK_ASSERT(vkCreateDescriptorPool(Device.GetInstanceHandle(), &DescriptorPoolCreateInfo, nullptr, &DescriptorPool));
}

void VulkanDescriptorPool::Destroy()
{
	// Implicitly destroys the descriptor sets created from it
	vkDestroyDescriptorPool(Device.GetInstanceHandle(), DescriptorPool, nullptr);
}
