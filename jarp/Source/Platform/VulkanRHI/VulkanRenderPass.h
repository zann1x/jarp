#pragma once

#include <vulkan/vulkan.h>

namespace jarp {

	class VulkanSwapchain;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanSwapchain& OutSwapchain);
		~VulkanRenderPass();

		void CreateRenderPass();
		void Destroy();

		inline const VkRenderPass& GetHandle() const { return RenderPass; }

	private:
		VulkanSwapchain& Swapchain;

		VkRenderPass RenderPass;
	};

}
