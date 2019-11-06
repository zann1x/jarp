#pragma once

#include <volk.h>

#include "Platform/VulkanRHI/VulkanQueue.h"
#include "Platform/VulkanRHI/VulkanSurface.h"

namespace jarp {

	class VulkanQueue;

	class VulkanDevice
	{
	public:
		VulkanDevice();
		~VulkanDevice();

		void SetupPresentQueue(VkSurfaceKHR surface);
		void CreateLogicalDevice();
		void Destroy();

		inline const VkDevice GetInstanceHandle() const { return m_LogicalDevice; }
		inline const VkPhysicalDevice GetPhysicalHandle() const { return m_PhysicalDevice; }
		inline const VulkanSurface& GetSurface() const { return *m_Surface; }
		inline const VulkanQueue& GetGraphicsQueue() const { return *m_GraphicsQueue; }
		inline const VulkanQueue& GetPresentQueue() const { return *m_PresentQueue; }

		void PickPhysicalDevice();

		uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, const VkMemoryPropertyFlags memoryProperties) const;
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling imageTiling, VkFormatFeatureFlags formatFeatureFlags);
		VkFormat FindDepthFormat();

		void WaitUntilIdle();

	private:
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
		VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
		VkPhysicalDeviceFeatures m_EnabledPhysicalDeviceFeatures;
		VkPhysicalDeviceMemoryProperties m_PhysicalDeviceMemoryProperties;
		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::vector<std::string> m_SupportedExtensions;
		std::vector<const char*> m_EnabledExtensions;

		VulkanSurface* m_Surface;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;

		VulkanQueue* m_GraphicsQueue;
		VulkanQueue* m_ComputeQueue;
		VulkanQueue* m_TransferQueue;
		VulkanQueue* m_PresentQueue;
	};

}
