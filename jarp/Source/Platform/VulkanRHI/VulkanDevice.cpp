#include "VulkanDevice.h"

#include "Platform/VulkanRHI/VulkanRendererAPI.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"

#include <algorithm>
#include <assert.h>

namespace jarp {

	VulkanDevice::VulkanDevice()
		: m_PhysicalDevice(VK_NULL_HANDLE)
	{
		m_Surface = new VulkanSurface();
		m_Surface->CreateSurface();

		PickPhysicalDevice();

		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_PhysicalDeviceMemoryProperties);

		// Check for desired features
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_PhysicalDeviceFeatures);
		m_EnabledPhysicalDeviceFeatures = { };
		if (!m_PhysicalDeviceFeatures.samplerAnisotropy)
			throw std::runtime_error("Not all enabled features are supported");
		m_EnabledPhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE; // for sampler creation

		// Get queue family properties for later use
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
		assert(queueFamilyCount > 0);
		m_QueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

		// Check for desired device extensions
		uint32_t propertyCount;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, nullptr);
		if (propertyCount > 0)
		{
			std::vector<VkExtensionProperties> extensionProperties(propertyCount);
			VK_ASSERT(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, extensionProperties.data()));
			for (auto extension : extensionProperties)
			{
				m_SupportedExtensions.push_back(extension.extensionName);
			}
		}

		if (std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), VK_KHR_SWAPCHAIN_EXTENSION_NAME) != m_SupportedExtensions.end())
			m_EnabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		else
			throw std::runtime_error("Not all required extensions supported by the physical device!");
	}

	VulkanDevice::~VulkanDevice()
	{
	}

	void VulkanDevice::Destroy()
	{
		delete m_PresentQueue;
		delete m_TransferQueue;
		delete m_ComputeQueue;
		delete m_GraphicsQueue;
		m_Surface->Destroy();
		delete m_Surface;

		if (m_LogicalDevice != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
			m_LogicalDevice = VK_NULL_HANDLE;
		}
	}

	void VulkanDevice::SetupPresentQueue(VkSurfaceKHR surface)
	{
		VkBool32 bIsPresentSupported;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_GraphicsQueue->GetFamilyIndex(), surface, &bIsPresentSupported);

		if (m_QueueFamilyProperties[m_GraphicsQueue->GetFamilyIndex()].queueCount > 0 && bIsPresentSupported)
			m_PresentQueue = new VulkanQueue(m_GraphicsQueue->GetFamilyIndex());
		else
			std::runtime_error("Graphics queue does not support present!");
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		// Prepare m_Queue Info for Device creation
		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		int numberOfQueuePriorities = 0;
		uint32_t graphicsFamilyIndex = -1;
		uint32_t computeFamilyIndex = -1;
		uint32_t transferFamilyIndex = -1;

		// Pick the first found queue that supports a desired queue
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++)
		{
			bool bIsValidQueue = false;

			if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (graphicsFamilyIndex == -1)
				{
					graphicsFamilyIndex = i;
					bIsValidQueue = true;
				}
			}

			if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (computeFamilyIndex == -1)
				{
					computeFamilyIndex = i;
					bIsValidQueue = true;
				}
			}

			if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (transferFamilyIndex == -1)
				{
					transferFamilyIndex = i;
					bIsValidQueue = true;
				}
			}

			if (!bIsValidQueue)
				continue;

			VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.pNext = nullptr;
			deviceQueueCreateInfo.flags = 0;
			deviceQueueCreateInfo.queueFamilyIndex = i;
			deviceQueueCreateInfo.queueCount = m_QueueFamilyProperties[i].queueCount;
			numberOfQueuePriorities += m_QueueFamilyProperties[i].queueCount;

			deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
		}

		std::vector<float> queuePriorities(numberOfQueuePriorities, 1.0f);
		for (auto& queueCreateInfo : deviceQueueCreateInfos)
			queueCreateInfo.pQueuePriorities = queuePriorities.data();

		if (graphicsFamilyIndex == -1)
			throw std::runtime_error("Could not find a queue family supporting VK_QUEUE_GRAPHICS_BIT!");
		if (computeFamilyIndex == -1)
			computeFamilyIndex = graphicsFamilyIndex;
		if (transferFamilyIndex == -1)
			transferFamilyIndex = computeFamilyIndex;

		// Create the logical device
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount = 0; // deprecated
		deviceCreateInfo.ppEnabledLayerNames = nullptr; // deprecated
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_EnabledExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &m_EnabledPhysicalDeviceFeatures;

		VK_ASSERT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice));

		// Get queues of the logical device
		m_GraphicsQueue = new VulkanQueue(graphicsFamilyIndex);
		m_ComputeQueue = new VulkanQueue(computeFamilyIndex);
		m_TransferQueue = new VulkanQueue(transferFamilyIndex);

		SetupPresentQueue(m_Surface->GetHandle());
	}

	void VulkanDevice::PickPhysicalDevice()
	{
		// TODO: properly check queue families, device extensions, device features and swapchain support here
		uint32_t physicalDeviceCount;
		VK_ASSERT(vkEnumeratePhysicalDevices(VulkanRendererAPI::s_Instance->GetHandle(), &physicalDeviceCount, nullptr));
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		VK_ASSERT(vkEnumeratePhysicalDevices(VulkanRendererAPI::s_Instance->GetHandle(), &physicalDeviceCount, physicalDevices.data()));

		// Get info about the available physical devices and pick one for use
		for (size_t i = 0; i < physicalDevices.size(); ++i)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, nullptr);
			if (queueFamilyCount < 1)
				continue;

			m_PhysicalDevice = physicalDevices[i];

			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				break;
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("No suitable physical device found!");
	}

	uint32_t VulkanDevice::GetMemoryTypeIndex(const uint32_t memoryTypeBits, const VkMemoryPropertyFlags memoryProperties) const
	{
		for (uint32_t i = 0; i < m_PhysicalDeviceMemoryProperties.memoryTypeCount; ++i)
		{
			// Pick the first found memory type that supports the memory type bits
			if ((memoryTypeBits & (1 << i) && (m_PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties))
			{
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type for buffer!");
	}

	VkFormat VulkanDevice::FindSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling imageTiling, VkFormatFeatureFlags formatFeatureFlags)
	{
		for (VkFormat format : formats)
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);

			if (imageTiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & formatFeatureFlags) == formatFeatureFlags)
				return format;
			else if (imageTiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & formatFeatureFlags) == formatFeatureFlags)
				return format;
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
		VK_ASSERT(vkDeviceWaitIdle(m_LogicalDevice));
	}

}
