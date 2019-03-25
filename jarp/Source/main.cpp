// Define NOMINMAX to prevent the min and max function of the windows.h header to be defined
#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR // TODO is this define really needed ?
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

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

#define WIDTH 800
#define HEIGHT 600

GLFWwindow* Window;

int StartGlfwWindow()
{
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO add window resizing possibility with swapchain recreation etc.
	Window = glfwCreateWindow(WIDTH, HEIGHT, "jarp", NULL, NULL);

	return 0;
}

void ShutdownGlfw()
{
	glfwDestroyWindow(Window);
}

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

VkInstance Instance;
VkSurfaceKHR SurfaceKHR;

VkPhysicalDevice PhysicalDevice;
VkDevice LogicalDevice;

SQueueFamilyIndices QueueFamilyIndices;
VkQueue GraphicsQueue;
VkQueue PresentQueue;

SSwapchainSupportDetails SwapchainSupportDetails;
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

void StartVulkan()
{
	// Use validation layers if this is a debug build
	std::vector<const char*> Layers;
#if defined(_DEBUG)
	uint32_t LayerPropertyCount;
	vkEnumerateInstanceLayerProperties(&LayerPropertyCount, NULL);
	std::vector<VkLayerProperties> LayerProperties(LayerPropertyCount);
	vkEnumerateInstanceLayerProperties(&LayerPropertyCount, LayerProperties.data());

	Layers.push_back("VK_LAYER_LUNARG_standard_validation"); // Standard validation layer always available

	uint32_t ExtensionPropertyCount;
	vkEnumerateInstanceExtensionProperties(NULL, &ExtensionPropertyCount, NULL);
	std::vector<VkExtensionProperties> ExtensionProperties(ExtensionPropertyCount);
	vkEnumerateInstanceExtensionProperties(NULL, &ExtensionPropertyCount, ExtensionProperties.data());
#endif
	uint32_t GlfwExtensionCount;
	const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
	std::vector<const char*> InstanceExtensions(GlfwExtensions, GlfwExtensions + GlfwExtensionCount);
	std::vector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

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
	InstInfo.enabledLayerCount = static_cast<uint32_t>(Layers.size());
	InstInfo.ppEnabledLayerNames = Layers.data();

	VK_ASSERT(vkCreateInstance(&InstInfo, NULL, &Instance));

	// Create the Win32 Surface KHR
	VK_ASSERT(glfwCreateWindowSurface(Instance, Window, NULL, &SurfaceKHR));

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
		SSwapchainSupportDetails SwapchainSupport;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, SurfaceKHR, &SwapchainSupport.SurfaceCapabilities);

		uint32_t SurfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, SurfaceKHR, &SurfaceFormatCount, NULL);
		SwapchainSupport.SurfaceFormats.resize(SurfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, SurfaceKHR, &SurfaceFormatCount, SwapchainSupport.SurfaceFormats.data());

		uint32_t PresentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, SurfaceKHR, &PresentModeCount, NULL);
		SwapchainSupport.PresentModes.resize(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, SurfaceKHR, &PresentModeCount, SwapchainSupport.PresentModes.data());

		// Check if device is suitable
		if (QueueIndices.IsComplete() && DeviceExtensionsSupported
			&& !SwapchainSupport.SurfaceFormats.empty() && !SwapchainSupport.PresentModes.empty())
		{
			PhysicalDevice = Device;
			PhysicalDeviceFeatures = DeviceFeatures;
			QueueFamilyIndices = QueueIndices;
			SwapchainSupportDetails = SwapchainSupport;

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

	// Check image count of swapchain
	// If max image count is 0 then there are no limits besides memory requirements
	uint32_t SwapchainMinImageCount = SwapchainSupportDetails.SurfaceCapabilities.minImageCount + 1;
	if (SwapchainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && SwapchainMinImageCount > SwapchainSupportDetails.SurfaceCapabilities.maxImageCount)
	{
		SwapchainMinImageCount = SwapchainSupportDetails.SurfaceCapabilities.maxImageCount;
	}

	// Choose surface format
	VkSurfaceFormatKHR ImageFormat;
	ImageFormat = SwapchainSupportDetails.SurfaceFormats[0];
	if (SwapchainSupportDetails.SurfaceFormats.size() == 1 && SwapchainSupportDetails.SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		ImageFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	else
	{
		for (const auto& AvailableImageFormat : SwapchainSupportDetails.SurfaceFormats)
		{
			if (AvailableImageFormat.format == VK_FORMAT_B8G8R8A8_UNORM && AvailableImageFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				ImageFormat = AvailableImageFormat;
				break;
			}
		}
	}

	// Choose extent
	VkExtent2D ImageExtent;
	if (SwapchainSupportDetails.SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		ImageExtent = SwapchainSupportDetails.SurfaceCapabilities.currentExtent;
	}
	else
	{
		VkExtent2D ActualExtent = { WIDTH, HEIGHT };
		ActualExtent.width = std::max(SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.width, std::min(SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = std::max(SwapchainSupportDetails.SurfaceCapabilities.minImageExtent.height, std::min(SwapchainSupportDetails.SurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

		ImageExtent = ActualExtent;
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
	SwapchainCreateInfoKHR.imageFormat = ImageFormat.format;
	SwapchainCreateInfoKHR.imageColorSpace = ImageFormat.colorSpace;
	SwapchainCreateInfoKHR.imageExtent = ImageExtent;
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
		ImageViewCreateInfo.format = ImageFormat.format;
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

	// Create render pass
	VkAttachmentDescription AttachmentDescription = {};
	AttachmentDescription.flags = 0;
	AttachmentDescription.format = ImageFormat.format;
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
	Viewport.width = static_cast<float>(ImageExtent.width);
	Viewport.height = static_cast<float>(ImageExtent.height);
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;

	VkRect2D Scissor = {};
	Scissor.offset = { 0, 0 };
	Scissor.extent = ImageExtent;

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
	GraphicsPipelineCreateInfo.pDynamicState = NULL;
	GraphicsPipelineCreateInfo.layout = PipelineLayout;
	GraphicsPipelineCreateInfo.renderPass = RenderPass;
	GraphicsPipelineCreateInfo.subpass = 0;
	GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	GraphicsPipelineCreateInfo.basePipelineIndex = -1;

	VK_ASSERT(vkCreateGraphicsPipelines(LogicalDevice, VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, NULL, &Pipeline));

	// Destroy shader modules of the pipeline
	vkDestroyShaderModule(LogicalDevice, VertShaderModule, NULL);
	vkDestroyShaderModule(LogicalDevice, FragShaderModule, NULL);

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
		FramebufferCreateInfo.width = ImageExtent.width;
		FramebufferCreateInfo.height = ImageExtent.height;
		FramebufferCreateInfo.layers = 1;

		VK_ASSERT(vkCreateFramebuffer(LogicalDevice, &FramebufferCreateInfo, NULL, &Framebuffers[i]));
	}

	// Create a command pool
	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.pNext = NULL;
	CommandPoolCreateInfo.flags = 0;
	CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndices.GraphicsFamily;

	VK_ASSERT(vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, NULL, &CommandPool));

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
		RenderPassBeginInfo.renderArea.extent = ImageExtent;
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		VkClearValue ClearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;

		vkCmdBeginRenderPass(CommandBuffers[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // Only primary command buffers, so inline subpass suffices
		// START RECORD
		vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
		vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);
		// END RECORD
		vkCmdEndRenderPass(CommandBuffers[i]);

		VK_ASSERT(vkEndCommandBuffer(CommandBuffers[i]));
	}

	// Create semaphores for command submission
	VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreCreateInfo.flags = 0;
	SemaphoreCreateInfo.pNext = NULL;

	VK_ASSERT(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &SignalSemaphore));
	VK_ASSERT(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &WaitSemaphore));
}

void ShutdownVulkan()
{
	// Free all resources
	vkDeviceWaitIdle(LogicalDevice);

	vkDestroySemaphore(LogicalDevice, WaitSemaphore, NULL);
	vkDestroySemaphore(LogicalDevice, SignalSemaphore, NULL);
	vkFreeCommandBuffers(LogicalDevice, CommandPool, static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	vkDestroyCommandPool(LogicalDevice, CommandPool, NULL);
	for (const auto& Framebuffer : Framebuffers)
	{
		vkDestroyFramebuffer(LogicalDevice, Framebuffer, NULL);
	}
	vkDestroyPipeline(LogicalDevice, Pipeline, NULL);
	vkDestroyPipelineLayout(LogicalDevice, PipelineLayout, NULL);
	vkDestroyRenderPass(LogicalDevice, RenderPass, NULL);
	for (const auto& ImageView : SwapchainImageViews)
	{
		vkDestroyImageView(LogicalDevice, ImageView, NULL);
	}
	vkDestroySwapchainKHR(LogicalDevice, SwapchainKHR, NULL);
	vkDestroyDevice(LogicalDevice, NULL);
	vkDestroySurfaceKHR(Instance, SurfaceKHR, NULL);
	vkDestroyInstance(Instance, NULL);
}

void DrawFrame()
{
	// Get the next available image to work on
	uint32_t ImageIndex;
	vkAcquireNextImageKHR(LogicalDevice, SwapchainKHR, std::numeric_limits<uint64_t>::max(), WaitSemaphore, VK_NULL_HANDLE, &ImageIndex);

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

	VK_ASSERT(vkQueuePresentKHR(GraphicsQueue, &PresentInfoKHR));
}

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
