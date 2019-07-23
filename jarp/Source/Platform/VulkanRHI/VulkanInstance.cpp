#include "jarppch.h"
#include "VulkanInstance.h"

#include "VulkanDebug.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanInstance::VulkanInstance()
	{
	}

	VulkanInstance::~VulkanInstance()
	{
	}

	void VulkanInstance::CreateInstance()
	{
		// Get the required extensions from the displaying window
		//InstanceExtensions = Window.GetInstanceExtensions();
#if defined(JARP_PLATFORM_WINDOWS)
		InstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		InstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#if defined(_DEBUG)
		InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		// Check layer support
		uint32_t LayerPropertyCount;
		vkEnumerateInstanceLayerProperties(&LayerPropertyCount, nullptr);
		std::vector<VkLayerProperties> LayerProperties(LayerPropertyCount);
		vkEnumerateInstanceLayerProperties(&LayerPropertyCount, LayerProperties.data());

		for (const char* LayerName : InstanceLayers)
		{
			bool LayerFound = false;
			for (const auto& LayerProperty : LayerProperties)
			{
				if (strcmp(LayerName, LayerProperty.layerName) == 0)
				{
					LayerFound = true;
					break;
				}
			}
			if (!LayerFound)
				throw std::runtime_error("Not all layers supported!");
		}

		// Create instance
		VkApplicationInfo AppInfo = {};
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pNext = nullptr;
		AppInfo.pApplicationName = nullptr;
		AppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		AppInfo.pEngineName = "JARP";
		AppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		AppInfo.apiVersion = VK_API_VERSION_1_1; // TODO: check for availability of that version

		VkInstanceCreateInfo InstInfo = {};
		InstInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		InstInfo.pNext = nullptr;
		InstInfo.flags = 0;
		InstInfo.pApplicationInfo = &AppInfo;
		InstInfo.enabledExtensionCount = static_cast<uint32_t>(InstanceExtensions.size());
		InstInfo.ppEnabledExtensionNames = InstanceExtensions.data();
#if defined(_DEBUG)
		for (auto& LayerName : VulkanDebug::ValidationLayers)
			InstanceLayers.push_back(LayerName);
#endif
		InstInfo.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
		InstInfo.ppEnabledLayerNames = InstanceLayers.data();

		VK_ASSERT(vkCreateInstance(&InstInfo, nullptr, &Instance));

#if defined(_DEBUG)
		VulkanDebug::SetupDebugCallback(Instance);
#endif
	}

	void VulkanInstance::Destroy()
	{
#if defined(_DEBUG)
		VulkanDebug::DestroyDebugCallback(Instance);
#endif
		vkDestroyInstance(Instance, nullptr);
	}

}
