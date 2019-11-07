#include "jarppch.h"
#include "VulkanDescriptorPool.h"

#include "VulkanRendererAPI.h"
#include "VulkanSwapchain.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanDescriptorPool::VulkanDescriptorPool()
		: m_DescriptorPool(VK_NULL_HANDLE)
	{
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
	}

	void VulkanDescriptorPool::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes;
		descriptorPoolSizes[0] = {};
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanRendererAPI::s_Swapchain->GetImages().size());
		descriptorPoolSizes[1] = {};
		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(VulkanRendererAPI::s_Swapchain->GetImages().size());

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(VulkanRendererAPI::s_Swapchain->GetImages().size());
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

		VK_ASSERT(vkCreateDescriptorPool(VulkanRendererAPI::s_Device->GetInstanceHandle(), &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));
	}

	void VulkanDescriptorPool::Destroy()
	{
		// Implicitly destroys the descriptor sets created from it
		vkDestroyDescriptorPool(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_DescriptorPool, nullptr);
	}

}
