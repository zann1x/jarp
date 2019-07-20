#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanDevice;

	/*
	Depends on:
	- Device
	*/
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(VulkanDevice& OutDevice);
		~VulkanCommandPool();

		void CreateCommandPool(const VkCommandPoolCreateFlags Flags = 0);
		void Destroy();

		inline VkCommandPool GetHandle() { return CommandPool; };

	private:
		VulkanDevice& Device;
		VkCommandPool CommandPool;
	};

}
