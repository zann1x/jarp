// Define NOMINMAX to prevent the min and max function of the windows.h header to be defined
#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR // TODO is this define really needed ?
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

#include <string>
#include <fstream>

#define VK_ASSERT(val)\
	if (val != VK_SUCCESS)\
	{\
		__debugbreak();\
	}

#if defined(_DEBUG)
#define CONSOLE_LOG(msg)\
	std::cout << msg << std::endl
#else
#define CONSOLE_LOG(msg)
#endif // _DEBUG

///////////////// GLFW /////////////////

#define WIDTH 800
#define HEIGHT 600

GLFWwindow* Window;

bool IsFramebufferResized;
bool IsWindowIconified;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	IsFramebufferResized = true;
}

void WindowIconifyCallback(GLFWwindow* window, int iconified)
{
	if (iconified)
	{
		IsWindowIconified = true;
	}
	else
	{
		IsWindowIconified = false;
	}
}

void StartGlfwWindow()
{
	if (!glfwInit())
		throw std::runtime_error("Could not initialize GLFW!");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // TODO add window resizing possibility with swapchain recreation etc.
	Window = glfwCreateWindow(WIDTH, HEIGHT, "jarp", NULL, NULL);

	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
	glfwSetWindowIconifyCallback(Window, WindowIconifyCallback);
}

void ShutdownGlfw()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}

///////////////// VULKAN /////////////////

struct SQueueFamilyIndices
{
	uint32_t GraphicsFamily;
	uint32_t PresentFamily;

	bool IsComplete()
	{
		return GraphicsFamily >= 0 && PresentFamily >= 0;
	}

	std::set<uint32_t> GetUniqueQueueFamilies()
	{
		return std::set<uint32_t>{ GraphicsFamily, PresentFamily };
	}

	bool HasMultipleQueueFamilies()
	{
		return GetUniqueQueueFamilies().size() > 1;
	}

	size_t GetNumberOfUniqueQueueFamilies()
	{
		return GetUniqueQueueFamilies().size();
	}

	std::vector<uint32_t> GetIndices()
	{
		return std::vector<uint32_t>{ GraphicsFamily, PresentFamily };
	}
};

struct SSwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> SurfaceFormats;
	std::vector<VkPresentModeKHR> PresentModes;
};

struct SSwapchainDetails
{
	VkSurfaceFormatKHR SurfaceFormat;
	VkExtent2D Extent;
};

VkInstance Instance;
VkDebugUtilsMessengerEXT DebugUtilsMessengerEXT;
VkSurfaceKHR SurfaceKHR;

VkPhysicalDevice PhysicalDevice;
VkDevice LogicalDevice;

SQueueFamilyIndices QueueFamilyIndices;
VkQueue GraphicsQueue;
VkQueue PresentQueue;

SSwapchainSupportDetails SwapchainSupportDetails;
SSwapchainDetails SwapchainDetails;
VkSwapchainKHR SwapchainKHR;
std::vector<VkImage> SwapchainImages;
std::vector<VkImageView> SwapchainImageViews;

VkRenderPass RenderPass;
VkPipelineLayout PipelineLayout;
VkPipeline Pipeline;
std::vector<VkFramebuffer> Framebuffers;

VkCommandPool CommandPool;
std::vector<VkCommandBuffer> CommandBuffers;

VkSemaphore SignalSemaphore;
VkSemaphore WaitSemaphore;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::string Severity;
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		Severity = "VERBOSE";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		Severity = "INFO";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		Severity = "WARNING";
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		Severity = "ERROR";

	std::string Type;
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
		Type = "GENERAL";
	else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		Type = "VALIDATION";
	else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		Type = "PERFORMANCE";

	std::cerr << Severity << " | " << Type << " - Validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
{
	auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
	if (Func != nullptr)
	{
		return Func(Instance, pCreateInfo, pAllocator, pCallback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance Instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
	if (Func != nullptr)
	{
		return Func(Instance, callback, pAllocator);
	}
}

std::vector<char> ReadFile(const std::string& Filename)
{
	std::ifstream File(Filename, std::ios::binary | std::ios::ate);
	if (!File.is_open())
	{
		throw std::runtime_error("Failed to open file!");
	}
	std::streampos Size = File.tellg();
	std::vector<char> Buffer(Size);

	File.seekg(0);
	File.read(Buffer.data(), Size);
	File.close();

	return Buffer;
}

VkShaderModule CreateShaderModule(const std::vector<char>& Code)
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.pNext = NULL;
	ShaderModuleCreateInfo.flags = 0;
	ShaderModuleCreateInfo.codeSize = Code.size();
	ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(Code.data());

	VkShaderModule ShaderModule;
	VK_ASSERT(vkCreateShaderModule(LogicalDevice, &ShaderModuleCreateInfo, NULL, &ShaderModule));
	return ShaderModule;
}

/* Depends on:
 */
void CreateInstance()
{
	// Use validation layers if this is a debug build
	std::vector<const char*> InstanceLayers;
#if defined(_DEBUG)
	InstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation"); // Standard validation layer is always available

	// Check validation layer support
	uint32_t LayerPropertyCount;
	vkEnumerateInstanceLayerProperties(&LayerPropertyCount, NULL);
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
			throw std::runtime_error("Not all validation layers supported!");
	}
#endif
	uint32_t GlfwExtensionCount;
	const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
	std::vector<const char*> InstanceExtensions(GlfwExtensions, GlfwExtensions + GlfwExtensionCount);
#if defined(_DEBUG)
	InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	// Create the Vulkan instance
	VkApplicationInfo AppInfo = {};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pNext = NULL;
	AppInfo.pApplicationName = NULL;
	AppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	AppInfo.pEngineName = "JARP";
	AppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	AppInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo InstInfo = {};
	InstInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstInfo.pNext = NULL;
	InstInfo.flags = 0;
	InstInfo.pApplicationInfo = &AppInfo;
	InstInfo.enabledExtensionCount = static_cast<uint32_t>(InstanceExtensions.size());
	InstInfo.ppEnabledExtensionNames = InstanceExtensions.data();
	InstInfo.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
	InstInfo.ppEnabledLayerNames = InstanceLayers.data();

	VK_ASSERT(vkCreateInstance(&InstInfo, NULL, &Instance));
}

/* Depends on:
 *	- Instance
 */
void SetupDebugCallback()
{
	VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT = {};
	DebugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugUtilsMessengerCreateInfoEXT.pNext = NULL;
	DebugUtilsMessengerCreateInfoEXT.flags = 0;
	DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
	DebugUtilsMessengerCreateInfoEXT.pUserData = NULL;

	VK_ASSERT(CreateDebugUtilsMessengerEXT(Instance, &DebugUtilsMessengerCreateInfoEXT, NULL, &DebugUtilsMessengerEXT));
}

/* Depends on:
 *	- Instance
 *  - GLFWwindow
 */
void CreateSurface()
{
	VK_ASSERT(glfwCreateWindowSurface(Instance, Window, NULL, &SurfaceKHR));
}

SSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice Device)
{
	SSwapchainSupportDetails SupportDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, SurfaceKHR, &SupportDetails.SurfaceCapabilities);

	uint32_t SurfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Device, SurfaceKHR, &SurfaceFormatCount, NULL);
	SupportDetails.SurfaceFormats.resize(SurfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(Device, SurfaceKHR, &SurfaceFormatCount, SupportDetails.SurfaceFormats.data());

	uint32_t PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Device, SurfaceKHR, &PresentModeCount, NULL);
	SupportDetails.PresentModes.resize(PresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(Device, SurfaceKHR, &PresentModeCount, SupportDetails.PresentModes.data());

	return SupportDetails;
}

/* Depends on:
 *	- Instance
 *  - SurfaceKHR
 */
void CreateDevice()
{
	std::vector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Find the physical device to use
	uint32_t PhysicalDeviceCount;
	vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, NULL);
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.data()));

	// Get info about the available physical devices and pick one for use
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;

	for (size_t i = 0; i < PhysicalDevices.size(); ++i)
	{
		VkPhysicalDevice Device = PhysicalDevices[i];

		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(Device, &PhysicalDeviceProperties);

		VkPhysicalDeviceFeatures DeviceFeatures;
		vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

		// Find the queue family
		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, NULL);
		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilyProperties.data());

		SQueueFamilyIndices QueueIndices = {};
		for (size_t j = 0; j < QueueFamilyProperties.size(); ++j)
		{
			const auto& QueueFamily = QueueFamilyProperties[j];

			// Pick an appriopriate queue
			if (QueueFamily.queueCount > 0 && (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				QueueIndices.GraphicsFamily = static_cast<uint32_t>(j);
			}
			VkBool32 IsSurfaceSupported;
			VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(Device, static_cast<uint32_t>(j), SurfaceKHR, &IsSurfaceSupported));
			if (QueueFamily.queueCount > 0 && IsSurfaceSupported)
			{
				QueueIndices.PresentFamily = static_cast<uint32_t>(j);
			}
			if (QueueIndices.IsComplete())
			{
				break;
			}
		}

		// Query device extensions
		uint32_t PropertyCount;
		vkEnumerateDeviceExtensionProperties(Device, NULL, &PropertyCount, NULL);
		std::vector<VkExtensionProperties> ExtensionProperties(PropertyCount);
		vkEnumerateDeviceExtensionProperties(Device, NULL, &PropertyCount, ExtensionProperties.data());

		std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());
		for (const auto& Extension : ExtensionProperties)
		{
			RequiredExtensions.erase(Extension.extensionName);
		}
		bool DeviceExtensionsSupported = RequiredExtensions.empty();
		if (!DeviceExtensionsSupported)
			continue;

		// Check swapchain support
		SSwapchainSupportDetails SwapchainSupport = QuerySwapchainSupport(Device);

		// Check if device is suitable
		if (QueueIndices.IsComplete() && DeviceExtensionsSupported
			&& !SwapchainSupport.SurfaceFormats.empty() && !SwapchainSupport.PresentModes.empty())
		{
			PhysicalDevice = Device;
			PhysicalDeviceFeatures = DeviceFeatures;
			QueueFamilyIndices = QueueIndices;

			// Prefer the use of discrete GPUs
			if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				break;
		}
	}

	// Prepare Queue Info for Device creation
	float QueuePriority = 1.f;
	std::vector<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos;
	for (uint32_t QueueFamily : QueueFamilyIndices.GetUniqueQueueFamilies())
	{
		VkDeviceQueueCreateInfo DeviceQueueCreateInfo;
		DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		DeviceQueueCreateInfo.pNext = NULL;
		DeviceQueueCreateInfo.flags = 0;
		DeviceQueueCreateInfo.queueFamilyIndex = QueueFamily;
		DeviceQueueCreateInfo.queueCount = 1;
		DeviceQueueCreateInfo.pQueuePriorities = &QueuePriority;

		DeviceQueueCreateInfos.push_back(DeviceQueueCreateInfo);
	}

	// Create the logical device
	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = NULL;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(DeviceQueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = 0; // deprecated
	DeviceCreateInfo.ppEnabledLayerNames = NULL; // deprecated
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	DeviceCreateInfo.pEnabledFeatures = &PhysicalDeviceFeatures;

	VK_ASSERT(vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, NULL, &LogicalDevice));

	// Get queues of the logical device
	vkGetDeviceQueue(LogicalDevice, QueueFamilyIndices.GraphicsFamily, 0, &GraphicsQueue);
	vkGetDeviceQueue(LogicalDevice, QueueFamilyIndices.PresentFamily, 0, &PresentQueue);
}

/* Depends on:
 *	- Device
 * (- SwapchainSupportDetails)
 *  - SurfaceKHR
 */
void CreateSwapchain()
{
	SwapchainSupportDetails = QuerySwapchainSupport(PhysicalDevice);

	// Check image count of swapchain
	// If max image count is 0 then there are no limits besides memory requirements
	uint32_t SwapchainMinImageCount = SwapchainSupportDetails.SurfaceCapabilities.minImageCount + 1;
	if (SwapchainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && SwapchainMinImageCount > SwapchainSupportDetails.SurfaceCapabilities.maxImageCount)
	{
		SwapchainMinImageCount = SwapchainSupportDetails.SurfaceCapabilities.maxImageCount;
	}

	// Choose surface format
	SwapchainDetails.SurfaceFormat = SwapchainSupportDetails.SurfaceFormats[0];
	if (SwapchainSupportDetails.SurfaceFormats.size() == 1 && SwapchainSupportDetails.SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		SwapchainDetails.SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	else
	{
		for (const auto& AvailableImageFormat : SwapchainSupportDetails.SurfaceFormats)
		{
			if (AvailableImageFormat.format == VK_FORMAT_B8G8R8A8_UNORM && AvailableImageFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				SwapchainDetails.SurfaceFormat = AvailableImageFormat;
				break;
			}
		}
	}

	// Choose extent
	if (SwapchainSupportDetails.SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		SwapchainDetails.Extent = SwapchainSupportDetails.SurfaceCapabilities.currentExtent;
	}
	else
	{
		int Width, Height;
		glfwGetFramebufferSize(Window, &Width, &Height);

		//VkExtent2D ActualExtent = { WIDTH, HEIGHT };
		VkExtent2D ActualExtent = { static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) };
		ActualExtent.width = std::max(SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.width, std::min(SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = std::max(SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.height, std::min(SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

		SwapchainDetails.Extent = ActualExtent;
	}

	// Choose present mode
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& AvailablePresentMode : SwapchainSupportDetails.PresentModes)
	{
		if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			PresentMode = AvailablePresentMode;
			break;
		}
		else if (AvailablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			PresentMode = AvailablePresentMode;
		}
	}

	// Create swapchain
	VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR = {};
	SwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfoKHR.pNext = NULL;
	SwapchainCreateInfoKHR.flags = 0;
	SwapchainCreateInfoKHR.surface = SurfaceKHR;
	SwapchainCreateInfoKHR.minImageCount = SwapchainMinImageCount;
	SwapchainCreateInfoKHR.imageFormat = SwapchainDetails.SurfaceFormat.format;
	SwapchainCreateInfoKHR.imageColorSpace = SwapchainDetails.SurfaceFormat.colorSpace;
	SwapchainCreateInfoKHR.imageExtent = SwapchainDetails.Extent;
	SwapchainCreateInfoKHR.imageArrayLayers = SwapchainSupportDetails.SurfaceCapabilities.maxImageArrayLayers;
	SwapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (QueueFamilyIndices.HasMultipleQueueFamilies())
	{
		SwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		SwapchainCreateInfoKHR.queueFamilyIndexCount = static_cast<uint32_t>(QueueFamilyIndices.GetNumberOfUniqueQueueFamilies());
		SwapchainCreateInfoKHR.pQueueFamilyIndices = QueueFamilyIndices.GetIndices().data();
	}
	else
	{
		SwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainCreateInfoKHR.queueFamilyIndexCount = 1;
		SwapchainCreateInfoKHR.pQueueFamilyIndices = NULL;
	}

	SwapchainCreateInfoKHR.preTransform = SwapchainSupportDetails.SurfaceCapabilities.currentTransform;
	SwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfoKHR.presentMode = PresentMode;
	SwapchainCreateInfoKHR.clipped = VK_TRUE;
	SwapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;

	VK_ASSERT(vkCreateSwapchainKHR(LogicalDevice, &SwapchainCreateInfoKHR, NULL, &SwapchainKHR));
}

/* Depends on:
 *	- Device
 *  - SwapchainKHR
 */
void CreateImageViews()
{
	uint32_t SwapchainImageCount;
	vkGetSwapchainImagesKHR(LogicalDevice, SwapchainKHR, &SwapchainImageCount, NULL);
	SwapchainImages.resize(SwapchainImageCount);
	vkGetSwapchainImagesKHR(LogicalDevice, SwapchainKHR, &SwapchainImageCount, SwapchainImages.data());

	// Create image view
	SwapchainImageViews.resize(SwapchainImages.size());
	for (size_t i = 0; i < SwapchainImageViews.size(); ++i)
	{
		VkImageViewCreateInfo ImageViewCreateInfo;
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.pNext = NULL;
		ImageViewCreateInfo.flags = 0;
		ImageViewCreateInfo.image = SwapchainImages[i];
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format = SwapchainDetails.SurfaceFormat.format;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_ASSERT(vkCreateImageView(LogicalDevice, &ImageViewCreateInfo, NULL, &SwapchainImageViews[i]));
	}
}

/* Depends on:
 *	- Device
 *  - SwapchainKHR
 */
void CreateRenderPass()
{
	// Create render pass
	VkAttachmentDescription AttachmentDescription = {};
	AttachmentDescription.flags = 0;
	AttachmentDescription.format = SwapchainDetails.SurfaceFormat.format;
	AttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	AttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	AttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	AttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	AttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	AttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	AttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference ColorAttachment = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	VkSubpassDescription SubpassDescription = {};
	SubpassDescription.flags = 0;
	SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	SubpassDescription.inputAttachmentCount = 0;
	SubpassDescription.pInputAttachments = NULL;
	SubpassDescription.colorAttachmentCount = 1;
	SubpassDescription.pColorAttachments = &ColorAttachment;
	SubpassDescription.pResolveAttachments = NULL;
	SubpassDescription.pDepthStencilAttachment = NULL;
	SubpassDescription.preserveAttachmentCount = 0;
	SubpassDescription.pPreserveAttachments = NULL;

	VkSubpassDependency SubpassDependency = {};
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.dstSubpass = 0; // Refers to our one and only subpass
	SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.srcAccessMask = 0;
	SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	SubpassDependency.dependencyFlags = 0;

	VkRenderPassCreateInfo RenderPassCreateInfo = {};
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.pNext = NULL;
	RenderPassCreateInfo.flags = 0;
	RenderPassCreateInfo.attachmentCount = 1;
	RenderPassCreateInfo.pAttachments = &AttachmentDescription;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &SubpassDescription;
	RenderPassCreateInfo.dependencyCount = 1;
	RenderPassCreateInfo.pDependencies = &SubpassDependency;

	VK_ASSERT(vkCreateRenderPass(LogicalDevice, &RenderPassCreateInfo, NULL, &RenderPass));
}

/* Depends on:
 *	- Device
 *  - SwapchainKHR
 */
void CreateGraphicsPipeline()
{
	// Create shader modules
	auto VertShaderCode = ReadFile("Shaders/Shader.vert.spv");
	auto FragShaderCode = ReadFile("Shaders/Shader.frag.spv");

	VkShaderModule VertShaderModule = CreateShaderModule(VertShaderCode);
	VkShaderModule FragShaderModule = CreateShaderModule(FragShaderCode);

	// Build the graphics pipeline
	std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageCreateInfos(2);

	PipelineShaderStageCreateInfos[0] = {};
	PipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	PipelineShaderStageCreateInfos[0].pNext = NULL;
	PipelineShaderStageCreateInfos[0].flags = 0;
	PipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	PipelineShaderStageCreateInfos[0].module = VertShaderModule;
	PipelineShaderStageCreateInfos[0].pName = "main";
	PipelineShaderStageCreateInfos[0].pSpecializationInfo = NULL;

	PipelineShaderStageCreateInfos[1] = {};
	PipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	PipelineShaderStageCreateInfos[1].pNext = NULL;
	PipelineShaderStageCreateInfos[1].flags = 0;
	PipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	PipelineShaderStageCreateInfos[1].module = FragShaderModule;
	PipelineShaderStageCreateInfos[1].pName = "main";
	PipelineShaderStageCreateInfos[1].pSpecializationInfo = NULL;

	VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo = {};
	PipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	PipelineVertexInputStateCreateInfo.pNext = NULL;
	PipelineVertexInputStateCreateInfo.flags = 0;
	PipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
	PipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = NULL;
	PipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	PipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = NULL;

	VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo = {};
	PipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	PipelineInputAssemblyStateCreateInfo.pNext = NULL;
	PipelineInputAssemblyStateCreateInfo.flags = 0;
	PipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	PipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport Viewport = {};
	Viewport.x = 0.0f;
	Viewport.y = 0.0f;
	Viewport.width = static_cast<float>(SwapchainDetails.Extent.width);
	Viewport.height = static_cast<float>(SwapchainDetails.Extent.height);
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;

	VkRect2D Scissor = {};
	Scissor.offset = { 0, 0 };
	Scissor.extent = SwapchainDetails.Extent;

	VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo = {};
	PipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	PipelineViewportStateCreateInfo.pNext = NULL;
	PipelineViewportStateCreateInfo.flags = 0;
	PipelineViewportStateCreateInfo.viewportCount = 1;
	PipelineViewportStateCreateInfo.pViewports = &Viewport;
	PipelineViewportStateCreateInfo.scissorCount = 1;
	PipelineViewportStateCreateInfo.pScissors = &Scissor;

	VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo = {};
	PipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	PipelineRasterizationStateCreateInfo.pNext = NULL;
	PipelineRasterizationStateCreateInfo.flags = 0;
	PipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	PipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	PipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	PipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	PipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	PipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	PipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	PipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	PipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	PipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo = {};
	PipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	PipelineMultisampleStateCreateInfo.pNext = NULL;
	PipelineMultisampleStateCreateInfo.flags = 0;
	PipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	PipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	PipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
	PipelineMultisampleStateCreateInfo.pSampleMask = NULL;
	PipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	PipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState = {};
	PipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
	PipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	PipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	PipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	PipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	PipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	PipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	PipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo = {};
	PipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	PipelineColorBlendStateCreateInfo.pNext = NULL;
	PipelineColorBlendStateCreateInfo.flags = 0;
	PipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	PipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	PipelineColorBlendStateCreateInfo.attachmentCount = 1;
	PipelineColorBlendStateCreateInfo.pAttachments = &PipelineColorBlendAttachmentState;
	PipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	PipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	PipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	PipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.pNext = NULL;
	PipelineLayoutCreateInfo.flags = 0;
	PipelineLayoutCreateInfo.setLayoutCount = 0;
	PipelineLayoutCreateInfo.pSetLayouts = NULL;
	PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	PipelineLayoutCreateInfo.pPushConstantRanges = NULL;

	VK_ASSERT(vkCreatePipelineLayout(LogicalDevice, &PipelineLayoutCreateInfo, NULL, &PipelineLayout));

	std::vector<VkDynamicState> DynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo = {};
	PipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	PipelineDynamicStateCreateInfo.pNext = NULL;
	PipelineDynamicStateCreateInfo.flags = 0;
	PipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
	PipelineDynamicStateCreateInfo.pDynamicStates = DynamicStates.data();

	VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
	GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	GraphicsPipelineCreateInfo.pNext = NULL;
	GraphicsPipelineCreateInfo.flags = 0;
	GraphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(PipelineShaderStageCreateInfos.size());
	GraphicsPipelineCreateInfo.pStages = PipelineShaderStageCreateInfos.data();
	GraphicsPipelineCreateInfo.pVertexInputState = &PipelineVertexInputStateCreateInfo;
	GraphicsPipelineCreateInfo.pInputAssemblyState = &PipelineInputAssemblyStateCreateInfo;
	GraphicsPipelineCreateInfo.pTessellationState = NULL;
	GraphicsPipelineCreateInfo.pViewportState = &PipelineViewportStateCreateInfo;
	GraphicsPipelineCreateInfo.pRasterizationState = &PipelineRasterizationStateCreateInfo;
	GraphicsPipelineCreateInfo.pMultisampleState = &PipelineMultisampleStateCreateInfo;
	GraphicsPipelineCreateInfo.pDepthStencilState = NULL;
	GraphicsPipelineCreateInfo.pColorBlendState = &PipelineColorBlendStateCreateInfo;
	GraphicsPipelineCreateInfo.pDynamicState = &PipelineDynamicStateCreateInfo;
	GraphicsPipelineCreateInfo.layout = PipelineLayout;
	GraphicsPipelineCreateInfo.renderPass = RenderPass;
	GraphicsPipelineCreateInfo.subpass = 0;
	GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	GraphicsPipelineCreateInfo.basePipelineIndex = -1;

	VK_ASSERT(vkCreateGraphicsPipelines(LogicalDevice, VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, NULL, &Pipeline));

	// Destroy shader modules of the pipeline
	vkDestroyShaderModule(LogicalDevice, VertShaderModule, NULL);
	vkDestroyShaderModule(LogicalDevice, FragShaderModule, NULL);
}

/* Depends on:
 *	- Device
 *  - SwapchainKHR
 */
void CreateFramebuffers()
{
	// Create framebuffer
	Framebuffers.resize(SwapchainImageViews.size());
	for (size_t i = 0; i < SwapchainImageViews.size(); ++i)
	{
		VkFramebufferCreateInfo FramebufferCreateInfo = {};
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.pNext = NULL;
		FramebufferCreateInfo.flags = 0;
		FramebufferCreateInfo.renderPass = RenderPass;
		FramebufferCreateInfo.attachmentCount = 1;
		FramebufferCreateInfo.pAttachments = &SwapchainImageViews[i];
		FramebufferCreateInfo.width = SwapchainDetails.Extent.width;
		FramebufferCreateInfo.height = SwapchainDetails.Extent.height;
		FramebufferCreateInfo.layers = 1;

		VK_ASSERT(vkCreateFramebuffer(LogicalDevice, &FramebufferCreateInfo, NULL, &Framebuffers[i]));
	}
}

/* Depends on:
 *	- Device
 */
void CreateCommandPool()
{
	// Create a command pool
	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.pNext = NULL;
	CommandPoolCreateInfo.flags = 0;
	CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndices.GraphicsFamily;

	VK_ASSERT(vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, NULL, &CommandPool));
}

/* Depends on:
 *	- Device
 *  - CommandPool
 *  - SwapchainKHR
 */
void CreateCommandBuffers()
{
	// Allocate command buffers
	CommandBuffers.resize(Framebuffers.size());

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = NULL;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	VK_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &CommandBufferAllocateInfo, CommandBuffers.data()));

	// Record on the command buffers
	for (size_t i = 0; i < CommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.pNext = NULL;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		CommandBufferBeginInfo.pInheritanceInfo = NULL; // This is a primary command buffer, so the value can be ignored

		VK_ASSERT(vkBeginCommandBuffer(CommandBuffers[i], &CommandBufferBeginInfo));

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = RenderPass;
		RenderPassBeginInfo.framebuffer = Framebuffers[i];
		RenderPassBeginInfo.renderArea.extent = SwapchainDetails.Extent;
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		VkClearValue ClearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;

		vkCmdBeginRenderPass(CommandBuffers[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // Only primary command buffers, so inline subpass suffices
		// START RECORD
		vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
		
		VkViewport Viewport = {};
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = static_cast<float>(SwapchainDetails.Extent.width);
		Viewport.height = static_cast<float>(SwapchainDetails.Extent.height);
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;
		vkCmdSetViewport(CommandBuffers[i], 0, 1, &Viewport);

		VkRect2D Scissor = {};
		Scissor.offset = { 0, 0 };
		Scissor.extent = SwapchainDetails.Extent;
		vkCmdSetScissor(CommandBuffers[i], 0, 1, &Scissor);

		vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);
		// END RECORD
		vkCmdEndRenderPass(CommandBuffers[i]);

		VK_ASSERT(vkEndCommandBuffer(CommandBuffers[i]));
	}
}

/* Depends on:
 *	- Device
 */
void CreateSyncObjects()
{
	VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreCreateInfo.flags = 0;
	SemaphoreCreateInfo.pNext = NULL;

	VK_ASSERT(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &SignalSemaphore));
	VK_ASSERT(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &WaitSemaphore));
}

void StartVulkan()
{
	CreateInstance();
#if defined(_DEBUG)
	SetupDebugCallback();
#endif
	CreateSurface();
	CreateDevice();
	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	CreateSyncObjects();
}

void CleanupSwapchain()
{
	vkFreeCommandBuffers(LogicalDevice, CommandPool, static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	for (const auto& Framebuffer : Framebuffers)
	{
		vkDestroyFramebuffer(LogicalDevice, Framebuffer, NULL);
	}
	vkDestroyRenderPass(LogicalDevice, RenderPass, NULL);
	for (const auto& ImageView : SwapchainImageViews)
	{
		vkDestroyImageView(LogicalDevice, ImageView, NULL);
	}
	vkDestroySwapchainKHR(LogicalDevice, SwapchainKHR, NULL);
}

void RecreateSwapchain()
{
	CONSOLE_LOG("Recreating swapchain");

	int Width = 0;
	int Height = 0;
	while (Width == 0 || Height == 0)
	{
		glfwGetFramebufferSize(Window, &Width, &Height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(LogicalDevice);
	CleanupSwapchain();

	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
	CreateFramebuffers();
	CreateCommandBuffers();
}

void ShutdownVulkan()
{
	// Free all resources
	vkDeviceWaitIdle(LogicalDevice);

	CleanupSwapchain();

	vkDestroySemaphore(LogicalDevice, WaitSemaphore, NULL);
	vkDestroySemaphore(LogicalDevice, SignalSemaphore, NULL);
	vkDestroyCommandPool(LogicalDevice, CommandPool, NULL);
	vkDestroyPipelineLayout(LogicalDevice, PipelineLayout, NULL);
	vkDestroyPipeline(LogicalDevice, Pipeline, NULL);
	vkDestroyDevice(LogicalDevice, NULL);
	vkDestroySurfaceKHR(Instance, SurfaceKHR, NULL);
#if defined(_DEBUG)
	DestroyDebugUtilsMessengerEXT(Instance, DebugUtilsMessengerEXT, nullptr);
#endif
	vkDestroyInstance(Instance, NULL);
}

/* Depends on:
 *  - Device
 *  - Semaphore
 *  - SwapchainKHR
 *  - CommandBuffer
 */
void DrawFrame()
{
	// Don't try to draw to a minimized window
	if (IsWindowIconified)
		return;

	// Get the next available image to work on
	uint32_t ImageIndex;
	{
		VkResult Result = vkAcquireNextImageKHR(LogicalDevice, SwapchainKHR, std::numeric_limits<uint64_t>::max(), WaitSemaphore, VK_NULL_HANDLE, &ImageIndex);
		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return;
		}
		else
		{
			VK_ASSERT(Result);
		}
	}

	// Submit commands to the queue
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = &WaitSemaphore;
	const VkPipelineStageFlags WaitDstStageMask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	SubmitInfo.pWaitDstStageMask = &WaitDstStageMask;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffers[ImageIndex];
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = &SignalSemaphore;

	VK_ASSERT(vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE));

	VkPresentInfoKHR PresentInfoKHR = {};
	PresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfoKHR.pNext = NULL;
	PresentInfoKHR.waitSemaphoreCount = 1;
	PresentInfoKHR.pWaitSemaphores = &SignalSemaphore;
	PresentInfoKHR.swapchainCount = 1;
	PresentInfoKHR.pSwapchains = &SwapchainKHR;
	PresentInfoKHR.pImageIndices = &ImageIndex;
	PresentInfoKHR.pResults = NULL;

	{
		VkResult Result = vkQueuePresentKHR(GraphicsQueue, &PresentInfoKHR);
		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || IsFramebufferResized)
		{
			IsFramebufferResized = false;
			RecreateSwapchain();
		}
		else
		{
			VK_ASSERT(Result);
		}
	}
}

///////////////// APP /////////////////

void MainLoop()
{
	while (!glfwWindowShouldClose(Window))
	{
		glfwPollEvents();
		DrawFrame();
	}
}

int main()
{
	StartGlfwWindow();
	StartVulkan();

	MainLoop();

	ShutdownVulkan();
	ShutdownGlfw();	

	return 0;
}
