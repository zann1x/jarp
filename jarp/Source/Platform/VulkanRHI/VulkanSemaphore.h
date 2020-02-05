#pragma once

#include <volk.h>

namespace jarp {

	class VulkanSemaphore
	{
	public:
		VulkanSemaphore();
		~VulkanSemaphore();

		void Create();
		void Destroy();

		inline const VkSemaphore& GetHandle() const { return m_Semaphore; }

	private:
		VkSemaphore m_Semaphore;
	};

}
