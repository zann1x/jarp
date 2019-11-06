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

		inline const VkFence GetHandle() const { return m_Fence; }
		inline const VkFence* GetHandlePointer() const { return &m_Fence; }

	private:
		VkFence m_Fence;
	};

}
