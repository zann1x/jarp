#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanDevice;

	class VulkanFence
	{
	public:
		VulkanFence(VulkanDevice& Device);
		~VulkanFence();

		void CreateFence();
		void Destroy();

		inline const VkFence GetHandle() const { return Fence; }
		inline const VkFence* GetHandlePointer() const { return &Fence; }

	private:
		VulkanDevice& Device;
		VkFence Fence;
	};

}
