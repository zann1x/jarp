// Define NOMINMAX to prevent the min and max function of the windows.h header to be defined
#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

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

GLFWwindow* Window;
VkInstance Instance;
VkSurfaceKHR SurfaceKHR;

VkPhysicalDevice PhysicalDevice;
VkDevice LogicalDevice;

SQueueFamilyIndices QueueFamilyIndices;
VkQueue GraphicsQueue;
VkQueue PresentQueue;

SSwapchainSupportDetails SwapchainSupport;
VkSwapchainKHR SwapchainKHR;
std::vector<VkImage> SwapchainImages;
std::vector<VkImageView> SwapchainImageViews;

int CreateGlfwWindow()
{
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO add window resizing possibility with swapchain recreation etc.
	Window = glfwCreateWindow(WIDTH, HEIGHT, "jarp", NULL, NULL);

	return 0;
}

void Run()
{
	while (!glfwWindowShouldClose(Window))
	{
		glfwPollEvents();
	}
}

int main()
{
	CreateGlfwWindow();
	Run();

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
	std::vector<const char*> Extensions(GlfwExtensions, GlfwExtensions + GlfwExtensionCount);

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
	InstInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
	InstInfo.ppEnabledExtensionNames = Extensions.data();
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

	// TODO pick physical device
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;

	// TODO actually pick the best physical device
	// Get info about the available physical devices and pick one for use
	for (size_t i = 0; i < PhysicalDevices.size(); ++i)
	{
		VkPhysicalDevice Device = PhysicalDevices[i];

		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(Device, &PhysicalDeviceProperties);

		VkPhysicalDeviceFeatures DeviceFeatures;
		vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

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

		PhysicalDevice = Device;
		PhysicalDeviceFeatures = DeviceFeatures;
		QueueFamilyIndices = QueueIndices;

		// Prefer the use of discrete GPUs
		if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && QueueFamilyIndices.IsComplete())
		{
			break;
		}
	}

	// Get surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, SurfaceKHR, &SwapchainSupport.SurfaceCapabilities);

	uint32_t SurfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, SurfaceKHR, &SurfaceFormatCount, NULL);
	SwapchainSupport.SurfaceFormats.resize(SurfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, SurfaceKHR, &SurfaceFormatCount, SwapchainSupport.SurfaceFormats.data());

	uint32_t PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, SurfaceKHR, &PresentModeCount, NULL);
	SwapchainSupport.PresentModes.resize(PresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, SurfaceKHR, &PresentModeCount, SwapchainSupport.PresentModes.data());

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

	std::vector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

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
	uint32_t SwapchainMinImageCount = SwapchainSupport.SurfaceCapabilities.minImageCount + 1;
	if (SwapchainSupport.SurfaceCapabilities.maxImageCount > 0 && SwapchainMinImageCount > SwapchainSupport.SurfaceCapabilities.maxImageCount)
	{
		SwapchainMinImageCount = SwapchainSupport.SurfaceCapabilities.maxImageCount;
	}

	// Choose surface format
	VkSurfaceFormatKHR ImageFormat;
	ImageFormat = SwapchainSupport.SurfaceFormats[0];
	if (SwapchainSupport.SurfaceFormats.size() == 1 && SwapchainSupport.SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		ImageFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	else
	{
		for (const auto& AvailableImageFormat : SwapchainSupport.SurfaceFormats)
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
	if (SwapchainSupport.SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		ImageExtent = SwapchainSupport.SurfaceCapabilities.currentExtent;
	}
	else
	{
		VkExtent2D ActualExtent = { WIDTH, HEIGHT };
		ActualExtent.width = std::max(SwapchainSupport.SurfaceCapabilities.minImageExtent.width, std::min(SwapchainSupport.SurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = std::max(SwapchainSupport.SurfaceCapabilities.minImageExtent.height, std::min(SwapchainSupport.SurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

		ImageExtent = ActualExtent;
	}

	// Choose present mode
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& AvailablePresentMode : SwapchainSupport.PresentModes)
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
	SwapchainCreateInfoKHR.imageArrayLayers = SwapchainSupport.SurfaceCapabilities.maxImageArrayLayers;
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

	SwapchainCreateInfoKHR.preTransform = SwapchainSupport.SurfaceCapabilities.currentTransform;
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
	

	//// Create render pass
	//VkAttachmentDescription AttachmentDescriptions[2] = {};
	//AttachmentDescriptions[0].flags = 0;
	//AttachmentDescriptions[0].format = VK_FORMAT_UNDEFINED; // TODO
	//AttachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
	//AttachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//AttachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//AttachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//AttachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//AttachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//AttachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//AttachmentDescriptions[1].flags = 0;
	//AttachmentDescriptions[1].format = VK_FORMAT_UNDEFINED; // TODO
	//AttachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
	//AttachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//AttachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//AttachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//AttachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//AttachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//AttachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//VkAttachmentReference ColorAttachment = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	//VkAttachmentReference DepthAttachment = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

	//VkSubpassDescription SubpassDescription = {};
	//SubpassDescription.flags = 0;
	//SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//SubpassDescription.inputAttachmentCount = 0;
	//SubpassDescription.pInputAttachments = NULL;
	//SubpassDescription.colorAttachmentCount = 1;
	//SubpassDescription.pColorAttachments = &ColorAttachment;
	//SubpassDescription.pResolveAttachments = NULL;
	//SubpassDescription.pDepthStencilAttachment = &DepthAttachment;
	//SubpassDescription.preserveAttachmentCount = 0;
	//SubpassDescription.pPreserveAttachments = NULL;

	//VkSubpassDependency SubpassDependency = {};

	//VkRenderPassCreateInfo RenderPassCreateInfo = {};
	//RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//RenderPassCreateInfo.pNext = NULL;
	//RenderPassCreateInfo.flags = 0;
	//RenderPassCreateInfo.attachmentCount = sizeof(AttachmentDescriptions) / sizeof(AttachmentDescriptions[0]);
	//RenderPassCreateInfo.pAttachments = AttachmentDescriptions;
	//RenderPassCreateInfo.subpassCount = 1;
	//RenderPassCreateInfo.pSubpasses = &SubpassDescription;
	//RenderPassCreateInfo.dependencyCount = 1;
	//RenderPassCreateInfo.pDependencies = &SubpassDependency;

	//VkRenderPass RenderPass = 0;
	//VK_ASSERT(vkCreateRenderPass(LogicalDevice, &RenderPassCreateInfo, NULL, &RenderPass));

	//// Create framebuffer
	//// TODO
	//VkFramebufferCreateInfo FramebufferCreateInfo = {};
	//FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	//FramebufferCreateInfo.pNext = NULL;
	//FramebufferCreateInfo.flags = 0;
	//FramebufferCreateInfo.renderPass = RenderPass;
	//FramebufferCreateInfo.attachmentCount;
	//FramebufferCreateInfo.pAttachments;
	//FramebufferCreateInfo.width;
	//FramebufferCreateInfo.height;
	//FramebufferCreateInfo.layers;

	//VkFramebuffer Framebuffer = 0;
	//VK_ASSERT(vkCreateFramebuffer(LogicalDevice, &FramebufferCreateInfo, NULL, &Framebuffer));

	//// Create shader module
	//// TODO
	//VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
	//ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	//ShaderModuleCreateInfo.pNext = NULL;
	//ShaderModuleCreateInfo.flags = 0;
	//ShaderModuleCreateInfo.codeSize;
	//ShaderModuleCreateInfo.pCode;

	//VkShaderModule ShaderModule = 0;
	//VK_ASSERT(vkCreateShaderModule(LogicalDevice, &ShaderModuleCreateInfo, NULL, &ShaderModule));

	//// Create graphics pipeline
	//// TODO
	//VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
	//GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//GraphicsPipelineCreateInfo.pNext = NULL;
	//GraphicsPipelineCreateInfo.flags;
	//GraphicsPipelineCreateInfo.stageCount;
	//GraphicsPipelineCreateInfo.pStages;
	//GraphicsPipelineCreateInfo.pVertexInputState;
	//GraphicsPipelineCreateInfo.pInputAssemblyState;
	//GraphicsPipelineCreateInfo.pTessellationState;
	//GraphicsPipelineCreateInfo.pViewportState;
	//GraphicsPipelineCreateInfo.pRasterizationState;
	//GraphicsPipelineCreateInfo.pMultisampleState;
	//GraphicsPipelineCreateInfo.pDepthStencilState;
	//GraphicsPipelineCreateInfo.pColorBlendState;
	//GraphicsPipelineCreateInfo.pDynamicState;
	//GraphicsPipelineCreateInfo.layout;
	//GraphicsPipelineCreateInfo.renderPass;
	//GraphicsPipelineCreateInfo.subpass;
	//GraphicsPipelineCreateInfo.basePipelineHandle;
	//GraphicsPipelineCreateInfo.basePipelineIndex;

	//VkPipeline Pipeline = 0;
	//VK_ASSERT(vkCreateGraphicsPipelines(LogicalDevice, NULL, 1, &GraphicsPipelineCreateInfo, NULL, &Pipeline));
	
	//// Create semaphores for command submission
	//VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
	//SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	//SemaphoreCreateInfo.flags = 0;
	//SemaphoreCreateInfo.pNext = NULL;

	//VkSemaphore SignalSemaphore;
	//VK_ASSERT(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &SignalSemaphore));

	//VkSemaphore WaitSemaphore;
	//VK_ASSERT(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &WaitSemaphore));

	//// Create a command pool
	//VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	//CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//CommandPoolCreateInfo.pNext = NULL;
	//CommandPoolCreateInfo.flags = 0;
	//CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndex;

	//VkCommandPool CommandPool = 0;
	//VK_ASSERT(vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, NULL, &CommandPool));

	//// Allocate command buffers
	//std::vector<VkCommandBuffer> CommandBuffers(1);

	//VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	//CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	//CommandBufferAllocateInfo.pNext = NULL;
	//CommandBufferAllocateInfo.commandPool = CommandPool;
	//CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());
	//
	//VK_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &CommandBufferAllocateInfo, CommandBuffers.data()));

	//// Start recording on the command buffer
	//VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	//CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//CommandBufferBeginInfo.pNext = NULL;
	//CommandBufferBeginInfo.flags = 0;
	//CommandBufferBeginInfo.pInheritanceInfo = NULL; // This is a primary command buffer, so the value can be ignored

	//VK_ASSERT(vkBeginCommandBuffer(CommandBuffers[0], &CommandBufferBeginInfo));

	//// Submit commands to the command buffer
	//VkSubmitInfo SubmitInfo = {};
	//SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//SubmitInfo.pNext = NULL;
	//SubmitInfo.waitSemaphoreCount = 1;
	//SubmitInfo.pWaitSemaphores = &WaitSemaphore;
	//SubmitInfo.pWaitDstStageMask = 0;
	//SubmitInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());
	//SubmitInfo.pCommandBuffers = CommandBuffers.data();
	//SubmitInfo.signalSemaphoreCount = 1;
	//SubmitInfo.pSignalSemaphores = &SignalSemaphore;

	//std::vector<VkSubmitInfo> SubmitInfos(1);
	//SubmitInfos.push_back(SubmitInfo);

	//// Create a fence for the submit
	//VkFenceCreateInfo FenceCreateInfo = {};
	//FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//FenceCreateInfo.pNext = NULL;
	//FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // TODO check if signaled state is correct to begin with

	//VkFence Fence = 0;
	//VK_ASSERT(vkCreateFence(LogicalDevice, &FenceCreateInfo, NULL, &Fence));

	//VK_ASSERT(vkQueueSubmit(Queue, static_cast<uint32_t>(SubmitInfos.size()), SubmitInfos.data(), Fence));

	//// End recording
	//VK_ASSERT(vkEndCommandBuffer(CommandBuffers[0]));

	// Free all resources
	vkDeviceWaitIdle(LogicalDevice);
	
	//vkDestroyFence(LogicalDevice, Fence, NULL);
	//vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, CommandBuffers.data());
	//vkDestroyCommandPool(LogicalDevice, CommandPool, NULL);
	//vkDestroySemaphore(LogicalDevice, WaitSemaphore, NULL);
	//vkDestroySemaphore(LogicalDevice, SignalSemaphore, NULL);
	//vkDestroyPipeline(LogicalDevice, Pipeline, NULL);
	//vkDestroyShaderModule(LogicalDevice, ShaderModule, NULL);
	//vkDestroyFramebuffer(LogicalDevice, Framebuffer, NULL);
	//vkDestroyRenderPass(LogicalDevice, RenderPass, NULL);
	for (const auto& ImageView : SwapchainImageViews)
		vkDestroyImageView(LogicalDevice, ImageView, NULL);
	vkDestroySwapchainKHR(LogicalDevice, SwapchainKHR, NULL);
	vkDestroyDevice(LogicalDevice, NULL);
	vkDestroySurfaceKHR(Instance, SurfaceKHR, NULL);
	vkDestroyInstance(Instance, NULL);

	glfwDestroyWindow(Window);

	return 0;
}
