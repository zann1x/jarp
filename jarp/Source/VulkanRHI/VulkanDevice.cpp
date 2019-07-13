#include "VulkanDevice.h"

#include "VulkanInstance.h"
#include "VulkanQueue.h"
#include "VulkanUtils.hpp"

#include <assert.h>

VulkanDevice::VulkanDevice(VulkanInstance& Instance)
	: Instance(Instance), PhysicalDevice(VK_NULL_HANDLE)
{
	PickPhysicalDevice();

	vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);

	// Check for desired features
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &PhysicalDeviceFeatures);
	EnabledPhysicalDeviceFeatures = {};
	if (!PhysicalDeviceFeatures.samplerAnisotropy)
		throw std::runtime_error("Not all enabled features are supported");
	EnabledPhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE; // for sampler creation
	
	// Get queue family properties for later use
	uint32_t QueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	assert(QueueFamilyCount > 0);
	QueueFamilyProperties.resize(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

	// Check for desired device extensions
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
		EnabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	else
		throw std::runtime_error("Not all required extensions supported by the physical device!");
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
		PresentQueue = new VulkanQueue(*this, GraphicsQueue->GetFamilyIndex());
	else
		std::runtime_error("Graphics queue does not support present!");
}

void VulkanDevice::CreateLogicalDevice()
{
	// Prepare Queue Info for Device creation
	std::vector<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos;
	int NumberOfQueuePriorities = 0;
	uint32_t GraphicsFamilyIndex = -1;
	uint32_t ComputeFamilyIndex = -1;
	uint32_t TransferFamilyIndex = -1;
	
	// Pick the first found queue that supports a desired queue
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
	DeviceCreateInfo.pEnabledFeatures = &EnabledPhysicalDeviceFeatures;

	VK_ASSERT(vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &LogicalDevice));

	// Get queues of the logical device
	GraphicsQueue = new VulkanQueue(*this, GraphicsFamilyIndex);
	ComputeQueue = new VulkanQueue(*this, ComputeFamilyIndex);
	TransferQueue = new VulkanQueue(*this, TransferFamilyIndex);
}

void VulkanDevice::PickPhysicalDevice()
{
	uint32_t PhysicalDeviceCount;
	VK_ASSERT(vkEnumeratePhysicalDevices(Instance.GetHandle(), &PhysicalDeviceCount, nullptr));
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(Instance.GetHandle(), &PhysicalDeviceCount, PhysicalDevices.data()));

	// Get info about the available physical devices and pick one for use
	for (size_t i = 0; i < PhysicalDevices.size(); ++i)
	{
		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(PhysicalDevices[i], &PhysicalDeviceProperties);

		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[i], &QueueFamilyCount, nullptr);
		if (QueueFamilyCount < 1)
			continue;

		PhysicalDevice = PhysicalDevices[i];

		if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			break;
	}

	if (PhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No suitable physical device found!");
}

uint32_t VulkanDevice::GetMemoryTypeIndex(const uint32_t MemoryTypeBits, const VkMemoryPropertyFlags MemoryProperties) const
{
	for (uint32_t i = 0; i < PhysicalDeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if ((MemoryTypeBits & (1 << i) && (PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & MemoryProperties) == MemoryProperties))
		{
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type for buffer!");
}

VkFormat VulkanDevice::FindSupportedFormat(const std::vector<VkFormat>& Formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags)
{
	for (VkFormat Format : Formats)
	{
		VkFormatProperties FormatProperties;
		vkGetPhysicalDeviceFormatProperties(PhysicalDevice, Format, &FormatProperties);

		if (ImageTiling == VK_IMAGE_TILING_LINEAR && (FormatProperties.linearTilingFeatures & FormatFeatureFlags) == FormatFeatureFlags)
			return Format;
		else if (ImageTiling == VK_IMAGE_TILING_OPTIMAL && (FormatProperties.optimalTilingFeatures & FormatFeatureFlags) == FormatFeatureFlags)
			return Format;
	}

	throw std::runtime_error("Failed to find a supported image format");
}

VkFormat VulkanDevice::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

void VulkanDevice::WaitUntilIdle()
{
	VK_ASSERT(vkDeviceWaitIdle(LogicalDevice));
}
