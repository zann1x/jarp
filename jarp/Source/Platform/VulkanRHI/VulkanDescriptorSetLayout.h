#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanDevice;

	/*
	Depends on:
	- Device
	*/
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout(VulkanDevice& OutDevice);
		~VulkanDescriptorSetLayout();

		void CreateDescriptorSetLayout();
		void Destroy();

		void AddLayout(uint32_t Binding, VkDescriptorType DescriptorType, VkShaderStageFlags StageFlags);

		inline const VkDescriptorSetLayout& GetHandle() const { return DescriptorSetLayout; }

	private:
		VulkanDevice& Device;

		std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
		VkDescriptorSetLayout DescriptorSetLayout;
	};

}
