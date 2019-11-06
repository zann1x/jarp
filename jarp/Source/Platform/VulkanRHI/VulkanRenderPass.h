#pragma once

#include <volk.h>

namespace jarp {

	class VulkanSwapchain;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanSwapchain& swapchain);
		~VulkanRenderPass();

		void CreateRenderPass();
		void Destroy();

		inline const VkRenderPass& GetHandle() const { return m_RenderPass; }

	private:
		VulkanSwapchain& m_Swapchain;

		VkRenderPass m_RenderPass;
	};

}
