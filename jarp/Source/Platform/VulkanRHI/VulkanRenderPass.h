#pragma once

#include <volk.h>

namespace jarp {

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass();
		~VulkanRenderPass();

		void CreateRenderPass();
		void Destroy();

		inline const VkRenderPass& GetHandle() const { return m_RenderPass; }

	private:
		VkRenderPass m_RenderPass;
	};

}
