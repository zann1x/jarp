#include "jarppch.h"
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

	void VulkanDescriptorSet::CreateDescriptorSets(const VulkanDescriptorSetLayout& DescriptorSetLayout, const VulkanDescriptorPool& DescriptorPool, const size_t Amount, const VkDeviceSize Size, const std::vector<VkBuffer>& Buffers, VkSampler Sampler, VkImageView ImageView)
	{
		std::vector<VkDescriptorSetLayout> DescriptorSetLayouts(Amount, DescriptorSetLayout.GetHandle());

		VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
		DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		DescriptorSetAllocateInfo.pNext = nullptr;
		DescriptorSetAllocateInfo.descriptorPool = DescriptorPool.GetHandle();
		DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
		DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayouts.data();

		DescriptorSets.resize(Amount);
		VK_ASSERT(vkAllocateDescriptorSets(VulkanRendererAPI::pDevice->GetInstanceHandle(), &DescriptorSetAllocateInfo, DescriptorSets.data()));

		for (size_t i = 0; i < Amount; ++i)
		{
			VkDescriptorBufferInfo DescriptorBufferInfo = {};
			DescriptorBufferInfo.buffer = Buffers[i];
			DescriptorBufferInfo.offset = 0;
			DescriptorBufferInfo.range = Size;

			VkDescriptorImageInfo DescriptorImageInfo = {};
			DescriptorImageInfo.sampler = Sampler;
			DescriptorImageInfo.imageView = ImageView;
			DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			std::array<VkWriteDescriptorSet, 2> WriteDescriptorSets;
			WriteDescriptorSets[0] = {};
			WriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			WriteDescriptorSets[0].pNext = nullptr;
			WriteDescriptorSets[0].dstSet = DescriptorSets[i];
			WriteDescriptorSets[0].dstBinding = 0;
			WriteDescriptorSets[0].dstArrayElement = 0;
			WriteDescriptorSets[0].descriptorCount = 1;
			WriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			WriteDescriptorSets[0].pImageInfo = nullptr;
			WriteDescriptorSets[0].pBufferInfo = &DescriptorBufferInfo;
			WriteDescriptorSets[0].pTexelBufferView = nullptr;
			WriteDescriptorSets[1] = {};
			WriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			WriteDescriptorSets[1].pNext = nullptr;
			WriteDescriptorSets[1].dstSet = DescriptorSets[i];
			WriteDescriptorSets[1].dstBinding = 1;
			WriteDescriptorSets[1].dstArrayElement = 0;
			WriteDescriptorSets[1].descriptorCount = 1;
			WriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			WriteDescriptorSets[1].pImageInfo = &DescriptorImageInfo;
			WriteDescriptorSets[1].pBufferInfo = nullptr;
			WriteDescriptorSets[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(VulkanRendererAPI::pDevice->GetInstanceHandle(), static_cast<uint32_t>(WriteDescriptorSets.size()), WriteDescriptorSets.data(), 0, nullptr);
		}
	}

}
