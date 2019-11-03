#include "jarppch.h"
#include "VulkanDebug.h"

#include "VulkanUtils.hpp"
#include "jarp/Utils.hpp"

namespace jarp {

	namespace VulkanDebug
	{
		VkDebugUtilsMessengerEXT DebugUtilsMessengerEXT;
		std::vector<const char*> ValidationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			const char* Type;
			if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
				Type = "GENERAL";
			else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
				Type = "VALIDATION";
			else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
				Type = "PERFORMANCE";

			if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
				JARP_CORE_TRACE("VERBOSE | {0} - Validation layer: {1}", Type, pCallbackData->pMessage);
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
				JARP_CORE_INFO("INFO | {0} - Validation layer: {1}", Type, pCallbackData->pMessage);
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
				JARP_CORE_WARN("WARNING | {0} - Validation layer: {1}", Type, pCallbackData->pMessage);
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
				JARP_CORE_ERROR("ERROR | {0} - Validation layer: {1}", Type, pCallbackData->pMessage);

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

			vkCreateDebugUtilsMessengerEXT(Instance, &DebugUtilsMessengerCreateInfoEXT, nullptr, &DebugUtilsMessengerEXT);
		}

		void DestroyDebugCallback(const VkInstance& Instance)
		{
			vkDestroyDebugUtilsMessengerEXT(Instance, DebugUtilsMessengerEXT, nullptr);
		}

	}

}
