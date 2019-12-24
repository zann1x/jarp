#include "VulkanDebug.h"

#include "VulkanUtils.hpp"
#include "jarp/Log.h"
#include "jarp/Utils.hpp"

namespace jarp {

	namespace VulkanDebug
	{
		VkDebugUtilsMessengerEXT debugUtilsMessengerEXT;
		std::vector<const char*> ValidationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			const char* type;
			if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
				type = "GENERAL";
			else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
				type = "VALIDATION";
			else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
				type = "PERFORMANCE";

			if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
				JARP_CORE_TRACE("VERBOSE | {0} - Validation layer: {1}", type, pCallbackData->pMessage);
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
				JARP_CORE_INFO("INFO | {0} - Validation layer: {1}", type, pCallbackData->pMessage);
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
				JARP_CORE_WARN("WARNING | {0} - Validation layer: {1}", type, pCallbackData->pMessage);
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
				JARP_CORE_ERROR("ERROR | {0} - Validation layer: {1}", type, pCallbackData->pMessage);

			return VK_FALSE;
		}

		void SetupDebugCallback(const VkInstance& instance)
		{
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT = {};
			debugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCreateInfoEXT.pNext = nullptr;
			debugUtilsMessengerCreateInfoEXT.flags = 0;
			debugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerCreateInfoEXT.pfnUserCallback = debugCallback;
			debugUtilsMessengerCreateInfoEXT.pUserData = nullptr;

			vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfoEXT, nullptr, &debugUtilsMessengerEXT);
		}

		void DestroyDebugCallback(const VkInstance& instance)
		{
			vkDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessengerEXT, nullptr);
		}

	}

}
