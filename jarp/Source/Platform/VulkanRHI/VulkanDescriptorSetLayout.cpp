#include "jarppch.h"
#include "VulkanDescriptorSetLayout.h"

#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
		: m_DescriptorSetLayout(VK_NULL_HANDLE)
	{
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
	}

	void VulkanDescriptorSetLayout::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(m_DescriptorSetLayoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = m_DescriptorSetLayoutBindings.data();

		VK_ASSERT(vkCreateDescriptorSetLayout(VulkanRendererAPI::s_Device->GetInstanceHandle(), &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
	}

	void VulkanDescriptorSetLayout::AddLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags)
	{
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = descriptorType;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = stageFlags;
		descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

		m_DescriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
	}

	void VulkanDescriptorSetLayout::Destroy()
	{
		vkDestroyDescriptorSetLayout(VulkanRendererAPI::s_Device->GetInstanceHandle(), m_DescriptorSetLayout, nullptr);
	}

}
