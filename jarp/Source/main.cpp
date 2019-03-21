#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

#define VK_ASSERT(val)\
	if (val != VK_SUCCESS)\
	{\
		__debugbreak();\
	}\

#define CONSOLE_LOG(msg) std::cout << msg << std::endl;

int main()
{
	// Use validation layers if this is a debug build
	std::vector<const char*> Layers;
#if defined(_DEBUG)
	Layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	// VkApplicationInfo allows the programmer to specifiy some basic information about the
	// program, which can be useful for layers and tools to provide more debug information.
	VkApplicationInfo AppInfo = {};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pNext = NULL;
	AppInfo.pApplicationName = NULL;
	AppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	AppInfo.pEngineName = "JACREP";
	AppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	AppInfo.apiVersion = VK_API_VERSION_1_1;

	// VkInstanceCreateInfo is where the programmer specifies the layers and/or extensions that
	// are needed. For now, none are enabled.
	VkInstanceCreateInfo InstInfo = {};
	InstInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstInfo.pNext = NULL;
	InstInfo.flags = 0;
	InstInfo.pApplicationInfo = &AppInfo;
	InstInfo.enabledExtensionCount = 0;
	InstInfo.ppEnabledExtensionNames = NULL;
	InstInfo.enabledLayerCount = static_cast<uint32_t>(Layers.size());
	InstInfo.ppEnabledLayerNames = Layers.data();

	// Create the Vulkan instance.
	VkInstance Instance;
	VK_ASSERT(vkCreateInstance(&InstInfo, NULL, &Instance));

	uint32_t PhysicalDeviceCount;
	vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, NULL);

	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.data()));

	// TODO pick physical device
	VkPhysicalDevice PhysicalDevice;
	for (VkPhysicalDevice Device : PhysicalDevices)
	{
		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(Device, &PhysicalDeviceProperties);

		VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
		vkGetPhysicalDeviceFeatures(Device, &PhysicalDeviceFeatures);

		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, NULL);
		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilyProperties.data());

		CONSOLE_LOG("Amount of queue familes: " << QueueFamilyProperties.size() << "\n")
		for (size_t i = 0; i < QueueFamilyProperties.size(); ++i)
		{
			CONSOLE_LOG("Queue Family #" << i)
			CONSOLE_LOG("VK_QUEUE_GRAPHICS_BIT	" << (QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			CONSOLE_LOG("VK_QUEUE_COMPUTE_BIT	" << (QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
			CONSOLE_LOG("VK_QUEUE_TRANSFER_BIT	" << (QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
			CONSOLE_LOG("Queue count: " << QueueFamilyProperties[i].queueCount)
			CONSOLE_LOG("Timestamp valid bits: " << QueueFamilyProperties[i].timestampValidBits)
			uint32_t width = QueueFamilyProperties[i].minImageTransferGranularity.width;
			uint32_t height = QueueFamilyProperties[i].minImageTransferGranularity.height;
			uint32_t depth = QueueFamilyProperties[i].minImageTransferGranularity.depth;
			CONSOLE_LOG("Min Image Timestamp Granularity: " << "w" << width << ", h" << height << ", d" << depth << "\n")
		}

		PhysicalDevice = Device;
	}

	// TODO set values based on chosen physical device
	//float QueuePriority = 1.f;
	//VkDeviceQueueCreateInfo DeviceQueueCreateInfo = {};
	//DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	//DeviceQueueCreateInfo.pNext = NULL;
	//DeviceQueueCreateInfo.flags = 0;
	//DeviceQueueCreateInfo.queueFamilyIndex;
	//DeviceQueueCreateInfo.queueCount;
	//DeviceQueueCreateInfo.pQueuePriorities = &QueuePriority;
	//
	//VkDeviceCreateInfo DeviceCreateInfo = {};
	//DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	//DeviceCreateInfo.pNext = NULL;
	//DeviceCreateInfo.flags = 0;
	//DeviceCreateInfo.queueCreateInfoCount;
	//DeviceCreateInfo.pQueueCreateInfos;
	//DeviceCreateInfo.enabledLayerCount;
	//DeviceCreateInfo.ppEnabledLayerNames;
	//DeviceCreateInfo.enabledExtensionCount;
	//DeviceCreateInfo.ppEnabledExtensionNames;
	//DeviceCreateInfo.pEnabledFeatures;



	vkDestroyInstance(Instance, NULL);

	return 0;
}
