#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <set>

class VulkanQueue;

class VulkanDevice
{
public:
	VulkanDevice(VkPhysicalDevice PhysicalDevice);
	~VulkanDevice();

	void SetupPresentQueue(VkSurfaceKHR Surface);
	void CreateLogicalDevice();

	inline VkDevice GetInstanceHandle() { return LogicalDevice; }
	inline VkPhysicalDevice GetPhysicalHandle() { return PhysicalDevice; }
	inline const VulkanQueue& GetGraphicsQueue() const { return *GraphicsQueue; }
	inline const VulkanQueue& GetPresentQueue() const { return *PresentQueue; }

	uint32_t GetMemoryTypeIndex(uint32_t MemoryTypeBits, const VkMemoryPropertyFlags MemoryProperties) const;

	void WaitUntilIdle();

private:
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
	VkPhysicalDeviceFeatures EnabledPhysicalDeviceFeatures;
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
	std::vector<std::string> SupportedExtensions;
	std::vector<const char*> EnabledExtensions;

	VkPhysicalDevice PhysicalDevice;
	VkDevice LogicalDevice;

	VulkanQueue* GraphicsQueue;
	VulkanQueue* ComputeQueue;
	VulkanQueue* TransferQueue;
	VulkanQueue* PresentQueue;
};
