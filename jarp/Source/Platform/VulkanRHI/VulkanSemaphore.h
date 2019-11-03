#pragma once

#include <volk.h>

namespace jarp {

	class VulkanSemaphore
	{
	public:
		VulkanSemaphore();
		~VulkanSemaphore();

		void CreateSemaphore();
		void Destroy();

		inline const VkSemaphore& GetHandle() const { return Semaphore; }

	private:
		VkSemaphore Semaphore;
	};

}
