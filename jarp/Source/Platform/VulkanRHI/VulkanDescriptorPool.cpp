#include "jarppch.h"
#include "VulkanDescriptorPool.h"

#include "VulkanRendererAPI.h"
#include "VulkanSwapchain.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanDescriptorPool::VulkanDescriptorPool(VulkanSwapchain& OutSwapchain)
		: Swapchain(OutSwapchain), DescriptorPool(VK_NULL_HANDLE)
	{
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
	}

	void VulkanDescriptorPool::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> DescriptorPoolSizes;
		DescriptorPoolSizes[0] = {};
		DescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(Swapchain.GetImages().size());
		DescriptorPoolSizes[1] = {};
		DescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		DescriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(Swapchain.GetImages().size());

		VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
		DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.pNext = nullptr;
		DescriptorPoolCreateInfo.flags = 0;
		DescriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(Swapchain.GetImages().size());
		DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolSizes.size());
		DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizes.data();

		VK_ASSERT(vkCreateDescriptorPool(VulkanRendererAPI::pDevice->GetInstanceHandle(), &DescriptorPoolCreateInfo, nullptr, &DescriptorPool));
	}

	void VulkanDescriptorPool::Destroy()
	{
		// Implicitly destroys the descriptor sets created from it
		vkDestroyDescriptorPool(VulkanRendererAPI::pDevice->GetInstanceHandle(), DescriptorPool, nullptr);
	}

}
