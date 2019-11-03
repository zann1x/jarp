#pragma once

#include <volk.h>

namespace jarp {

	class VulkanCommandPool;

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(VulkanCommandPool& OutCommandPool);
		~VulkanCommandBuffer();

		void CreateCommandBuffer();
		void Destroy();

		inline const VkCommandBuffer& GetHandle() const { return CommandBuffer; }

		void BeginOneTimeSubmitCommand();
		void EndOneTimeSubmitCommand();

		void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);

	private:
		VulkanCommandPool& CommandPool;

		VkCommandBuffer CommandBuffer;
	};

}
