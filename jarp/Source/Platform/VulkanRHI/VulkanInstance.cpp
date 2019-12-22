#include "jarppch.h"
#include "VulkanInstance.h"

#include "VulkanDebug.h"
#include "VulkanUtils.hpp"

namespace jarp {

	VulkanInstance::VulkanInstance()
		: m_Instance{ VK_NULL_HANDLE }
	{
		VK_ASSERT(volkInitialize());
	}

	VulkanInstance::~VulkanInstance()
	{
	}

	void VulkanInstance::CreateInstance()
	{
		// Get the required extensions from the displaying window
		//m_InstanceExtensions = Window.GetInstanceExtensions();
		m_InstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		m_InstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		m_InstanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

#if defined(_DEBUG)
		m_InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		// Check layer support
		uint32_t layerPropertyCount;
		vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);
		std::vector<VkLayerProperties> layerProperties(layerPropertyCount);
		vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data());

		for (const char* layerName : m_InstanceLayers)
		{
			bool bLayerFound = false;
			for (const auto& layerProperty : layerProperties)
			{
				if (strcmp(layerName, layerProperty.layerName) == 0)
				{
					bLayerFound = true;
					break;
				}
			}
			if (!bLayerFound)
				throw std::runtime_error("Not all layers supported!");
		}

		// Create instance
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = nullptr;
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.pEngineName = "JARP";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_1; // TODO: check for availability of that version

		VkInstanceCreateInfo instInfo = {};
		instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instInfo.pNext = nullptr;
		instInfo.flags = 0;
		instInfo.pApplicationInfo = &appInfo;
		instInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensions.size());
		instInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();
#if defined(_DEBUG)
		for (auto& LayerName : VulkanDebug::ValidationLayers)
			m_InstanceLayers.push_back(LayerName);
#endif
		instInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayers.size());
		instInfo.ppEnabledLayerNames = m_InstanceLayers.data();

		VK_ASSERT(vkCreateInstance(&instInfo, nullptr, &m_Instance));

		volkLoadInstance(m_Instance);

#if defined(_DEBUG)
		VulkanDebug::SetupDebugCallback(m_Instance);
#endif
	}

	void VulkanInstance::Destroy()
	{
#if defined(_DEBUG)
		VulkanDebug::DestroyDebugCallback(m_Instance);
#endif
		vkDestroyInstance(m_Instance, nullptr);
	}

}
