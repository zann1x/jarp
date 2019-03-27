#include "VulkanDevice.h"

#include "VulkanQueue.h"
#include "VulkanUtils.hpp"

#include <assert.h>

VulkanDevice::VulkanDevice(VkPhysicalDevice PhysicalDevice)
	: PhysicalDevice(PhysicalDevice)
{
	vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &PhysicalDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
	
	uint32_t QueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	assert(QueueFamilyCount > 0);
	QueueFamilyProperties.resize(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

	uint32_t PropertyCount;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &PropertyCount, nullptr);
	if (PropertyCount > 0)
	{
		std::vector<VkExtensionProperties> ExtensionProperties(PropertyCount);
		VK_ASSERT(vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &PropertyCount, ExtensionProperties.data()));
		for (auto Extension : ExtensionProperties)
		{
			SupportedExtensions.push_back(Extension.extensionName);
		}
	}

	if (std::find(SupportedExtensions.begin(), SupportedExtensions.end(), VK_KHR_SWAPCHAIN_EXTENSION_NAME) != SupportedExtensions.end())
	{
		EnabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
	else
	{
		throw std::runtime_error("Not all required extensions supported by the physical device!");
	}
}

VulkanDevice::~VulkanDevice()
{
	delete PresentQueue;
	delete TransferQueue;
	delete ComputeQueue;
	delete GraphicsQueue;
	

	if (LogicalDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(LogicalDevice, nullptr);
		LogicalDevice = VK_NULL_HANDLE;
	}
}

void VulkanDevice::SetupPresentQueue(VkSurfaceKHR Surface)
{
	VkBool32 PresentSupported;
	vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, GraphicsQueue->GetFamilyIndex(), Surface, &PresentSupported);
	if (QueueFamilyProperties[GraphicsQueue->GetFamilyIndex()].queueCount > 0 && PresentSupported)
	{
		PresentQueue = new VulkanQueue(*this, GraphicsQueue->GetFamilyIndex());
	}
	else
	{
		std::runtime_error("Graphics queue does not support present!");
	}
}

void VulkanDevice::CreateLogicalDevice()
{
	// Prepare Queue Info for Device creation
	std::vector<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos;
	int NumberOfQueuePriorities = 0;
	uint32_t GraphicsFamilyIndex = -1;
	uint32_t ComputeFamilyIndex = -1;
	uint32_t TransferFamilyIndex = -1;

	for (uint32_t i = 0; i < static_cast<uint32_t>(QueueFamilyProperties.size()); i++)
	{
		bool IsValidQueue = false;

		if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			if (GraphicsFamilyIndex == -1)
			{
				GraphicsFamilyIndex = i;
				IsValidQueue = true;
			}
		}

		if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			if (ComputeFamilyIndex == -1)
			{
				ComputeFamilyIndex = i;
				IsValidQueue = true;
			}
		}

		if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			if (TransferFamilyIndex == -1)
			{
				TransferFamilyIndex = i;
				IsValidQueue = true;
			}
		}

		if (!IsValidQueue)
			continue;

		VkDeviceQueueCreateInfo DeviceQueueCreateInfo = {};
		DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		DeviceQueueCreateInfo.pNext = nullptr;
		DeviceQueueCreateInfo.flags = 0;
		DeviceQueueCreateInfo.queueFamilyIndex = i;
		DeviceQueueCreateInfo.queueCount = QueueFamilyProperties[i].queueCount;
		NumberOfQueuePriorities += QueueFamilyProperties[i].queueCount;

		DeviceQueueCreateInfos.push_back(DeviceQueueCreateInfo);
	}

	std::vector<float> QueuePriorities(NumberOfQueuePriorities, 1.0f);
	for (auto& QueueCreateInfo : DeviceQueueCreateInfos)
		QueueCreateInfo.pQueuePriorities = QueuePriorities.data();

	if (GraphicsFamilyIndex == -1)
		throw std::runtime_error("Could not find a queue family supporting VK_QUEUE_GRAPHICS_BIT!");
	if (ComputeFamilyIndex == -1)
		ComputeFamilyIndex = GraphicsFamilyIndex;
	if (TransferFamilyIndex == -1)
		TransferFamilyIndex = ComputeFamilyIndex;

	// Create the logical device
	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = nullptr;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(DeviceQueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = 0; // deprecated
	DeviceCreateInfo.ppEnabledLayerNames = nullptr; // deprecated
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(EnabledExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = EnabledExtensions.data();
	//DeviceCreateInfo.pEnabledFeatures = &EnabledPhysicalDeviceFeatures;

	VK_ASSERT(vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &LogicalDevice));

	// Get queues of the logical device
	GraphicsQueue = new VulkanQueue(*this, GraphicsFamilyIndex);
	ComputeQueue = new VulkanQueue(*this, ComputeFamilyIndex);
	TransferQueue = new VulkanQueue(*this, TransferFamilyIndex);
}

void VulkanDevice::WaitUntilIdle()
{
	VK_ASSERT(vkDeviceWaitIdle(LogicalDevice));
}
