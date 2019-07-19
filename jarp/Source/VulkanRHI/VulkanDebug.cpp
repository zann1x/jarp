#include "VulkanDebug.h"

#include "VulkanUtils.hpp"
#include "Utils.hpp"

namespace VulkanDebug
{
	VkDebugUtilsMessengerEXT DebugUtilsMessengerEXT;
	std::vector<const char*> ValidationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::string Severity;
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			Severity = "VERBOSE";
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			Severity = "INFO";
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			Severity = "WARNING";
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			Severity = "ERROR";

		std::string Type;
		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			Type = "GENERAL";
		else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			Type = "VALIDATION";
		else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			Type = "PERFORMANCE";

		CONSOLE_LOG(Severity.data() << " | " << Type.data() << " - Validation layer: " << pCallbackData->pMessage);
		return VK_FALSE;
	}

	void SetupDebugCallback(const VkInstance& Instance)
	{
		VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT = {};
		DebugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugUtilsMessengerCreateInfoEXT.pNext = nullptr;
		DebugUtilsMessengerCreateInfoEXT.flags = 0;
		DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		DebugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
		DebugUtilsMessengerCreateInfoEXT.pUserData = nullptr;

		auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
		if (Func != nullptr)
		{
			VK_ASSERT(Func(Instance, &DebugUtilsMessengerCreateInfoEXT, nullptr, &DebugUtilsMessengerEXT));
		}
		else
		{
			throw std::runtime_error("Debug extension not present!");
		}
	}

	void DestroyDebugCallback(const VkInstance& Instance)
	{
		auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
		if (Func != nullptr)
		{
			Func(Instance, DebugUtilsMessengerEXT, nullptr);
		}
	}

}
