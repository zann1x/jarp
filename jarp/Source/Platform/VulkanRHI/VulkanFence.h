#pragma once

#include <volk.h>

namespace jarp {

	class VulkanFence
	{
	public:
		VulkanFence();
		~VulkanFence();

		void CreateFence();
		void Destroy();

		inline const VkFence GetHandle() const { return Fence; }
		inline const VkFence* GetHandlePointer() const { return &Fence; }

	private:
		VkFence Fence;
	};

}
