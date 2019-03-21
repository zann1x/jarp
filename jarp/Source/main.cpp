#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

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

int main()
{
	GLFWwindow* Window;
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO add window resizing possibility with swapchain recreation etc.
	Window = glfwCreateWindow(800, 600, "jarp", NULL, NULL);

	while (!glfwWindowShouldClose(Window))
	{
		glfwPollEvents();
	}

	// Use validation layers if this is a debug build
	std::vector<const char*> Layers;
#if defined(_DEBUG)
	uint32_t LayerPropertyCount;
	vkEnumerateInstanceLayerProperties(&LayerPropertyCount, NULL);
	std::vector<VkLayerProperties> LayerProperties(LayerPropertyCount);
	vkEnumerateInstanceLayerProperties(&LayerPropertyCount, LayerProperties.data());

	CONSOLE_LOG("\nAmount of instance layers: " << LayerPropertyCount);
	for (size_t i = 0; i < LayerProperties.size(); ++i)
	{
		CONSOLE_LOG("\n=> Layer #" << i);
		CONSOLE_LOG("\tLayer name: " << LayerProperties[i].layerName);
		CONSOLE_LOG("\tDescription: " << LayerProperties[i].description);
		CONSOLE_LOG("\tImpl version: " << LayerProperties[i].implementationVersion);
		CONSOLE_LOG("\tSpec version: " << LayerProperties[i].specVersion);
	}
	
	Layers.push_back("VK_LAYER_LUNARG_standard_validation"); // Standard validation layer always available

	uint32_t ExtensionPropertyCount;
	vkEnumerateInstanceExtensionProperties(NULL, &ExtensionPropertyCount, NULL);
	std::vector<VkExtensionProperties> ExtensionProperties(ExtensionPropertyCount);
	vkEnumerateInstanceExtensionProperties(NULL, &ExtensionPropertyCount, ExtensionProperties.data());

	CONSOLE_LOG("\nAmount of extensions: " << ExtensionPropertyCount);
	for (size_t i = 0; i < ExtensionProperties.size(); ++i)
	{
		CONSOLE_LOG("\nExtension #" << i);
		CONSOLE_LOG("\tExtension name: " << ExtensionProperties[i].extensionName);
		CONSOLE_LOG("\tSpec version: " << ExtensionProperties[i].specVersion);
	}
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
	AppInfo.pEngineName = "JACREP";
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

	VkInstance Instance = 0;
	VK_ASSERT(vkCreateInstance(&InstInfo, NULL, &Instance));

	// Create the Win32 Surface KHR
	VkSurfaceKHR SurfaceKHR;
	VK_ASSERT(glfwCreateWindowSurface(Instance, Window, NULL, &SurfaceKHR));

	// Find the physical device to use
	uint32_t PhysicalDeviceCount;
	vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, NULL);

	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.data()));

	// TODO pick physical device
	VkPhysicalDevice PhysicalDevice;
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
	uint32_t QueueFamilyIndex;
	uint32_t QueueCount;

	// TODO actually pick the best physical device
	// Get info about the available physical devices and pick one for use
	CONSOLE_LOG("\nAmount of physical devices: " << PhysicalDevices.size());
	for (size_t i = 0; i < PhysicalDevices.size(); ++i)
	{
		VkPhysicalDevice Device = PhysicalDevices[i];
		CONSOLE_LOG("\n=> Physical Device #" << i);

		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(Device, &PhysicalDeviceProperties);

		CONSOLE_LOG("\tDevice type: " << PhysicalDeviceProperties.deviceType);
		CONSOLE_LOG("\tDevice name: " << PhysicalDeviceProperties.deviceName);

		VkPhysicalDeviceFeatures DeviceFeatures;
		vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, NULL);
		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilyProperties.data());

		CONSOLE_LOG("\nAmount of queue familes: " << QueueFamilyProperties.size());
		for (size_t j = 0; j < QueueFamilyProperties.size(); ++j)
		{
			CONSOLE_LOG("\n=> Queue Family #" << j);
			CONSOLE_LOG("\tVK_QUEUE_GRAPHICS_BIT	" << (QueueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT));
			CONSOLE_LOG("\tVK_QUEUE_COMPUTE_BIT	" << (QueueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT));
			CONSOLE_LOG("\tVK_QUEUE_TRANSFER_BIT	" << (QueueFamilyProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT));
			CONSOLE_LOG("\tQueue count: " << QueueFamilyProperties[j].queueCount);
			CONSOLE_LOG("\tTimestamp valid bits: " << QueueFamilyProperties[j].timestampValidBits);
			uint32_t width = QueueFamilyProperties[j].minImageTransferGranularity.width;
			uint32_t height = QueueFamilyProperties[j].minImageTransferGranularity.height;
			uint32_t depth = QueueFamilyProperties[j].minImageTransferGranularity.depth;
			CONSOLE_LOG("\tMin Image Timestamp Granularity: " << "w" << width << ", h" << height << ", d" << depth);

			// Pick an appriopriate queue
			if (QueueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				QueueFamilyIndex = static_cast<uint32_t>(j);
				QueueCount = QueueFamilyProperties[j].queueCount;
				CONSOLE_LOG("\tFound Queue!");
				break;
			}
		}

		PhysicalDevice = Device;
		PhysicalDeviceFeatures = DeviceFeatures;

		// Prefer the use of discrete GPUs
		if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			CONSOLE_LOG("\tFound physical device!");
			break;
		}
	}

	// TODO set correct queue index and count etc
	// Prepare Queue Info for Device creation
	float QueuePriority = 1.f;
	VkDeviceQueueCreateInfo DeviceQueueCreateInfo = {};
	DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueCreateInfo.pNext = NULL;
	DeviceQueueCreateInfo.flags = 0;
	DeviceQueueCreateInfo.queueFamilyIndex = QueueFamilyIndex;
	DeviceQueueCreateInfo.queueCount = QueueCount;
	DeviceQueueCreateInfo.pQueuePriorities = &QueuePriority;
	
	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos(1);
	QueueCreateInfos.push_back(DeviceQueueCreateInfo);

	// Create the logical device
	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = NULL;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = 0; // deprecated
	DeviceCreateInfo.ppEnabledLayerNames = NULL; // deprecated
	DeviceCreateInfo.enabledExtensionCount = 0;
	DeviceCreateInfo.ppEnabledExtensionNames = NULL;
	DeviceCreateInfo.pEnabledFeatures = &PhysicalDeviceFeatures;

	VkDevice LogicalDevice = 0;
	VK_ASSERT(vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, NULL, &LogicalDevice));

	// TODO get correct queue index for graphics queue
	VkQueue Queue = 0;
	vkGetDeviceQueue(LogicalDevice, QueueFamilyIndex, 1, &Queue);

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
	vkDestroyDevice(LogicalDevice, NULL);
	vkDestroySurfaceKHR(Instance, SurfaceKHR, NULL);
	vkDestroyInstance(Instance, NULL);

	glfwDestroyWindow(Window);

	return 0;
}
