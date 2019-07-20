#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanDevice;

	/*
	Depends on:
	- Device
	*/
	class VulkanSemaphore
	{
	public:
		VulkanSemaphore(VulkanDevice& Device);
		~VulkanSemaphore();

		void CreateSemaphore();
		void Destroy();

		inline const VkSemaphore& GetHandle() const { return Semaphore; }

	private:
		VulkanDevice& Device;

		VkSemaphore Semaphore;
	};

}
