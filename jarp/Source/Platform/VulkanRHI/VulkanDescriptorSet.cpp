#include "VulkanDescriptorSet.h"

#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanDescriptorSet::VulkanDescriptorSet()
	{
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
	}

	void VulkanDescriptorSet::CreateDescriptorSets(const VulkanDescriptorSetLayout& descriptorSetLayout, const VulkanDescriptorPool& descriptorPool, const size_t amount, const VkDeviceSize size, const std::vector<VkBuffer>& buffers, VkSampler sampler, VkImageView imageView)
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts(amount, descriptorSetLayout.GetHandle());

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool.GetHandle();
		descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		m_DescriptorSets.resize(amount);
		VK_ASSERT(vkAllocateDescriptorSets(VulkanRendererAPI::s_Device->GetInstanceHandle(), &descriptorSetAllocateInfo, m_DescriptorSets.data()));

		for (size_t i = 0; i < amount; ++i)
		{
			VkDescriptorBufferInfo descriptorBufferInfo = {};
			descriptorBufferInfo.buffer = buffers[i];
			descriptorBufferInfo.offset = 0;
			descriptorBufferInfo.range = size;

			VkDescriptorImageInfo descriptorImageInfo = {};
			descriptorImageInfo.sampler = sampler;
			descriptorImageInfo.imageView = imageView;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			std::array<VkWriteDescriptorSet, 2> writeDescriptorSets;
			writeDescriptorSets[0] = {};
			writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSets[0].pNext = nullptr;
			writeDescriptorSets[0].dstSet = m_DescriptorSets[i];
			writeDescriptorSets[0].dstBinding = 0;
			writeDescriptorSets[0].dstArrayElement = 0;
			writeDescriptorSets[0].descriptorCount = 1;
			writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSets[0].pImageInfo = nullptr;
			writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;
			writeDescriptorSets[0].pTexelBufferView = nullptr;
			writeDescriptorSets[1] = {};
			writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSets[1].pNext = nullptr;
			writeDescriptorSets[1].dstSet = m_DescriptorSets[i];
			writeDescriptorSets[1].dstBinding = 1;
			writeDescriptorSets[1].dstArrayElement = 0;
			writeDescriptorSets[1].descriptorCount = 1;
			writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;
			writeDescriptorSets[1].pBufferInfo = nullptr;
			writeDescriptorSets[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(VulkanRendererAPI::s_Device->GetInstanceHandle(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
		}
	}

}
