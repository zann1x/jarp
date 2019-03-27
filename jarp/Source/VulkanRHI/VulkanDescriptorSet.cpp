#include "VulkanDescriptorSet.h"

#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include "VulkanUtils.hpp"

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice& OutDevice)
	: Device(OutDevice)
{
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
}

void VulkanDescriptorSet::CreateDescriptorSets(const VulkanDescriptorSetLayout& DescriptorSetLayout, const VulkanDescriptorPool& DescriptorPool, const size_t Amount, const VkDeviceSize Size, const std::vector<VkBuffer>& Buffers)
{
	std::vector<VkDescriptorSetLayout> DescriptorSetLayouts(Amount, DescriptorSetLayout.GetHandle());

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.pNext = nullptr;
	DescriptorSetAllocateInfo.descriptorPool = DescriptorPool.GetHandle();
	DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
	DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayouts.data();

	DescriptorSets.resize(Amount);
	VK_ASSERT(vkAllocateDescriptorSets(Device.GetInstanceHandle(), &DescriptorSetAllocateInfo, DescriptorSets.data()));

	for (size_t i = 0; i < Amount; ++i)
	{
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = Buffers[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = Size;

		VkWriteDescriptorSet WriteDescriptorSet = {};
		WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptorSet.pNext = nullptr;
		WriteDescriptorSet.dstSet = DescriptorSets[i];
		WriteDescriptorSet.dstBinding = 0;
		WriteDescriptorSet.dstArrayElement = 0;
		WriteDescriptorSet.descriptorCount = 1;
		WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		WriteDescriptorSet.pImageInfo = nullptr;
		WriteDescriptorSet.pBufferInfo = &DescriptorBufferInfo;
		WriteDescriptorSet.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Device.GetInstanceHandle(), 1, &WriteDescriptorSet, 0, nullptr);
	}
}
