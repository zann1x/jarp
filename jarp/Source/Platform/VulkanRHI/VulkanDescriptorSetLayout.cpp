#include "jarppch.h"
#include "VulkanDescriptorSetLayout.h"

#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice& OutDevice)
		: Device(OutDevice)
	{
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
	}

	void VulkanDescriptorSetLayout::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
		DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		DescriptorSetLayoutCreateInfo.pNext = nullptr;
		DescriptorSetLayoutCreateInfo.flags = 0;
		DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(DescriptorSetLayoutBindings.size());
		DescriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindings.data();

		VK_ASSERT(vkCreateDescriptorSetLayout(Device.GetInstanceHandle(), &DescriptorSetLayoutCreateInfo, nullptr, &DescriptorSetLayout));
	}

	void VulkanDescriptorSetLayout::AddLayout(uint32_t Binding, VkDescriptorType DescriptorType, VkShaderStageFlags StageFlags)
	{
		VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding = {};
		DescriptorSetLayoutBinding.binding = Binding;
		DescriptorSetLayoutBinding.descriptorType = DescriptorType;
		DescriptorSetLayoutBinding.descriptorCount = 1;
		DescriptorSetLayoutBinding.stageFlags = StageFlags;
		DescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

		DescriptorSetLayoutBindings.push_back(DescriptorSetLayoutBinding);
	}

	void VulkanDescriptorSetLayout::Destroy()
	{
		vkDestroyDescriptorSetLayout(Device.GetInstanceHandle(), DescriptorSetLayout, nullptr);
	}

}
