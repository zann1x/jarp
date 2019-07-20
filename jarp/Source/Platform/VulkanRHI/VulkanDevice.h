#pragma once

#include <vulkan/vulkan.h>

class VulkanInstance;
class VulkanQueue;

class VulkanDevice
{
public:
	VulkanDevice(VulkanInstance& Instance);
	~VulkanDevice();

	void SetupPresentQueue(VkSurfaceKHR Surface);
	void CreateLogicalDevice();

	inline const VkDevice GetInstanceHandle() const  { return LogicalDevice; }
	inline const VkPhysicalDevice GetPhysicalHandle() const { return PhysicalDevice; }
	inline const VulkanQueue& GetGraphicsQueue() const { return *GraphicsQueue; }
	inline const VulkanQueue& GetPresentQueue() const { return *PresentQueue; }

	void PickPhysicalDevice();

	uint32_t GetMemoryTypeIndex(uint32_t MemoryTypeBits, const VkMemoryPropertyFlags MemoryProperties) const;
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& Formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags);
	VkFormat FindDepthFormat();

	void WaitUntilIdle();

private:
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
	VkPhysicalDeviceFeatures EnabledPhysicalDeviceFeatures;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
	std::vector<std::string> SupportedExtensions;
	std::vector<const char*> EnabledExtensions;

	VulkanInstance& Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice LogicalDevice;

	VulkanQueue* GraphicsQueue;
	VulkanQueue* ComputeQueue;
	VulkanQueue* TransferQueue;
	VulkanQueue* PresentQueue;
};
