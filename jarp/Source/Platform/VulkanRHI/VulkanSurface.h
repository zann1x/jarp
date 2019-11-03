#pragma once

#include <volk.h>

namespace jarp {

	class Window;

	class VulkanSurface
	{
	public:
		VulkanSurface();
		~VulkanSurface();

		void CreateSurface();
		void Destroy();

		inline VkSurfaceKHR GetHandle() const { return SurfaceKHR; }

	private:
		VkSurfaceKHR SurfaceKHR;
	};

}
